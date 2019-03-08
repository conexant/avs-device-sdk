/**
 * Copyright Synaptics.com, Inc. All Rights Reserved.
 */
#include "GPIO/GPIOKeyWordDetector.h"
#include <AVSCommon/Utils/Memory/Memory.h>
#include <AVSCommon/Utils/Logger/Logger.h>

#include <exception>
#include <wiringPi.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <memory>

//TODO: Add build option
//#define WRITETOFILE
#ifdef WRITETOFILE
#include <fstream>
#endif

namespace alexaClientSDK {


namespace kwd {
using namespace avsCommon::utils::logger;

/// String to identify log entries originating from this file.
static const std::string TAG("GPIOKeywordDetector");
/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) alexaClientSDK::avsCommon::utils::logger::LogEntry(TAG, event)
static const int GPIO_PIN = 2;//Rpi Pin#13 
static const int MICROSECONDS_BETWEEN_READINGS = 1000000;

/// The number of hertz per kilohertz.
static const size_t HERTZ_PER_KILOHERTZ = 1000;

/// The timeout to use for read calls to the SharedDataStream.
const std::chrono::milliseconds TIMEOUT_FOR_READ_CALLS = std::chrono::milliseconds(1000);

#ifdef WRITETOFILE
static int fileindex = 0;
#endif

std::unique_ptr<GPIOKeyWordDetector> GPIOKeyWordDetector::create(
        std::shared_ptr<AudioInputStream> stream,
        avsCommon::utils::AudioFormat audioFormat,
        std::unordered_set<std::shared_ptr<KeyWordObserverInterface>> keyWordObservers,
        std::unordered_set<std::shared_ptr<KeyWordDetectorStateObserverInterface>> keyWordDetectorStateObservers,
        std::chrono::milliseconds msToPushPerIteration)  {
    
    if (!stream) {
        ACSDK_ERROR(LX("createFailed").d("reason", "nullStream"));
        return nullptr;
    }

    // TODO: ACSDK-249 - Investigate cpu usage of converting bytes between endianness and if it's not too much, do it.
    if (isByteswappingRequired(audioFormat)) {
        ACSDK_ERROR(LX("createFailed").d("reason", "endianMismatch"));
        return nullptr;
    }

    std::unique_ptr<GPIOKeyWordDetector> detector(new GPIOKeyWordDetector(
        stream,
        keyWordObservers,
        keyWordDetectorStateObservers,
        audioFormat
        ));

    if (!detector->init()) {
        ACSDK_ERROR(LX("createFailed").d("reason", "initDetectorFailed"));
        return nullptr;
    }
    
    return detector;
}



GPIOKeyWordDetector::GPIOKeyWordDetector(
    std::shared_ptr<AudioInputStream> stream,
    std::unordered_set<std::shared_ptr<KeyWordObserverInterface>> keyWordObservers,
    std::unordered_set<std::shared_ptr<KeyWordDetectorStateObserverInterface>> keyWordDetectorStateObservers,
    avsCommon::utils::AudioFormat audioFormat,
    std::chrono::milliseconds msToPushPerIteration) :

        AbstractKeywordDetector(keyWordObservers, keyWordDetectorStateObservers),
        m_stream{stream},
        m_maxSamplesPerPush((audioFormat.sampleRateHz / HERTZ_PER_KILOHERTZ) * msToPushPerIteration.count()),
        m_isRunning { false } {}

GPIOKeyWordDetector::~GPIOKeyWordDetector() {
    m_isRunning = false;
    if (m_thread->joinable())
        m_thread->join();
}

#ifdef AURORAGPIO
bool GPIOKeyWordDetector::getAuroraStartEnd(int* start, int* end) {
    char buffer[128], start_buf[11], duration_buf[11];
    bool sep = false;
    int j = 0;

    FILE* pipe = popen("./host_demo_aur.exe -u --aurtrig 2 2>/dev/null", "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        fgets(buffer, 128, pipe);
    } catch (...) {
        pclose(pipe);
        ACSDK_ERROR(LX("getAuroraDurationFailed").d("reason", "fgetsFailed"));
        return false;
    }
    pclose(pipe);
    for(unsigned int i = 0; i < sizeof(buffer); i++)
    {
        if(buffer[i] == '\n')
            break;
        else if(buffer[i] == ',')
        {
            sep = true;
            j = 0;
            continue;
        }

        if(sep)
            start_buf[j++] = buffer[i];
        else
            duration_buf[j++] = buffer[i];
    }
    if(!sep)
    {
        ACSDK_ERROR(LX("getAuroraDurationFailed").d("reason", "unexpectedHostReturn"));
        printf("%s",buffer);
        return false;   
    }
    *start = atoi(start_buf);
    *end = atoi(duration_buf);
    *end = *start - *end;
    return true;
}
#endif
bool GPIOKeyWordDetector::init() {

    // log(Logger::INFO, "CnxtGPIOKeyWord: initializing");
    setenv("WIRINGPI_GPIOMEM", "1", 1);
    if (wiringPiSetup() < 0) {
        ACSDK_ERROR(LX("initFailed").d("reason", "wiringPiSetup failed"));
        return false;
    }

    // INPUT is defined in "wiringPi.h"
    pinMode(GPIO_PIN, INPUT);

    m_streamReader = m_stream->createReader(AudioInputStream::Reader::Policy::BLOCKING);
    if (!m_streamReader) {
        ACSDK_ERROR(LX("initFailed").d("reason", "createStreamReaderFailed"));
        return false;
    }

    // log(Logger::INFO, "Starting GPIO reading thread");
    m_isRunning = true;
    m_thread = avsCommon::utils::memory::make_unique<std::thread>(&GPIOKeyWordDetector::mainLoop, this);
    return true;
}

void GPIOKeyWordDetector::mainLoop() {
    int gpioValue = 0;
    notifyKeyWordDetectorStateObservers(KeyWordDetectorStateObserverInterface::KeyWordDetectorState::ACTIVE);
    std::vector<int16_t> audioDataToPush(m_maxSamplesPerPush);
    //ssize_t wordsRead;
    bool didErrorOccur = false;
#ifndef NO_REVALIDATION
    const size_t before = m_maxSamplesPerPush * 50;
#endif
#ifdef AURORAGPIO
    int start = 0, end = 0;
#endif //AURORAGPIO

    while (m_isRunning) {
        didErrorOccur = false;

        //wordsRead = 
        readFromStream(
            m_streamReader,
            m_stream,
            audioDataToPush.data(),
            audioDataToPush.size(),
            TIMEOUT_FOR_READ_CALLS,
            &didErrorOccur);

        if (didErrorOccur)
            break;

        gpioValue = digitalRead(GPIO_PIN);

        if (gpioValue == HIGH) {
#ifdef AURORAGPIO
            if(getAuroraStartEnd(&start, &end) && end >= 0)
            {                
#ifdef WRITETOFILE
                m_streamReader->seek(start+m_maxSamplesPerPush*50,AudioInputStream::Reader::Reference::BEFORE_WRITER);
                std::vector<int16_t> audioDataToWrite(start+m_maxSamplesPerPush*50);
                readFromStream(
                    m_streamReader,
                    m_stream,
                    audioDataToWrite.data(),
                    audioDataToWrite.size(),
                    TIMEOUT_FOR_READ_CALLS,
                    &didErrorOccur);
                if (!didErrorOccur)
                {
                    std::ofstream file;
                    file.open(std::to_string(fileindex++)+".raw", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
                    file.write((char*) audioDataToWrite.data(),(std::streamsize) audioDataToWrite.size()*2);
                    file.close();
                }
#endif //WRITETOFILE

                notifyKeyWordObservers(
                m_stream,
                "alexa",
#ifdef NO_REVALIDATION
                avsCommon::sdkInterfaces::KeyWordObserverInterface::UNSPECIFIED_INDEX,
#else //NO_REVALIDATION
                (m_streamReader->tell() < (unsigned int)start ? 0 : m_streamReader->tell() - start),
#endif //NO_REVALIDATION
                (m_streamReader->tell() < (unsigned int)end ? 0 : m_streamReader->tell() - end ));
            }
            else
            {
#ifdef WRITETOFILE
                m_streamReader->seek(m_maxSamplesPerPush*101,AudioInputStream::Reader::Reference::BEFORE_WRITER);
                std::vector<int16_t> audioDataToWrite(m_maxSamplesPerPush*101);
                readFromStream(
                    m_streamReader,
                    m_stream,
                    audioDataToWrite.data(),
                    audioDataToWrite.size(),
                    TIMEOUT_FOR_READ_CALLS,
                    &didErrorOccur);
                if (!didErrorOccur)
                {
                    std::ofstream file;
                    file.open(std::to_string(fileindex++)+".raw", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
                    file.write((char*) audioDataToWrite.data(),(std::streamsize) audioDataToWrite.size()*2);
                    file.close();
                }
#endif //WRITETOFILE

                notifyKeyWordObservers(
                m_stream,
                "alexa",
#ifdef NO_REVALIDATION
                avsCommon::sdkInterfaces::KeyWordObserverInterface::UNSPECIFIED_INDEX,
#else //NO_REVALIDATION
                (m_streamReader->tell() < before ? 0 : m_streamReader->tell() - before),
#endif //NO_REVALIDATION
                m_streamReader->tell());
            }
#else //AURORAGPIO
#ifdef WRITETOFILE
                m_streamReader->seek(m_maxSamplesPerPush*101,AudioInputStream::Reader::Reference::BEFORE_WRITER);
                std::vector<int16_t> audioDataToWrite(m_maxSamplesPerPush*101);
                readFromStream(
                    m_streamReader,
                    m_stream,
                    audioDataToWrite.data(),
                    audioDataToWrite.size(),
                    TIMEOUT_FOR_READ_CALLS,
                    &didErrorOccur);
                if (!didErrorOccur)
                {
                    std::ofstream file;
                    file.open(std::to_string(fileindex++)+".raw", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
                    file.write((char*) audioDataToWrite.data(),(std::streamsize) audioDataToWrite.size()*2);
                    file.close();
                }
#endif //WRITETOFILE
            notifyKeyWordObservers(
                m_stream,
                "alexa",
#ifdef NO_REVALIDATION
                avsCommon::sdkInterfaces::KeyWordObserverInterface::UNSPECIFIED_INDEX,
#else //NO_REVALIDATION
                (m_streamReader->tell() < before ? 0 : m_streamReader->tell() - before),
#endif //NO_REVALIDATION
                m_streamReader->tell());
#endif //AURORAGPIO
            usleep(MICROSECONDS_BETWEEN_READINGS);
        }

    }
}

}  // namespace kwd
}  // namespace alexaClientSDK
