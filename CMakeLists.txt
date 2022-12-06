cmake_minimum_required(VERSION 3.23)
project(freehand)

set(CMAKE_CXX_STANDARD 20)

find_package(LLVM REQUIRED)
find_package(Boost REQUIRED)

include_directories(.)

add_executable(freehand
        FreeHand.cpp
        FreeHand.h)

target_link_libraries(freehand LLVM)
