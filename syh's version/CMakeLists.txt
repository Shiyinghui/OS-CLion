cmake_minimum_required(VERSION 3.10)
project(OSV4)

set(CMAKE_CXX_STANDARD 11)

add_executable(OSV4 main.cpp dir.h virtualDisk.h dirPath.h command.h doItem.cpp doBlock.cpp doFile.cpp function.cpp function.h)