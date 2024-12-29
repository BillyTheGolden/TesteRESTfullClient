#pragma once

#include <stdexcept>

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

/**
 * @class GoogleBooksInterfaceException
 * 
 * Custom exception class.
 */
class GoogleBooksInterfaceException : public std::exception
{
public:
    GoogleBooksInterfaceException(const std::string& message) : m_what{message} {}
    const char* what() const noexcept { return m_what.c_str(); }

private:
    std::string m_what;
};

/**
 * @class GoogleBooksInterface
 * @brief Interface for interacting with the Google Books API.
 *
 * This class provides methods to search for books by subject, title, author, and ISBN.
 * It also handles HTTP requests and responses, and parses JSON data.
 */
class DLL_API GoogleBooksInterface
{
public:
    /**
     * @brief Constructs a GoogleBooksInterface with the given API key.
     * @param apiKey The API key for accessing the Google Books API.
     */
    GoogleBooksInterface(const std::string& apiKey);

    /**
     * @brief Default constructor for GoogleBooksInterface.
     */
    GoogleBooksInterface();

    /**
     * @brief Destructor for GoogleBooksInterface.
     */
    ~GoogleBooksInterface();

    /**
     * @brief Retrieves all books by subject.
     * @param subject The subject to search for.
     * @param startIndex The index of the first result to return.
     * @param maxResults The maximum number of results to return.
     * @return A JSON value containing the search results.
     */
    virtual Json::Value getAllBooksBySubject(const std::string& subject, int startIndex = 0, int maxResults = 40);

    /**
     * @brief Retrieves all books by title.
     * @param bookTitle The title of the book to search for.
     * @param startIndex The index of the first result to return.
     * @param maxResults The maximum number of results to return.
     * @return A JSON value containing the search results.
     */
    virtual Json::Value getAllBooksByTitle(const std::string& bookTitle, int startIndex = 0, int maxResults = 40);

    /**
     * @brief Retrieves all books by author.
     * @param author The author of the book to search for.
     * @param startIndex The index of the first result to return.
     * @param maxResults The maximum number of results to return.
     * @return A JSON value containing the search results.
     */
    virtual Json::Value getAllBooksByAuthor(const std::string& author, int startIndex = 0, int maxResults = 40);

    /**
     * @brief Retrieves all books by ISBN.
     * @param ISBN The ISBN of the book to search for.
     * @param startIndex The index of the first result to return.
     * @param maxResults The maximum number of results to return.
     * @return A JSON value containing the search results.
     */
    virtual Json::Value getAllBooksByIsbn(const std::string& ISBN, int startIndex = 0, int maxResults = 40);

    /**
     * @brief Sets the API key for accessing the Google Books API.
     * @param apiKey The API key to set.
     */
    void setApiKey(const std::string& apiKey);

    /**
     * @brief Initializes the interface by setting up the CURL instance.
     */
    inline void initInterface()
    {
        initCurl();
    }

protected:
    std::string m_apiKey; ///< The API key for accessing the Google Books API.
    std::string m_response; ///< The response from the Google Books API.

    /**
     * @brief Performs an HTTP GET request.
     * @param url The URL to send the request to.
     * @return The response as a string.
     */
    virtual std::string httpGet(const std::string& url);

    /**
     * @brief Parses the JSON response.
     * @param response The response to parse.
     * @return A JSON value containing the parsed data.
     */
    virtual Json::Value parseResponse(const std::string& response);

private:
    CURL* m_curl; ///< The CURL instance for making HTTP requests.

    /**
     * @brief Initializes the CURL instance.
     */
    void initCurl();

    /**
     * @brief Replaces spaces in a string with a specified character.
     * @param str The string to modify.
     * @param sign The character to replace spaces with.
     * @return The modified string.
     */
    std::string replaceSpaces(std::string str, const char sign = '+');
};

/**
 * @typedef GoogleBooksInterfaceSingleton
 * @brief Singleton instance of GoogleBooksInterface.
 */
using GoogleBooksInterfaceSingleton = Singleton<GoogleBooksInterface>;

/**
 * @typedef GoogleBooksInterfaceWithKeySingleton
 * @brief Singleton instance of GoogleBooksInterface with an API key.
 */
using GoogleBooksInterfaceWithKeySingleton = Singleton<GoogleBooksInterface, const std::string&>;

/**
 * @brief Callback function for writing data received from CURL.
 * @param contents The data received.
 * @param size The size of each data element.
 * @param nmemb The number of data elements.
 * @param userContent The user content to write to.
 * @return The number of bytes written.
 */
static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userContent);
