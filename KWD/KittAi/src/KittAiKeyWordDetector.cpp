/*
 * Copyright 2017-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <memory>
#include <sstream>

#include <AVSCommon/Utils/Logger/Logger.h>
#include <AVSCommon/Utils/Memory/Memory.h>

#include "KittAi/KittAiKeyWordDetector.h"

#ifdef TWO_STAGE_TRIGGER
#include <unistd.h>
#include "WakeWordUtils.h"
#include "WakeWordException.h"
#define DETECKT_WAKE_WORD_TIMEOUT_SECONDS (2)
using namespace AlexaWakeWord; 
#endif


namespace alexaClientSDK {
namespace kwd {

using namespace avsCommon;
using namespace avsCommon::avs;
using namespace avsCommon::sdkInterfaces;
using namespace avsCommon::utils;

static const std::string TAG("KittAiKeyWordDetector");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) alexaClientSDK::avsCommon::utils::logger::LogEntry(TAG, event)

/// The number of hertz per kilohertz.
static const size_t HERTZ_PER_KILOHERTZ = 1000;

/// The timeout to use for read calls to the SharedDataStream.
const std::chrono::milliseconds TIMEOUT_FOR_READ_CALLS = std::chrono::milliseconds(1000);

/// The delimiter for Kitt.ai engine constructor parameters
static const std::string KITT_DELIMITER = ",";

/// The Kitt.ai compatible audio encoding of LPCM.
static const avsCommon::utils::AudioFormat::Encoding KITT_AI_COMPATIBLE_ENCODING =
    avsCommon::utils::AudioFormat::Encoding::LPCM;

/// The Kitt.ai compatible endianness which is little endian.
static const avsCommon::utils::AudioFormat::Endianness KITT_AI_COMPATIBLE_ENDIANNESS =
    avsCommon::utils::AudioFormat::Endianness::LITTLE;

/// Kitt.ai returns -2 if silence is detected.
static const int KITT_AI_SILENCE_DETECTION_RESULT = -2;

/// Kitt.ai returns -1 if an error occurred.
static const int KITT_AI_ERROR_DETECTION_RESULT = -1;

/// Kitt.ai returns 0 if no keyword was detected but audio has been heard.
static const int KITT_AI_NO_DETECTION_RESULT = 0;

std::unique_ptr<KittAiKeyWordDetector> KittAiKeyWordDetector::create(
#ifdef TWO_STAGE_TRIGGER
			std::shared_ptr<sampleApp::PortAudioMicrophoneWrapper> micWrapper,
#endif
    std::shared_ptr<AudioInputStream> stream,
    AudioFormat audioFormat,
    std::unordered_set<std::shared_ptr<KeyWordObserverInterface>> keyWordObservers,
    std::unordered_set<std::shared_ptr<KeyWordDetectorStateObserverInterface>> keyWordDetectorStateObservers,
    const std::string& resourceFilePath,
    const std::vector<KittAiConfiguration> kittAiConfigurations,
    float audioGain,
    bool applyFrontEnd,
    std::chrono::milliseconds msToPushPerIteration) {
    if (!stream) {
        ACSDK_ERROR(LX("createFailed").d("reason", "nullStream"));
        return nullptr;
    }
    // TODO: ACSDK-249 - Investigate cpu usage of converting bytes between endianness and if it's not too much, do it.
    if (isByteswappingRequired(audioFormat)) {
        ACSDK_ERROR(LX("createFailed").d("reason", "endianMismatch"));
        return nullptr;
    }
    std::unique_ptr<KittAiKeyWordDetector> detector(new KittAiKeyWordDetector(
#ifdef TWO_STAGE_TRIGGER
			micWrapper,
#endif
        stream,
        audioFormat,
        keyWordObservers,
        keyWordDetectorStateObservers,
        resourceFilePath,
        kittAiConfigurations,
        audioGain,
        applyFrontEnd,
        msToPushPerIteration));
    if (!detector->init(audioFormat)) {
        ACSDK_ERROR(LX("createFailed").d("reason", "initDetectorFailed"));
        return nullptr;
    }
    return detector;
}

KittAiKeyWordDetector::~KittAiKeyWordDetector() {
    m_isShuttingDown = true;
    if (m_detectionThread.joinable()) {
        m_detectionThread.join();
    }
}

KittAiKeyWordDetector::KittAiKeyWordDetector(
#ifdef TWO_STAGE_TRIGGER
		std::shared_ptr<sampleApp::PortAudioMicrophoneWrapper> micWrapper,
#endif
    std::shared_ptr<AudioInputStream> stream,
    avsCommon::utils::AudioFormat audioFormat,
    std::unordered_set<std::shared_ptr<KeyWordObserverInterface>> keyWordObservers,
    std::unordered_set<std::shared_ptr<KeyWordDetectorStateObserverInterface>> keyWordDetectorStateObservers,
    const std::string& resourceFilePath,
    const std::vector<KittAiConfiguration> kittAiConfigurations,
    float audioGain,
    bool applyFrontEnd,
    std::chrono::milliseconds msToPushPerIteration) :
        AbstractKeywordDetector(keyWordObservers, keyWordDetectorStateObservers),
        m_stream{stream},
        m_maxSamplesPerPush{(audioFormat.sampleRateHz / HERTZ_PER_KILOHERTZ) * msToPushPerIteration.count()}{
#ifdef TWO_STAGE_TRIGGER  
	m_micWrapper = micWrapper;
	m_currentState = State::UNINITIALIZED;
#endif
    std::stringstream sensitivities;
    std::stringstream modelPaths;
    for (unsigned int i = 0; i < kittAiConfigurations.size(); ++i) {
        modelPaths << kittAiConfigurations.at(i).modelFilePath;
        sensitivities << kittAiConfigurations.at(i).sensitivity;
        m_detectionResultsToKeyWords[i + 1] = kittAiConfigurations.at(i).keyword;
        if (kittAiConfigurations.size() - 1 != i) {
            modelPaths << KITT_DELIMITER;
            sensitivities << KITT_DELIMITER;
        }
    }
    m_kittAiEngine = avsCommon::utils::memory::make_unique<snowboy::SnowboyDetect>(resourceFilePath, modelPaths.str());
    m_kittAiEngine->SetSensitivity(sensitivities.str());
    m_kittAiEngine->SetAudioGain(audioGain);
    m_kittAiEngine->ApplyFrontend(applyFrontEnd);
}

bool KittAiKeyWordDetector::init(avsCommon::utils::AudioFormat audioFormat) {
    if (!isAudioFormatCompatibleWithKittAi(audioFormat)) {
        return false;
    }
    m_streamReader = m_stream->createReader(AudioInputStream::Reader::Policy::BLOCKING);
    if (!m_streamReader) {
        ACSDK_ERROR(LX("initFailed").d("reason", "createStreamReaderFailed"));
        return false;
    }
    m_isShuttingDown = false;
    m_detectionThread = std::thread(&KittAiKeyWordDetector::detectionLoop, this);
    return true;
}

bool KittAiKeyWordDetector::isAudioFormatCompatibleWithKittAi(avsCommon::utils::AudioFormat audioFormat) {
    if (audioFormat.numChannels != static_cast<unsigned int>(m_kittAiEngine->NumChannels())) {
        ACSDK_ERROR(LX("isAudioFormatCompatibleWithKittAiFailed")
                        .d("reason", "numChannelsMismatch")
                        .d("kittAiNumChannels", m_kittAiEngine->NumChannels())
                        .d("numChannels", audioFormat.numChannels));
        return false;
    }
    if (audioFormat.sampleRateHz != static_cast<unsigned int>(m_kittAiEngine->SampleRate())) {
        ACSDK_ERROR(LX("isAudioFormatCompatibleWithKittAiFailed")
                        .d("reason", "sampleRateMismatch")
                        .d("kittAiSampleRate", m_kittAiEngine->SampleRate())
                        .d("sampleRate", audioFormat.sampleRateHz));
        return false;
    }
    if (audioFormat.sampleSizeInBits != static_cast<unsigned int>(m_kittAiEngine->BitsPerSample())) {
        ACSDK_ERROR(LX("isAudioFormatCompatibleWithKittAiFailed")
                        .d("reason", "sampleSizeInBitsMismatch")
                        .d("kittAiSampleSizeInBits", m_kittAiEngine->BitsPerSample())
                        .d("sampleSizeInBits", audioFormat.sampleSizeInBits));
        return false;
    }
    if (audioFormat.endianness != KITT_AI_COMPATIBLE_ENDIANNESS) {
        ACSDK_ERROR(LX("isAudioFormatCompatibleWithKittAiFailed")
                        .d("reason", "endiannessMismatch")
                        .d("kittAiEndianness", KITT_AI_COMPATIBLE_ENDIANNESS)
                        .d("endianness", audioFormat.endianness));
        return false;
    }
    if (audioFormat.encoding != KITT_AI_COMPATIBLE_ENCODING) {
        ACSDK_ERROR(LX("isAudioFormatCompatibleWithKittAiFailed")
                        .d("reason", "encodingMismatch")
                        .d("kittAiEncoding", KITT_AI_COMPATIBLE_ENCODING)
                        .d("encoding", audioFormat.encoding));
        return false;
    }
    return true;
}
#ifdef TWO_STAGE_TRIGGER   

bool KittAiKeyWordDetector::gpioDetect() {
	int gpioWakeWordDetect = false;
	if(m_threadCheckTimeOut){
	  m_threadCheckTimeOut->join();
	  m_threadCheckTimeOut = nullptr;
  	}
	if(m_gpioWakeWord == NULL){
			m_gpioWakeWord = make_unique<GPIOWakeWord>();
			if(m_gpioWakeWord == NULL){
				ACSDK_ERROR(LX("gpioDetect Failed").d("reason", "make_unique GPIOWakeWord."));
				return false;
			}
	}
		
	if((m_currentState == State::WORD_DETECTED_TIMEOUT) 
		|| (m_currentState == State::UNINITIALIZED)){
		m_micWrapper->stopStreamingMicrophoneData();
	}
	if(m_currentState == State::WORD_DETECTED){
		sleep(DETECKT_WAKE_WORD_TIMEOUT_SECONDS);
	}

	/*If time out, Seek streamReader to the last of the writer cursor, because PortAudioMicrophoneWrapper aready stopped,
	*It is the new beginning of trigger words.
	*/
	m_streamReader->seek(0, AudioInputStream::Reader::Reference::BEFORE_WRITER);
	
	while(false == gpioWakeWordDetect){
			gpioWakeWordDetect = m_gpioWakeWord->WakeWordDetect();
	}	

	/*If wake word be detected, Again Seeking streamReader to the last of the writer cursor, because PortAudioMicrophoneWrapper aready stopped,
	*It is the new beginning of trigger words.
	*/
	if(m_currentState == State::WORD_DETECTED){
		m_streamReader->seek(0, AudioInputStream::Reader::Reference::BEFORE_WRITER);
	}
	m_micWrapper->startStreamingMicrophoneData();
	m_currentState = State::WORD_DETECTING;
	m_threadCheckTimeOut = make_unique<std::thread>(&KittAiKeyWordDetector::checkTimeOut, this);
	return gpioWakeWordDetect;
}
void KittAiKeyWordDetector::checkTimeOut(){
	struct timespec abs_time;
	struct timespec max_wait;
	
	memset(&max_wait, 0, sizeof(max_wait));
	max_wait.tv_sec = DETECKT_WAKE_WORD_TIMEOUT_SECONDS;
	pthread_mutex_lock(&m_checkDetectTimeOut);
	clock_gettime(CLOCK_REALTIME, &abs_time);
	abs_time.tv_sec += max_wait.tv_sec;
	abs_time.tv_nsec += max_wait.tv_nsec;

	int  err = pthread_cond_timedwait(&m_detectTrigger, &m_checkDetectTimeOut, &abs_time);
	if (err == ETIMEDOUT){
	   m_currentState = State::WORD_DETECTED_TIMEOUT;
	 }
	pthread_mutex_unlock(&m_checkDetectTimeOut); 
}

#endif	

void KittAiKeyWordDetector::detectionLoop() {
    notifyKeyWordDetectorStateObservers(KeyWordDetectorStateObserverInterface::KeyWordDetectorState::ACTIVE);
    int16_t audioDataToPush[m_maxSamplesPerPush];
    ssize_t wordsRead;
    while (!m_isShuttingDown) {
        bool didErrorOccur;
#ifdef TWO_STAGE_TRIGGER 
		if(m_currentState != State::WORD_DETECTING){
			gpioDetect();
		}
#endif		
        wordsRead = readFromStream(
            m_streamReader, m_stream, audioDataToPush, m_maxSamplesPerPush, TIMEOUT_FOR_READ_CALLS, &didErrorOccur);
        if (didErrorOccur) {
            break;
        } else if (wordsRead > 0) {
            // Words were successfully read.
            notifyKeyWordDetectorStateObservers(KeyWordDetectorStateObserverInterface::KeyWordDetectorState::ACTIVE);
            int detectionResult = m_kittAiEngine->RunDetection(audioDataToPush, wordsRead);
            if (detectionResult > 0) {
                // > 0 indicates a keyword was found
                if (m_detectionResultsToKeyWords.find(detectionResult) == m_detectionResultsToKeyWords.end()) {
                    ACSDK_ERROR(LX("detectionLoopFailed").d("reason", "retrievingDetectedKeyWordFailed"));
                    notifyKeyWordDetectorStateObservers(
                        KeyWordDetectorStateObserverInterface::KeyWordDetectorState::ERROR);
                    break;
                } else {
#ifdef TWO_STAGE_TRIGGER                    
					pthread_cond_signal(&m_detectTrigger);
					m_currentState = State::WORD_DETECTED;
#endif
                    notifyKeyWordObservers(
                        m_stream,
                        m_detectionResultsToKeyWords[detectionResult],
                        KeyWordObserverInterface::UNSPECIFIED_INDEX,
                        m_streamReader->tell());
                }
            } else {
                switch (detectionResult) {
                    case KITT_AI_ERROR_DETECTION_RESULT:
                        ACSDK_ERROR(LX("detectionLoopFailed").d("reason", "kittAiEngineError"));
                        notifyKeyWordDetectorStateObservers(
                            KeyWordDetectorStateObserverInterface::KeyWordDetectorState::ERROR);
                        didErrorOccur = true;
                        break;
                    case KITT_AI_SILENCE_DETECTION_RESULT:
                        break;
                    case KITT_AI_NO_DETECTION_RESULT:
                        break;
                    default:
                        ACSDK_ERROR(LX("detectionLoopEnded")
                                        .d("reason", "unexpectedDetectionResult")
                                        .d("detectionResult", detectionResult));
                        notifyKeyWordDetectorStateObservers(
                            KeyWordDetectorStateObserverInterface::KeyWordDetectorState::ERROR);
                        didErrorOccur = true;
                        break;
                }
                if (didErrorOccur) {
                    break;
                }
            }
        }
    }
    m_streamReader->close();
}

}  // namespace kwd
}  // namespace alexaClientSDK
