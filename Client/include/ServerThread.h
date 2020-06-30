//
// Created by adi on 12/01/2020.
//

/*
 * this class is in charge of reading messages from the server and handling them properly.
 */

#ifndef ASSIGNMENT3_SOCKETTHREAD_H
#define ASSIGNMENT3_SOCKETTHREAD_H

#include <iostream>
#include <mutex>
#include <thread>
#include <connectionHandler.h>
#include <User.h>

class ServerThread {
private:


    User* user;
    ConnectionHandler* connectionHandler;
    std::timed_mutex & _mutex;

    void handleMESSAGE(Frame& msg);
    void handleERROR(Frame& msg);
    void handleRECIEPT(Frame& msg);

public:
    ServerThread(User& user, ConnectionHandler& connectionHandler, std::timed_mutex& mutex);
    ServerThread(ServerThread const &other);
    ServerThread& operator=(ServerThread const &other);
    void run();
    void handleMessage(Frame& msg);
    void sendMessage(Frame& msgToSend);
};

#endif //ASSIGNMENT3_SOCKETTHREAD_H
