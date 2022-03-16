#include "server.hpp"

Server::Server(boost::asio::io_context &context_, int port)
: context(context_),
  acceptor(context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
}

void Server::startServer()
{
    socket.emplace(context);

    acceptor.async_accept(*socket, [&](boost::system::error_code){
        std::cout << "New connection..." << socket->remote_endpoint().address().to_string() << std::endl;
        std::make_shared<ClientConnection>(std::move(*socket))->startReceive();
    });
}

Server::~Server()
{
}