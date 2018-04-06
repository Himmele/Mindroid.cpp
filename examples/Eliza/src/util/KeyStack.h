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

#ifndef ELIZA_KEYSTACK_H_
#define ELIZA_KEYSTACK_H_

#include "Key.h"
#include <mindroid/lang/String.h>
#include <mindroid/util/ArrayList.h>

namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

/**
 *  A stack of keys in rank order.
 */
class KeyStack : public Object {
public:
    void print() {
        printf("Stack: %u\n", mTop);
        for (size_t i = 0; i < mTop; i++) {
            mStack[i]->printKey(0);
        }
    }

    uint32_t top() {
        return mTop;
    }

    void reset() {
        mTop = 0;
    }

    sp<Key> key(size_t n) {
        if (n >= mTop) {
            return nullptr;
        }
        return mStack[n];
    }

    void pushKey(const sp<Key>& key) {
        if (key == nullptr) {
            return;
        }
        size_t i;
        for (i = mTop; i > 0; i--) {
            if (key->rank() > mStack[i - 1]->rank()) {
                mStack[i] = mStack[i - 1];
            } else {
                break;
            }
        }
        mStack[i] = key;
        mTop++;
    }

private:
    /** The stack size */
    static const size_t SIZE = 20;
    /** The key stack */
    sp<Key>* mStack = new sp<Key>[SIZE];
    /** The key stack top */
    uint32_t mTop = 0;
};

} /* namespace eliza */

#endif /* ELIZA_KEYSTACK_H_ */
