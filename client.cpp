#include<iostream>
#include <asio.hpp>
#include <thread>
#include <functional>
#include <string>
#include <memory>
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

        void run()
        {
            while (true)
            {
                //write part
                std::cout<<"client: ";
                std::string message;
                std::getline(std::cin, message);
                if (this->send_message(message))
                {
                    std::cout<<"error in send_message, stopped.\n";
                    break;
                }
                
                //read part
                message.clear();
                if (this->read_message(std::ref(message)))
                {
                    std::cout<<"error in read_message, stopped.\n";
                    break;
                }

                std::cout<<"server: "<<message<<std::endl;                
            }          
        }

        std::error_code send_message(std::string message)
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

        std::error_code read_message(std::string& message)
        {
        
            std::error_code r_ec;
            std::vector<char> buffer(PREFIX_LENGTH);
            size_t bytes_read = asio::read(client_socket, asio::buffer(buffer, buffer.size()), r_ec);

            if (r_ec)
            {
                std::cout<<"read error: "<<r_ec.message()<<std::endl;
                return r_ec;
            }

            //std::cout<<"bytes_read: "<<bytes_read<<std::endl;

            uint32_t net_len = 0;
            memcpy(
                &net_len,
                buffer.data(),
                PREFIX_LENGTH
            );

            uint32_t buffer_len = ntohl(net_len);
            message.resize(buffer_len);
           
            bytes_read = asio::read(client_socket, asio::buffer(message, message.length()), r_ec);

            if(r_ec)
            {
                std::cout<<"read error: "<<r_ec.message()<<std::endl;
                return r_ec;
            }
            
            return r_ec;
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