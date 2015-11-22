cmake_minimum_required(VERSION 2.8.12)
project(usig)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pthread")

set(SOURCE_FILES main.cpp)
add_executable(usig ${SOURCE_FILES})

include_directories(${CMAKE_SOURCE_DIR})

add_executable(ufrp ufrp/ufrp_example.cpp)
add_executable(ufrp2 ufrp/ufrp2example.cpp)
add_executable(ufrp3 ufrp/ufrp3_example.cpp)
add_executable(ufrp4 ufrp/ufrp4_example.cpp)