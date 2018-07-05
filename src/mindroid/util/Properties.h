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

#ifndef MINDROID_UTIL_PROPERTIES_H_
#define MINDROID_UTIL_PROPERTIES_H_

namespace mindroid {

/**
 * The {@code Properties} class contains a set of Mindroid properties.
 */
class Properties final {
public:
    static const char* const DEVELOPER_MODE;
    static const char* const INTEGRATION_TESTING;

    Properties() noexcept = delete;
    ~Properties() noexcept = delete;
    Properties(const Properties&) = delete;
    Properties& operator=(const Properties&) = delete;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_PROPERTIES_H_ */
