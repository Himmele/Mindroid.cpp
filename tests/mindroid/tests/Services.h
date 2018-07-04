/*
 * Copyright (C) 2018 E.S.R.Labs
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

#ifndef MINDROID_TESTS_SERVICES_H_
#define MINDROID_TESTS_SERVICES_H_

#include <mindroid/content/ComponentName.h>
#include <mindroid/lang/Boolean.h>
#include <mindroid/util/concurrent/Promise.h>
#include <mindroid/testing/IntegrationTest.h>

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

class Services : public mindroid::IntegrationTest {
public:
    void test1();
    void test2();

    sp<mindroid::Promise<sp<mindroid::ComponentName>>> startService();
    sp<mindroid::Promise<sp<mindroid::Boolean>>> stopService();
};

#endif /* MINDROID_TESTS_SERVICES_H_ */
