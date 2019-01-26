/*
 * Copyright (C) 2019 E.S.R.Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <mindroid/net/URI.h>

using namespace mindroid;

TEST(Mindroid, UrisEquality) {
    sp<URI> a, b;

#define COMPARE_URI(stringA, stringB, result) \
    a = new URI(stringA); \
    b = new URI(stringB); \
    ASSERT_EQ(a->equals(b), result);

    COMPARE_URI("/path", "/path#frag", false)
    COMPARE_URI("#frag", "#frag2", false)
    COMPARE_URI("#frag", "#FRag", false)

    // Case insensitive hex escape sequences.
    COMPARE_URI("#fr%4F", "#fr%4f", true)

    // Schemes.
    COMPARE_URI("scheme:test", "scheme2:test", false)
    COMPARE_URI("test", "http:test", false)
    COMPARE_URI("http:test", "test", false)
    COMPARE_URI("SCheme:test", "schEMe:test", true)

    // Hierarchical/opaque URIs.
    COMPARE_URI("mailto:jim", "mailto://jim", false)
    COMPARE_URI("mailto://test", "mailto:test", false)

    // Opaque URIs.
    COMPARE_URI("mailto:name", "mailto:name", true)
    COMPARE_URI("mailtO:john", "mailto:jim", false)

    // Test case insensitivity hex escape sequences.
    COMPARE_URI("mailto:te%4Fst", "mailto:te%4fst", true)

    COMPARE_URI("mailto:john#frag", "mailto:john#frag2", false)

    // Hierarchical URIs.
    COMPARE_URI("/test", "/test", true) // Paths.
    COMPARE_URI("/te%F4st", "/te%f4st", true)
    COMPARE_URI("/TEst", "/teSt", false)

    COMPARE_URI("//host.com:80err", "//host.com:80e", false)
    COMPARE_URI("//host.com:81e%Abrr", "//host.com:81e%abrr", true)

    COMPARE_URI("/test", "//auth.com/test", false)
    COMPARE_URI("//test.com", "/test", false)

    COMPARE_URI("//test.com", "//test.com", true) // Hosts.

    // Case insensitivity for hosts.
    COMPARE_URI("//HoSt.coM/", "//hOsT.cOm/", true)
    COMPARE_URI("//te%ae.com", "//te%aE.com", true)
    COMPARE_URI("//test.com:80", "//test.com:81", false)
    COMPARE_URI("//joe@test.com:80", "//test.com:80", false)
    COMPARE_URI("//jo%3E@test.com:82", "//jo%3E@test.com:82", true)
    COMPARE_URI("//test@test.com:85", "//test@test.com", false)

#undef COMPARE_URI
}
