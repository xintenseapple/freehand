CMAKE_MINIMUM_REQUIRED(VERSION 3.22)
PROJECT(freehand)

SET(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE(LLVM 14 REQUIRED)
FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options filesystem)

INCLUDE_DIRECTORIES(${LLVM_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

# Main executable
ADD_EXECUTABLE(freehand
        FreeHand.cpp
        MemoryAllocationAnalyzer.cpp
        MemoryAllocationAnalyzer.hpp
        UAF.hpp
        DF.hpp)

TARGET_LINK_LIBRARIES(freehand PUBLIC LLVM)
TARGET_LINK_LIBRARIES(freehand PUBLIC boost_program_options boost_filesystem)
