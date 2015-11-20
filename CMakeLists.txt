cmake_minimum_required(VERSION 2.8.12)
project(usig)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pthread")

set(SOURCE_FILES main.cpp)
add_executable(usig ${SOURCE_FILES})
add_executable(ufrp ufrp_example.cpp)
add_executable(ufrp2 ufrp2example.cpp)
add_executable(ufrp3 ufrp3_example.cpp)