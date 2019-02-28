add_definitions("-DACSDK_LOG_MODULE=kittAiKeyWordDetector")
add_library(KITTAI SHARED
    KittAiKeyWordDetector.cpp)

if(TWO_STAGE_TRIGGER_CONF)
target_include_directories(KITTAI PUBLIC
	"${KITTAI_KEY_WORD_DETECTOR_INCLUDE_DIR}"
	"${KWD_SOURCE_DIR}/include"
	"${KITTAI_SOURCE_DIR}/include"
	"${SampleApp_SOURCE_DIR}/include/SampleApp"
	"${KWD_SOURCE_DIR}/twostageTrigger"
	"${PORTAUDIO_INCLUDE_DIR}")
set_target_properties(KITTAI PROPERTIES COMPILE_FLAGS "-std=c++11" )
target_link_libraries(KITTAI KWD AVSCommon "${KITTAI_KEY_WORD_DETECTOR_LIB_PATH}" blas TwoStageTrigger)
else()
target_include_directories(KITTAI PUBLIC
	"${KITTAI_KEY_WORD_DETECTOR_INCLUDE_DIR}"
	"${KWD_SOURCE_DIR}/include"
	"${KITTAI_SOURCE_DIR}/include")

target_link_libraries(KITTAI KWD AVSCommon "${KITTAI_KEY_WORD_DETECTOR_LIB_PATH}" blas)
endif()

# install target
asdk_install()