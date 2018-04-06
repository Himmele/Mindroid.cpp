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

#ifndef ELIZA_STRINGS_H_
#define ELIZA_STRINGS_H_

#include <mindroid/lang/String.h>
#include <mindroid/lang/StringArray.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/util/ArrayList.h>
#include "WordList.h"

namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

/**
 *  Eliza string functions.
 */
class Strings : public Object {
public:
    /**
     *  An approximate string matching method that looks
     *  for a match between the string and the pattern.
     *  Return count of matching characters before * or #.
     *  Return -1 if strings do not match.
     */
    static ssize_t amatch(const sp<String>& s, const sp<String>& pattern) {
        size_t count = 0;
        size_t i = 0;  // s index.
        size_t j = 0;  // pattern index.
        while (i < s->length() && j < pattern->length()) {
            char p = pattern->charAt(j);
            // Stop if pattern is * or #.
            if (p == '*' || p == '#') {
                return count;
            }
            if (s->charAt(i) != p) {
                return -1;
            }
            i++; j++; count++;
        }
        return count;
    }

    /**
     *  Search in successive positions of the string,
     *  looking for a match to the pattern.
     *  Return the string position in s in case of a match,
     *  or -1 for no match.
     */
    static ssize_t findPattern(const sp<String>& s, const sp<String>& pattern) {
        size_t count = 0;
        for (size_t i = 0; i < s->length(); i++) {
            if (amatch(s->substring(i), pattern) >= 0) {
                return count;
            }
            count++;
        }
        return -1;
    }

    /**
     *  Returns the number of digits at the beginning of s.
     */
    static size_t findNumbers(const sp<String>& s) {
        size_t count = 0;
        for (size_t i = 0; i < s->length(); i++) {
            if (NUMBERS->indexOf(s->charAt(i)) == -1) {
                return count;
            }
            count++;
        }
        return count;
    }

    /**
     *  Match the string against a pattern and fills in
     *  matches array with the pieces that matched * and #
     */
    static bool matchA(const sp<String>& s, const sp<String>& pattern, const sp<mindroid::StringArray>& matches) {
        size_t i = 0;   // s index.
        size_t j = 0;   // matches index.
        size_t pos = 0; // pattern index.
        while (pos < pattern->length() && j < matches->size()) {
            char p = pattern->charAt(pos);
            if (p == '*') {
                ssize_t n;
                if (pos + 1 == pattern->length()) {
                    // * is the last char in pattern.
                    // n is remaining string length.
                    n = s->length() - i;
                } else {
                    // * is not last char in pattern.
                    // find using remaining pattern.
                    n = findPattern(s->substring(i), pattern->substring(pos + 1));
                }
                if (n < 0) {
                    return false;
                }
                matches->set(j++, s->substring(i, i + n));
                i += n;
                pos++;
            } else if (p == '#') {
                size_t n = findNumbers(s->substring(i));
                matches->set(j++, s->substring(i, i + n));
                i += n;
                pos++;
            } else {
                ssize_t n = amatch(s->substring(i), pattern->substring(pos));
                if (n <= 0) {
                    return false;
                }
                i += n;
                pos += n;
            }
        }
        if (i >= s->length() && pos >= pattern->length()) return true;
        return false;
    }

    /*
     *  This version is clearer, but hopelessly slow
     */
    static bool matchB(sp<String> s, sp<String> pattern, const sp<mindroid::StringArray>& matches) {
        size_t j = 0; // matches index.
        while (pattern->length() > 0 && s->length() >= 0 && j < matches->size()) {
            char p = pattern->charAt(0);
            if (p == '*') {
                int n;
                if (pattern->length() == 1) {
                    // * is the last char in pattern.
                    // n is remaining string length.
                    n = s->length();
                } else {
                    // * is not last char in pattern.
                    // find using remaining pattern.
                    n = findPattern(s, pattern->substring(1));
                }
                if (n < 0) {
                    return false;
                }
                matches->set(j++, s->substring(0, n));
                s = s->substring(n);
                pattern = pattern->substring(1);
            } else if (p == '#') {
                size_t n = findNumbers(s);
                matches->set(j++, s->substring(0, n));
                s = s->substring(n);
                pattern = pattern->substring(1);
            } else {
                int n = amatch(s, pattern);
                if (n <= 0) {
                    return false;
                }
                s = s->substring(n);
                pattern = pattern->substring(n);
            }
        }
        if (s->length() == 0 && pattern->length() == 0) {
            return true;
        }
        return false;
    }

    static bool match(const sp<String>& s, const char* pattern, const sp<mindroid::StringArray>& matches) {
        return match(s, String::valueOf(pattern), matches);
    }

    static bool match(const sp<String>& s, const sp<String>& pattern, const sp<mindroid::StringArray>& matches) {
        return matchA(s, pattern, matches);
    }

    /**
     *  Translates corresponding characters in source to destination.
     */
    static sp<String> translate(sp<String> string, const char* source, const char* destination) {
        return translate(string, String::valueOf(source), String::valueOf(destination));
    }

    static sp<String> translate(sp<String> string, const sp<String>& source, const sp<String>& destination) {
        if (source->length() != destination->length()) {
            throw mindroid::IllegalArgumentException();
        }
        for (size_t i = 0; i < source->length(); i++) {
            string = string->replace(source->charAt(i), destination->charAt(i));
        }
        return string;
    }

    /**
     *  Compresses its input by:
     *  (1) Dropping space before space, comma, and period;
     *  (2) Adding space before question, if char before is not a space; and
     *  (3) copying all others
     */
    static sp<String> compress(const sp<String>& s) {
        sp<String> destination = String::EMPTY_STRING;
        if (s->length() == 0) {
            return s;
        }
        char c = s->charAt(0);
        for (size_t i = 1; i < s->length(); i++) {
            if (c == ' ' &&
                 ((s->charAt(i) == ' ') ||
                 (s->charAt(i) == ',') ||
                 (s->charAt(i) == '.'))) {
                // Do nothing.
            } else if (c != ' ' && s->charAt(i) == '?') {
                destination = destination->append(String::format("%c ", c));
            } else {
                destination = destination->append(c);
            }
            c = s->charAt(i);
        }
        destination = destination->append(c);
        return destination;
    }

    /**
     *  Trim off leading spaces.
     */
    static sp<String> trim(const sp<String>& s) {
        for (size_t i = 0; i < s->length(); i++) {
            if (s->charAt(i) != ' ') {
                return s->substring(i);
            }
        }
        return String::EMPTY_STRING;
    }

    /**
     *  Pad by ensuring there are spaces before and after the sentence.
     */
    static sp<String> pad(const sp<String>& s) {
        if (s->length() == 0) {
            return String::valueOf(" ");
        }
        char first = s->charAt(0);
        char last = s->charAt(s->length() - 1);
        if (first == ' ' && last == ' ') return s;
        if (first == ' ' && last != ' ') return String::format("%s ", s->c_str());
        if (first != ' ' && last == ' ') return String::format(" %s", s->c_str());
        if (first != ' ' && last != ' ') return String::format(" %s ", s->c_str());
        return s;
    }

    /**
     *  Count number of occurrences of c in s.
     */
    static size_t count(const sp<String>& s, char c) {
        size_t count = 0;
        for (size_t i = 0; i < s->length(); i++) {
            if (s->charAt(i) == c) count++;
        }
        return count;
    }

private:
    /** The digits. */
    static const sp<String> NUMBERS;
};

} /* namespace eliza */

#endif /* ELIZA_STRINGS_H_ */
