cmake_minimum_required(VERSION 3.3)
project(usig)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pthread")

set(SOURCE_FILES main.cpp)
add_executable(usig ${SOURCE_FILES})