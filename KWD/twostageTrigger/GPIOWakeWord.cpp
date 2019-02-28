/**
 * Copyright Synaptics.com, Inc. All Rights Reserved.
 */
#include "GPIOWakeWord.h"
//#include "Logger.h"
#include "WakeWordUtils.h"
#include "WakeWordException.h"

#include <wiringPi.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>

namespace AlexaWakeWord {

static const int GPIO_PIN = 2;//Rpi Pin#13 
static const int MICROSECONDS_BETWEEN_READINGS = 1000;

GPIOWakeWord::GPIOWakeWord() : m_isRunning { false } {

  try {
    init();
  } catch (WakeWordException& e) {
   // log(Logger::ERROR,
   //     std::string("GPIOWakeWord: Initialization error:") + e.what());
    throw;
  }

}

void GPIOWakeWord::pause() {

  //log(Logger::INFO, "CnxtGPIOWakeWord: handling pause");
}

void GPIOWakeWord::resume() {

  //log(Logger::INFO, "CnxtGPIOWakeWord: handling resume");
}

void GPIOWakeWord::init() {

 // log(Logger::INFO, "CnxtGPIOWakeWord: initializing");
  setenv("WIRINGPI_GPIOMEM", "1", 1);
  if (wiringPiSetup() < 0) {
    std::string errorMsg = "Failed to initialize WiringPi library";
    throw WakeWordException(errorMsg);
  }

  // INPUT is defined in "wiringPi.h"
  pinMode(GPIO_PIN, INPUT);

 // log(Logger::INFO, "Starting GPIO reading thread");
  m_isRunning = true;
  //m_thread = make_unique<std::thread>(&GPIOWakeWord::mainLoop, this);
}

bool GPIOWakeWord::WakeWordDetect() {
  int gpioValue = 0;
  while (m_isRunning) {
    gpioValue = digitalRead(GPIO_PIN);

    // HIGH and LOW are defined in "wiringPi.h"
    // The following detects the rising edge of GPIO input
    if (gpioValue == HIGH) {
	  return true;
    }

    usleep(MICROSECONDS_BETWEEN_READINGS);
  }
  return false;
}

} // namespace AlexaWakeWord
