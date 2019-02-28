#
# Set up LED libraries for the sample app.
#
# To build with rpi_ws281x based LED, run the following command,
#     cmake <path-to-source> 
#       -DLED=ON 
#           -DLED_INCLUDE_DIR=<path-to-rpi_ws281x-include-dir>
#

option(LED "Enable LED for the sample app." OFF)
option(LED24 "Enable LED for the sample app." OFF)

if(LED)
    if(NOT LED_INCLUDE_DIR)
        message(FATAL_ERROR "Must pass include dir path of rpi_ws281x to enable LEDs.")
    else()
        add_definitions(-DLED)
        if(LED24)
            add_definitions(-DLED24)
        endif()
    endif()
#else()
#    message("LED not defined, skipping build of LED module")
#
#    return()
endif()