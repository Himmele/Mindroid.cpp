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

#ifndef ELIZA_SYNONYMLIST_H_
#define ELIZA_SYNONYMLIST_H_

#include "WordList.h"
#include "Strings.h"
#include <mindroid/lang/String.h>
#include <mindroid/lang/StringArray.h>
#include <mindroid/util/ArrayList.h>

namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

/**
 *  Eliza synonym list.
 */
class SynonymList : public mindroid::ArrayList<sp<WordList>> {
public:
    bool add(const sp<WordList>& words) {
        return ArrayList<sp<WordList>>::add(words);
    }

    void print(size_t indent) {
        for (size_t i = 0; i < size(); i++) {
            for (size_t j = 0; j < indent; j++) {
                printf(" ");
            }
            printf("Synonyms: ");
            sp<WordList> wl = get(i);
            wl->print(indent);
        }
    }

    /**
     *  Find a synonym word list.
     */
    sp<WordList> find(const sp<String>& s) {
        for (size_t i = 0; i < size(); i++) {
            sp<WordList> wl = get(i);
            if (wl->contains(s)) {
                return wl;
            }
        }
        return nullptr;
    }
    
    /**
     *  Decomposition match, if decomposition has no synonyms,
     *  do a regular match.
     *  Otherwise, try all synonyms.
     */
    bool matchDecomposition(const sp<String>& string, sp<String> pattern, const sp<mindroid::StringArray>& lines) {
        if (!Strings::match(pattern, "*@* *", lines)) {
            //  No synonyms in decomposition pattern.
            return Strings::match(string, pattern, lines);
        }
        //  Decomposition pattern has synonym.
        sp<String> first = lines->get(0);
        sp<String> synonym = lines->get(1);
        sp<String> rest = String::format(" %s", lines->get(2)->c_str());
        //  Look up the synonym.
        sp<WordList> wl = find(synonym);
        if (wl == nullptr) {
            printf("Cannot find synonym list for %s\n", synonym->c_str());
            return false;
        }
        //  Try each synonym individually.
        for (size_t i = 0; i < wl->size(); i++) {
            //  Make a modified pattern
            pattern = String::format("%s%s%s", first->c_str(), wl->get(i)->c_str(), rest->c_str());
            if (Strings::match(string, pattern, lines)) {
                size_t n = Strings::count(first, '*');
                //  Make room for the synonym in the match list.
                for (size_t j = lines->size() - 2; j >= n; j--) {
                    lines->set(j + 1, lines->get(j));
                }
                //  Put the synonym in the match list.
                lines->set(n, wl->get(i));
                return true;
            }
        }
        return false;
    }
};

} /* namespace eliza */

#endif /* ELIZA_SYNONYMLIST_H_ */
