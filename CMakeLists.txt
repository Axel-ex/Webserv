cmake_minimum_required(VERSION 3.8)

#language used for the project
project(${This} C CXX)
set(CMAKE_CXX_STANDARD 98)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror")

#executable
set(This webserv)

set(src_dir sources/)
set(inc_dir includes/)

set(srcs 
	${src_dir}Cluster.cpp
	${src_dir}Server.cpp
	${src_dir}Parser.cpp
	${src_dir}Log.cpp
	${src_dir}Config.cpp
	${src_dir}RequestBuffer.cpp
	${src_dir}Request.cpp
	${src_dir}RequestHandlers.cpp
	${src_dir}CgiRequestHandler.cpp
	${src_dir}Response.cpp
	${src_dir}signal.cpp
	${src_dir}Tools.cpp
	${src_dir}main.cpp)

set(headers
	${inc_dir}utils.hpp
	${inc_dir}Parser.hpp
	${inc_dir}Server.hpp
	${inc_dir}Cluster.hpp
	${inc_dir}Log.hpp
	${inc_dir}Config.hpp
	${inc_dir}Request.hpp
	${inc_dir}RequestHandlers.hpp
	${inc_dir}CgiRequestHandler.hpp
	${inc_dir}Response.hpp)

# #we will perform some testing
# enable_testing()
#
# #go to the directory and look for Cmakelists
# add_subdirectory(googletest)
#
# add_library(${This}_lib STATIC ${srcs} ${headers})
#
# #go to the directory and look for Cmakelists
# add_subdirectory(test)

add_executable(${This} ${srcs})
