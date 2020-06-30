//
// Created by adi on 10/01/2020.
//

#include "../include/EncoderDecoder.h"

#include <iostream>
#include <string.h>

using namespace std;

/**
* -----------------------------------------------------EncoderDecoder---------------------------------------------------
*/

//-----------------------------------------------------PrivateFunctions-------------------------------------------------
string EncoderDecoder::FrameToString(Frame &msg) {
    string output=msg.getCommand()+'\n';
    for(auto header : msg.getHeaders())
    {
        output+=header.first+':'+header.second+'\n';
    }
    output+='\n'+msg.getBody()+'\0';
    return output;
}

Frame *EncoderDecoder::StringToFrame(string& msg) {
    string command="";
    string body="";
    map<string,string> headers;

    //get the command
    while(msg.at(0)!='\n') {
        command += msg.at(0);
        msg=msg.substr(1);
    }
    msg=msg.substr(1); //removes the '\n'

    //check if some headers exists
    if(msg.at(0)!='\n')
    {

        //get the headers
        while(!(msg.at(0)=='\n') ){
            string key="";
            string value="";
            //get the key of the header
            while (msg.at(0)!=':')
            {
                key+=msg.at(0);
                msg=msg.substr(1);
            }
            msg=msg.substr(1); //removes the ':'

            //get the value of the header
            while (msg.at(0)!='\n')
            {
                value+=msg.at(0);
                msg=msg.substr(1);
            }
            msg=msg.substr(1); //removes the '\n'

            //add the header to the map
            headers.insert(std::pair<string ,string >(key,value));
        }
    }
    msg=msg.substr(1); //removes '\n'

    //get the body
    if(msg.length()>0)
    {
        while (msg.length()>0)
        {
            if((msg.length()==1) & (msg.at(0)=='\n'))
                break;
            body+=msg.at(0);
            msg=msg.substr(1);
        }
    }


    return  new Frame(command,headers,body);

}



//-----------------------------------------------------PublicFunctions-------------------------------------------------
string  EncoderDecoder::Encode(Frame &msg) {
    return this->FrameToString(msg);

}

Frame *EncoderDecoder::Decode(string& msg) {
    return StringToFrame(msg);
}


/**
* -----------------------------------------------------EncoderDecoder---------------------------------------------------
*/
