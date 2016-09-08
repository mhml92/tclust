#cmake_minimum_required(VERSION 2.8)
#
## project name
#project(TransClust) 
#
## version
#set (CMAKEDEMO_VERSION_MAJOR 1)
#set (CMAKEDEMO_VERSION_MINOR 0)
#
#
#
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -g -O0")
##set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
#
#include_directories(
#   ${CMAKE_SOURCE_DIR}/include
#   ${CMAKE_SOURCE_DIR}/lib/tclap/include
#   ${CMAKE_SOURCE_DIR}/lib/plog/include
#)
## Set the output folder where your program will be created
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
#
#add_subdirectory(src)
#
################################################################################
cmake_minimum_required(VERSION 2.8.9)
# project name
project(TransClust) 
set(CMAKE_BUILD_TYPE Release)
 
#Bring the headers, such as Student.h into the project
include_directories(
   ${CMAKE_SOURCE_DIR}/include
   ${CMAKE_SOURCE_DIR}/lib/tclap/include
   ${CMAKE_SOURCE_DIR}/lib/plog/include
)
 
# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -g -O0")

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")
 
#Generate the static library from the sources
add_executable(transclust ${SOURCES}) 

list(REMOVE_ITEM SOURCES "src/program.cpp")
add_library(TransClust SHARED ${SOURCES})
 
#Set the location for library installation -- i.e., /usr/lib in this case
# not really necessary in this example. Use "sudo make install" to apply
#install(TARGETS testStudent DESTINATION /usr/lib)
