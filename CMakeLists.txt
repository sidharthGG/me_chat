cmake_minimum_required(VERSION 3.5) # Specifies the minimum required CMake version
project(me_chat LANGUAGES CXX)   # Defines the project name and enables C++ support

add_library(myLib 
    lib/utils.cpp
    lib/session.cpp
)
target_include_directories(myLib PUBLIC lib/include/)

add_executable(server src/server.cpp) # Creates an executable target named 'env_test' from 'env_test.cpp'
add_executable(client src/client.cpp)


find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)# this lib included in env_test code!  
target_link_libraries(client PRIVATE nlohmann_json::nlohmann_json)


find_package(asio CONFIG REQUIRED)
target_link_libraries(server PRIVATE asio::asio)# this lib included in env_test code!  
target_link_libraries(client PRIVATE asio::asio)
target_link_libraries(myLib PRIVATE asio::asio)

target_link_libraries(server PUBLIC myLib)
target_link_libraries(client PUBLIC myLib)


# For threading support
# find_package(Threads REQUIRED)
# target_link_libraries(server Threads::Threads)
# target_link_libraries(server Threads::Threads)