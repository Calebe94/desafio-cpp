#include <iostream>
#include <string>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define DEFAULT_PORT          12345
#define DEFAULT_PREFIX        "data"
#define DEFAULT_MAX_FILE_SIZE 1024

using boost::asio::ip::tcp;

void usage(const char *name)
{
    std::cout << "usage: " << name << " <config_file_path>" << std::endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    int status_code = 0;
    try
    {
        boost::property_tree::ptree config;
        if (argc < 2)
        {
            usage(argv[0]);
        }

        boost::property_tree::read_json(argv[1], config);

        int port = config.get<int>("port", DEFAULT_PORT);
        std::string prefix = config.get<std::string>("prefix", DEFAULT_PREFIX);
        std::size_t max_file_size = config.get<std::size_t>("max_file_size", DEFAULT_MAX_FILE_SIZE);
        std::cout << "Configurações:" << std::endl;
        std::cout << "Porta: " << port << std::endl;
        std::cout << "Prefix: " << prefix << std::endl;
        std::cout << "tamanho máximo: " << max_file_size << std::endl;

        boost::asio::io_service io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), port));

        std::cout << "Server started. Listening on port " << port << "..." << std::endl;

        while(true)
        {
            tcp::socket socket(io);
            acceptor.accept(socket);
            std::cout << "Connection!" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Erro: " << e.what() << std::endl;
        status_code = 1;
    }
    return status_code;
}
