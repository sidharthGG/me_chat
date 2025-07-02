#include<iostream>
#include <nlohmann/json.hpp>
#include <asio.hpp>
#include <thread>
#include <string>
#include <memory>

//client side 'echo server' code!!

int main()
{
    asio::io_context io_context_;
    asio::ip::tcp::socket client(io_context_);

    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"),8080);
    client.connect(endpoint);


    asio::streambuf buffer;
    asio::read_until(client, buffer, '\n');

    std::istream is(&buffer);
    std::string message;
    std::getline(is, message);

    std::cout<<"message from server :"<<message<<"\n";
    return 0;
}