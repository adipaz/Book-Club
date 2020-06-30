
#include "../include/connectionHandler.h"
#include <User.h>
#include <map>
#include <list>
#include <IOThread.h>
#include <ServerThread.h>
#include <iostream>
#include <mutex>
#include <thread>

/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/


int main (int argc, char *argv[]) {
    //arguments for communication
    string host ;
    short port ;
    std::timed_mutex mutex;
    //arguments for book club
    User* user;

    //waits for a login command
    const short bufsize = 1024;
    char buf[bufsize];
    std::cin.getline(buf, bufsize);
    std::string line(buf);
    while (line.substr(0, line.find(' ')).compare("login")!=0) {
        cout<<"waiting for a login command"<<endl;
        std::cin.getline(buf, bufsize);
        line=buf;
    }
    //received a login command
    string command=line;
    command.erase(0, command.find(' ') + 1); //erase the login
    host= command.substr(0, command.find(':')); //host
    command.erase(0, command.find(':') + 1); //erase the host:
    string portAsString=command.substr(0, command.find(' '));
    port= stoi(portAsString);//port
    command.erase(0, command.find(' ') + 1);//erase the port
    string username=command.substr(0, command.find(' '));//username;
    command.erase(0, command.find(' ') + 1);//erase the username
    user=new User(username,command);
    line="login "+host+":"+portAsString+" "+username+" "+command;

    //creates the connection handler and tries to connect to the server at the beginning
    ConnectionHandler* connectionHandler = new ConnectionHandler(host, port,mutex);
    if (!connectionHandler->connect()) {
        std::cerr << "Could not connect to server" << std::endl;
        return 1;
    }

    //run the threads
    IOThread io(line,*user,*connectionHandler,mutex);
    ServerThread server(*user,*connectionHandler,mutex);
    std::thread th1(&IOThread::run, io);
    std::thread th2(&ServerThread::run, server);
    th1.join();
    th2.join();
    delete connectionHandler;
    delete user;
}
