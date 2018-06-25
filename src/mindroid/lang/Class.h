/*
 * Copyright (C) 2016 E.S.R.Labs
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

#ifndef MINDROID_LANG_CLASS_H_
#define MINDROID_LANG_CLASS_H_

#include <mindroid/lang/String.h>
#include <mindroid/util/HashMap.h>
#include <mutex>

namespace mindroid {

class Factory;

/// @private
class Classes {
public:
    void put(const sp<String>& name, Factory* factory);
    Factory* get(const sp<String>& name);

    static Classes* getInstance();

private:
    Classes() : mClasses(new HashMap<sp<String>, Factory*>()) { }

    static std::mutex sLock;
    static Classes* sInstance;

    sp<HashMap<sp<String>, Factory*>> mClasses;
};

/// @private
class Factory {
public:
    Factory(const char* name) {
        Classes::getInstance()->put(String::valueOf(name), this);
    }

    virtual ~Factory() = default;

    virtual sp<Object> newInstance() = 0;
};

template<typename T>
class Class final :
        public Object {
public:
    static bool isInstance(const sp<Object>& o) {
        if (o != nullptr) {
            sp<T> t = dynamic_cast<T*>(o.getPointer());
            return t != nullptr;
        } else {
            return false;
        }
    }

    static sp<T> cast(const sp<Object>& o) {
        if (o != nullptr) {
            sp<T> t = dynamic_cast<T*>(o.getPointer());
            return t;
        } else {
            return nullptr;
        }
    }

    static sp<Class> forName(const char* className) {
        return forName(String::valueOf(className));
    }

    static sp<Class> forName(const sp<String>& className) {
        return new Class(className);
    }

    sp<String> getName() {
        return mName;
    }

    sp<T> newInstance() {
        Factory* factory = Classes::getInstance()->get(mName);
        if (factory != nullptr) {
            return object_cast<T>(factory->newInstance());
        } else {
            return nullptr;
        }
    }

private:
    Class(const sp<String>& name) : mName(name) {
    }

    sp<String> mName;
};

#define GET_CLASS_MACRO(_1, _2, NAME, ...) NAME
#define CLASS(...) GET_CLASS_MACRO(__VA_ARGS__, CLASS2, CLASS1)(__VA_ARGS__)

#define CLASS1(Clazz) \
class Clazz##Factory : public mindroid::Factory { \
public: \
    Clazz##Factory() : mindroid::Factory("::"#Clazz) { \
    } \
    \
    virtual mindroid::sp<mindroid::Object> newInstance() { \
        return new ::Clazz(); \
    } \
}; \
static volatile Clazz##Factory s##Clazz##Factory;

#define CLASS2(Package, Clazz) \
class Package##Clazz##Factory : public mindroid::Factory { \
public: \
    Package##Clazz##Factory() : mindroid::Factory(#Package"::"#Clazz) { \
    } \
    \
    virtual mindroid::sp<mindroid::Object> newInstance() { \
        return new Package::Clazz(); \
    } \
}; \
static volatile Package##Clazz##Factory s##Package##Clazz##Factory;

} /* namespace mindroid */

#endif /* MINDROID_LANG_CLASS_H_ */
