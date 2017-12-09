cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(LED LANGUAGES CXX)

include(../build/BuildDefaults.cmake)

if(NOT LED)
    message("LED will not be built")
else()
	add_subdirectory("src")
endif()