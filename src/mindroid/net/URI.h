/*
 * Copyright (C) 2018 ESR Labs
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

#ifndef MINDROID_URI_H_
#define MINDROID_URI_H_

#include <mindroid/lang/String.h>

namespace mindroid {

class URI : public Object {
public:
    URI(const sp<String>& spec);
    URI(const char* spec);

    /**
     * Creates a new URI instance of the given unencoded component parts.
     *
     * @param scheme the URI scheme, or null for a non-absolute URI.
     */
    URI(const sp<String>& scheme,
        const sp<String>& authority,
        const sp<String>& path,
        const sp<String>& query,
        const sp<String>& fragment);

    URI(const char* scheme,
        const char* authority,
        const char* path,
        const char* query,
        const char* fragment);

    static sp<URI> create(const sp<String>& uri);
    static sp<URI> create(const char* uri);

    sp<String> getScheme() const {
        return mScheme;
    }

    sp<String> getAuthority() const {
        return mAuthority;
    }

    sp<String> getPath() const {
        return mPath;
    }

    sp<String> getQuery() const {
        return mQuery;
    }

    sp<String> getFragment() const {
        return mFragment;
    }

    sp<String> toString() const;

private:
    /**
     * Breaks uri into its component parts. This first splits URI into scheme,
     * scheme-specific part and fragment:
     *   [scheme:][scheme-specific part][#fragment]
     *
     * Then it breaks the scheme-specific part into authority, path and query:
     *   [//authority][path][?query]
     *
     * Finally it delegates to parseAuthority to break the authority into user
     * info, host and port:
     *   [user-info@][host][:port]
     */
    void parseURI(const sp<String>& uri);

    static size_t indexOf(const sp<String>& string, const char c, size_t start, size_t end);
    static size_t indexOf(const sp<String>& string, const char* chars, size_t start, size_t end);

    sp<String> mString;
    sp<String> mScheme;
    sp<String> mAuthority;
    sp<String> mPath;
    sp<String> mQuery;
    sp<String> mFragment;
};

} /* namespace mindroid */

#endif /* MINDROID_URI_H_ */
