//
// Created by adi on 11/01/2020.
//

#ifndef ASSIGNMENT3_COMMAND_H
#define ASSIGNMENT3_COMMAND_H

#include <string>
#include <Frame.h>
#include <map>
#include <vector>
using namespace std;

class User{
private:
    int idIndex;
    int receiptIndex;
    string username;
    string password;
    map<string,vector<string>> Inventory;
    map<string,int> subscribeID;
    map<string,string> borrowedBooks;
    map<int,string> receipt;
    vector<string> wishlist;
    bool loggedIn;
public:
    User(string& username,string& password);
    virtual ~User();
    string getName();
    void addBook(string& genre,string& bookname);
    int addGenre(string& genre);
    int addReceipt(string& receipt);
    void removeGenre(string& genre);
    bool removeBook(string& genre,string& bookName);
    void borrowBook(string& genre, string& bookName,string& lender);
    string returnBook(string& genre,string& bookName);
    string getReceipt(int index);
    string getBooksByGenre(string& genre);
    void addToWishlist(string& bookName);
    bool hasBook(string& genre,string& bookName);
    bool inWishlist(string& bookName);
    int getId(string& genre);
    void setLoggedIn(bool value);
    bool getLoggedIn();
};

#endif //ASSIGNMENT3_USER_H
