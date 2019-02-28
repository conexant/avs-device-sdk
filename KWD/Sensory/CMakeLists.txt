cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(SENSORY LANGUAGES CXX)

include(../../build/BuildDefaults.cmake)

add_subdirectory("src")

if(NOT TWO_STAGE_TRIGGER_CONF)
add_subdirectory("test")
endif()

