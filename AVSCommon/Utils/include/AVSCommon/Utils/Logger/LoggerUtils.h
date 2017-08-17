#ifndef ALEXA_CLIENT_SDK_AVS_COMMON_UTILS_INCLUDE_AVS_COMMON_UTILS_LOGGER_LOGGERUTILS_H_
#define ALEXA_CLIENT_SDK_AVS_COMMON_UTILS_INCLUDE_AVS_COMMON_UTILS_LOGGER_LOGGERUTILS_H_
/*
 * LoggerUtils.h
 *
 * Copyright 2016-2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include <chrono>

#include "AVSCommon/Utils/Logger/Logger.h"

namespace alexaClientSDK {
namespace avsCommon {
namespace utils {
namespace logger {

/**
 * Log a LEVEL::DEBUG9 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug9(const LogEntry& entry);

/**
 * Log a LEVEL::DEBUG8 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug8(const LogEntry& entry);

/**
 * Log a LEVEL::DEBUG7 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug7(const LogEntry& entry);

/**
 * Log a LEVEL::DEBUG6 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug6(const LogEntry& entry);

/**
 * Log a LEVEL::DEBUG5 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug5(const LogEntry& entry);

/**
 * Log a LEVEL::DEBUG4 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug4(const LogEntry& entry);

/**
 * Log a LEVEL::DEBUG3 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug3(const LogEntry& entry);

/**
 * Log a LEVEL::DEBUG2 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug2(const LogEntry& entry);

/**
 * Log a LEVEL::DEBUG1 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug1(const LogEntry& entry);

/**
 * Log a LEVEL::DEBUG0 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug0(const LogEntry& entry);

/**
 * Log a LEVEL::DEBUG0 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug(const LogEntry& entry);

/**
 * Log a Level::INFO severity entry.
 *
 * @param entry A log entry.
 */
void acsdkInfo(const LogEntry& entry);

/**
 * Log a Level::WARN severity entry.
 *
 * @param entry A log entry.
 */
void acsdkWarn(const LogEntry& entry);

/**
 * Log a Level::ERROR severity entry.
 *
 * @param entry A log entry.
 */
void acsdkError(const LogEntry& entry);

/**
 * Log a Level::CRITICAL severity entry.
 *
 * @param entry A log entry.
 */
void acsdkCritical(const LogEntry& entry);

/**
 * Logs an entry at a severity level using the logger instance defined by
 * ACSDK_GET_LOGGER_FUNCTION (refer to Logger.h).
 *
 * @param level The severity level of the log.
 * @param entry A log entry.
 */
void logEntry(Level level, const LogEntry& entry);

/**
 * Formats a log message into a printable string with other metadata regarding the log message.
 *
 * @param level The severity Level of this log line.
 * @param time The time that the event to log occurred.
 * @param threadMoniker Moniker of the thread that generated the event.
 * @param text The text of the entry to log.
 * @return The formatted string.
 */
std::string formatLogString(
        Level level,
        std::chrono::system_clock::time_point time,
        const char *threadMoniker,
        const char *text);

} // namespace logger
} // namespace utils
} // namespace avsCommon
} // namespace alexaClientSDK

#endif // ALEXA_CLIENT_SDK_AVS_COMMON_UTILS_INCLUDE_AVS_COMMON_UTILS_LOGGER_LOGGERUTILS_H_
