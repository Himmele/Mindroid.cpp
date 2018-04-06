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

#ifndef ELIZA_PREPOSTLIST_H_
#define ELIZA_PREPOSTLIST_H_

#include "PrePostEntry.h"
#include <mindroid/lang/String.h>
#include <mindroid/util/ArrayList.h>

namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

/**
 *  Eliza pre-post list.
 *  This list of pre-post entries is used to perform word transformations.
 */
class PrePostList : public mindroid::ArrayList<sp<PrePostEntry>> {
public:
    bool add(const sp<String>& source, const sp<String>& destination) {
        return ArrayList<sp<PrePostEntry>>::add(new PrePostEntry(source, destination));
    }

    void print(size_t indent) {
        for (size_t i = 0; i < size(); i++) {
            sp<PrePostEntry> p = get(i);
            p->print(indent);
        }
    }

    /**
     *  Map a string.
     *  If string matches a source string on the list,
     *  return he corresponding destination.
     *  Otherwise, return the input.
     */
    sp<String> map(const sp<String>& string) {
        for (size_t i = 0; i < size(); i++) {
            sp<PrePostEntry> p = get(i);
            if (string->equals(p->source())) {
                return p->destination();
            }
        }
        return string;
    }

    /**
     *  Translate a string s.
     *  (1) Trim spaces off.
     *  (2) Break s into words.
     *  (3) For each word, substitute matching source word with destination.
     */
    sp<String> translate(sp<String> s) {
        sp<mindroid::StringArray> lines = new mindroid::StringArray(2);
        sp<String> tmp = Strings::trim(s);
        s = String::EMPTY_STRING;
        while (Strings::match(tmp, "* *", lines)) {
            s = s->append(String::format("%s ", map(lines->get(0))->c_str()));
            tmp = Strings::trim(lines->get(1));
        }
        s = s->append(map(tmp));
        return s;
    }
};

} /* namespace eliza */

#endif /* ELIZA_PREPOSTLIST_H_ */
