#include <iostream>
#include "GoogleBooksInterface.h"
#include "IniParser.h"

#include <json/json.h>

#pragma comment (lib,"GoogleBooksApi.lib")

using namespace std;
using namespace NSIniParser;

void printBookDetails(const Json::Value& book)
{
    cout << "----------------------------------------" << endl;
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
}

int main()
{
    auto iniParser{ IniParser() };
    iniParser.parseFromFile("TestGoogleBooksApiDll.cfg");

    auto apiKey = iniParser.getValue("Api Key", "Key");

    auto apiGoogleBooks = GoogleBooksInterfaceWithKeySingleton::getInstance(apiKey);

    int opt{ 0 };

    do
    {
        cout << "\nSearch books menu:\n1) general string\n2) by subject\n3) by author\n4) by title\n5) by isbn\n9) quit\nEnter you choice, please: ";

        cin >> opt;

        cin.ignore();

        try
        {
            Json::Value books;

            string searchTerm;
            if (opt != 0 && opt != 9)
            {
                cout << "Enter search term: ";
                getline(cin, searchTerm);
            }

            switch (opt)
            {
            case 1:
            {
                books = apiGoogleBooks.getAllBooksByTerm(searchTerm);
                break;
            }
            case 2:
            {
                string subject;
                cout << "Enter subject: ";
                getline(cin, subject);
                books = apiGoogleBooks.getAllBooksBySubject(searchTerm, subject);
                break;
            }
            case 3:
            {
                string author;
                cout << "Enter author: ";
                getline(cin, author);
                books = apiGoogleBooks.getAllBooksByAuthor(searchTerm, author);
                break;
            }
            case 4:
            {
                string title;
                cout << "Enter title: ";
                getline(cin, title);
                books = apiGoogleBooks.getAllBooksByTitle(searchTerm,title);
                break;
            }
            case 5:
            {
                books = apiGoogleBooks.getAllBooksByIsbn(searchTerm);
                break;
            }
            case 0:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid option. Please try again." << endl;
                continue;
            }

            if (opt != 0)
            {
                auto totalItems = books["totalItems"].asInt();
                if (totalItems > 0)
                {
                    cout << totalItems << " book(s) found." << endl;

                    for (const auto& book : books["items"])
                        printBookDetails(book);
                }
                else
                    cout << "No items found using the term provided." << endl;
            }
        }
        catch (const GoogleBooksInterfaceException& ex)
        {
            cerr << ex.what() << endl;
        }
    } while (opt != 0);

    return EXIT_SUCCESS;
}
