add_definitions("-DACSDK_LOG_MODULE=GPIOWakeWordDetector")
add_library(GPIO SHARED 
	GPIOKeyWordDetector.cpp) 
set_target_properties( GPIO PROPERTIES COMPILE_FLAGS "-std=c++11" )

target_include_directories(GPIO PUBLIC
	"${KWD_SOURCE_DIR}/include" 
	"${GPIO_SOURCE_DIR}/include")
target_link_libraries( GPIO KWD wiringPi AVSCommon )