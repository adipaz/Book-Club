//
// Created by adi on 10/01/2020.
//


#include "../include/Frame.h"


//-----------------------------------------------------Frame-----------------------------------------------------------
Frame::Frame(string &command, map<string, string> &headers, string &body): command(command),headers(headers),body(body) {
}

string Frame::getCommand() {
    return  this->command;
}

map<string, string>& Frame::getHeaders() {
    return (this->headers);
}

string Frame::getBody() {
    return this->body;
}

string Frame::toString() {
    string output="";
    output+=command+'\n';
    for(auto header : headers)
        output+=header.first+" : "+header.second+'\n';
    output+=body;
    return  output;
}

Frame::~Frame() {
    headers.clear();

}
//-----------------------------------------------------Frame-----------------------------------------------------------
