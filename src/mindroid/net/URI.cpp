/*
 * Copyright (C) 2018 E.S.R.Labs
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

#include <mindroid/net/URI.h>
#include <mindroid/net/URISyntaxException.h>
#include <mindroid/lang/StringBuilder.h>
#include <mindroid/lang/Integer.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/NumberFormatException.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/lang/IndexOutOfBoundsException.h>

namespace mindroid {

URI::URI(const sp<String>& spec) {
    if (spec == nullptr) {
        throw NullPointerException();
    }
    parseURI(spec);
}

URI::URI(const char* spec) {
    if (spec == nullptr) {
        throw NullPointerException();
    }
    parseURI(String::valueOf(spec));
}

sp<URI> URI::create(const sp<String>& uri) {
    try {
        return new URI(uri);
    } catch (const URISyntaxException& e) {
        throw IllegalArgumentException(e);
    }
}

sp<URI> URI::create(const char* uri) {
    try {
        return new URI(uri);
    } catch (const URISyntaxException& e) {
        throw IllegalArgumentException(e);
    }
}

URI::URI(const sp<String>& scheme, const sp<String>& userInfo, const sp<String>& host, int32_t port,
        const sp<String>& path, const sp<String>& query, const sp<String>& fragment) {
    if (scheme == nullptr && userInfo == nullptr && host == nullptr && path == nullptr && query == nullptr && fragment == nullptr) {
        mPath = String::EMPTY_STRING;
        return;
    }
    if (scheme != nullptr && path != nullptr && !path->isEmpty() && path->charAt(0) != '/') {
        throw URISyntaxException("Relative path");
    }

    sp<StringBuilder> uri = new StringBuilder();
    if (scheme != nullptr) {
        uri->append(scheme);
        uri->append(':');
    }

    if (userInfo != nullptr || host != nullptr || port != -1) {
        uri->append("//");
    }
    if (userInfo != nullptr) {
        uri->append(userInfo);
        uri->append('@');
    }
    if (host != nullptr) {
        sp<String> h = host;
        // Check for IPv6 addresses without square brackets.
        if (host->indexOf(':') != -1 && host->indexOf(']') == -1 && host->indexOf('[') == -1) {
            h = String::format("[%s]", host->c_str());
        }
        uri->append(h);
    }
    if (port != -1) {
        uri->append(':');
        uri->append(port);
    }

    if (path != nullptr) {
        uri->append(path);
    }
    if (query != nullptr) {
        uri->append('?');
        uri->append(query);
    }
    if (fragment != nullptr) {
        uri->append('#');
        uri->append(fragment);
    }

    parseURI(uri->toString());
}

URI::URI(const char* scheme, const char* userInfo, const char* host, int32_t port, const char* path, const char* query, const char* fragment) :
        URI(String::valueOf(scheme), String::valueOf(userInfo), String::valueOf(host), port,
                String::valueOf(path), String::valueOf(query), String::valueOf(fragment)) {
}

URI::URI(const sp<String>& scheme, const sp<String>& authority, const sp<String>& path, const sp<String>& query, const sp<String>& fragment) {
    if (scheme != nullptr && path != nullptr && !path->isEmpty() && path->charAt(0) != '/') {
        throw URISyntaxException("Relative path");
    }

    sp<StringBuilder> uri = new StringBuilder();
    if (scheme != nullptr) {
        uri->append(scheme);
        uri->append(':');
    }
    if (authority != nullptr) {
        uri->append("//");
        uri->append(authority);
    }

    if (path != nullptr) {
        uri->append(path);
    }
    if (query != nullptr) {
        uri->append('?');
        uri->append(query);
    }
    if (fragment != nullptr) {
        uri->append('#');
        uri->append(fragment);
    }

    parseURI(uri->toString());
}

URI::URI(const char* scheme, const char* authority, const char* path, const char* query, const char* fragment) :
        URI(String::valueOf(scheme), String::valueOf(authority), String::valueOf(path),
                String::valueOf(query), String::valueOf(fragment)) {
}

void URI::parseURI(const sp<String>& uri) {
    mString = uri;

    // "#Fragment"
    size_t fragmentStart = indexOf(uri, '#', 0, uri->length());
    if (fragmentStart < uri->length()) {
        mFragment = uri->substring(fragmentStart + 1);
    }

    // Scheme:
    size_t hierPartStart;
    size_t colon = indexOf(uri, ':', 0, fragmentStart);
    if (colon < indexOf(uri, "/?#", 0, fragmentStart)) {
        mScheme = uri->substring(0, colon);
        hierPartStart = colon + 1;

        if (hierPartStart == fragmentStart) {
            throw URISyntaxException("Scheme-specific part expected");
        }
    } else {
        hierPartStart = 0;
    }

    // "//Authority"
    size_t pathStart;
    if (uri->regionMatches(hierPartStart, "//", 0, 2)) {
        size_t authorityStart = hierPartStart + 2;
        if (authorityStart == uri->length()) {
            throw URISyntaxException("Authority expected");
        }
        pathStart = indexOf(uri, "/?", authorityStart, fragmentStart);
        if (authorityStart < pathStart) {
            mAuthority = uri->substring(authorityStart, pathStart);
        }
    } else {
        pathStart = hierPartStart;
    }

    // "Path"
    size_t queryStart = indexOf(uri, '?', pathStart, fragmentStart);
    mPath = uri->substring(pathStart, queryStart);

    // "?Query"
    if (queryStart < fragmentStart) {
        mQuery = uri->substring(queryStart + 1, fragmentStart);
    }

    parseAuthority();
}

void URI::parseAuthority()  {
    if (mAuthority == nullptr) {
        return;
    }

    sp<String> authority = mAuthority;
    sp<String> userInfo = nullptr;
    ssize_t userIndex = authority->indexOf('@');
    __attribute__((unused)) ssize_t hostIndex = 0;
    if (userIndex != -1) {
        userInfo = authority->substring(0, userIndex);
        authority = authority->substring(userIndex + 1); // Host[:Port]
        hostIndex = userIndex + 1;
    }

    sp<String> host;
    int32_t port = -1;
    ssize_t portIndex = authority->lastIndexOf(':');
    ssize_t endIndex = authority->indexOf(']');
    if (portIndex != -1 && endIndex < portIndex) {
        host = authority->substring(0, portIndex);

        if (portIndex < ((ssize_t) authority->length() - 1)) {
            try {
                char firstPortChar = authority->charAt(portIndex + 1);
                if (firstPortChar >= '0' && firstPortChar <= '9') {
                    port = Integer::valueOf(authority->substring(portIndex + 1))->intValue();
                } else {
                    throw URISyntaxException("Invalid port number");
                }
            } catch (const NumberFormatException& e) {
                throw URISyntaxException("Invalid port number");
            }
        }
    } else {
        host = authority;
    }

    if (host->isEmpty()) {
        throw URISyntaxException("Invalid host");
    }

    mUserInfo = userInfo;
    mHost = host;
    mPort = port;
}

size_t URI::indexOf(const sp<String>& string, const char c, size_t start, size_t end) {
    ssize_t index = string->indexOf(c);
    if (index >= 0 && ((size_t) index < end)) {
        return (size_t) index;
    } else {
        return end;
    }
}

size_t URI::indexOf(const sp<String>& string, const char* chars, size_t start, size_t end) {
    if (start >= string->length() || start > end || end > string->length()) {
        throw IndexOutOfBoundsException();
    }
    for (size_t i = start; i < end; i++) {
        char c = string->charAt(i);
        const char* substr = strchr(chars, c);
        if (substr != nullptr) {
            return i;
        }
    }
    return end;
}

} /* namespace mindroid */
