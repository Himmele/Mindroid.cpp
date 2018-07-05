/*
 * Copyright (C) 2018 Daniel Himmelein
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

#include <mindroid/app/Service.h>
#include <mindroid/os/AsyncTask.h>

namespace examples {
namespace concurrency {

using mindroid::sp;

class AsyncTaskExample :
        public mindroid::Service {
public:
    AsyncTaskExample();
    virtual ~AsyncTaskExample();

    void onCreate() override;

    void onDestroy() override;

    sp<mindroid::IBinder> onBind(const sp<mindroid::Intent>& intent) override;

    bool onUnbind(const sp<mindroid::Intent>& intent) override;

private:
    class Task : public mindroid::AsyncTask<std::vector<int32_t>, int32_t, int32_t> {
    public:
        void onPreExecute() override;

        int32_t doInBackground(std::vector<int32_t> params) override;

        void onProgressUpdate(int32_t value) override;

        void onPostExecute(int32_t result) override;

        void onCancelled() override;
    };

    static const char* const TAG;
};

} /* namespace concurrecny */
} /* namespace examples */
