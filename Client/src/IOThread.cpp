//
// Created by adi on 12/01/2020.
//


#include <IOThread.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <EncoderDecoder.h>
#include <connectionHandler.h>

Frame *IOThread::getFrameByCommand(string& command) {
    string first = command.substr(0, command.find(' ')); //first word of the command
    command.erase(0, command.find(' ') + 1); //erase the first word of the command
    if(first.compare("login")==0)
    {
        return loginCommand(command);
    }
    else if(first.compare("join")==0)
    {
        return  joinCommand(command);
    }
    else if(first.compare("exit")==0)
    {
        return  exitCommand(command);
    }
    else if(first.compare("add")==0)
    {
        return  addCommand(command);
    }
    else if(first.compare("borrow")==0)
    {
        return borrowCommand(command);
    }
    else if(first.compare("return")==0)
    {
        return returnCommand(command);
    }
    else if(first.compare("status")==0)
    {
        return statusCommand(command);
    }
    else if(first.compare("logout")==0)
    {
        return logoutCommand(command);
    }
    else
        throw invalid_argument( "illegal command" );
}

Frame *IOThread::loginCommand(string& command) {
    string cmd="CONNECT";
    string body="";
    map<string ,string > headers;
    string host= command.substr(0, command.find(':')); //host
    command.erase(0, command.find(':') + 1); //erase the host:
    string port= command.substr(0, command.find(' ')); //port
    command.erase(0, command.find(' ') + 1); //erase the port
    string username = command.substr(0, command.find(' ')); //username
    command.erase(0, command.find(' ') + 1); //erase the username
    string password= command; //password
    if(user->getLoggedIn())
        user->setLoggedIn(false);
    headers.insert(std::pair<string ,string >("accept-version","1.2"));
    headers.insert(std::pair<string ,string >("host",host));
    headers.insert(std::pair<string ,string >("login",username));
    headers.insert(std::pair<string ,string >("passcode",password));

    return new Frame(cmd, (headers), body);
}

Frame *IOThread::joinCommand(string& command) {
    string cmd="SUBSCRIBE";
    string body="";
    map<string ,string > headers;
    string genre= command; //genre
    int id=user->addGenre(genre);
    string msg="Joined club "+genre;
    int receipt=user->addReceipt(msg);
    headers.insert(std::pair<string ,string >("destination",genre));
    headers.insert(std::pair<string ,string >("id",std::to_string(id)));
    headers.insert(std::pair<string ,string >("receipt",std::to_string(receipt)));

    return new Frame(cmd, (headers), body);
}

Frame *IOThread::exitCommand(string& command) {
    string cmd="UNSUBSCRIBE";
    string body="";
    map<string ,string > headers;
    string genre= command; //genre
    user->removeGenre(genre);
    string msg="Exited club "+genre;
    int receipt=user->addReceipt(msg);
    headers.insert(std::pair<string ,string >("id",std::to_string(user->getId(genre))));
    headers.insert(std::pair<string ,string >("receipt",std::to_string(receipt)));

    return new Frame(cmd, (headers), body);
}

Frame *IOThread::addCommand(string& command) {
    string genre= command.substr(0, command.find(' ')); //genre
    command.erase(0, command.find(' ') + 1); //erase the genre
    string bookName= command; //book name
    string cmd="SEND";
    string body=user->getName()+" has added the book "+bookName;
    user->addBook(genre,bookName);
    map<string ,string > headers;
    headers.insert(std::pair<string ,string >("destination",genre));

    return new Frame(cmd, (headers), body);
}

Frame *IOThread::borrowCommand(string& command) {
    string genre= command.substr(0, command.find(' ')); //genre
    command.erase(0, command.find(' ') + 1); //erase the genre
    string bookName= command; //book name
    string cmd="SEND";
    string body=user->getName()+" wish to borrow "+bookName;
    user->addToWishlist(bookName);
    map<string ,string > headers;
    headers.insert(std::pair<string ,string >("destination",genre));

    return new Frame(cmd, (headers), body);
}

Frame *IOThread::returnCommand(string& command) {
    string genre= command.substr(0, command.find(' ')); //genre
    command.erase(0, command.find(' ') + 1); //erase the genre
    string bookName= command; //book name
    string cmd="SEND";
    string lender=user->returnBook(genre,bookName);
    if(lender!="")
    {
        string body="Returning "+bookName+" to "+lender;
        map<string ,string > headers;
        headers.insert(std::pair<string ,string >("destination",genre));

        return new Frame(cmd, (headers), body);
    }
    return nullptr;

}

Frame *IOThread::statusCommand(string& command) {
    string cmd="SEND";
    string body="book status";
    map<string ,string > headers;
    string genre= command; //genre
    headers.insert(std::pair<string ,string >("destination",genre));

    return new Frame(cmd, (headers), body);
}

Frame *IOThread::logoutCommand(string& command) {
    this->isReading=false;
    string cmd="DISCONNECT";
    string body="";
    map<string ,string > headers;
    string logout="logout";
    int receipt=user->addReceipt(logout);
    headers.insert(std::pair<string ,string >("receipt",std::to_string(receipt)));

    return new Frame(cmd, (headers), body);
}

IOThread::IOThread( string& command,User &user, ConnectionHandler &connectionHandler, std::timed_mutex& mutex) : command(command), connectionHandler(&connectionHandler), user(&user), _mutex(mutex) ,isReading(true){
}

IOThread::IOThread(IOThread const &other): command(other.command), connectionHandler(other.connectionHandler), user(other.user), _mutex(other._mutex) ,isReading(other.isReading){
}

void IOThread::run() {

    //CONNECT FRAME
    EncoderDecoder encoderDecoder;
    Frame *msg = getFrameByCommand(this->command);
    string bytes = encoderDecoder.Encode(*msg);
    if (!connectionHandler->sendLine(bytes)) {
        std::cout << "Disconnected. Exiting...\n" << std::endl;
    }
    if(msg != nullptr)
    {
        delete msg;
        msg= nullptr;
    }


    while(connectionHandler->getIsConnected()) {
        const short bufsize = 1024;
        char buf[bufsize];
        if(isReading & user->getLoggedIn())
        {
            std::cin.getline(buf, bufsize);
            std::string line(buf);
            msg = getFrameByCommand(line);
        }
        if(msg!= nullptr)
        {
            bytes = encoderDecoder.Encode(*msg);
            if (!connectionHandler->getIsConnected()||!connectionHandler->sendLine(bytes)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            delete msg;
            msg = nullptr;
        }

    }

}

IOThread& IOThread::operator=(IOThread const &other) {
    delete this->user;
    delete this->connectionHandler;
    this->command=other.command;
    this->connectionHandler=other.connectionHandler;
    this->isReading=other.isReading;
    this->user=other.user;
    return *this;
}

