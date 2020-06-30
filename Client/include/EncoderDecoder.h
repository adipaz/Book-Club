//
// Created by adi on 10/01/2020.
//

#ifndef ASSIGNMENT3_ENCODERDECODER_H
#define ASSIGNMENT3_ENCODERDECODER_H

#include "../include/Frame.h"
#include <map>
#include <string>
using namespace std;

class EncoderDecoder{
private:


    //methods

    string FrameToString(Frame& msg);
    Frame* StringToFrame(string& msg);


public:
    string Encode(Frame& msg);
    Frame* Decode(string& msg);

};

#endif //ASSIGNMENT3_ENCODERDECODER_H
