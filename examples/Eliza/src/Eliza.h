/*
 * Copyright (C) 2018 Daniel Himmelein
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

#ifndef EXAMPLES_ELIZA_H_
#define EXAMPLES_ELIZA_H_

#include "IEliza.h"
#include "IElizaListener.h"
#include <mindroid/lang/Object.h>
#include <mindroid/os/Binder.h>
#include <mindroid/util/ArrayList.h>
#include <mindroid/util/function/Function.h>

namespace mindroid {
class Bundle;
class Context;
class URI;
}

namespace examples {
namespace eliza {

using mindroid::sp;
using mindroid::wp;
using mindroid::Object;
using mindroid::String;

class IEliza;

class Eliza : public Object {
public:
    static const sp<mindroid::URI> ELIZA;

    class ListenerWrapper : public binder::ElizaListener::Stub {
    public:
        ListenerWrapper(const sp<mindroid::Function<sp<String>, void>>& listener, const sp<Eliza>& eliza) :
                mListener(listener),
                mEliza(eliza) {
        }

        void onReply(const sp<String>& reply) override {
            sp<Eliza> eliza = mEliza.get();
            if (eliza != nullptr) {
                eliza->mListeners->remove(this);
            }
            mListener->apply(reply);
        }

    private:
        sp<mindroid::Function<sp<String>, void>> mListener;
        wp<Eliza> mEliza;
    };

    Eliza(const sp<mindroid::Context>& context);

    Eliza(const sp<mindroid::Context>& context, const sp<mindroid::Bundle>& extras);

    sp<String> ask1(const sp<String>& question) /* throws RemoteException*/;

    sp<mindroid::Promise<sp<String>>> ask2(const sp<String>& question) /* throws RemoteException*/;

    void ask3(const sp<String>& question, const std::function<void (const sp<String>&)>& listener) /* throws RemoteException*/;

private:
    sp<IEliza> mService;
    sp<mindroid::ArrayList<sp<ListenerWrapper>>> mListeners = new mindroid::ArrayList<sp<ListenerWrapper>>();
};

} /* namespace eliza */
} /* namespace examples */

#endif /* EXAMPLES_ELIZA_H_ */
