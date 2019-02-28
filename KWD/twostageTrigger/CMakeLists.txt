add_library(TwoStageTrigger SHARED 
	GPIOWakeWord.cpp
	WakeWordException.cpp
	) 
set_target_properties( TwoStageTrigger PROPERTIES COMPILE_FLAGS "-std=c++11" )
target_link_libraries( TwoStageTrigger wiringPi )