//
// Created by adi on 12/01/2020.
//

#include <ServerThread.h>
#include <EncoderDecoder.h>
#include<stdio.h>

ServerThread::ServerThread(User& user, ConnectionHandler& connectionHandler, std::timed_mutex& mutex) : user(&user),connectionHandler(&connectionHandler), _mutex(mutex) {
}

void ServerThread::run() {
    EncoderDecoder encedec;
    Frame* msg;
    while(connectionHandler->getIsConnected())
    {
        string answer="";
        if (!connectionHandler->getFrameAscii(answer,'\0')) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }

        msg=encedec.Decode(answer);
        handleMessage(*msg);
        delete msg;
    }

}

void ServerThread::handleMessage(Frame &msg) {
    if(msg.getHeaders().find("destination")!=msg.getHeaders().end())
        cout<<msg.getHeaders()["destination"]<<" : "<<msg.getBody()<<endl;


    if(msg.getCommand().compare("CONNECTED")==0)
    {
        cout<< "Login successfully "<<endl;
        user->setLoggedIn(true);
    }
    else if((msg.getCommand().compare("ERROR")==0) | (msg.getCommand().compare("Error")==0))
    {
        handleERROR(msg);
    }
    else if((msg.getCommand().compare("MESSAGE")==0) | (msg.getCommand().compare("Message")==0))
    {
        handleMESSAGE(msg);
    }
    else if((msg.getCommand().compare("RECEIPT")==0) | (msg.getCommand().compare("Receipt")==0))
    {
        handleRECIEPT(msg);
    }
}

void ServerThread::handleMESSAGE(Frame& msg) {
    string body=msg.getBody().substr(0,msg.getBody().find('\n'));
    string first=body.substr(0,body.find(' '));// the first word of the body
    if((body.compare("Book status")==0) | (body.compare("book status")==0))
    {

        string genre=msg.getHeaders()["destination"];
        if(user->getBooksByGenre(genre).size() != 0) //only if there are books
        {
            string body=user->getBooksByGenre(genre);
            string cmd="SEND";
            map<string ,string > headers;
            headers.insert(std::pair<string ,string >("destination",genre));
            Frame statusFrame(cmd, (headers), body);
            sendMessage(statusFrame);
        }
    }
    else if(first.compare("Returning")==0)
    {
        body.erase(0,body.find(' ')+1);// erase the 'Returning'
        string bookName=body.substr(0,body.find("to"));//bookName
        body.erase(0,body.find("to")+1);// erase the bookName
        body.erase(0,body.find(' ')+1);// erase the 'to'
        string owner=body;
        if(owner.compare(user->getName())==0)
        {
            user->addBook(msg.getHeaders()["destination"],bookName);
        }
    }
    else if(first.compare("Taking")==0)
    {
        body.erase(0,body.find(' '));// erase the 'Taking'
        string bookName=body.substr(0,body.find("from"));//bookName
        body.erase(0,body.find("from"));// erase the bookName
        body.erase(0,body.find(' '));// erase the 'from'
        string owner=body;
        if(owner.find(user->getName())!=string::npos)
        {
            user->removeBook(msg.getHeaders()["destination"],bookName);
        }
    }
    else if(first.compare(user->getName())!=0)
    {
        body.erase(0,body.find(' ')+1);// erase the first word (represents a username)
        string second=body.substr(0,body.find(' '));//the second word
        if(second.compare("wish")==0)//some user wants to borrow a book
        {
            body.erase(0,body.find(' ')+1);// erase the 'wish'
            body.erase(0,body.find(' ')+1);// erase the 'to'
            body.erase(0,body.find(' ')+1);// erase the 'borrow'
            string bookName=body;
            if(user->hasBook(msg.getHeaders()["destination"],bookName))
            {
                //user->removeBook(msg.getHeaders()["destination"],bookName);
                string cmd="SEND";
                map<string ,string > headers;
                headers.insert(std::pair<string ,string >("destination",msg.getHeaders()["destination"]));
                string hasBookBody=user->getName()+" has "+bookName;
                Frame hasBookFrame(cmd, (headers), hasBookBody);
                sendMessage(hasBookFrame);
            }
        }
        else if(second.compare("has")==0)//some user has book that other user wants to borrow
        {
            body.erase(0,body.find(' ')+1);// erase the 'has'
            string bookName=body;
            if(user->inWishlist(bookName))
            {
                user->borrowBook(msg.getHeaders()["destination"],bookName,first);
                string cmd="SEND";
                map<string ,string > headers;
                headers.insert(std::pair<string ,string >("destination",msg.getHeaders()["destination"]));
                string takingBookBody="Taking "+bookName+" from "+first;
                Frame takingBookFrame(cmd, (headers), takingBookBody);
                sendMessage(takingBookFrame);
            }
        }
    }

}

void ServerThread::sendMessage(Frame& msgToSend) {
    EncoderDecoder encoderDecoder;
    string bytes = encoderDecoder.Encode(msgToSend);
    if (!connectionHandler->sendLine(bytes)) {
        std::cout << "Disconnected. Exiting...\n" << std::endl;
    }

}

void ServerThread::handleERROR(Frame &msg) {
    if(msg.getBody().find("User already logged in")!=string::npos)
        cout<< "User already logged in"<<endl;
    if(msg.getBody().find("Wrong password")!=string::npos)
        cout<< "Wrong password"<<endl;
    connectionHandler->close();
}

void ServerThread::handleRECIEPT(Frame &msg) {
    string receipt_id=msg.getHeaders()["receipt-id"];
    int x=stoi(receipt_id);
    string receipt=user->getReceipt(x);
    if(receipt.compare("logout")!=0)
    {
        cout<< receipt<<endl;
    }
    //in case receipt is "logout" it means that the client sent DISCONNECT frame to the server
    else{
        connectionHandler->close();
    }
}

ServerThread::ServerThread(ServerThread const &other): user(other.user),connectionHandler(other.connectionHandler), _mutex(other._mutex) {
}

ServerThread &ServerThread::operator=(ServerThread const &other) {
    delete this->user;
    delete this->connectionHandler;
    this->connectionHandler=other.connectionHandler;
    this->user=other.user;
    return *this;
}

