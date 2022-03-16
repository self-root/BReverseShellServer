#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <iostream>

#include "clientconnection.hpp"

class Server
{
private:
    boost::asio::io_context &context;
    boost::asio::ip::tcp::acceptor acceptor;
    std::optional<boost::asio::ip::tcp::socket> socket;
public:
    Server(boost::asio::io_context &context_, int port);
    ~Server();
    void startServer();
};


#endif //SERVER_HPP