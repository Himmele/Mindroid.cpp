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

#ifndef ELIZA_DECOMPOSITION_H_
#define ELIZA_DECOMPOSITION_H_

#include "ReassemblyList.h"
#include <mindroid/lang/String.h>
#include <mindroid/lang/Math.h>
#include <mindroid/util/ArrayList.h>

namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

/**
 *  Eliza decomposition rule
 */
class Decomposition : public Object {
public:
    Decomposition(const sp<String>& pattern, bool memory, const sp<ReassemblyList>& reassemblyList) :
            mPattern(pattern),
            mMemory(memory),
            mReassemblyList(reassemblyList),
            mReassemblyIndex(100) {
    }

    void print(size_t indent) {
        sp<String> m = mMemory ? String::valueOf("true") : String::valueOf("false");
        for (size_t i = 0; i < indent; i++) {
            printf(" ");
        }
        printf("Decomposition: %s %s\n", mPattern->c_str(), m->c_str());
        mReassemblyList->print(indent + 2);
    }

    sp<String> pattern() const {
        return mPattern;
    }

    bool memory() const {
        return mMemory;
    }

    sp<String> nextReassemblyRule() {
        if (mReassemblyList->size() == 0) {
            return nullptr;
        }
        return mReassemblyList->get(mReassemblyIndex);
    }

    /**
     *  Step to the next reassembly rule.
     *  If memory is true, pick a random rule.
     */
    void stepReassemblyRule() {
        size_t size = mReassemblyList->size();
        if (mMemory) {
            mReassemblyIndex = ((uint32_t) ((mindroid::Math::random() * 1000))) % size;
        }
        mReassemblyIndex++;
        if (mReassemblyIndex >= size) {
            mReassemblyIndex = 0;
        }
    }

private:
    /** The decomposition pattern */
    sp<String> mPattern;
    /** The memory flag */
    bool mMemory;
    /** The reassembly list */
    sp<ReassemblyList> mReassemblyList;
    /** The reassembly index */
    uint32_t mReassemblyIndex;
};

} /* namespace eliza */

#endif /* ELIZA_DECOMPOSITION_H_ */
