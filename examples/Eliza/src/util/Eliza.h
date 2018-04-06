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

#ifndef ELIZA_ELIZA_H_
#define ELIZA_ELIZA_H_

#include "WordList.h"
#include "KeyList.h"
#include "KeyStack.h"
#include "SynonymList.h"
#include "PrePostEntry.h"
#include "PrePostList.h"
#include "Memory.h"
#include "DecompositionList.h"
#include "ReassemblyList.h"
#include <mindroid/lang/String.h>
#include <mindroid/util/ArrayList.h>

namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

/**
 *  Eliza main class.
 *  Performs the input transformations.
 */
class Eliza : public Object {
public:
    Eliza();

    sp<String> talk(const char* s) {
        return talk(String::valueOf(s));
    }

    sp<String> talk(sp<String> s);

    void print();

private:
    void processConfigurationRule(sp<String> s);

    /**
     *  Process a sentence.
     *  (1) Make pre transformations.
     *  (2) Check for quit word.
     *  (3) Scan sentence for keys, build key stack.
     *  (4) Try decompositions for each key.
     */
    sp<String> sentence(sp<String> s);

    /**
     *  Decompose a string according to the given key.
     *  Try each decomposition rule in order.
     *  If it matches, assemble a reply and return it.
     *  If assembly fails, try another decomposition rule.
     *  If assembly is a goto rule, return null and give the key.
     *  If assembly succeeds, return the reply;
     */
    sp<String> decompose(const sp<Key>& key, const sp<String>& s, const sp<Key>& gotoKey);

    /**
     *  Assembly a reply from a decomposition rule and the input.
     *  If the reassembly rule is a goto rule, return null and give the gotoKey to use.
     *  Otherwise return the response.
     */
    sp<String> assemble(const sp<Decomposition>& d, const sp<mindroid::StringArray>& reply, const sp<Key>& gotoKey);
    
    static const char* const TAG;

    /** The key list */
    sp<KeyList> keys = new KeyList();
    /** The synonym list */
    sp<SynonymList> synonyms = new SynonymList();
    /** The pre list */
    sp<PrePostList> preList = new PrePostList();
    /** The post list */
    sp<PrePostList> postList = new PrePostList();
    /** Welcome string */
    sp<String> welcome = String::valueOf("Hello.");
    /** Goodbye string */
    sp<String> goodbye = String::valueOf("Goodbye.");
    /** Quit list */
    sp<WordList> quit = new WordList();
    /** Key stack */
    sp<KeyStack> keyStack = new KeyStack();
    /** Memory */
    sp<Memory> memory = new Memory();

    sp<DecompositionList> decompositionList;
    sp<ReassemblyList> reassemblyList;
};

} /* namespace eliza */

#endif /* ELIZA_ELIZA_H_ */
