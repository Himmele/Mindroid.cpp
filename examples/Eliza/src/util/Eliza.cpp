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

#include "Eliza.h"
#include <mindroid/lang/Integer.h>
#include <mindroid/lang/NumberFormatException.h>
#include <mindroid/util/Log.h>
#include <fstream>
#include <string>

using namespace mindroid;

namespace eliza {

const char* const Eliza::TAG = "Eliza";
const sp<String> Strings::NUMBERS = String::valueOf("0123456789");

Eliza::Eliza() {
    std::ifstream file("examples/Eliza/res/Eliza.cfg");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            processConfigurationRule(String::valueOf(line.c_str())->trim());
        }
        file.close();
    } else {
        Log::e(TAG, "Cannot load Eliza configuration");
    }
}

void Eliza::processConfigurationRule(sp<String> s) {
    sp<StringArray> lines = new StringArray(4);

    if (Strings::match(s, "*reasmb: *", lines)) {
        if (reassemblyList == nullptr) {
            printf("Error: no reassembly list\n");
            return;
        }
        reassemblyList->add(lines->get(1));
    } else if (Strings::match(s, "*decomp: *", lines)) {
        if (decompositionList == nullptr) {
            printf("Error: no decomposition list\n");
            return;
        }
        reassemblyList = new ReassemblyList();
        sp<String> tmp = lines->get(1);
        if (Strings::match(tmp, "$ *", lines)) {
            decompositionList->add(lines->get(0), true, reassemblyList);
        } else {
            decompositionList->add(tmp, false, reassemblyList);
        }
    } else if (Strings::match(s, "*key: * #*", lines)) {
        decompositionList = new DecompositionList();
        reassemblyList = nullptr;
        int n = 0;
        if (lines->get(2)->length() != 0) {
            try {
                n = Integer::valueOf(lines->get(2))->intValue();
            } catch (const NumberFormatException& e) {
                printf("Number is wrong in key: %s\n", lines->get(2)->c_str());
            }
        }
        keys->add(lines->get(1), n, decompositionList);
    } else if (Strings::match(s, "*key: *", lines)) {
        decompositionList = new DecompositionList();
        reassemblyList = nullptr;
        keys->add(lines->get(1), 0, decompositionList);
    } else if (Strings::match(s, "*synon: * *", lines)) {
        sp<WordList> words = new WordList();
        words->add(lines->get(1));
        s = lines->get(2);
        while (Strings::match(s, "* *", lines)) {
            words->add(lines->get(0));
            s = lines->get(1);
        }
        words->add(s);
        synonyms->add(words);
    } else if (Strings::match(s, "*pre: * *", lines)) {
        preList->add(lines->get(1), lines->get(2));
    } else if (Strings::match(s, "*post: * *", lines)) {
        postList->add(lines->get(1), lines->get(2));
    } else if (Strings::match(s, "*initial: *", lines)) {
        welcome = lines->get(1);
    } else if (Strings::match(s, "*final: *", lines)) {
        goodbye = lines->get(1);
    } else if (Strings::match(s, "*quit: *", lines)) {
        quit->add(String::format(" %s ", lines->get(1)->c_str()));
    } else {
        printf("Unrecognized input: %s\n", s->c_str());
    }
}

sp<String> Eliza::talk(sp<String> s) {
    sp<String> reply;
    //  Do some input transformations.
    s = Strings::translate(s, "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                             "abcdefghijklmnopqrstuvwxyz");
    s = Strings::translate(s, "@#$%^&*()_-+=~`{[}]|:;<>\\\"",
                             "                          "  );
    s = Strings::translate(s, ",?!", "...");
    //  Remove multiple spaces.
    s = Strings::compress(s);
    sp<StringArray> lines = new StringArray(2);
    //  Break apart sentences, and do each separately.
    while (Strings::match(s, "*.*", lines)) {
        reply = sentence(lines->get(0));
        if (reply != nullptr) {
            return reply;
        }
        s = Strings::trim(lines->get(1));
    }
    if (s->length() != 0) {
        reply = sentence(s);
        if (reply != nullptr) {
            return reply;
        }
    }
    //  Nothing matched, so try memory.
    sp<String> m = memory->get();
    if (m != nullptr) {
        return m;
    }

    //  No memory, reply with xnone.
    sp<Key> key = keys->getKey(String::valueOf("xnone"));
    if (key != nullptr) {
        sp<Key> dummy = nullptr;
        reply = decompose(key, s, dummy);
        if (reply != nullptr) {
            return reply;
        }
    }
    //  No xnone, just say anything.
    return String::valueOf("I am at a loss for words.");
}

sp<String> Eliza::sentence(sp<String> s) {
    s = preList->translate(s);
    s = Strings::pad(s);
    if (quit->contains(s)) {
        return goodbye;
    }
    keys->build(keyStack, s);
    for (size_t i = 0; i < keyStack->top(); i++) {
        sp<Key> gotoKey = new Key();
        sp<String> reply = decompose(keyStack->key(i), s, gotoKey);
        if (reply != nullptr) {
            return reply;
        }
        //  If decomposition returned gotoKey, try it.
        while (gotoKey->key() != nullptr) {
            reply = decompose(gotoKey, s, gotoKey);
            if (reply != nullptr) {
                return reply;
            }
        }
    }
    return nullptr;
}

sp<String> Eliza::decompose(const sp<Key>& key, const sp<String>& s, const sp<Key>& gotoKey) {
    sp<StringArray> reply = new StringArray(10);
    for (size_t i = 0; i < key->decompositions()->size(); i++) {
        sp<Decomposition> d = key->decompositions()->get(i);
        sp<String> pattern = d->pattern();
        if (synonyms->matchDecomposition(s, pattern, reply)) {
            sp<String> rep = assemble(d, reply, gotoKey);
            if (rep != nullptr) {
                return rep;
            }
            if (gotoKey->key() != nullptr) {
                return nullptr;
            }
        }
    }
    return nullptr;
}

sp<String> Eliza::assemble(const sp<Decomposition>& d, const sp<StringArray>& reply, const sp<Key>& gotoKey) {
    sp<StringArray> lines = new StringArray(3);
    d->stepReassemblyRule();
    sp<String> rule = d->nextReassemblyRule();
    if (Strings::match(rule, "goto *", lines)) {
        // Goto rule, set gotoKey and return false.
        gotoKey->copy(keys->getKey(lines->get(0)));
        if (gotoKey->key() != nullptr) {
            return nullptr;
        }
        printf("Goto rule did not match key: %s\n", lines->get(0)->c_str());
        return nullptr;
    }
    sp<String> tmp = String::EMPTY_STRING;
    while (Strings::match(rule, "* (#)*", lines)) {
        // Reassembly rule with number substitution.
        rule = lines->get(2);
        size_t n = 0;
        try {
            n = Integer::valueOf(lines->get(1))->intValue() - 1;
        } catch (const NumberFormatException& e) {
            printf("Number is wrong in reassembly rule %s\n", lines->get(1)->c_str());
        }
        if (n >= reply->size()) {
            printf("Substitution number is bad %s\n", lines->get(1)->c_str());
            return nullptr;
        }
        reply->set(n, postList->translate(reply->get(n)));
        tmp = tmp->append(String::format("%s %s", lines->get(0)->c_str(), reply->get(n)->c_str()));
    }
    tmp = tmp->append(rule);
    if (d->memory()) {
        memory->save(tmp);
        return nullptr;
    }
    return tmp;
}

void Eliza::print() {
    keys->print(0);
    synonyms->print(0);
    preList->print(0);
    postList->print(0);
    quit->print(0);
}

} /* namespace eliza */
