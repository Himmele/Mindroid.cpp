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

#include <mindroid/util/logging/LoggerService.h>
#include <mindroid/content/Intent.h>
#include <mindroid/os/Environment.h>
#include <mindroid/os/IRemoteCallback.h>
#include <mindroid/lang/Boolean.h>
#include <mindroid/lang/System.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/Properties.h>
#include <cstdio>

namespace mindroid {

const char* const LoggerService::TAG = "Logger";

void LoggerService::LoggerThread::run() {
    sp<HashMap<int32_t, sp<Promise<sp<LogBuffer::LogRecord>>>>> loggers = new HashMap<int32_t, sp<Promise<sp<LogBuffer::LogRecord>>>>();

    while (!isInterrupted()) {
        {
            AutoLock autoLock(mService->mLock);
            auto itr = mService->mLogHandlers->iterator();
            while (itr.hasNext()) {
                auto entry = itr.next();
                const int32_t id = entry.getKey();
                if (!loggers->containsKey(id)) {
                    sp<LogBuffer> logBuffer = Log::getLogBuffer(id);
                    loggers->put(id, logBuffer->get());
                }
            }
            mWaitForLogs = Promise<sp<LogBuffer::LogRecord>>::anyOf(loggers->values());
        }

        try {
            if (isInterrupted()) {
                break;
            }
            mWaitForLogs->get();
        } catch (const CancellationException& ignore) {
        } catch (const ExecutionException& ignore) {
        }

        auto itr = loggers->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            sp<Promise<sp<LogBuffer::LogRecord>>> promise = entry.getValue();
            if (promise->isDone()) {
                if (!promise->isCompletedExceptionally()) {
                    sp<LogBuffer::LogRecord> logRecord;
                    try {
                        logRecord = promise->get();
                    } catch (const CancellationException& ignore) {
                    } catch (const ExecutionException& ignore) {
                    }
                    if (logRecord != nullptr) {
                        AutoLock autoLock(mService->mLock);
                        sp<ArrayList<sp<LogHandler>>> handlers = mService->mLogHandlers->get(entry.getKey());
                        if (handlers != nullptr) {
                            auto itr = handlers->iterator();
                            while (itr.hasNext()) {
                                auto handler = itr.next();
                                handler->publish(logRecord);
                            }
                        }
                    }
                }
                itr.remove();
            }
        }
    }

    Log::println('I', TAG, "Logger has been shut down");

    mService.clear();
}

void LoggerService::LoggerThread::reset() {
    AutoLock autoLock(mService->mLock);
    if (mWaitForLogs != nullptr) {
        mWaitForLogs->complete(nullptr);
    }
}

void LoggerService::LoggerThread::quit() {
    interrupt();
    reset();
    join();
}

void LoggerService::onCreate() {
    const sp<String> integrationTesting = System::getProperty(Properties::INTEGRATION_TESTING);
    if (integrationTesting != nullptr && Boolean::valueOf(integrationTesting)->booleanValue()) {
        mTestHandler = new TestHandler();
        sp<ArrayList<sp<LogHandler>>> handlers = new ArrayList<sp<LogHandler>>();
        handlers->add(mTestHandler);
        const int32_t id = Log::LOG_ID_TEST;
        AutoLock autoLock(mLock);
        mLogHandlers->put(id, handlers);
    }
    mLogger->start();
    ServiceManager::addService(Context::LOGGER_SERVICE, mBinder);
}

int32_t LoggerService::onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) {
    sp<String> action = intent->getAction();
    if (action != nullptr) {
        if (action->equals(Logger::ACTION_LOG)) {
            stopLogging(intent->getExtras());
            startLogging(intent->getExtras());
        } else if (action->equals(Logger::ACTION_DUMP_LOG)) {
            dumpLog(intent->getExtras());
        } else if (action->equals(Logger::ACTION_FLUSH_LOG)) {
            flushLog(intent->getExtras());
        } else if (action->equals(Logger::ACTION_CLEAR_LOG)) {
            clearLog(intent->getExtras());
        }
    }

    return 0;
}

void LoggerService::onDestroy() {
    ServiceManager::removeService(mBinder);
    mLogger->quit();
    if (mTestHandler != nullptr) {
        mTestHandler->clear();
    }
    Log::println('D', TAG, "Flushing logs");
    AutoLock autoLock(mLock);
    auto itr = mLogHandlers->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        sp<LogBuffer> logBuffer = Log::getLogBuffer(entry.getKey());
        logBuffer->reset();
        sp<ArrayList<sp<LogHandler>>> handlers = entry.getValue();
        auto itr = handlers->iterator();
        while (itr.hasNext()) {
            sp<LogHandler> handler = itr.next();
            handler->flush();
            handler->close();
        }
        itr.remove();
    }
}

sp<IBinder> LoggerService::onBind(const sp<Intent>& intent) {
    return mBinder;
}

void LoggerService::startLogging(const sp<Bundle>& arguments) {
    const int32_t logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    AutoLock autoLock(mLock);
    if (!mLogHandlers->containsKey(logBuffer)) {
        Log::println('D', TAG, "Starting logging {%d}", logBuffer);
        try {
            if (Log::getLogBuffer(logBuffer) == nullptr) {
                throw IllegalArgumentException(String::format("Invalid log buffer: %d", logBuffer));
            }

            sp<ArrayList<sp<LogHandler>>> logHandlers = new ArrayList<sp<LogHandler>>();
            int32_t priority = arguments->getInt("logPriority", Log::INFO);

            sp<ArrayList<sp<String>>> flags = arguments->getStringArrayList("logFlags");
            bool consoleLogging = arguments->getBoolean("consoleLogging", false);
            if (consoleLogging) {
                sp<ConsoleHandler> consoleHandler = new ConsoleHandler();
                if (flags != nullptr && flags->contains(String::valueOf("timestamp"))) {
                    consoleHandler->setFlag(ConsoleHandler::FLAG_TIMESTAMP);
                }
                consoleHandler->setPriority(priority);
                logHandlers->add(consoleHandler);
            } else {
                Log::println('D', TAG, "Console logging: disabled");
            }

            bool fileLogging = arguments->getBoolean("fileLogging", false);
            if (fileLogging) {
                sp<String> directory = arguments->getString("logDirectory", Environment::getLogDirectory()->getAbsolutePath());
                sp<String> fileName = arguments->getString("logFileName", String::format("%s%s", String::format("Log%d", logBuffer)->c_str(), "-%g.log"));
                int32_t fileLimit = arguments->getInt("logFileLimit", 262144);
                int32_t fileCount = arguments->getInt("logFileCount", 4);
                int32_t bufferSize = arguments->getInt("logBufferSize", 0);
                int32_t dataVolumeLimit = arguments->getInt("logDataVolumeLimit", 0);
                sp<FileHandler> fileHandler = new FileHandler(String::format("%s%c%s", directory->c_str(), File::separatorChar, fileName->c_str()), fileLimit, fileCount, true,
                        bufferSize, dataVolumeLimit);
                fileHandler->setPriority(priority);
                logHandlers->add(fileHandler);
            }

            if (!logHandlers->isEmpty()) {
                mLogHandlers->put(logBuffer, logHandlers);
                mLogger->reset();
                Log::println('D', TAG, "Logging has been started {%d}", logBuffer);
            } else {
                Log::println('D', TAG, "Logging has been disabled {%d}", logBuffer);
            }
        } catch (const Exception& e) {
            Log::println('E', TAG, "Failed to start logging {%d}", logBuffer);
        }
    }
}

void LoggerService::stopLogging(const sp<Bundle>& arguments) {
    const int32_t logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    AutoLock autoLock(mLock);
    if (mLogHandlers->containsKey(logBuffer)) {
        Log::println('D', TAG, "Stopping logging {%d}", logBuffer);
        sp<ArrayList<sp<LogHandler>>> logHandlers = mLogHandlers->get(logBuffer);
        auto itr = logHandlers->iterator();
        while (itr.hasNext()) {
            sp<LogHandler> handler = itr.next();
            handler->close();
        }
        mLogHandlers->remove(logBuffer);
        Log::println('D', TAG, "Logging has been stopped {%d}", logBuffer);
    }
}

void LoggerService::dumpLog(const sp<Bundle>& arguments) {
    const int32_t logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    sp<String> fileName = arguments->getString("fileName");
    sp<IRemoteCallback> callback = binder::RemoteCallback::Stub::asInterface(arguments->getBinder("binder"));
    AutoLock autoLock(mLock);
    if (mLogHandlers->containsKey(logBuffer)) {
        Log::println('D', TAG, "Dumping log to file %s {%d}", fileName->c_str(), logBuffer);
        sp<ArrayList<sp<LogHandler>>> logHandlers = mLogHandlers->get(logBuffer);
        sp<Bundle> result = new Bundle();
        auto itr = logHandlers->iterator();
        while (itr.hasNext()) {
            sp<LogHandler> handler = itr.next();
            if (Class<FileHandler>::isInstance(handler)) {
                if (object_cast<FileHandler>(handler)->dump(fileName)) {
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
    }
}

void LoggerService::flushLog(const sp<Bundle>& arguments) {
    const int32_t logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    AutoLock autoLock(mLock);
    if (mLogHandlers->containsKey(logBuffer)) {
        Log::println('D', TAG, "Flushing log {%d}", logBuffer);
        sp<ArrayList<sp<LogHandler>>> logHandlers = mLogHandlers->get(logBuffer);
        auto itr = logHandlers->iterator();
        while (itr.hasNext()) {
            sp<LogHandler> handler = itr.next();
            handler->flush();
        }
    }
}

void LoggerService::clearLog(const sp<Bundle>& arguments) {
    const int32_t logBuffer = arguments->getInt("logBuffer", Log::LOG_ID_MAIN);
    AutoLock autoLock(mLock);
    if (mLogHandlers->containsKey(logBuffer)) {
        Log::println('D', TAG, "Clearing log {%d}", logBuffer);
        sp<ArrayList<sp<LogHandler>>> logHandlers = mLogHandlers->get(logBuffer);
        auto itr = logHandlers->iterator();
        while (itr.hasNext()) {
            sp<LogHandler> handler = itr.next();
            if (Class<FileHandler>::isInstance(handler)) {
                object_cast<FileHandler>(handler)->clear();
            }
        }
    }
}

sp<Promise<sp<String>>> LoggerService::assumeThat(const sp<String>& tag, const sp<String>& message, int64_t timeout) {
    if (mTestHandler != nullptr) {
        return mTestHandler->assumeThat(tag, message, timeout);
    } else {
        return new Promise<sp<String>>(sp<Exception>(new ExecutionException()));
    }
}

void LoggerService::mark() {
    if (mTestHandler != nullptr) {
        mTestHandler->mark();
    }
}

void LoggerService::reset() {
    if (mTestHandler != nullptr) {
        mTestHandler->reset();
    }
}

void LoggerService::TestHandler::publish(const sp<LogBuffer::LogRecord>& logRecord) {
    AutoLock autoLock(mLock);
    mLogHistory->add(logRecord);
    auto itr = mAssumptions->iterator();
    while (itr.hasNext()) {
        sp<Assumption> assumption = itr.next();
        if (assumption->match(logRecord->getPriority(), logRecord->getTag(), logRecord->getMessage())) {
            itr.remove();
        }
    }
}

sp<Promise<sp<String>>> LoggerService::TestHandler::assumeThat(const sp<String>& tag, const sp<String>& message, uint64_t timeout) {
    AutoLock autoLock(mLock);
    sp<Assumption> assumption = new Assumption(tag, message);
    if (matchLogHistory(assumption)) {
        try {
            Log::println('D', TAG, "Log assumption success: %s", assumption->get()->c_str());
        } catch (const CancellationException& ignore) {
        } catch (const ExecutionException& ignore) {
        }
        return assumption;
    } else {
        mAssumptions->add(assumption);
        sp<Promise<sp<String>>> p = assumption->orTimeout(timeout, String::format("Log assumption timeout: %s", assumption->toString()->c_str()))
        ->catchException([=] (const sp<Exception>& exception) {
            AutoLock autoLock(mLock);
            mAssumptions->remove(assumption);
        })
        ->then([=] (const sp<String>& value, const sp<Exception>& exception) {
            if (exception == nullptr) {
                Log::println('D', TAG, "Log assumption success: %s", value->c_str());
            } else {
                Log::println('E', TAG, "Log assumption timeout: %s", assumption->toString()->c_str());
            }
        });
        return p;
    }
}

void LoggerService::TestHandler::clear() {
    AutoLock autoLock(mLock);
    auto itr = mAssumptions->iterator();
    while (itr.hasNext()) {
        sp<Assumption> assumption = itr.next();
        assumption->cancel();
    }
    mAssumptions->clear();
    mLogHistory->clear();
}

void LoggerService::TestHandler::mark() {
    AutoLock autoLock(mLock);
    mMark = mLogHistory->size();
}

void LoggerService::TestHandler::reset() {
    AutoLock autoLock(mLock);
    auto itr = mAssumptions->iterator();
    while (itr.hasNext()) {
        sp<Assumption> assumption = itr.next();
        assumption->cancel();
    }
    mAssumptions->clear();
    while (mLogHistory->size() > mMark) {
        mLogHistory->remove(mLogHistory->size() - 1);
    }
}

bool LoggerService::TestHandler::matchLogHistory(const sp<Assumption>& assumption) {
    auto itr = mLogHistory->iterator();
    while (itr.hasNext()) {
        sp<LogBuffer::LogRecord> logRecord = itr.next();
        if (logRecord->getTag()->matches(assumption->mTag) && logRecord->getMessage()->matches(assumption->mMessage)) {
            return assumption->complete(String::format("%c/%s: %s", Logger::LOG_LEVELS[logRecord->getPriority()], logRecord->getTag()->c_str(), logRecord->getMessage()->c_str()));
        }
    }
    return false;
}

bool LoggerService::TestHandler::Assumption::match(int32_t priority, const sp<String>& tag, const sp<String>& message) {
    if (tag->matches(mTag) && message->matches(mMessage)) {
        return complete(String::format("%c/%s: %s", Logger::LOG_LEVELS[priority], tag->c_str(), message->c_str()));
    }
    return false;
}

} /* namespace mindroid */
