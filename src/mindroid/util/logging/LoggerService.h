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

#ifndef MINDROID_UTIL_LOGGING_LOGGERSERVICE_H_
#define MINDROID_UTIL_LOGGING_LOGGERSERVICE_H_

#include <mindroid/app/Service.h>
#include <mindroid/lang/Thread.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/LinkedList.h>
#include <mindroid/util/concurrent/locks/ReentrantLock.h>
#include <mindroid/util/logging/Logger.h>
#include <mindroid/util/logging/ConsoleHandler.h>
#include <mindroid/util/logging/FileHandler.h>
#include <mindroid/util/logging/ILogger.h>

namespace mindroid {

class LoggerService : public Service {
public:
    class LoggerThread : public Thread {
    public:
        LoggerThread(const sp<LoggerService>& service) : Thread("Logger"), mService(service) {
        }

        void run() override;
        void reset();
        void quit();

    private:
        sp<LoggerService> mService;
        sp<Promise<sp<LogBuffer::LogRecord>>> mWaitForLogs = new Promise<sp<LogBuffer::LogRecord>>();
    };

    void onCreate() override;
    int32_t onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) override;
    void onDestroy() override;
    sp<IBinder> onBind(const sp<Intent>& intent) override;

private:
    class LoggerImpl : public binder::Logger::Stub {
    public:
        LoggerImpl(const sp<LoggerService>& service) : mService(service) { }

        sp<Promise<sp<String>>> assumeThat(const sp<String>& tag, const sp<String>& message, int64_t timeout) override {
            return mService->assumeThat(tag, message, timeout);
        }

    private:
        sp<LoggerService> mService;
    };

    class TestHandler : public LogHandler {
    public:
        class Assumption : public Promise<sp<String>> {
        public:
            Assumption(const sp<String>& tag, const sp<String>& message) : mTag(tag), mMessage(message) {
            }

            bool match(int32_t priority, const sp<String>& tag, const sp<String>& message);

            sp<String> toString() {
                return String::format("%s: %s", mTag->c_str(), mMessage->c_str());
            }

            sp<String> mTag;
            sp<String> mMessage;
        };

        void close() override {
        }

        void flush() override {
        }

        void publish(const sp<LogBuffer::LogRecord>& logRecord) override ;

        sp<Promise<sp<String>>> assumeThat(const sp<String>& tag, const sp<String>& message, uint64_t timeout);

        void clear();
        void mark();
        void reset();

    private:
        bool matchLogHistory(const sp<Assumption>& assumption);

        sp<LinkedList<sp<LogBuffer::LogRecord>>> mLogHistory = new LinkedList<sp<LogBuffer::LogRecord>>();
        sp<LinkedList<sp<Assumption>>> mAssumptions = new LinkedList<sp<Assumption>>();
        sp<Lock> mLock = new ReentrantLock();
        size_t mMark = 0;
    };

    sp<binder::Logger::Stub> mBinder = new LoggerImpl(this);

    void startLogging(const sp<Bundle>& arguments);
    void stopLogging(const sp<Bundle>& arguments);
    void dumpLog(const sp<Bundle>& arguments);
    void flushLog(const sp<Bundle>& arguments);
    void clearLog(const sp<Bundle>& arguments);
    sp<Promise<sp<String>>> assumeThat(const sp<String>& tag, const sp<String>& message, int64_t timeout);

    static const char* const TAG;

    sp<LoggerThread> mLogger = new LoggerThread(this);
    sp<HashMap<int32_t, sp<ArrayList<sp<LogHandler>>>>> mLogHandlers = new HashMap<int32_t, sp<ArrayList<sp<LogHandler>>>>();
    sp<TestHandler> mTestHandler;
    sp<Lock> mLock = new ReentrantLock();
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_LOGGING_LOGGERSERVICE_H_ */
