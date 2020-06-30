//
// Created by adi on 11/01/2020.
//

#include <User.h>
#include <netdb.h>
#include <iostream>
#include <string.h>



using  namespace std;

User::User(string& username,string& password): idIndex(0),receiptIndex(0),username(username),password(password),Inventory(),subscribeID(),borrowedBooks(),receipt(),wishlist(),loggedIn(false) {
}

string User::getName() {
    return this->username;
}

void User::addBook(string &genre, string &bookname) {
    if(Inventory.find(genre)==Inventory.end())
    {
        Inventory.insert(std::pair<string,vector<string>>(genre,vector<string>()));
    }
    Inventory.at(genre).push_back(bookname);
}

int User::addGenre(string &genre) {
    if(Inventory.find(genre)==Inventory.end())
    {
        Inventory.insert(std::pair<string,vector<string>>(genre,vector<string>()));
    }
    subscribeID.insert(std::pair<string,int>(genre,idIndex));
    idIndex++;
    return idIndex-1;
}

int User::addReceipt(string &receipt) {
    this->receipt.insert(std::pair<int,string>(receiptIndex,receipt));
    receiptIndex++;
    return receiptIndex-1;
}

void User::removeGenre(string &genre) {
    if(Inventory.find(genre)!=Inventory.end())
    {
        Inventory.erase(genre);
    }

}

bool User::removeBook(string &genre, string& bookName) {
    if(Inventory.find(genre)!=Inventory.end()){
        int size = Inventory.at(genre).size();
        for(int i=0;i< size;i++)
        {
            if(bookName.find(Inventory.at(genre).at(i))!=string::npos)
            {
                Inventory.at(genre).erase(Inventory.at(genre).begin()+i);
                return true;
            }

        }
    }
    return false;


}

void User::borrowBook(string &genre, string &bookName, string &lender) {
    addBook(genre,bookName);
    borrowedBooks.insert(std::pair<string,string>(bookName,lender));
    int size = wishlist.size();
    for(int i=0;i< size;i++)
    {
        if(wishlist.at(i).compare(bookName)==0)
            wishlist.erase(wishlist.begin()+i);
    }
}

string User::returnBook(string &genre, string &bookName) {
    bool removed=removeBook(genre,bookName);
    if(removed)
    {
        if(borrowedBooks.find(bookName)!=borrowedBooks.end())
        {
            string lender=borrowedBooks.at(bookName);
            borrowedBooks.erase(bookName);
            return  lender;
        }
        return "";
    }
    return "";

}

string User::getReceipt(int index) {
    return receipt.at(index);
}

string User::getBooksByGenre(string& genre) {
    string books=username+":";
    for(auto book :Inventory.at(genre))
    {
        books+=book+",";
    }
    return books.substr(0,books.size()-1);
}

void User::addToWishlist(string &bookName) {
    this->wishlist.push_back(bookName);
}

bool User::hasBook(string& genre,string& bookName) {
    int size = Inventory.at(genre).size();
    for(int i=0;i < size;i++)
    {
        if(Inventory.at(genre).at(i).compare(bookName)==0)
            return true;
    }
    return false;
}

bool User::inWishlist(string& bookName) {
    for(auto book : wishlist)
    {
        if(book.compare(bookName)==0)
            return true;
    }
    return false;
}

int User::getId(string &genre) {
    return this->subscribeID.at(genre);
}

User::~User() {
    for(auto elem : Inventory)
    {
        elem.second.clear();
    }
    Inventory.clear();
    subscribeID.clear();
    borrowedBooks.clear();
    receipt.clear();
    wishlist.clear();
}

void User::setLoggedIn(bool value) {
    loggedIn=value;
}

bool User::getLoggedIn() {
    return loggedIn;
}


