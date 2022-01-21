cmake_minimum_required(VERSION 3.10)
project(smkhe)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES src/initial.cpp)

add_library(smkhe SHARED ${SOURCE_FILES})

target_include_directories(smkhe PRIVATE ./include)
