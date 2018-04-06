/*
 * Copyright (C) 2012 Daniel Himmelein
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <mindroid/util/logging/Logger.h>
#include <mindroid/content/Intent.h>
#include <mindroid/os/Environment.h>
#include <mindroid/os/IRemoteCallback.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <cstdio>

namespace mindroid {

const char* const Logger::TAG = "Logger";
const char* const Logger::ACTION_LOG = "mindroid.util.logging.LOG";
const char* const Logger::ACTION_DUMP_LOG = "mindroid.util.logging.DUMP_LOG";
const char* const Logger::ACTION_FLUSH_LOG = "mindroid.util.logging.FLUSH_LOG";
const char* const Logger::ACTION_CLEAR_LOG = "mindroid.util.logging.CLEAR_LOG";

Logger::LogWorker::LogWorker(const sp<Bundle>& arguments) :
        Thread(Logger::TAG) {
    int32_t logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    mLogBuffer = Log::getLogBuffer(logBuffer);
    if (mLogBuffer == nullptr) {
        throw IllegalArgumentException(String::format("Invalid log buffer: %d", logBuffer));
    }
    mLogPriority = arguments->getInt("logPriority", Log::INFO);
    mFlags = arguments->getStringArrayList("logFlags");
    bool consoleLogging = arguments->getBoolean("consoleLogging", false);
    if (consoleLogging) {
        mConsoleHandler = new ConsoleHandler();
        if (mFlags != nullptr && mFlags->contains(String::valueOf("timestamp"))) {
            mConsoleHandler->setFlag(ConsoleHandler::FLAG_TIMESTAMP);
        }
    } else {
        Log::println('D', TAG, "Console logging: disabled");
    }
    bool fileLogging = arguments->getBoolean("fileLogging", false);
    if (fileLogging) {
        sp<String> directory = arguments->getString("logDirectory", Environment::getLogDirectory()->getAbsolutePath());
        sp<String> fileName = arguments->getString("logFileName", String::format("%s%s", String::format("Log%d", mLogBuffer->getId())->c_str(), "-%g.log"));
        int fileLimit = arguments->getInt("logFileLimit", 262144);
        int fileCount = arguments->getInt("logFileCount", 4);
        int bufferSize = arguments->getInt("logBufferSize", 0);
        int dataVolumeLimit = arguments->getInt("logDataVolumeLimit", 0);
        mFileHandler = new FileHandler(String::format("%s%c%s", directory->c_str(), File::separatorChar, fileName->c_str()), fileLimit, fileCount, true,
                bufferSize, dataVolumeLimit);
    }

    if (mConsoleHandler == nullptr && mFileHandler == nullptr) {
        throw Exception("Illegal logging configuration");
    }
}

void Logger::LogWorker::run() {
    open();

    while (!isInterrupted()) {
        sp<LogBuffer::LogRecord> logRecord = mLogBuffer->take(mLogPriority);
        if (logRecord != nullptr) {
            if (mConsoleHandler != nullptr) {
                mConsoleHandler->publish(logRecord);
            }
            if (mFileHandler != nullptr) {
                mFileHandler->publish(logRecord);
            }
        }
    }

    close();
}

void Logger::LogWorker::open() {
}

void Logger::LogWorker::close() {
    if (mConsoleHandler != nullptr) {
        mConsoleHandler->close();
        mConsoleHandler = nullptr;
    }

    if (mFileHandler != nullptr) {
        mFileHandler->close();
        mFileHandler = nullptr;
    }
}

void Logger::LogWorker::quit() {
    interrupt();
    mLogBuffer->quit();
    join();
}

bool Logger::LogWorker::dumpLog(const sp<String>& fileName) {
    if (mFileHandler != nullptr) {
        return mFileHandler->dump(fileName);
    } else {
        return false;
    }
}

void Logger::LogWorker::flush() {
    if (mFileHandler != nullptr) {
        mFileHandler->flush();
    }
}

void Logger::LogWorker::clear() {
    if (mFileHandler != nullptr) {
        mFileHandler->clear();
    }
}

void Logger::onCreate() {
}

int32_t Logger::onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) {
    sp<String> action = intent->getAction();
    if (action != nullptr) {
        if (action->equals(ACTION_LOG)) {
            stopLogging(intent->getExtras());
            startLogging(intent->getExtras());
        } else if (action->equals(ACTION_DUMP_LOG)) {
            dumpLog(intent->getExtras());
        } else if (action->equals(ACTION_FLUSH_LOG)) {
            flushLog(intent->getExtras());
        } else if (action->equals(ACTION_CLEAR_LOG)) {
            clearLog(intent->getExtras());
        }
    }

    return 0;
}

void Logger::onDestroy() {
    Log::println('D', TAG, "Flushing logs");
    auto itr = mLogWorkers->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        sp<LogWorker> logWorker = entry.getValue();
        logWorker->flush();
        logWorker->quit();
        itr.remove();
    }
}

sp<IBinder> Logger::onBind(const sp<Intent>& intent) {
    return nullptr;
}

void Logger::startLogging(const sp<Bundle>& arguments) {
    int32_t logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    if (!mLogWorkers->containsKey(logBuffer)) {
        Log::println('D', TAG, "Starting logging {%d}", logBuffer);
        try {
            sp<LogWorker> logWorker = new LogWorker(arguments);
            mLogWorkers->put(logBuffer, logWorker);
            logWorker->start();
            Log::println('D', TAG, "Logging has been started {%d}", logBuffer);
        } catch (const Exception& e) {
            Log::println('D', TAG, "Logging: disabled {%d}", logBuffer);
        }
    }
}

void Logger::stopLogging(const sp<Bundle>& arguments) {
    int logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    if (mLogWorkers->containsKey(logBuffer)) {
        Log::println('D', TAG, "Stopping logging {%d}", logBuffer);
        sp<LogWorker> logWorker = mLogWorkers->get(logBuffer);
        logWorker->quit();
        mLogWorkers->remove(logBuffer);
        Log::println('D', TAG, "Logging has been stopped {%d}", logBuffer);
    }
}

void Logger::dumpLog(const sp<Bundle>& arguments) {
    int logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    sp<String> fileName = arguments->getString("fileName");
    sp<IRemoteCallback> callback = binder::RemoteCallback::Stub::asInterface(arguments->getBinder("binder"));
    if (mLogWorkers->containsKey(logBuffer)) {
        Log::println('D', TAG, "Dumping log to file %s {%d}", fileName->c_str(), logBuffer);
        sp<LogWorker> logWorker = mLogWorkers->get(logBuffer);
        sp<Bundle> result = new Bundle();
        if (logWorker->dumpLog(fileName)) {
            Log::println('D', TAG, "Log has been dumped to file %s {%d}", fileName->c_str(), logBuffer);
            result->putBoolean("result", true);
        } else {
            Log::println('D', TAG, "Failed to dump log to file %s {%d}", fileName->c_str(), logBuffer);
            result->putBoolean("result", false);
        }
        if (callback != nullptr) {
            try {
                callback->sendResult(result);
            } catch (const RemoteException& ignore) {
            }
        }
    }
}

void Logger::flushLog(const sp<Bundle>& arguments) {
    int logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    if (mLogWorkers->containsKey(logBuffer)) {
        Log::println('D', TAG, "Flushing log {%d}", logBuffer);
        sp<LogWorker> logWorker = mLogWorkers->get(logBuffer);
        logWorker->flush();
    }
}

void Logger::clearLog(const sp<Bundle>& arguments) {
    int logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    if (mLogWorkers->containsKey(logBuffer)) {
        Log::println('D', TAG, "Clearing log {%d}", logBuffer);
        sp<LogWorker> logWorker = mLogWorkers->get(logBuffer);
        logWorker->clear();
    }
}

} /* namespace mindroid */
