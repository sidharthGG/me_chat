#include "utils.hpp"

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

std::vector<std::string> split(std::string message)
{
    std::vector<std::string> parsed_str;
    std::string cur_str = "";

    for(int i=0;i<message.length();i++)
    {
        if (message[i] == ' ')
        {
            parsed_str.push_back(cur_str);
            cur_str = "";
            continue;
        }

        cur_str += message[i];
    }

    if (cur_str != "")
    {
        parsed_str.push_back(cur_str);
    }

    return parsed_str;
}


std::string unify(std::vector<std::string>& msg, int start, int end)
{
    std::string message="";
    for(int i=start;i<end;i++)
    {
        message+=msg[i];
    }

    return message;
}