#include <iostream>
#include "GoogleBooksInterface.h"
#include "IniParser.h"

#include <json/json.h>

#pragma comment (lib,"GoogleBooksApi.lib")

using namespace std;
using namespace NSIniParser;

void printBookDetails(const Json::Value& book)
{
    cout << "Title: " << book["volumeInfo"]["title"].asString() << endl;

    cout << "Authors: ";
    for (const auto& author : book["volumeInfo"]["authors"])
        cout << author.asString() << ", ";
    cout << endl;

    cout << "Publisher: " << book["volumeInfo"]["publisher"].asString() << endl;
    cout << "Published Date: " << book["volumeInfo"]["publishedDate"].asString() << endl;
    cout << "Description: " << book["volumeInfo"]["description"].asString() << endl;
    cout << "Page Count: " << book["volumeInfo"]["pageCount"].asInt() << endl;

    cout << "Categories: ";
    for (const auto& category : book["volumeInfo"]["categories"])
        cout << category.asString() << ", ";
    cout << endl;

    cout << "Average Rating: " << book["volumeInfo"]["averageRating"].asDouble() << endl;
    cout << "Ratings Count: " << book["volumeInfo"]["ratingsCount"].asInt() << endl;
    cout << "Image Link: " << book["volumeInfo"]["imageLinks"]["thumbnail"].asString() << endl;
    cout << "Preview Link: " << book["volumeInfo"]["previewLink"].asString() << endl;
    cout << "Info Link: " << book["volumeInfo"]["infoLink"].asString() << endl;
    cout << "Canonical Volume Link: " << book["volumeInfo"]["canonicalVolumeLink"].asString() << endl;
    cout << "----------------------------------------" << endl;
}

int main()
{
    auto iniParser{ IniParser() };
    iniParser.parseFromFile("TestGoogleBooksApiDll.cfg");

    auto apiKey = iniParser.getValue("Api Key", "Key");

    auto apiGoogleBooks = GoogleBooksInterfaceWithKeySingleton::getInstance(apiKey);

    try
    {
        auto books = apiGoogleBooks.getAllBooksBySubject("terror");

        for (const auto& book : books["items"])
            printBookDetails(book);
    }
    catch (const runtime_error& ex)
    {
        cerr << ex.what() << endl;
    }

    return EXIT_SUCCESS;
}
