#include <iostream>
#include "server.hpp"

int main()
{
    try
    {
        std::cout << "It works\n";

        boost::asio::io_context context;
        Server server(context, 9003);

        server.startServer();

        context.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    
    return EXIT_SUCCESS;
}
