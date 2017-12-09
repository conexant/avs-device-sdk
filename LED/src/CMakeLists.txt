cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

find_package(Threads ${THREADS_PACKAGE_CONFIG})
add_definitions("-DACSDK_LOG_MODULE=led")
add_library(LED SHARED
    LEDControl.cpp
    LEDManager.cpp)
target_include_directories(LED PUBLIC
    "${LED_SOURCE_DIR}/include"
    "${AVSCommon_INCLUDE_DIRS}"
    "${LED_INCLUDE_DIR}")
target_link_libraries(LED
    ${CMAKE_THREAD_LIBS_INIT}
    AVSCommon
    "${LED_INCLUDE_DIR}/libws2811.a")

# install target
asdk_install()