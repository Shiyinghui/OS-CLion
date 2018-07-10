cmake_minimum_required(VERSION 3.10)
project(OS)

set(CMAKE_CXX_STANDARD 11)

add_executable(OS main.cpp scanner.h path.h disk.h item.cpp file.cpp block.cpp)