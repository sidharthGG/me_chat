#pragma once

#include<iostream>
#include <asio.hpp>
#include "utils.hpp"

class Session: std::enable_shared_from_this<Session> {
    public:
        std::string username_;
        bool is_logged_in_ = false;
        asio::ip::tcp::socket client_socket_;
    
    
        Session(asio::ip::tcp::socket client_socket):
        client_socket_(std::move(client_socket))
        {
            this-> is_logged_in_ = false;
        }
        
        // std::shared_ptr<Session> create_session(asio::ip::tcp::socket client_socket);
        // void on_read(asio::ip::tcp::socket client_socket);
};