cmake_minimum_required(VERSION 3.5.1)

project(TransClust) 


# disallow in-source builds
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
	message(SEND_ERROR "In-source builds are not allowed.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")


# required boost libraries
list(APPEND BOOST_REQUIRED_LIBRARIES iostreams)
list(APPEND BOOST_REQUIRED_LIBRARIES system)
list(APPEND BOOST_REQUIRED_LIBRARIES filesystem)
list(APPEND BOOST_REQUIRED_LIBRARIES mpi)
list(APPEND BOOST_REQUIRED_LIBRARIES serialization)
find_package(Boost COMPONENTS ${BOOST_REQUIRED_LIBRARIES} REQUIRED)
find_package(MPI REQUIRED)


# add stxxl
add_subdirectory(stxxl)

# get header files
include_directories(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/lib/tclap/include
	${CMAKE_SOURCE_DIR}/lib/plog/include
	${Boost_INCLUDE_DIR}
	${STXXL_INCLUDE_DIRS}
	${MPI_INCLUDE_PATH}
	)

message(STATUS "${CMAKE_CXX_COMPILER_ID}") 
message(STATUS "${CMAKE_BUILD_TYPE}") 
message(STATUS "${Boost_INCLUDE_DIR}") 

# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Set custom compile flags
IF(${CMAKE_BUILD_TYPE} STREQUAL "Release" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")
ELSEIF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -pedantic -W -Wall")
ELSE()
	message(FATAL_ERROR "Build type not supported. see CMakeLists.txt.")
ENDIF()

# set openmp flag
find_package(OpenMP)
if (OPENMP_FOUND)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# apply STXXL CXXFLAGS to our configuration
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")

add_subdirectory(src/tc)
add_executable(tclust src/tccli.cpp) 
target_link_libraries(tclust 
	tc_core
	${STXXL_LIBRARIES}
	${Boost_LIBRARIES}
	${MPI_LIBRARIES}
	)

#add_executable(cluster_cost src/cluster_cost.cpp)
#target_link_libraries(cluster_cost 
#	${Boost_LIBRARIES}
#	)
