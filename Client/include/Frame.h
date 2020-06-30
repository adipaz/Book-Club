//
// Created by adi on 10/01/2020.
//

#ifndef ASSIGNMENT3_FRAME_H
#define ASSIGNMENT3_FRAME_H


#include <map>
#include <string>
using namespace std;

class Frame
{
private:
    string command;
    map<string,string> headers;
    string body;


public:
    Frame(string& command,map<string,string>& headers, string& body);
    virtual ~Frame();
    string getCommand();
    map<string,string>& getHeaders();
    string getBody();
    string toString();

};

#endif //ASSIGNMENT3_FRAME_H
