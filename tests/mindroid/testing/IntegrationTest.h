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

#ifndef MINDROID_TESTING_INTEGRATIONTEST_H_
#define MINDROID_TESTING_INTEGRATIONTEST_H_

#include <gtest/gtest.h>
#include <mindroid/lang/Object.h>

namespace mindroid {

class ServiceManager;

class IntegrationTest : public ::testing::Test {
public:
    IntegrationTest() = default;
    virtual ~IntegrationTest() = default;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static void startSystemSerices();
    static void startServices();
    static void shutdownSystemSerices();
    static void shutdownServices();

    static const char* const TAG;
    static sp<ServiceManager> sServiceManager;
};

} /* namespace mindroid */

#endif /* MINDROID_TESTING_INTEGRATIONTEST_H_ */
