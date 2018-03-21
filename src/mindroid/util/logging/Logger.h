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

#ifndef MINDROID_LOGGER_H_
#define MINDROID_LOGGER_H_

#include <mindroid/app/Service.h>
#include <mindroid/lang/Thread.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/logging/ConsoleHandler.h>
#include <mindroid/util/logging/FileHandler.h>

namespace mindroid {

class Logger : public Service {
public:
    static const char* const ACTION_LOG;
    static const char* const ACTION_DUMP_LOG;
    static const char* const ACTION_FLUSH_LOG;
    static const char* const ACTION_CLEAR_LOG;

    class LogWorker : public Thread {
    public:
        LogWorker(const sp<Bundle>& arguments);

        void run() override;
        void open();
        void close();
        void quit();
        bool dumpLog(const sp<String>& fileName);
        void flush();
        void clear();

    private:
        sp<LogBuffer> mLogBuffer;
        int32_t mLogPriority = Log::VERBOSE;
        sp<ArrayList<sp<String>>> mFlags;
        sp<ConsoleHandler> mConsoleHandler;
        sp<FileHandler> mFileHandler;
    };

    void onCreate() override;
    int32_t onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) override;
    void onDestroy() override;
    sp<IBinder> onBind(const sp<Intent>& intent) override;

private:
    void startLogging(const sp<Bundle>& arguments);
    void stopLogging(const sp<Bundle>& arguments);
    void dumpLog(const sp<Bundle>& arguments);
    void flushLog(const sp<Bundle>& arguments);
    void clearLog(const sp<Bundle>& arguments);

    static const char* const TAG;

    sp<HashMap<int32_t, sp<LogWorker>>> mLogWorkers = new HashMap<int32_t, sp<LogWorker>>();
};

} /* namespace mindroid */

#endif /* MINDROID_LOGGER_H_ */
