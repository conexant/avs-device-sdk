/**
 * Copyright 2017 Conexant.com, Inc. All Rights Reserved.
 */
#ifndef GPIO_WAKE_WORD_H
#define GPIO_WAKE_WORD_H
#include <thread>
#include <memory>
#include <atomic>
namespace AlexaWakeWord {

// A specialization of a WakeWordEngine, where a trigger comes from GPIO
class GPIOWakeWord {
public:
  GPIOWakeWord();
  ~GPIOWakeWord() = default;
  void pause();
  void resume();
  bool WakeWordDetect();

private:

  void init();

  // GPIO is monitored in this thread
  std::unique_ptr<std::thread> m_thread;
  std::atomic<bool> m_isRunning;

};

}
#endif // GPIO_WAKE_WORD_H