cmake_minimum_required(VERSION 3.8)

#language used for the project
project(${This} C CXX)
set(CMAKE_CXX_STANDARD 98)

#executable
set(This webserv)

set(src_dir sources/)
set(inc_dir includes/)

set(srcs 
	${src_dir}Server.cpp
	${src_dir}Response.cpp
	${src_dir}Config.cpp
	${src_dir}main.cpp)

set(headers
	${inc_dir}Server.hpp
	${inc_dir}Config.hpp
	${inc_dir}Response.hpp)

#we will perform some testing
enable_testing()

#go to the directory and look for Cmakelists
add_subdirectory(googletest)

add_library(${This}_lib STATIC ${srcs} ${headers})

#go to the directory and look for Cmakelists
add_subdirectory(test)

add_executable(${This} ${srcs})
