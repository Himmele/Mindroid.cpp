/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 * 
 *  Charles Hayden's implementation of ELIZA (by Joseph Weizenbaum): http://www.chayden.net/eliza/Eliza.html.
 */

#ifndef ELIZA_PREPOSTENTRY_H_
#define ELIZA_PREPOSTENTRY_H_

#include <mindroid/lang/String.h>
#include <mindroid/util/ArrayList.h>

namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

/**
 *  Eliza pre-post entry (two words).
 */
class PrePostEntry : public Object {
public:
    PrePostEntry(const sp<String>& source, const sp<String>& destination) :
            mSource(source),
            mDestination(destination) {
    }

    void print(size_t indent) {
        for (size_t i = 0; i < indent; i++) {
            printf(" ");
        }
        printf("Pre-post entry: %s  %s", mSource->c_str(), mDestination->c_str());
    }

    sp<String> source() const {
        return mSource;
    }

    sp<String> destination() const {
        return mDestination;
    }

private:
    /** The words */
    sp<String> mSource;
    sp<String> mDestination;
};

} /* namespace eliza */

#endif /* ELIZA_PREPOSTENTRY_H_ */
