
#include<iostream>
#include <asio.hpp>
#include <thread>
#include <functional>
#include <string>
#include <memory>
#include <arpa/inet.h>
#include <thread>
#include "utils.hpp"
#include "session.hpp"

class TcpServer
{
    private:
        int server_port;
        /*have to use lvalue ref in context, since not copyassignable or copyconstructible
          and we do not use move operators since dont want to move io_context resources. 
        */
        asio::io_context& server_context;  
        asio::ip::tcp::acceptor server_socket;
        std::mutex active_clients_mtx;
        std::map<std::string, std::shared_ptr<Session>> active_clients_;
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
                
                //make session and pass to handle_client
                auto new_session = create_session(std::move(client_socket));
                std::cout<<"new session created\n";
                std::thread client_thread(&TcpServer::handle_client,this, new_session);
                client_thread.detach();
            }
        }

        void read(asio::ip::tcp::socket& client_socket)
        {
            std::string message;

            if (!client_socket.is_open())
            {
                std::cout<<"client socket is not open\n";
                return;
            }

            while (true)
            {
                if (read_message(client_socket, message) )
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
                if (send_message(client_socket, message))
                {
                    std::cout<<"error in send_message, stopping.\n";
                    break;
                }
            }
        }

        void handle_login(std::shared_ptr<Session> new_session)
        {
            if (new_session == NULL)
            {
                std::cout<<"new_session is null\n";
                return;
            }

            if (!(new_session -> client_socket_.is_open()))
            {
                std::cout<<"client socket is closed\n";
                return;
            }

            //take lock for active_clients_
            std::lock_guard<std::mutex> lock(active_clients_mtx);

            std::string message;
            //indefinitely ask for LOGIN
            while (!(new_session->is_logged_in_))
            {
                std::string login_msg;
                send_message(new_session->client_socket_, "To Login, try: LOGIN <username>\n");
                read_message(new_session->client_socket_, message);//blocking 
                std::vector<std::string> parsed_str = split(message);
                if (parsed_str.size() == 2 and parsed_str[0] == "LOGIN")
                {
                    if (this-> active_clients_.find(parsed_str[1]) != active_clients_.end())
                    {
                        send_message(new_session->client_socket_, "\nusername already taken, try something else\n");
                        continue;
                    }

                    //register user
                    new_session-> username_ = parsed_str[1];
                    new_session-> is_logged_in_ = true;
                    this-> active_clients_[new_session->username_] = new_session;
                    send_message(new_session->client_socket_, "Successfully logged in, welcome!");
                    send_message(new_session->client_socket_, "For Unicast, use: MSG <username> <message>\n");
                    send_message(new_session->client_socket_, "For Broadcase, use: ALL <message>");
                }
                else
                {
                    send_message(new_session->client_socket_, "Login attempt unsuccessful, try again!\n");
                }
            }
        }

        void handle_chat(std::shared_ptr<Session> this_session)
        {
            while (true)
            {
                std::string message;
                read_message(this_session->client_socket_, message);

                std::vector<std::string> parsed_msg = split(message);

                std::string msg_type = parsed_msg[0];
                if (msg_type == "MSG")
                {
                    if (parsed_msg.size() < 3)
                    {
                        send_message(this_session->client_socket_, "3 args required for unicast, MSG <username> <msg>\n");
                        continue;
                    }
                    std::string username = parsed_msg[1];
                    std::string msg = unify(parsed_msg, 2, parsed_msg.size());
                    
                    unicast(username, msg);
                }
                else if(msg_type == "ALL")
                {
                    if (parsed_msg.size() < 2)
                    {
                        send_message(this_session->client_socket_, "2 or more args required for broadcase message\n");
                        continue;
                    }

                    std::string b_msg = unify(parsed_msg, 1, parsed_msg.size());
                    broadcast(b_msg);
                }
                else
                {
                    send_message(this_session->client_socket_, "unknown message operation type\n");
                }
            }    
        }

        uint8_t unicast(std::string recipient, std::string message)
        {
            if (recipient == "")
            {
                std::cout<<"Invalid username\n";
                return 1;
            }

            if (message == "")
            {
                std::cout<<"empty message\n";
                return 2;
            }

            std::lock_guard<std::mutex> lock(active_clients_mtx);

            if (this->active_clients_.find(recipient) == this->active_clients_.end())
            {
                std::cout<<"No such username as "<<recipient<<std::endl;
                return 3;
            }

            if (send_message(this->active_clients_[recipient]->client_socket_, message))
            {
                std::cout<<"some error while sending to "<<recipient<<std::endl;
                return 4;
            }

            return 0; 
        }

        uint8_t broadcast(std::string message)
        {
            if (message == "")
            {
                std::cout<<"empty message\n";
                return 2;
            }

            std::lock_guard<std::mutex> lock(active_clients_mtx);

            for(auto it = this->active_clients_.begin();it!=this->active_clients_.end();it++)
            {
                std::string recipient = it->first;
                if (send_message(this->active_clients_[recipient]->client_socket_, message))
                {
                    std::cout<<"some error while sending to "<<recipient<<std::endl;
                    return 4;
                }
            }

            return 0;
        }

        std::shared_ptr<Session> create_session(asio::ip::tcp::socket client_socket)
        {
            //create session
            //NOTE: pre-init object, username is not initialised yet
            auto new_session = std::make_shared<Session>(std::move(client_socket));
            return new_session;
        }

        void handle_client(std::shared_ptr<Session> this_session)
        {
            
            if (this_session == NULL)
            {
                std::cout<<"client session null, aborting.\n";
                return;
            }
            
            if (!(this_session -> client_socket_.is_open()))
            {
                std::cout<<"client socket not open, exiting.\n";
                return;
            }

            handle_login(this_session);
            
            handle_chat(this_session);
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
