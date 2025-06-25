cmake_minimum_required(VERSION 3.5) # Specifies the minimum required CMake version
project(me_chat LANGUAGES CXX)   # Defines the project name and enables C++ support
add_executable(env_test env_test.cpp) # Creates an executable target named 'env_test' from 'env_test.cpp'


find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(env_test PRIVATE nlohmann_json::nlohmann_json)# this lib included in env_test code!  