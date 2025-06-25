#include<iostream>
#include <nlohmann/json.hpp>

int main()
{
    std::cout << nlohmann::json::parse("{ \"msg\": \"Hello, World!\" }") << std::endl;
}