#include "pch.h"

//#define CURL_STATICLIB
#include <curl/curl.h>
#include <json/json.h>

#include <string>
#include <sstream>
#include <algorithm>

#include "GoogleBooksInterface.h"

#ifdef _DEBUG
#    pragma comment (lib,"libcurl_debug.lib")
#else
#    pragma comment (lib,"libcurl.lib")
#endif // _DEBUG

/*Windows Specific Additional Dependencies*/
#pragma comment (lib,"Normaliz.lib")
#pragma comment (lib,"Ws2_32.lib")
#pragma comment (lib,"Wldap32.lib")
#pragma comment (lib,"Crypt32.lib")

using namespace std;

GoogleBooksInterface::GoogleBooksInterface(const string& apiKey) : m_apiKey{ apiKey }, m_curl{ nullptr }, m_response{}
{
    initCurl();
}

GoogleBooksInterface::GoogleBooksInterface() : m_apiKey{}, m_curl{ nullptr }, m_response{}
{
    initCurl();
}

GoogleBooksInterface::~GoogleBooksInterface()
{
    if (m_curl)
        curl_easy_cleanup(m_curl);
}

void GoogleBooksInterface::setApiKey(const std::string& apiKey)
{
    m_apiKey = apiKey;
}

Json::Value GoogleBooksInterface::getAllBooksBySubject(const string& subject, int startIndex, int maxResults)
{
    const string url = "https://www.googleapis.com/books/v1/volumes?q=subject:" + replaceSpaces(subject)
        + "&startIndex=" + to_string(startIndex)
        + "&maxResults=" + to_string(maxResults)
        + "&key=" + m_apiKey;

    auto response = httpGet(url);

    if (!response.empty())
        return parseResponse(response);

    return {};
}

Json::Value GoogleBooksInterface::getAllBooksByTitle(const std::string& bookTitle, int startIndex, int maxResults)
{
    const string url = "https://www.googleapis.com/books/v1/volumes?q=intitle:" + replaceSpaces(bookTitle) + "&key=" + m_apiKey;

    auto response = httpGet(url);

    if (!response.empty())
        return parseResponse(response);

    return {};
}

Json::Value GoogleBooksInterface::getAllBooksByAuthor(const std::string& author, int startIndex, int maxResults)
{
    const string url = "https://www.googleapis.com/books/v1/volumes?q=inauthor:" + replaceSpaces(author) + "&key=" + m_apiKey;

    auto response = httpGet(url);

    if (!response.empty())
        return parseResponse(response);
    
    return {};
}

Json::Value GoogleBooksInterface::getAllBooksByIsbn(const std::string& ISBN, int startIndex, int maxResults)
{
    const string url = "https://www.googleapis.com/books/v1/volumes?q=isbn:" + ISBN + "&key=" + m_apiKey;

    auto response = httpGet(url);

    if (!response.empty())
        return parseResponse(response);

    return {};
}

void GoogleBooksInterface::initCurl()
{
    curl_global_init(CURL_GLOBAL_ALL);
    m_curl = curl_easy_init();
    if (!m_curl)
        throw GoogleBooksInterfaceException{ "Failed to initialize CURL" };
}

std::string GoogleBooksInterface::replaceSpaces(string str, const char sign)
{
    replace(begin(str), end(str), ' ', sign);
    return str;
}

std::string GoogleBooksInterface::httpGet(const std::string& url)
{
    if (m_curl)
    {
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_response);
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
        auto result = curl_easy_perform(m_curl);
        if (result != CURLE_OK)
            throw GoogleBooksInterfaceException{ "Failed to get data from URL" };

        return m_response;
    }

    throw GoogleBooksInterfaceException{ "CURL Interface not initialized" };

    return {};
}

Json::Value GoogleBooksInterface::parseResponse(const std::string& response)
{
    Json::Value jsonData;
    Json::CharReaderBuilder builder;
    string errors;

    istringstream iss(response);

    if (!Json::parseFromStream(builder, iss, &jsonData, &errors))
        throw GoogleBooksInterfaceException{ "Failed to parse JSON response:\n" + m_response };

    return jsonData;
}

size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userContent)
{
    const size_t realsize{ size * nmemb };

    if (!userContent)
        return 0;

    userContent->append((char*)contents, realsize);
    
    return realsize;
}
