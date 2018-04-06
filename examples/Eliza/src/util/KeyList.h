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

#ifndef ELIZA_KEYLIST_H_
#define ELIZA_KEYLIST_H_

#include "Key.h"
#include "KeyStack.h"
#include "Strings.h"
#include <mindroid/lang/String.h>
#include <mindroid/util/ArrayList.h>

namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

/**
 *  Eliza key list.
 */
class KeyList : public mindroid::ArrayList<sp<Key>> {
public:
    bool add(const sp<String>& key, uint32_t rank, const sp<DecompositionList>& decompositions) {
        return ArrayList<sp<Key>>::add(new Key(key, rank, decompositions));
    }

    void print(size_t indent) {
        for (size_t i = 0; i < size(); i++) {
            sp<Key> k = get(i);
            k->print(indent);
        }
    }

    sp<Key> getKey(const sp<String>& s) {
        for (size_t i = 0; i < size(); i++) {
            sp<Key> key = get(i);
            if (s->equals(key->key())) {
                return key;
            }
        }
        return nullptr;
    }

    /**
     *  Break the string s into words.
     *  For each word, if isKey is true, push the key
     *  into the stack.
     */
    void build(const sp<KeyStack>& stack, sp<String> s) {
        stack->reset();
        s = Strings::trim(s);
        sp<mindroid::StringArray> lines = new mindroid::StringArray(2);
        sp<Key> k;
        while (Strings::match(s, "* *", lines)) {
            k = getKey(lines->get(0));
            if (k != nullptr) {
                stack->pushKey(k);
            }
            s = lines->get(1);
        }
        k = getKey(s);
        if (k != nullptr) {
            stack->pushKey(k);
        }
    }
};

} /* namespace eliza */

#endif /* ELIZA_KEYLIST_H_ */
