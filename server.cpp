#include<iostream>
#include <asio.hpp>
#include <thread>
#include <functional>
#include <string>
#include <memory>
#include <arpa/inet.h>
#include <thread>
#define PREFIX_LENGTH 4 // 4 bytes for length prefix

class TcpServer
{
    private:
        int server_port;
        /*have to use lvalue ref in context, since not copyassignable or copyconstructible
          and we do not use move operators since dont want to move io_context resources. 
        */
        asio::io_context& server_context;  
        asio::ip::tcp::acceptor server_socket;
    public:
        TcpServer(asio::io_context& context, int port) : 
        server_port(port),
        server_context(context),
        server_socket(server_context)
        {
            server_socket.open(asio::ip::tcp::v4());
            server_socket.set_option(asio::ip::tcp::acceptor::reuse_address(true));
            asio::ip::tcp::endpoint server_endpoint(asio::ip::address::from_string("127.0.0.1"),server_port);
            server_socket.bind(server_endpoint);
        }

        void run()
        {
            this->server_socket.listen();

            while (true) {
                asio::ip::tcp::socket client_socket(this->server_context);
                this->server_socket.accept(client_socket);
                std::cout<<"client connected from "<<client_socket.remote_endpoint()<<std::endl;

                // handle client, send an example message
                // handle_client2(std::move(client_socket));
                std::thread client_thread(&TcpServer::handle_client2,this, std::move(client_socket));
                client_thread.detach();
            }
        }

        void read(asio::ip::tcp::socket& client_socket)
        {
            std::string message;
            // int fd = client_socket.native_handle();
            // std::cout << "Socket FD: " << fd << "\n"; // -1 means invalid
            if (!client_socket.is_open())
            {
                std::cout<<"client socket is not open\n";
                return;
            }

            while (true)
            {
                if (this->read_message(client_socket, message) )
                {
                    std::cout<<"error in read_message, stopping.\n";
                    break;
                }   
                std::cout<<"\nclient: "<<message<<std::endl; 
            }
        }

        void write(asio::ip::tcp::socket& client_socket)
        {
            std::string message;
            while (true)
            {
                std::cout<<"Server:";
                std::getline(std::cin, message);
                if (this->send_message(client_socket, message))
                {
                    std::cout<<"error in send_message, stopping.\n";
                    break;
                }
            }
        }
        void handle_client2(asio::ip::tcp::socket client_socket)
        {
            std::thread read_thread(&TcpServer::read, this, std::ref(client_socket));

            std::thread write_thread(&TcpServer::write, this, std::ref(client_socket));
            read_thread.join();
            write_thread.join();
        }

        std::error_code read_message(asio::ip::tcp::socket& client_socket, std::string& message)
        {
           
            std::error_code r_ec;
            std::vector<char> buffer(PREFIX_LENGTH);
            size_t bytes_read = asio::read(client_socket, asio::buffer(buffer, buffer.size()), r_ec);

            if (r_ec)
            {
                std::cout<<"read error: "<<r_ec.message()<<std::endl;
                return r_ec;
            }

            // std::cout<<"bytes_read: "<<bytes_read<<std::endl;

            uint32_t net_len = 0;
            memcpy(
                &net_len,
                buffer.data(),
                PREFIX_LENGTH
            );

            uint32_t buffer_len = ntohl(net_len);

            message.clear();
            message.resize(buffer_len);
          
            bytes_read = asio::read(client_socket, asio::buffer(message, message.length()), r_ec);

            if(r_ec)
            {
                std::cout<<"read error: "<<r_ec.message()<<std::endl;
                return r_ec;
            }
            
            return r_ec;
        }

        std::error_code send_message(asio::ip::tcp::socket& client_socket, std::string message)
        {
            std::error_code w_ec;
            uint32_t message_len = message.length();
            uint32_t net_len = htonl(message_len);//handle the endian-ness, convert to big-endian
            std::vector<char> prefix_buffer(PREFIX_LENGTH);
            memcpy(
                prefix_buffer.data(),
                &net_len,
                PREFIX_LENGTH
            );
            asio::write(client_socket, asio::buffer(prefix_buffer), w_ec);

            if (w_ec)
            {
                std::cout<<"write error: "<<w_ec.message()<<std::endl;
                return w_ec;
            }

            asio::write(client_socket, asio::buffer(message), w_ec);

            if (w_ec)
            {
                std::cout<<"write error: "<<w_ec.message()<<std::endl;
                return w_ec;
            }

            return w_ec;
        }

};


int main()
{
    try
    {
        asio::io_context server_context;
        TcpServer server(std::ref(server_context), 8080);
        server.run();
    }
    catch (std::exception& e)
    {
        std::cout<<"Exception Caught: "<<e.what()<<std::endl;
    }
}
