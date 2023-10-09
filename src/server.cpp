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

struct Config {
    int port;
    std::string prefix;
    std::size_t max_file_size;
};

void usage(const char *name)
{
    std::cout << "usage: " << name << " <config_file_path>" << std::endl;
    exit(1);
}

Config readConfig(const std::string& configFilePath) {
    Config config;

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(configFilePath, pt);

    config.port = pt.get("port", DEFAULT_PORT);
    config.prefix = pt.get("prefix", DEFAULT_PREFIX);
    config.max_file_size = pt.get("max_file_size", DEFAULT_MAX_FILE_SIZE);

    return config;
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

void handleData(tcp::socket& socket, const std::string& prefix, std::size_t max_file_size, boost::asio::io_service& io)
{
    std::size_t current_file_size = 0;
    int file_index = 0;
    std::string filename;
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    filename = createFileWithIncrement(prefix, file_index, timestamp);

    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(CLIENT_TIMEOUT));
    bool timer_expired = false;

    std::function<void(const boost::system::error_code&)> onTimerExpired = [&](const boost::system::error_code& error)
    {
        if (!error)
        {
            std::cout << "Client timed out." << std::endl;
            socket.close();
            timer_expired = true;
        }
    };

    while (true)
    {
        timer.expires_from_now(boost::posix_time::seconds(CLIENT_TIMEOUT));
        timer.async_wait(onTimerExpired);

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

        if (timer_expired)
        {
            std::cout << "Timeout occurred during processing. Closing connection." << std::endl;
            break;
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

void handleClient(tcp::socket& socket, const std::string& prefix, std::size_t max_file_size, boost::asio::io_service& io)
{
    try
    {
        std::cout << "Client connected!" << std::endl;

        handleData(socket, prefix, max_file_size, io);
    }
    catch (std::exception& e)
    {
        std::cerr << "Error " << e.what() << std::endl;
    }
}

void startServer(const Config& config) {
    boost::asio::io_service io;
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), config.port));

    std::cout << "Server started. Listening on port " << config.port << "..." << std::endl;

    while (true)
    {
        tcp::socket socket(io);
        acceptor.accept(socket);
        handleClient(socket, config.prefix, config.max_file_size, io);
    }
}

int main(int argc, char* argv[])
{
    int status_code = 0;
    try
    {
        if (argc < 2)
        {
            usage(argv[0]);
        }

        Config config = readConfig(argv[1]);

        std::cout << "Settings:" << std::endl;
        std::cout << "Port: " << config.port << std::endl;
        std::cout << "Prefix: " << config.prefix << std::endl;
        std::cout << "Max size: " << config.max_file_size << " bytes" << std::endl;

        startServer(config);
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        status_code = 1;
    }
    return status_code;
}
