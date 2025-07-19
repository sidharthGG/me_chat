#pragma once

#include<iostream>
#include <asio.hpp>
#include <map>
#define PREFIX_LENGTH 4

std::error_code read_message(asio::ip::tcp::socket& client_socket, std::string& message);
std::error_code send_message(asio::ip::tcp::socket& client_socket, std::string message);
std::vector<std::string> split(std::string message);
std::string unify(std::vector<std::string>& msg, int start, int end);