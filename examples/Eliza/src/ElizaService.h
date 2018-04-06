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

#ifndef EXAMPLES_ELIZA_SERVICE_H_
#define EXAMPLES_ELIZA_SERVICE_H_

#include "IEliza.h"
#include <mindroid/app/Service.h>

namespace eliza {
class Eliza;
}

namespace examples {

using mindroid::sp;

class ElizaService :
        public mindroid::Service {
public:
    void onCreate() override;

    void onDestroy() override;

    sp<mindroid::IBinder> onBind(const sp<mindroid::Intent>& intent) override;

private:
    class Stub : public binder::Eliza::Stub {
    public:
        Stub();
        sp<String> ask1(const sp<String>& question) override;
        sp<mindroid::Promise<sp<String>>> ask2(const sp<String>& question) override;
        void ask3(const sp<String> question, const sp<IElizaListener>& listener) override;

    private:
        sp<String> talk(const sp<String>& input);

        sp<mindroid::Handler> mHandler;
        sp<eliza::Eliza> mEliza;
    };

    sp<binder::Eliza::Stub> mBinder;
};

} /* namespace examples */

#endif /* EXAMPLES_ELIZA_SERVICE_H_ */
