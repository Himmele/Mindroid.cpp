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

#ifndef ELIZA_MEMORY_H_
#define ELIZA_MEMORY_H_

#include <mindroid/lang/String.h>
#include <mindroid/util/ArrayList.h>

namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

/**
 *  Eliza memory class
 */
class Memory : public Object {
public:
    void save(const sp<String>& str) {
        if (top < SIZE) {
            memory->set(top++, str);
        }
    }

    sp<String> get() {
        if (top == 0) return nullptr;
        sp<String> s = memory->get(0);
        for (size_t i = 0; i < top - 1; i++) {
            memory->set(i, memory->get(i+1));
        }
        top--;
        return s;
    }

private:
    /** The memory size */
    static const size_t SIZE = 20;
    /** The memory */
    sp<mindroid::StringArray> memory = new mindroid::StringArray(SIZE);
    /** The memory top */
    size_t top = 0;
};

} /* namespace eliza */

#endif /* ELIZA_MEMORY_H_ */
