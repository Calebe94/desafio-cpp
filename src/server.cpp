#include <iostream>
#include <string>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <chrono>

#define DEFAULT_PORT          12345
#define DEFAULT_PREFIX        "data"
#define DEFAULT_MAX_FILE_SIZE 1024
#define CLIENT_TIMEOUT        10

using boost::asio::ip::tcp;

void usage(const char *name)
{
    std::cout << "usage: " << name << " <config_file_path>" << std::endl;
    exit(1);
}

std::string createFileWithIncrement(const std::string& prefix, int& file_index, const std::time_t& timestamp)
{
    std::string filename = prefix + "_" + std::to_string(timestamp);
    if (file_index > 0)
    {
        filename += "-" + std::to_string(file_index);
    }
    filename += ".txt";
    file_index++;
    return filename;
}

void handleData(tcp::socket& socket, const std::string& prefix, std::size_t max_file_size)
{
    std::size_t current_file_size = 0;
    int file_index = 0;
    std::string filename;
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    filename = createFileWithIncrement(prefix, file_index, timestamp);
    while (true)
    {
        char data[1024];
        boost::system::error_code error;
        std::size_t bytes_received = socket.read_some(boost::asio::buffer(data, max_file_size), error);

        if (error == boost::asio::error::eof)
        {
            std::cout << "Client disconnected." << std::endl;
            break;
        }
        else if (error)
        {
            throw boost::system::system_error(error);
        }

        if (current_file_size + bytes_received > max_file_size)
        {
            std::cout << "Max size reached. Creating new file." << std::endl;
            current_file_size = 0;
            filename = createFileWithIncrement(prefix, file_index, timestamp);
        }

        std::ofstream file(filename, std::ios_base::app);
        if (!file.is_open())
        {
            std::cerr << "Error opening the file " << filename << std::endl;
            return;
        }
        file.write(data, bytes_received);
        current_file_size += bytes_received;
        file.close();
    }
}

void handle_client(tcp::socket& socket, const std::string& prefix, std::size_t max_file_size)
{
    try
    {
        std::cout << "Client connected!" << std::endl;

        boost::asio::deadline_timer timer(socket.get_executor());

        handleData(socket, prefix, max_file_size);
    }
    catch (std::exception& e)
    {
        std::cerr << "Error " << e.what() << std::endl;
    }
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
        std::cout << "Tamanho máximo: " << max_file_size << " bytes" << std::endl;

        boost::asio::io_service io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), port));

        std::cout << "Server started. Listening on port " << port << "..." << std::endl;

        while(true)
        {
            tcp::socket socket(io);
            acceptor.accept(socket);
            handle_client(socket, prefix, max_file_size);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Erro: " << e.what() << std::endl;
        status_code = 1;
    }
    return status_code;
}
