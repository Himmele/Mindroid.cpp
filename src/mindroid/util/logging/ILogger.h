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

#ifndef MINDROID_UTIL_LOGGING_ILOGGER_H_
#define MINDROID_UTIL_LOGGING_ILOGGER_H_

#include <mindroid/os/Binder.h>

namespace mindroid {

class Intent;

class ILogger :
        public IInterface {
public:
    virtual sp<Promise<sp<String>>> assumeThat(const sp<String>& tag, const sp<String>& message, int64_t timeout) = 0;
    virtual void mark() = 0;
    virtual void reset() = 0;
};

namespace binder {

class Logger {
public:
    class Proxy;

    class Stub : public Binder, public ILogger {
    public:
        Stub() {
            attachInterface(this, String::valueOf(DESCRIPTOR));
        }

        static sp<ILogger> asInterface(const sp<IBinder>& binder) {
            if (binder == nullptr) {
                return nullptr;
            }
            return new Logger::Proxy(binder);
        }

        sp<IBinder> asBinder() override {
            return this;
        }

        void onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Thenable>& result) override;

        class Proxy : public ILogger {
        public:
            Proxy(const sp<IBinder>& remote) {
                mRemote = remote;
            }

            sp<IBinder> asBinder() override {
                return mRemote;
            }

            bool equals(const sp<Object>& obj) const override {
                if (obj == nullptr) return false;
                if (obj == this) return true;
                if (Class<Stub::Proxy>::isInstance(obj)) {
                    sp<Stub::Proxy> other = Class<Stub::Proxy>::cast(obj);
                    return mRemote->equals(other->mRemote);
                } else {
                    return false;
                }
            }

            size_t hashCode() const override {
                return mRemote->hashCode();
            }

            sp<Promise<sp<String>>> assumeThat(const sp<String>& tag, const sp<String>& message, int64_t timeout) override;
            void mark() override;
            void reset() override;

        private:
            sp<IBinder> mRemote;
        };

    private:
        static const char* const DESCRIPTOR;
        static const int32_t MSG_ASSUME_THAT = 1;
        static const int32_t MSG_MARK = 2;
        static const int32_t MSG_RESET = 3;

        friend class Logger::Proxy;
    };

    class Proxy : public ILogger {
    public:
        Proxy(const sp<IBinder>& binder);

        sp<IBinder> asBinder() override {
            return mBinder;
        }

        bool equals(const sp<Object>& obj) const override {
            if (obj == nullptr) return false;
            if (obj == this) return true;
            if (Class<Proxy>::isInstance(obj)) {
                sp<Proxy> other = Class<Proxy>::cast(obj);
                return mBinder->equals(other->mBinder);
            } else {
                return false;
            }
        }

        size_t hashCode() const override {
            return mBinder->hashCode();
        }

        sp<Promise<sp<String>>> assumeThat(const sp<String>& tag, const sp<String>& message, int64_t timeout) override;
        void mark() override;
        void reset() override;

    private:
        sp<IBinder> mBinder;
        sp<Logger::Stub> mStub;
        sp<ILogger> mProxy;
    };
};

} /* namespace binder */
} /* namespace mindroid */

#endif /* MINDROID_UTIL_LOGGING_ILOGGER_H_ */
