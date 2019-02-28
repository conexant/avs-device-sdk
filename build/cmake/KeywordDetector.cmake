#
# Setup the Keyword Detector type and compiler options.
#
# To build with a Keyword Detector, run the following command with a keyword detector type of AMAZON_KEY_WORD_DETECTOR,
# AMAZONLITE_KEY_WORD_DETECTOR, KITTAI_KEY_WORD_DETECTOR, or SENSORY_KEY_WORD_DETECTOR:
#     cmake <path-to-source> 
#       -DAMAZON_KEY_WORD_DETECTOR=ON 
#           -DAMAZON_KEY_WORD_DETECTOR_LIB_PATH=<path-to-amazon-lib> 
#           -DAMAZON_KEY_WORD_DETECTOR_INCLUDE_DIR=<path-to-amazon-include-dir>
#       -DAMAZONLITE_KEY_WORD_DETECTOR=ON 
#           -DAMAZONLITE_KEY_WORD_DETECTOR_LIB_PATH=<path-to-amazon-lib> 
#           -DAMAZONLITE_KEY_WORD_DETECTOR_INCLUDE_DIR=<path-to-amazon-include-dir>
#           -DAMAZONLITE_KEY_WORD_DETECTOR_DYNAMIC_MODEL_LOADING=<ON_OR_OFF>
#           -DAMAZONLITE_KEY_WORD_DETECTOR_MODEL_CPP_PATH=<path-to-model-cpp-file-if-dynamic-model-loading-disabled>
#       -DKITTAI_KEY_WORD_DETECTOR=ON 
#           -DKITTAI_KEY_WORD_DETECTOR_LIB_PATH=<path-to-kittai-lib>
#           -DKITTAI_KEY_WORD_DETECTOR_INCLUDE_DIR=<path-to-kittai-include-dir>
#       -DSENSORY_KEY_WORD_DETECTOR=ON 
#           -DSENSORY_KEY_WORD_DETECTOR_LIB_PATH=<path-to-sensory-lib>
#           -DSENSORY_KEY_WORD_DETECTOR_INCLUDE_DIR=<path-to-sensory-include-dir>
#

option(AMAZON_KEY_WORD_DETECTOR "Enable Amazon keyword detector." OFF)
option(AMAZONLITE_KEY_WORD_DETECTOR "Enable AmazonLite keyword detector." OFF)
option(AMAZONLITE_KEY_WORD_DETECTOR_DYNAMIC_MODEL_LOADING "Enable AmazonLite keyword detector dynamic model loading." OFF)
option(KITTAI_KEY_WORD_DETECTOR "Enable KittAi keyword detector." OFF)
option(SENSORY_KEY_WORD_DETECTOR "Enable Sensory keyword detector." OFF)
option(GPIO_KEY_WORD_DETECTOR "Enable GPIO keyword detector." OFF)
option(TWO_STAGE_TRIGGER_CONF "Enable Two stage trigger detector." OFF)
option(TWO_STAGE_TRIGGER_I2S_MODE "Enable Two stage trigger I2S Mode." OFF)
option(AURORA_GPIO "Enable getting Aurora trigger data in GPIO mode." OFF)
option(CURSES "Enable USB keycode triggering" OFF)
option(NO_CLOUD_REVALIDATION "Disable AVS trigger cloud revalidation" OFF)

if(CURSES)
    set(CURSES ON)
endif()

if(NOT AMAZON_KEY_WORD_DETECTOR AND NOT AMAZONLITE_KEY_WORD_DETECTOR AND NOT KITTAI_KEY_WORD_DETECTOR AND NOT SENSORY_KEY_WORD_DETECTOR AND NOT GPIO_KEY_WORD_DETECTOR)
    message("No keyword detector type specified, skipping build of keyword detector.")
    return()
endif()

if(NO_CLOUD_REVALIDATION)
    message("Cloud trigger revalidation is off")
    add_definitions(-DNO_REVALIDATION)
endif()

if(AMAZON_KEY_WORD_DETECTOR)
    message("Creating ${PROJECT_NAME} with keyword detector type: Amazon")
    if(NOT AMAZON_KEY_WORD_DETECTOR_LIB_PATH)
        message(FATAL_ERROR "Must pass library path of Amazon KeywordDetector!")
    endif()
    if(NOT AMAZON_KEY_WORD_DETECTOR_INCLUDE_DIR)
        message(FATAL_ERROR "Must pass include dir path of Amazon KeywordDetector!")
    endif()
    add_definitions(-DKWD)
    add_definitions(-DKWD_AMAZON)
    set(KWD ON)
endif()

if(AMAZONLITE_KEY_WORD_DETECTOR)
    message("Creating ${PROJECT_NAME} with keyword detector type: AmazonLite")
    if(NOT AMAZONLITE_KEY_WORD_DETECTOR_LIB_PATH)
        message(FATAL_ERROR "Must pass library path of AmazonLite KeywordDetector!")
    endif()
    if(NOT AMAZONLITE_KEY_WORD_DETECTOR_INCLUDE_DIR)
        message(FATAL_ERROR "Must pass include dir path of AmazonLite KeywordDetector!")
    endif()
    if(NOT AMAZONLITE_KEY_WORD_DETECTOR_DYNAMIC_MODEL_LOADING)
        if(NOT AMAZONLITE_KEY_WORD_DETECTOR_MODEL_CPP_PATH)
            message(FATAL_ERROR "Must pass the path of the desired model .cpp file for the AmazonLite Keyword Detector if dynamic loading of model is disabled!")
        endif()
    else()
        add_definitions(-DKWD_AMAZONLITE_DYNAMIC_MODEL_LOADING)
    endif()
    add_definitions(-DKWD)
    add_definitions(-DKWD_AMAZONLITE)
    set(KWD ON)
endif()

if(TWO_STAGE_TRIGGER_I2S_MODE)
	message("Adding Two stage trigger I2S MODE")
	add_definitions(-DI2S_MODE)
endif()

if(KITTAI_KEY_WORD_DETECTOR)
    message("Creating ${PROJECT_NAME} with keyword detector type: KittAi")
    if(NOT KITTAI_KEY_WORD_DETECTOR_LIB_PATH)
        message(FATAL_ERROR "Must pass library path of Kitt.ai KeywordDetector!")
    endif()
    if(NOT KITTAI_KEY_WORD_DETECTOR_INCLUDE_DIR)
        message(FATAL_ERROR "Must pass include dir path of Kitt.ai KeywordDetector!")
    endif()
    add_definitions(-DKWD)
    add_definitions(-DKWD_KITTAI)
	if(TWO_STAGE_TRIGGER_CONF)
		message("Adding Two stage trigger for : KittAi")
		add_definitions(-DTWO_STAGE_TRIGGER)
	endif()
    set(KWD ON)
endif()

if(SENSORY_KEY_WORD_DETECTOR)
    message("Creating ${PROJECT_NAME} with keyword detector type: Sensory")
    if(NOT SENSORY_KEY_WORD_DETECTOR_LIB_PATH)
        message(FATAL_ERROR "Must pass library path of Sensory KeywordDetector!")
    endif()
    if(NOT SENSORY_KEY_WORD_DETECTOR_INCLUDE_DIR)
        message(FATAL_ERROR "Must pass include dir path of Sensory KeywordDetector!")
    endif()
    add_definitions(-DKWD)
    add_definitions(-DKWD_SENSORY)
    if(TWO_STAGE_TRIGGER_CONF)
		message("Adding Two stage trigger for : Sensory")
		add_definitions(-DTWO_STAGE_TRIGGER)
    endif()
    set(KWD ON)
endif()

if(GPIO_KEY_WORD_DETECTOR)
    message("Creating ${PROJECT_NAME} with keyword detector type: GPIO")
    add_definitions(-DKWD)
    add_definitions(-DKWD_GPIO)
    if(TWO_STAGE_TRIGGER_CONF)
        message("Adding Two stage trigger for : GPIO")
        add_definitions(-DTWO_STAGE_TRIGGER)
    endif()
    if(AURORA_GPIO)
        add_definitions(-DAURORAGPIO)
    endif()
    set(KWD ON)
endif()