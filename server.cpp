#include<iostream>
#include <nlohmann/json.hpp>
#include <asio.hpp>
#include <thread>
#include <functional>
#include <string>
#include <memory>


void handle_client(asio::ip::tcp::socket client)
{
    std::cout<<"Any message for client... ?\n";
    
    std::string message;
    std::getline(std::cin, message);
    message = message + '\n'; //getline does not incluce '\n' , we need because client reads until '\n'
    asio::write(client, asio::buffer(message));
}

int main()
{
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor(io_context_, asio::ip::tcp::v4());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));

    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"),8080);
    acceptor.bind(endpoint);

    acceptor.listen();

    while (true) {
        asio::ip::tcp::socket client_socket(io_context_);
        acceptor.accept(client_socket);
        std::cout<<"client connected from "<<client_socket.remote_endpoint()<<std::endl;

        // handle client, send an example message
       handle_client(std::move(client_socket));
    }
    return 0;
}