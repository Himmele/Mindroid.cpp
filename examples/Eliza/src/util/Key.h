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

#ifndef ELIZA_KEY_H_
#define ELIZA_KEY_H_

#include "DecompositionList.h"
#include <mindroid/lang/String.h>
#include <mindroid/util/ArrayList.h>

namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

/**
 *  Eliza key.
 *  A key has the key itself, a rank, and a list of decompositon rules.
 */
class Key : public Object {
public:
    Key(const sp<String>& key, uint32_t rank, const sp<DecompositionList>& decompositions) :
            mKey(key),
            mRank(rank),
            mDecompositions(decompositions) {
    }

    Key() :
            mKey(nullptr),
            mRank(0),
            mDecompositions(nullptr) {
    }

    void copy(const sp<Key>& k) {
        mKey = k->key();
        mRank = k->rank();
        mDecompositions = k->decompositions();
    }

    void print(size_t indent) {
        for (size_t i = 0; i < indent; i++) {
            printf(" ");
        }
        printf("Key: %s %u\n", mKey->c_str(), mRank);
        mDecompositions->print(indent + 2);
    }

    void printKey(size_t indent) {
        for (size_t i = 0; i < indent; i++) {
            printf(" ");
        }
        printf("Key: %s %u\n", mKey->c_str(), mRank);
    }

    sp<String> key() const {
        return mKey;
    }

    uint32_t rank() const {
        return mRank;
    }

    sp<DecompositionList> decompositions() const {
        return mDecompositions;
    }

private:
    /** The key */
    sp<String> mKey;
    /** The rank */
    uint32_t mRank;
    /** The list of decompositions */
    sp<DecompositionList> mDecompositions;
};

} /* namespace eliza */

#endif /* ELIZA_KEY_H_ */

