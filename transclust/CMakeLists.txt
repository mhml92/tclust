cmake_minimum_required(VERSION 3.5.1)
# project name
project(TransClust) 
#set(CMAKE_BUILD_TYPE Debug)
 
# get header files
include_directories(
   ${CMAKE_SOURCE_DIR}/include
   ${CMAKE_SOURCE_DIR}/lib/tclap/include
   ${CMAKE_SOURCE_DIR}/lib/plog/include
)
message(STATUS "${CMAKE_CXX_COMPILER_ID}") 
message(STATUS "${CMAKE_BUILD_TYPE}") 


list(APPEND BOOST_REQUIRED_LIBRARIES iostreams)
find_package(Boost REQUIRED COMPONENTS ${BOOST_REQUIRED_LIBRARIES})

 
# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)


# Set custom compile flags
IF(${CMAKE_BUILD_TYPE} STREQUAL "Release" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
ELSEIF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall")
ELSE()
	message(FATAL_ERROR "Build type not supported. see CMakeLists.txt.")
ENDIF()


find_package(OpenMP)
if (OPENMP_FOUND)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_subdirectory(src/tc)
add_executable(tclust src/tccli.cpp) 
target_link_libraries(tclust 
	tc_core
)

file(GLOB SOURCES "src/tc/*.cpp")
add_library(transclust SHARED ${SOURCES})
install(TARGETS transclust DESTINATION lib)
install(DIRECTORY include/transclust DESTINATION include FILES_MATCHING PATTERN "*.hpp")
