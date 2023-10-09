#include <iostream>
#include <string>
#include <fstream>
#include <boost/asio.hpp>
#include <cstdlib>
#include <thread>

using boost::asio::ip::tcp;

bool parseCommandLine(int argc, char* argv[], std::string& host, std::string& port, bool& isDebug) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port> [debug]" << std::endl;
        return false;
    }

    host = argv[1];
    port = argv[2];
    isDebug = (argc == 4 && std::string(argv[3]) == "debug");
    return true;
}

void runClient(const std::string& host, const std::string& port, bool isDebug) {
    try {
        boost::asio::io_service io_service;
        tcp::socket socket(io_service);
        tcp::resolver resolver(io_service);

        tcp::resolver::query query(host, port);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        boost::asio::connect(socket, endpoint_iterator);

        if (isDebug) {
            std::cout << "Debug Mode!" << std::endl;
            while (true) {
                std::string message = "Debug Message: " + std::to_string(std::rand());
                std::cout << "Sending a debug message: " << message << std::endl;
                boost::asio::write(socket, boost::asio::buffer(message + "\n"));

                ::sleep(1);
            }
        } else {
            while (true) {
                std::string message;
                std::cout << "Enter a message to send to the server (or 'quit' to exit): ";
                std::getline(std::cin, message);

                if (message == "quit") {
                    break;
                }

                boost::asio::write(socket, boost::asio::buffer(message + "\n"));
            }
        }

        socket.close();
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::string host;
    std::string port;
    bool isDebug;

    if (!parseCommandLine(argc, argv, host, port, isDebug)) {
        return 1;
    }

    runClient(host, port, isDebug);

    return 0;
}
