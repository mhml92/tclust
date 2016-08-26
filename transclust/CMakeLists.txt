cmake_minimum_required(VERSION 2.8)

# project name
project( TransClust) 

# version
set (CMAKEDEMO_VERSION_MAJOR 1)
set (CMAKEDEMO_VERSION_MINOR 0)



#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

include_directories(
   ${CMAKE_SOURCE_DIR}/include
   ${CMAKE_SOURCE_DIR}/lib/tclap/include
   ${CMAKE_SOURCE_DIR}/lib/plog/include
)
# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src)

