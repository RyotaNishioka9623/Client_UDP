cmake_minimum_required(VERSION 3.22)
project(client_udp)

set(CMAKE_CXX_STANDARD 20)

add_executable(client_udp src/Client_UDP.cpp)

target_link_libraries(client_udp PRIVATE pthread)