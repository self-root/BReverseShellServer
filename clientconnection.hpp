#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <string>
#include <mutex>
#include <boost/array.hpp>
#include <nlohmann/json.hpp>

#include "utils/utility.h"

const int BUFFER_SIZE = 1024 * 1024; 

using json = nlohmann::json;

class Input
{
private:
    std::string currentDir = "> ";
    std::mutex mtx;
public:
    void get()
    {
        std::string in;
        mtx.lock();
        std::cout << currentDir;
        mtx.unlock();
        std::getline(std::cin, in);
        if (!in.empty())
            ready(in);
        get();
    }

    void setCurrentDir(const std::string &cwd_)
    {
        currentDir = cwd_ + "> ";
    }

    boost::signals2::signal<void(std::string)> ready;
};

class ClientConnection : public std::enable_shared_from_this<ClientConnection>
{
private:
    boost::asio::ip::tcp::socket socket;
    boost::signals2::signal<void(std::string)> inputValueReady;
    std::thread inputThread;
    Input input;
    boost::array<uint8_t, BUFFER_SIZE> bufferData;
    std::vector<uint8_t> tempData;  

private:
    void handleDataReading(const boost::system::error_code &err, std::size_t receivedBytes);
    void parse();

    /**
     * @brief Write received data into the disk
     * 
     * @param data 
     * @param filename 
     */
    void writeData(const nlohmann::byte_container_with_subtype<std::vector<uint8_t>> &data,
    const std::string &filename);

    /**
     * @brief Function called when the user entered a command
     * 
     * @param cwd_ reference to the value of the current working directory 
     */
    void onInputValue(std::string value);

    /**
     * @brief Send those command to the server
     * 
     * @param res_ 
     */
    void sendCommand(const json &res_);
    void readJson(const json &j_);
public:
    ClientConnection(boost::asio::ip::tcp::socket&& socket_);
    ~ClientConnection();
    void startReceive();
};


#endif