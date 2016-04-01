cmake_minimum_required(VERSION 2.8)

# project name
project( DRAFT ) 

# version
set (CMAKEDEMO_VERSION_MAJOR 0)
set (CMAKEDEMO_VERSION_MINOR 1)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -g -O0")

include_directories(
   ${CMAKE_SOURCE_DIR}/include
   ${CMAKE_SOURCE_DIR}/lib/tclap/include
)
# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src)

