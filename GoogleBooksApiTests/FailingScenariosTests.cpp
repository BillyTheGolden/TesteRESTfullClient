#include "pch.h"

#include <json/json.h>
#include "GoogleBooksInterface.h"

namespace FailingScenarios
{
	using namespace std;

	const string InvalidKeyMessage = R"({"error":{"code":400,"message":"API key not valid. Please pass a valid API key.",
"errors":[{"message":"API key not valid. Please pass a valid API key.","domain":"global","reason":"badRequest"}],
"status":"INVALID_ARGUMENT","details":[{"@type":"type.googleapis.com/google.rpc.ErrorInfo","reason":"API_KEY_INVALID",
"domain":"googleapis.com","metadata":{"service":"books.googleapis.com"}},
{"@type":"type.googleapis.com/google.rpc.LocalizedMessage","locale":"en-US",
"message":"API key not valid. Please pass a valid API key."}]}})";

	const string NoBooksFound = R"({"kind":"books#volumes","totalItems":0})";

	class MockGoogleBooksInterface : public GoogleBooksInterface
	{
	public:
		MockGoogleBooksInterface(const string& key)
		{
			setApiKey(key);
		}

		Json::Value getAllBooksBySubject(const std::string& term, const std::string& subject, int startIndex = 0, int maxResults = 40) override
		{
			if (subject == "InvalidSubject")
				return parseResponse(subject);

			auto responseJson = GoogleBooksInterface::getAllBooksBySubject(term, subject);

			return responseJson;
		}

		Json::Value getAllBooksByTitle(const std::string& term, const std::string& bookTitle, int startIndex = 0, int maxResults = 40) override
		{
			auto responseJson = httpGet(bookTitle);

			return parseResponse(responseJson);
		}

		Json::Value getAllBooksByAuthor(const std::string& term, const std::string& author, int startIndex = 0, int maxResults = 40) override
		{
			auto responseJson = httpGet(author);
			return parseResponse(responseJson);
		}

		Json::Value getAllBooksByIsbn(const std::string& ISBN, int startIndex = 0, int maxResults = 40) override
		{
			return Json::Value{};
		}

		std::string httpGet(const std::string& url) override
		{
			if (m_apiKey.empty())
				return InvalidKeyMessage;

			if (url == "UnknownAuthor"s)
				return NoBooksFound;

			return GoogleBooksInterface::httpGet(url);
		}

		Json::Value parseResponse(const std::string& response) override
		{
			return GoogleBooksInterface::parseResponse(response);
		}
	};

	TEST(TestGoogleBooksApi, NoApiKeyRequest)
	{
		auto gbIf = MockGoogleBooksInterface{ "" };
		auto books = gbIf.getAllBooksByTitle("Terra", "Os pilares da terra");

		ASSERT_EQ(400, books["error"]["code"].asInt());
	}

	TEST(TestGoogleBooksApi, NoBooksFound)
	{
		auto gbIf = MockGoogleBooksInterface{ "NoEmptyKey" };
		auto books = gbIf.getAllBooksByAuthor("general term", "UnknownAuthor");

		ASSERT_EQ(0, books["totalItems"].asInt());
	}

	TEST(TestGoogleBooksApi, CurlInterfaceNotInitialized)
	{
		try
		{
			auto gbIf = MockGoogleBooksInterface{ "NoEmptyKey" };
			auto books = gbIf.getAllBooksByAuthor("general term", "UnknownAuthor");
		}
		catch (const GoogleBooksInterfaceException& ex)
		{
			ASSERT_EQ("CURL Interface not initialized"s, ex.what());
		}
	}

	TEST(TestGoogleBooksApi, CurlInterfaceError)
	{
		try
		{
			auto gbIf = MockGoogleBooksInterface{ "NoEmptyKey" };
			gbIf.initInterface();

			auto books = gbIf.getAllBooksBySubject("general term", "terror");
		}
		catch (const GoogleBooksInterfaceException& ex)
		{
			ASSERT_EQ("Failed to get data from URL"s, ex.what());
		}
	}

	TEST(TestGoogleBooksApi, ParseResponseError)
	{
		try
		{
			auto gbIf = MockGoogleBooksInterface{ "NoEmptyKey" };
			gbIf.initInterface();

			auto books = gbIf.getAllBooksBySubject("general term", "InvalidSubject");
		}
		catch (const GoogleBooksInterfaceException& ex)
		{
			const auto subStr{ "Failed to parse JSON response:"s };
			const string exMessage{ ex.what() };

			auto pos = exMessage.find_first_of(subStr);
			ASSERT_TRUE(pos == 0);
		}
	}

	TEST(TestGoogleBooksApi, EscapeUrlCharacters)
	{
		auto gbIf = MockGoogleBooksInterface{ "AIzaSyBs5HC9JhqtuS99XHebTt7YEUzrMx2jGuI" };
		gbIf.initInterface();

		auto books = gbIf.getAllBooksBySubject("http://www.gooblebooks.com/v1/Fiction", "science");

		ASSERT_FALSE(books["totalItems"].asInt() > 0);
	}
}