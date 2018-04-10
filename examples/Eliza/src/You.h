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

#ifndef EXAMPLES_YOU_H_
#define EXAMPLES_YOU_H_

#include <mindroid/app/Service.h>

namespace examples {

using mindroid::sp;

class You :
        public mindroid::Service {
public:
    void onCreate() override;

    void onDestroy() override;

    sp<mindroid::IBinder> onBind(const sp<mindroid::Intent>& intent) override;

private:
    static const char* const TAG;
};

} /* namespace examples */

#endif /* EXAMPLES_YOU_H_ */