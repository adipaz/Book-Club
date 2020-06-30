#include "../include/connectionHandler.h"
#include <thread>
#include <mutex>

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
 
ConnectionHandler::ConnectionHandler(string host, short port, std::timed_mutex& mutex): host_(host), port_(port), io_service_(), socket_(io_service_), _mutex(mutex), isConnected(false){

}
    
ConnectionHandler::~ConnectionHandler() {
    //close();
}
 
bool ConnectionHandler::connect() {
    std::cout << "Starting connect to "
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
		this->isConnected=true;
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        _mutex.try_lock_for(std::chrono::milliseconds(10));
        while (!error && bytesToRead > tmp ) {
            if(socket_.is_open())
                tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);
        }
		if(error) {
            throw boost::system::system_error(error);
        }
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {

    int tmp = 0;
	boost::system::error_code error;
    try {
        _mutex.try_lock_for(std::chrono::milliseconds(10));
        while (!error && bytesToWrite > tmp ) {
            if(socket_.is_open())
                tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error) {
            throw boost::system::system_error(error);
        }
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\0');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, '\0');
}
 

bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
	do{
		if(!getBytes(&ch, 1))
		{
			return false;
		}
		if(ch!='\0')  
			frame.append(1, ch);
	}while (delimiter != ch);
    } catch (std::exception& e) {
	std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
	return false;
    }
    return true;
}
 
 
bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
	bool result=sendBytes(frame.c_str(),frame.length());
	if(!result) return false;
	return sendBytes(&delimiter,1);
}
 
// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
        this->isConnected.exchange(false);

    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}

bool ConnectionHandler::getIsConnected() {
    return this->isConnected.load() & socket_.is_open();
}
