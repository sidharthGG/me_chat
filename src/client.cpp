#include<iostream>
#include <asio.hpp>
#include <thread>
#include <functional>
#include <string>
#include <memory>
#include <utils.hpp>
#define PREFIX_LENGTH 4 // 4 bytes for length prefix

class TcpClient{
    private:
        int port;
        asio::io_context& client_context;
        asio::ip::tcp::socket client_socket;
    
    public:
        TcpClient(asio::io_context& context, int port):
        port(port),
        client_context(context),
        client_socket(client_context)
        {
            asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"),8080);
            client_socket.connect(endpoint);
        }

        void read()
        {
            std::string message;
            // int fd = client_socket.native_handle();
            // std::cout << "Socket FD: " << fd << "\n"; // -1 means invalid
            if (!this->client_socket.is_open())
            {
                std::cout<<"client socket is not open\n";
                return;
            }

            while (true)
            {
                if (read_message(this->client_socket, message) )
                {
                    std::cout<<"error in read_message, stopping.\n";
                    break;
                }   
                std::cout<<"\nserver: "<<message<<std::endl; 
            }
        }

        void write()
        {
            std::string message;
            while (true)
            {
                std::cout<<"Client: ";
                std::getline(std::cin, message);
                if (send_message(this->client_socket, message))
                {
                    std::cout<<"error in send_message, stopping.\n";
                    break;
                }
            }
        }

        void run ()
        {
            std::thread read_thread(&TcpClient::read, this);
            std::thread write_thread(&TcpClient::write, this);

            read_thread.join();
            write_thread.join();
        }
        
};

int main()
{
    try
    {
        asio::io_context client_context;
        TcpClient my_client(client_context, 8080);

        my_client.run();
    }
    catch (std::exception& e)
    {
        std::cout<<"exception caught: "<<e.what()<<std::endl;
    }
}