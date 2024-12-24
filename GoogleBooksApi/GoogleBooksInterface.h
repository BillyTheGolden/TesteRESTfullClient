#pragma once

#include "Singleton.h"

#ifdef GOOGLEBOOKSAPI_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif // IRESTCLIENT_EXPORTS

#pragma warning(disable : 4251)

using CURL = void;

namespace Json
{
    class Value;
}

class DLL_API GoogleBooksInterface
{
public:
    GoogleBooksInterface(const std::string& apiKey);
    GoogleBooksInterface();
    ~GoogleBooksInterface();

    void setApiKey(const std::string& apiKey);

    inline void initInterface()
    {
        initCurl();
    }

    Json::Value getAllBooksBySubject(const std::string& subject, int startIndex = 0, int maxResults = 40);
    Json::Value getAllBooksByTitle(const std::string& bookTitle, int startIndex = 0, int maxResults = 40);
    Json::Value getAllBooksByAuthor(const std::string& author, int startIndex = 0, int maxResults = 40);
    Json::Value getAllBooksByIsbn(const std::string& ISBN, int startIndex = 0, int maxResults = 40);

private:
    std::string m_apiKey;
    std::string m_response;
    CURL* m_curl;

private:
    void initCurl();
    std::string httpGet(const std::string& url);
    Json::Value parseResponse(const std::string& response);
};

using GoogleBooksInterfaceSingleton = Singleton<GoogleBooksInterface>;
using GoogleBooksInterfaceWithKeySingleton = Singleton<GoogleBooksInterface, const std::string&>;

static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userContent);
