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

#ifndef MINDROID_NET_URI_H_
#define MINDROID_NET_URI_H_

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
        const sp<String>& userInfo,
        const sp<String>& host,
        int32_t port,
        const sp<String>& path,
        const sp<String>& query,
        const sp<String>& fragment);

    URI(const char* scheme,
        const char* userInfo,
        const char* host,
        int32_t port,
        const char* path,
        const char* query,
        const char* fragment);

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

    /**
     * Returns the URI formed by parsing {@code uri}. This method behaves
     * identically to the string constructor but throws a different exception
     * on failure. The constructor fails with a checked {@link
     * URISyntaxException}; this method fails with an unchecked {@link
     * IllegalArgumentException}.
     */
    static sp<URI> create(const sp<String>& uri);
    static sp<URI> create(const char* uri);

    /**
     * Returns the scheme of this URI, or null if this URI has no scheme. This
     * is also known as the protocol.
     */
    sp<String> getScheme() const {
        return mScheme;
    }

    /**
     * Returns the decoded authority part of this URI, or null if this URI has
     * no authority.
     */
    sp<String> getAuthority() const {
        return mAuthority;
    }

    /**
     * Returns the decoded user info of this URI, or null if this URI has no
     * user info.
     */
    sp<String> getUserInfo() const {
        return mUserInfo;
    }

    /**
     * Returns the host of this URI, or null if this URI has no host.
     */
    sp<String> getHost() const {
        return mHost;
    }

    /**
     * Returns the port number of this URI, or {@code -1} if this URI has no
     * explicit port.
     */
    int32_t getPort() const{
        return mPort;
    }

    /**
     * Returns the decoded path of this URI, or null if this URI has no path.
     */
    sp<String> getPath() const {
        return mPath;
    }

    /**
     * Returns the decoded query of this URI, or null if this URI has no query.
     */
    sp<String> getQuery() const {
        return mQuery;
    }

    /**
     * Returns the decoded fragment of this URI, or null if this URI has no
     * fragment.
     */
    sp<String> getFragment() const {
        return mFragment;
    }

    /**
     * Returns the encoded URI.
     */
    sp<String> toString() const {
        return mString;
    }

    /**
     * Returns true if this URI is absolute, which means that a scheme is
     * defined.
     */
    bool isAbsolute() const {
        return mScheme != nullptr && !mScheme->isEmpty();
    }

    /**
     * Returns true if this URI is opaque. Opaque URIs are absolute and have a
     * scheme-specific part that does not start with a slash character. All
     * parts except scheme, scheme-specific and fragment are undefined.
     */
    bool isOpaque() const {
        return mOpaque;
    }

    bool equals(const sp<Object>& other) const override;
    bool equals(const sp<URI>& uri) const;

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
    void parseURI(const sp<String>& uri, bool serverBasedNamingAuthority = false);

    /**
     * Breaks this URI's authority into user info, host and port parts.
     *   [user-info@][host][:port]
     * If any part of this fails this method will give up and potentially leave
     * these fields with their default values.
     */
    void parseAuthority(bool serverBasedNamingAuthority);

    static size_t indexOf(const sp<String>& string, const char c, size_t start, size_t end);
    static size_t indexOf(const sp<String>& string, const char* chars, size_t start, size_t end);

    /**
     * Returns true if {@code first} and {@code second} are equal after
     * unescaping hex sequences like %F1 and %2b.
     */
    static bool escapedEqualsIgnoreCase(const sp<String>& first, const sp<String>& second);

    sp<String> mString;
    sp<String> mScheme;
    sp<String> mSchemeSpecificPart;
    sp<String> mAuthority;
    sp<String> mUserInfo;
    sp<String> mHost;
    int32_t mPort = -1;
    sp<String> mPath;
    sp<String> mQuery;
    sp<String> mFragment;
    bool mOpaque;
};

} /* namespace mindroid */

#endif /* MINDROID_NET_URI_H_ */
