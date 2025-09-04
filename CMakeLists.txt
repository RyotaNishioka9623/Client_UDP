cmake_minimum_required(VERSION 3.22)
project(client_udp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_library(WIRINGPI_LIB wiringPi REQUIRED)

add_executable(client_udp
  src/Client_UDP.cpp
)

target_link_libraries(client_udp
  PRIVATE
    ${OpenCV_LIBS}
    ${WIRINGPI_LIB}
)
