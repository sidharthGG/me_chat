#include<iostream>
#include <asio.hpp>
#include "session.hpp"
#include "utils.hpp"


// std::shared_ptr<Session> Session::create_session(asio::ip::tcp::socket client_socket)
// {
//     //create session
//     //NOTE: pre-init object, username is not initialised yet
//     auto new_session = std::make_shared<Session>(std::move(client_socket), "");

//     return new_session;
// }

