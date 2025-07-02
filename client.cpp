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

    //continue the chat with server in while loop
    while (true)
    {
        asio::streambuf buffer;
        asio::read_until(client, buffer, '\n');

        std::istream is(&buffer);
        std::string message;
        std::getline(is, message);
        std::cout<<"server : "<<message<<"\n";

        std::cout<<"client: ";
        std::string send_message;
        std::getline(std::cin, send_message);
        send_message = send_message + '\n'; //for reading on other side
        asio::write(client, asio::buffer(send_message));
    }
    return 0;
}