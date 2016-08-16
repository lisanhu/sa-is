cmake_minimum_required(VERSION 3.6)
project(sa_is)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp sais.cc)
add_executable(sa_is ${SOURCE_FILES})