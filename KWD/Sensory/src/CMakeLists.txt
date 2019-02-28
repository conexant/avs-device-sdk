add_definitions("-DACSDK_LOG_MODULE=sensoryKeywordDetector")
add_library(SENSORY SHARED
    SensoryKeywordDetector.cpp)
    

if(TWO_STAGE_TRIGGER_CONF)
target_include_directories(SENSORY PUBLIC
	"${SENSORY_KEY_WORD_DETECTOR_INCLUDE_DIR}"
	"${KWD_SOURCE_DIR}/include" 
	"${SENSORY_SOURCE_DIR}/include"
	"${SampleApp_SOURCE_DIR}/include/SampleApp"
	"${KWD_SOURCE_DIR}/twostageTrigger"
	"${PORTAUDIO_INCLUDE_DIR}")
	target_link_libraries(SENSORY KWD AVSCommon "${SENSORY_KEY_WORD_DETECTOR_LIB_PATH}" TwoStageTrigger)
else()
target_include_directories(SENSORY PUBLIC
	"${SENSORY_KEY_WORD_DETECTOR_INCLUDE_DIR}"
	"${KWD_SOURCE_DIR}/include" 
	"${SENSORY_SOURCE_DIR}/include")
	target_link_libraries(SENSORY KWD AVSCommon "${SENSORY_KEY_WORD_DETECTOR_LIB_PATH}")
endif()

# install target
asdk_install()