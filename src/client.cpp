#include <iostream>
#include <string>
#include <fstream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return 1;
    }

    const std::string host = argv[1];
    const std::string port = argv[2];

    try
    {
        boost::asio::io_service io_service;
        tcp::socket socket(io_service);
        tcp::resolver resolver(io_service);

        tcp::resolver::query query(host, port);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        boost::asio::connect(socket, endpoint_iterator);

        while (true)
        {
            std::string message;
            std::cout << "Enter a message to send to the server (or 'quit' to exit): ";
            std::getline(std::cin, message);

            if (message == "quit")
            {
                break;
            }

            boost::asio::write(socket, boost::asio::buffer(message + "\n"));
        }

        socket.close();
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
