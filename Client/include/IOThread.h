//
// Created by adi on 12/01/2020.
//

/*
 * this class is in charge of reading commands from keyboard and handling them properly
 */

#ifndef ASSIGNMENT3_IOTHREAD_H
#define ASSIGNMENT3_IOTHREAD_H

#include <iostream>
#include <mutex>
#include <thread>
#include <string>
#include <Frame.h>
#include <User.h>
#include <connectionHandler.h>
using namespace std;

class IOThread {
private:
    string command;
    ConnectionHandler* connectionHandler;
    User* user;
    std::timed_mutex & _mutex;
    bool isReading;

    Frame* loginCommand(string& command);
    Frame* joinCommand(string& command);
    Frame* exitCommand(string& command);
    Frame* addCommand(string& command);
    Frame* borrowCommand(string& command);
    Frame* returnCommand(string& command);
    Frame* statusCommand(string& command);
    Frame* logoutCommand(string& command);
    Frame* getFrameByCommand(string& command);
public:
    IOThread(string& command,User& user, ConnectionHandler& connectionHandler, std::timed_mutex& mutex);
    IOThread(IOThread const &other);
    IOThread& operator =(IOThread const &other);
    void run();
};

#endif //ASSIGNMENT3_IOTHREAD_H
