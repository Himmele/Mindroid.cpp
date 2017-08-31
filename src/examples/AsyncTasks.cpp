#include "mindroid/lang/Closure.h"
#include "mindroid/lang/Thread.h"
#include "mindroid/os/Looper.h"
#include "mindroid/os/Handler.h"
#include "mindroid/os/Message.h"
#include "mindroid/os/AsyncTask.h"
#include "mindroid/os/ServiceManager.h"
#include "mindroid/lang/Class.h"
#include "mindroid/util/Log.h"
#include "mindroid/util/logging/Logger.h"

using namespace mindroid;

CLASS(mindroid, Logger);

class TestAsyncTask : public AsyncTask<int32_t, int32_t, int32_t> {
public:
    virtual void onPreExecute() {
        Log::d("TestAsyncTask", "onPreExecute on thread %d", (int32_t) pthread_self());
    }

    virtual int32_t doInBackground(int32_t param) {
        Log::d("TestAsyncTask", "doInBackground on thread %d with param %d", (int32_t) pthread_self(), param);
        Thread::sleep(250);
        int32_t sum = 0;
        int32_t i;
        for (i = 0; i < param / 2; i++) {
            sum++;
        }
        publishProgress(sum);
        Thread::sleep(250);
        for (; i < param; i++) {
            sum++;
        }
        return sum;
    }

    virtual void onProgressUpdate(int32_t value) {
        Log::d("TestAsyncTask", "onProgressUpdate on thread %d with value %d", (int32_t) pthread_self(), value);
    }

    virtual void onPostExecute(int32_t result) {
        Log::d("TestAsyncTask", "onPostExecute on thread %d with result %d", (int32_t) pthread_self(), result);
    }

    virtual void onCancelled() {
        Log::d("TestAsyncTask", "onCancelled on thread %d", (int32_t) pthread_self());
    }
};

int main() {
    Looper::prepare();

    sp<ServiceManager> serviceManager = new ServiceManager();
    serviceManager->start();
    sp<IServiceManager> sm = ServiceManager::getServiceManager();

    sp<ArrayList<sp<String>>> logFlags = new ArrayList<sp<String>>();
    logFlags->add(String::valueOf("timestamp"));
    sp<Intent> logger = new Intent(Logger::ACTION_LOG);
    logger->setComponent(new ComponentName("mindroid", "Logger"))
            ->putExtra("name", Context::LOGGER_SERVICE)
            ->putExtra("process", "main")
            ->putExtra("logBuffer", Log::LOG_ID_MAIN)
            ->putExtra("logPriority", Log::DEBUG)
            ->putStringArrayListExtra("logFlags", logFlags)
            ->putExtra("consoleLogging", true);
    serviceManager->startSystemService(logger);

    sp<TestAsyncTask> asyncTask1;
    sp<TestAsyncTask> asyncTask2;
    sp<Handler> handler = new Handler(Looper::myLooper());
    handler->post([&] {
            asyncTask1 = new TestAsyncTask();
            asyncTask1->execute(1234567);

            asyncTask2 = new TestAsyncTask();
            asyncTask2->executeOnExecutor(AsyncTaskBase::THREAD_POOL_EXECUTOR, 123);

            handler->postDelayed([=] {
                Looper::myLooper()->quit();
            }, 2000);
    });
    Looper::loop();

    logger->setComponent(new ComponentName("mindroid", "Logger"));
    sm->stopSystemService(logger);

    serviceManager->shutdown();

    return 0;
}
