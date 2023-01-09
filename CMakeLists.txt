#Template for INFOH502
#Heavily inspired from https://github.com/Polytonic/Glitter/blob/master/CMakeLists.txt

#project setup
project(Project_VRPZ)

#set the minimal version of cmake and the minimal version of the c++ language
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)     
set(CMAKE_VERBOSE_MAKEFILE ON)

#define some variable
set(COMPILE_PROJECT ON CACHE BOOL "Compile the PROJECT folder")



find_package(OpenGL REQUIRED)

#for glad library
add_library( glad STATIC 3rdParty/glad/src/glad.c )
set(GLAD_INCLUDE "3rdParty/glad/include")
target_include_directories(glad PUBLIC ${GLAD_INCLUDE})

#for GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(3rdParty/glfw)



include_directories(3rdParty/glad/include/
                    3rdParty/glfw/include/
                    3rdParty/glm/
                    3rdParty/stb/)


if(COMPILE_PROJECT)
	add_subdirectory(Project)
endif()


