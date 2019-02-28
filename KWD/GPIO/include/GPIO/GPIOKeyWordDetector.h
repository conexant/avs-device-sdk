/**
 * Copyright 2017 Conexant.com, Inc. All Rights Reserved.
 */
#ifndef GPIO_Key_WORD_H
#define GPIO_Key_WORD_H

#include <thread>
#include <memory>
#include <atomic>

#include <AVSCommon/Utils/AudioFormat.h>
#include <AVSCommon/AVS/AudioInputStream.h>
#include <AVSCommon/SDKInterfaces/KeyWordObserverInterface.h>
#include <AVSCommon/SDKInterfaces/KeyWordDetectorStateObserverInterface.h>

#include "KWD/AbstractKeywordDetector.h"

namespace alexaClientSDK {
namespace kwd {

using namespace avsCommon;
using namespace avsCommon::avs;
using namespace avsCommon::sdkInterfaces;

// A specialization of a KeyWordEngine, where a trigger comes from GPIO
class GPIOKeyWordDetector : public AbstractKeywordDetector {
public:
    static std::unique_ptr<GPIOKeyWordDetector> create(
        std::shared_ptr<AudioInputStream> stream,
        avsCommon::utils::AudioFormat audioFormat,
        std::unordered_set<std::shared_ptr<KeyWordObserverInterface>> keyWordObservers,
        std::unordered_set<std::shared_ptr<KeyWordDetectorStateObserverInterface>> keyWordDetectorStateObservers,
        std::chrono::milliseconds msToPushPerIteration = std::chrono::milliseconds(10));

    ~GPIOKeyWordDetector();

private:
    GPIOKeyWordDetector(
        std::shared_ptr<AudioInputStream> stream,
        std::unordered_set<std::shared_ptr<KeyWordObserverInterface>> keyWordObservers,
        std::unordered_set<std::shared_ptr<KeyWordDetectorStateObserverInterface>> keyWordDetectorStateObservers,
        avsCommon::utils::AudioFormat audioFormat,
        std::chrono::milliseconds msToPushPerIteration = std::chrono::milliseconds(10));

    bool init();
    void mainLoop();
#ifdef AURORAGPIO
    bool getAuroraStartEnd(int* start, int* end);
#endif

    // GPIO is monitored in this thread
    std::unique_ptr<std::thread> m_thread;

    /// The stream of audio data.
    const std::shared_ptr<avsCommon::avs::AudioInputStream> m_stream;

    /// The reader that will be used to read audio data from the stream.
    std::shared_ptr<avsCommon::avs::AudioInputStream::Reader> m_streamReader;

    /**
     * This serves as a reference point used when notifying observers of keyword detection indices since Sensory has no
     * way of specifying a start index.
     */
    avsCommon::avs::AudioInputStream::Index m_beginIndexOfStreamReader;

    /**
     * The max number of samples to push into the underlying engine per iteration. This will be determined based on the
     * sampling rate of the audio data passed in.
     */
    const size_t m_maxSamplesPerPush;

    
    std::atomic<bool> m_isRunning;
};


}  // namespace kwd
}  // namespace alexaClientSDK

#endif // GPIO_Key_WORD_H