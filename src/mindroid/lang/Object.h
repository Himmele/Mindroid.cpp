/*
 * Copyright (C) 2005 The Android Open Source Project
 * Copyright (C) 2011 Daniel Himmelein
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

#ifndef MINDROID_LANG_OBJECT_H_
#define MINDROID_LANG_OBJECT_H_

#include <atomic>
#include <cstdint>
#include <cinttypes>
#include <cstddef>
#include <cassert>
#include <sys/types.h>

#define ASSERT(cond, ...) if (!(cond)) { printf(__VA_ARGS__); printf("\n"); assert(cond); }
#define DEBUG_INFO(...) printf(__VA_ARGS__)
#define ERROR_INFO(...) printf(__VA_ARGS__)

namespace mindroid {

template<typename T> class sp;
template<typename T> class wp;

class Object {
public:
    Object();
    void incStrongReference(const void* id) const;
    void decStrongReference(const void* id) const;

    /// @private
    class WeakReference {
    public:
        Object* toObject() const;
        void incWeakReference(const void* id);
        void decWeakReference(const void* id);
        bool tryIncStrongReference(const void* id);

        // Debugging APIs.
        int32_t getWeakReferenceCount() const;
        void printReferences() const;
        void trackReference(bool doTracking, bool memorizeRefOperationsDuringRefTracking);
    };

    WeakReference* createWeakReference(const void* id) const;
    WeakReference* getWeakReference() const;

    // Debugging APIs.
    int32_t getStrongReferenceCount() const;
    void moveStrongReference(const void* oldId, const void* newId) const;
    // Print all references for this object
    inline void printReferences() const { getWeakReference()->printReferences(); }
    inline void trackReference(bool trackReferences, bool memorizeRefOperationsDuringRefTracking) {
        getWeakReference()->trackReference(trackReferences, memorizeRefOperationsDuringRefTracking);
    }

    // Object destroyer.
    class Destroyer {
    public:
        virtual ~Destroyer();

    protected:
        template<typename T>
        sp<T> reviveObject(Object* object);

    private:
        virtual void destroy(Object* object) = 0;

        friend class Object;
    };

    void setDestroyer(Destroyer* destroyer);

    virtual bool equals(const sp<Object>& object) const;
    virtual size_t hashCode() const;

protected:
    virtual ~Object();
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    enum {
        // Normal object life-time: the object gets destroyed when the last strong reference goes away.
        OBJECT_LIFETIME_STRONG_REFERENCE = 0x0000,
        // Extended object life-time: the object gets destroyed when the last weak reference goes away.
        OBJECT_LIFETIME_WEAK_REFERENCE   = 0x0001,
        OBJECT_LIFETIME_MASK             = 0x0001
    };

    void setObjectLifetime(int32_t mode) const;

    virtual void onFirstReference();
    virtual void onLastReference(const void* id);

private:
    class WeakReferenceImpl;
    WeakReferenceImpl* const mReference;

    friend class WeakReference;

    template<typename Y>
    friend sp<Y> Object::Destroyer::reviveObject(Object* obj);
};

template<class T>
class LightweightObject {
public:
    inline LightweightObject() : mReferenceCounter(0) { }

    inline void incStrongReference(__attribute__((unused)) const void* id) const {
        mReferenceCounter.fetch_add(1, std::memory_order_relaxed);
    }

    inline void decStrongReference(__attribute__((unused)) const void* id) const {
        if (mReferenceCounter.fetch_sub(1, std::memory_order_release) == 1) {
            std::atomic_thread_fence(std::memory_order_acquire);
            delete static_cast<const T*>(this);
        }
    }

    // Debugging APIs
    inline int32_t getStrongReferenceCount() const {
        return mReferenceCounter.load(std::memory_order_relaxed);
    }

    inline void moveStrongReference(__attribute__((unused)) const void* oldId,
            __attribute__((unused)) const void* newId) const {
    }

protected:
    inline ~LightweightObject() { }

private:
    mutable std::atomic<int32_t> mReferenceCounter;
};

void spDataRaceException();

#define COMPARE_STRONG_REFERENCE(_operator_)                   \
inline bool operator _operator_ (const sp<T>& o) const {       \
    return mPointer _operator_ o.mPointer;                     \
}                                                              \
inline bool operator _operator_ (const T* o) const {           \
    return mPointer _operator_ o;                              \
}                                                              \
template<typename U>                                           \
inline bool operator _operator_ (const sp<U>& o) const {       \
    return mPointer _operator_ o.mPointer;                     \
}                                                              \
template<typename U>                                           \
inline bool operator _operator_ (const U* o) const {           \
    return mPointer _operator_ o;                              \
}                                                              \
inline bool operator _operator_ (const wp<T>& o) const {       \
    return mPointer _operator_ o.mPointer;                     \
}                                                              \
template<typename U>                                           \
inline bool operator _operator_ (const wp<U>& o) const {       \
    return mPointer _operator_ o.mPointer;                     \
}

template<typename T>
class sp {
public:
    inline sp() : mPointer(nullptr) { }

    sp(T* other);
    sp(const sp<T>& other);
    sp(sp<T>&& other);
    template<typename U> sp(U* other);
    template<typename U> sp(const sp<U>& other);
    template<typename U> sp(sp<U>&& other);

    ~sp();

    // Assignment operators
    sp& operator=(T* other);
    sp& operator=(const sp<T>& other);
    sp& operator = (sp<T>&& other);

    template<typename U> sp& operator=(const sp<U>& other);
    template<typename U> sp& operator=(sp<U>&& other);
    template<typename U> sp& operator=(U* other);

    // Accessor methods
    inline T& operator*() const { return *mPointer; }
    inline T* operator->() const { return mPointer; }
    inline T* getPointer() const { return mPointer; }
    inline T const* getConstPointer() const { return mPointer; }

    // Operators
    COMPARE_STRONG_REFERENCE(==)
    COMPARE_STRONG_REFERENCE(!=)
    COMPARE_STRONG_REFERENCE(>)
    COMPARE_STRONG_REFERENCE(<)
    COMPARE_STRONG_REFERENCE(<=)
    COMPARE_STRONG_REFERENCE(>=)

    void clear();

    template<typename ... Args>
    static sp<T> of(Args&& ... args) {
        return new T(std::forward<Args>(args)...);
    }

private:
    template<typename Y> friend class sp;
    template<typename Y> friend class wp;

    void setPointer(T* pointer);

    T* mPointer;

    template<typename Y>
    friend sp<Y> Object::Destroyer::reviveObject(Object* obj);
};

#undef COMPARE_STRONG_REFERENCE


#define COMPARE_WEAK_REFERENCE(_operator_)                     \
inline bool operator _operator_ (const sp<T>& o) const {       \
    return mPointer _operator_ o.mPointer;                     \
}                                                              \
inline bool operator _operator_ (const T* o) const {           \
    return mPointer _operator_ o;                              \
}                                                              \
template<typename U>                                           \
inline bool operator _operator_ (const sp<U>& o) const {       \
    return mPointer _operator_ o.mPointer;                     \
}                                                              \
template<typename U>                                           \
inline bool operator _operator_ (const U* o) const {           \
    return mPointer _operator_ o;                              \
}

template<typename T>
class wp {
public:
    typedef typename Object::WeakReference WeakReference;

    inline wp() : mPointer(nullptr), mReference(nullptr) { }

    wp(T* other);
    wp(const wp<T>& other);
    wp(const sp<T>& other);
    template<typename U> wp(U* other);
    template<typename U> wp(const sp<U>& other);
    template<typename U> wp(const wp<U>& other);

    ~wp();

    sp<T> get() const;

    // Assignment operators
    wp& operator=(T* other);
    wp& operator=(const wp<T>& other);
    wp& operator=(const sp<T>& other);

    template<typename U> wp& operator=(U* other);
    template<typename U> wp& operator=(const wp<U>& other);
    template<typename U> wp& operator=(const sp<U>& other);

    // Operators
    COMPARE_WEAK_REFERENCE(==)
    COMPARE_WEAK_REFERENCE(!=)
    COMPARE_WEAK_REFERENCE(>)
    COMPARE_WEAK_REFERENCE(<)
    COMPARE_WEAK_REFERENCE(<=)
    COMPARE_WEAK_REFERENCE(>=)

    void clear();

    inline bool operator==(const wp<T>& o) const {
        return (mPointer == o.mPointer) && (mReference == o.mReference);
    }

    template<typename U>
    inline bool operator==(const wp<U>& o) const {
        return mPointer == o.mPointer;
    }

    inline bool operator>(const wp<T>& o) const {
        return (mPointer == o.mPointer) ?
                (mReference > o.mReference) : (mPointer > o.mPointer);
    }

    template<typename U>
    inline bool operator>(const wp<U>& o) const {
        return (mPointer == o.mPointer) ?
                (mReference > o.mReference) : (mPointer > o.mPointer);
    }

    inline bool operator<(const wp<T>& o) const {
        return (mPointer == o.mPointer) ?
                (mReference < o.mReference) : (mPointer < o.mPointer);
    }

    template<typename U>
    inline bool operator<(const wp<U>& o) const {
        return (mPointer == o.mPointer) ?
                (mReference < o.mReference) : (mPointer < o.mPointer);
    }

    inline bool operator!=(const wp<T>& o) const {
        return mReference != o.mReference;
    }

    template<typename U> inline bool operator!=(const wp<U>& o) const {
        return !operator ==(o);
    }

    inline bool operator<=(const wp<T>& o) const {
        return !operator >(o);
    }

    template<typename U> inline bool operator<=(const wp<U>& o) const {
        return !operator >(o);
    }

    inline bool operator>=(const wp<T>& o) const {
        return !operator <(o);
    }

    template<typename U> inline bool operator>=(const wp<U>& o) const {
        return !operator <(o);
    }

    template<typename ... Args>
    static wp<T> of(Args&& ... args) {
        return new T(std::forward<Args>(args)...);
    }

private:
    template<typename Y> friend class sp;
    template<typename Y> friend class wp;

    T* mPointer;
    WeakReference* mReference;
};

#undef COMPARE_WEAK_REFERENCE


template<typename T>
sp<T>::sp(T* other) :
        mPointer(other) {
    if (other) {
        other->incStrongReference(this);
    }
}

template<typename T>
sp<T>::sp(const sp<T>& other) :
        mPointer(other.mPointer) {
    if (mPointer) {
        mPointer->incStrongReference(this);
    }
}

template<typename T>
sp<T>::sp(sp<T>&& other) :
        mPointer(other.mPointer) {
    other.mPointer = nullptr;
    if (mPointer) {
        mPointer->moveStrongReference(&other, this);
    }
}

template<typename T> template<typename U>
sp<T>::sp(U* other) :
        mPointer(other) {
    if (other) {
        (static_cast<T*>(other))->incStrongReference(this);
    }
}

template<typename T> template<typename U>
sp<T>::sp(const sp<U>& other) :
        mPointer(other.mPointer) {
    if (mPointer) {
        mPointer->incStrongReference(this);
    }
}

template<typename T> template<typename U>
sp<T>::sp(sp<U>&& other) :
        mPointer(other.mPointer) {
    other.mPointer = nullptr;
    if (mPointer) {
        mPointer->moveStrongReference(&other, this);
    }
}

template<typename T>
sp<T>::~sp() {
    if (mPointer) {
        mPointer->decStrongReference(this);
    }
}

template<typename T>
sp<T>& sp<T>::operator=(const sp<T>& other) {
    T* pointer(*const_cast<T* volatile*>(&mPointer));
    T* otherPointer(other.mPointer);
    if (otherPointer) {
        otherPointer->incStrongReference(this);
    }
    if (pointer) {
        pointer->decStrongReference(this);
    }
    if (pointer != *const_cast<T* volatile*>(&mPointer)) {
        spDataRaceException();
    }
    mPointer = otherPointer;
    return *this;
}

template<typename T>
sp<T>& sp<T>::operator=(sp<T>&& other) {
    T* pointer(*const_cast<T* volatile*>(&mPointer));
    if (pointer) {
        pointer->decStrongReference(this);
    }
    if (pointer != *const_cast<T* volatile*>(&mPointer)) {
        spDataRaceException();
    }
    mPointer = other.mPointer;
    other.mPointer = nullptr;
    if (mPointer) {
        mPointer->moveStrongReference(&other, this);
    }
    return *this;
}

template<typename T>
sp<T>& sp<T>::operator=(T* other) {
    T* pointer(*const_cast<T* volatile*>(&mPointer));
    if (other) {
        other->incStrongReference(this);
    }
    if (pointer) {
        pointer->decStrongReference(this);
    }
    if (pointer != *const_cast<T* volatile*>(&mPointer)) {
        spDataRaceException();
    }
    mPointer = other;
    return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator=(const sp<U>& other) {
    T* pointer(*const_cast<T* volatile*>(&mPointer));
    T* otherPointer(other.mPointer);
    if (otherPointer) {
        otherPointer->incStrongReference(this);
    }
    if (pointer) {
        pointer->decStrongReference(this);
    }
    if (pointer != *const_cast<T* volatile*>(&mPointer)) {
        spDataRaceException();
    }
    mPointer = otherPointer;
    return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator=(sp<U>&& other) {
    T* pointer(*const_cast<T* volatile*>(&mPointer));
    if (mPointer) {
        mPointer->decStrongReference(this);
    }
    if (pointer != *const_cast<T* volatile*>(&mPointer)) {
        spDataRaceException();
    }
    mPointer = other.mPointer;
    other.mPointer = nullptr;
    if (mPointer) {
        mPointer->moveStrongReference(&other, this);
    }
    return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator=(U* other) {
    T* pointer(*const_cast<T* volatile*>(&mPointer));
    if (other) {
        (static_cast<T*>(other))->incStrongReference(this);
    }
    if (pointer) {
        pointer->decStrongReference(this);
    }
    if (pointer != *const_cast<T* volatile*>(&mPointer)) {
        spDataRaceException();
    }
    mPointer = other;
    return *this;
}

template<typename T>
void sp<T>::clear() {
    if (mPointer) {
        // Make sure that mPointer is already set to nullptr before decStrongReference triggers the sp<T>'s dtor.
        T* pointer = mPointer;
        mPointer = nullptr;
        pointer->decStrongReference(this);
    }
}

template<typename T>
void sp<T>::setPointer(T* pointer) {
    mPointer = pointer;
}


template<typename T>
wp<T>::wp(T* other) :
        mPointer(other),
        mReference(nullptr) {
    if (other) {
        mReference = other->createWeakReference(this);
    }
}

template<typename T>
wp<T>::wp(const wp<T>& other) :
        mPointer(other.mPointer),
        mReference(other.mReference) {
    if (mPointer) {
        mReference->incWeakReference(this);
    }
}

template<typename T>
wp<T>::wp(const sp<T>& other) :
        mPointer(other.mPointer),
        mReference(nullptr) {
    if (mPointer) {
        mReference = mPointer->createWeakReference(this);
    }
}

template<typename T> template<typename U>
wp<T>::wp(U* other) :
        mPointer(other),
        mReference(nullptr) {
    if (other) {
        mReference = other->createWeakReference(this);
    }
}

template<typename T> template<typename U>
wp<T>::wp(const wp<U>& other) :
        mPointer(other.mPointer),
        mReference(nullptr) {
    if (mPointer) {
        mReference = other.mReference;
        mReference->incWeakReference(this);
    }
}

template<typename T> template<typename U>
wp<T>::wp(const sp<U>& other) :
        mPointer(other.mPointer),
        mReference(nullptr) {
    if (mPointer) {
        mReference = mPointer->createWeakReference(this);
    }
}

template<typename T>
wp<T>::~wp() {
    if (mPointer) {
        mReference->decWeakReference(this);
    }
}

template<typename T>
wp<T>& wp<T>::operator=(T* other) {
    WeakReference* newReference = other ? other->createWeakReference(this) : nullptr;
    if (mPointer) {
        mReference->decWeakReference(this);
    }
    mPointer = other;
    mReference = newReference;
    return *this;
}

template<typename T>
wp<T>& wp<T>::operator=(const wp<T>& other) {
    WeakReference* otherReference(other.mReference);
    T* otherPointer(other.mPointer);
    if (otherPointer) {
        otherReference->incWeakReference(this);
    }
    if (mPointer) {
        mReference->decWeakReference(this);
    }
    mPointer = otherPointer;
    mReference = otherReference;
    return *this;
}

template<typename T>
wp<T>& wp<T>::operator=(const sp<T>& other) {
    WeakReference* newReference = other != nullptr ? other->createWeakReference(this) : nullptr;
    T* otherPointer(other.mPointer);
    if (mPointer) {
        mReference->decWeakReference(this);
    }
    mPointer = otherPointer;
    mReference = newReference;
    return *this;
}

template<typename T> template<typename U>
wp<T>& wp<T>::operator=(U* other) {
    WeakReference* newReference = other ? other->createWeakReference(this) : nullptr;
    if (mPointer) {
        mReference->decWeakReference(this);
    }
    mPointer = other;
    mReference = newReference;
    return *this;
}

template<typename T> template<typename U>
wp<T>& wp<T>::operator=(const wp<U>& other) {
    WeakReference* otherReference(other.mReference);
    U* otherPointer(other.mPointer);
    if (otherPointer) {
        otherReference->incWeakReference(this);
    }
    if (mPointer) {
        mReference->decWeakReference(this);
    }
    mPointer = otherPointer;
    mReference = otherReference;
    return *this;
}

template<typename T> template<typename U>
wp<T>& wp<T>::operator=(const sp<U>& other) {
    WeakReference* newReference = other != nullptr ? other->createWeakReference(this) : nullptr;
    U* otherPointer(other.mPointer);
    if (mPointer) {
        mReference->decWeakReference(this);
    }
    mPointer = otherPointer;
    mReference = newReference;
    return *this;
}

template<typename T>
sp<T> wp<T>::get() const {
    sp<T> newReference;
    if (mPointer && mReference->tryIncStrongReference(&newReference)) {
        newReference.setPointer(mPointer);
    }
    return newReference;
}

template<typename T>
void wp<T>::clear() {
    if (mPointer) {
        // Make sure that mPointer is already set to nullptr before decWeakReference triggers the wp<T>'s dtor.
        mPointer = nullptr;
        mReference->decWeakReference(this);
    }
}

template<typename T>
sp<T> Object::Destroyer::reviveObject(Object* obj) {
    sp<T> newReference;
    if (obj != nullptr) {
        obj->setObjectLifetime(OBJECT_LIFETIME_WEAK_REFERENCE);
        if (obj->getWeakReference()->tryIncStrongReference(&newReference)) {
            newReference.setPointer(static_cast<T*>(obj));
        }
        obj->setObjectLifetime(OBJECT_LIFETIME_STRONG_REFERENCE);
    }
    return newReference;
}


template<typename DESTINATION_CLASS, typename SOURCE_CLASS>
inline sp<DESTINATION_CLASS> object_cast(const sp<SOURCE_CLASS>& object) {
    return static_cast<DESTINATION_CLASS*>(object.getPointer());
}

template<typename DESTINATION_CLASS, typename SOURCE_CLASS>
inline sp<DESTINATION_CLASS> object_cast(SOURCE_CLASS* object) {
    return static_cast<DESTINATION_CLASS*>(object);
}

/// @private
template<typename K>
struct Hasher {
};

/// @private
template<typename K>
struct Hasher<mindroid::sp<K>> {
    std::size_t operator()(const mindroid::sp<K>& object) const {
        const mindroid::sp<mindroid::Object>& self = object;
        return self->hashCode();
    }
};

/// @private
template<typename K>
struct EqualityComparator {
};

/// @private
template<typename K>
struct EqualityComparator<mindroid::sp<K>> {
    bool operator() (const mindroid::sp<K>& lhs, const mindroid::sp<K>& rhs) const {
        const mindroid::sp<mindroid::Object>& object1 = lhs;
        const mindroid::sp<mindroid::Object>& object2 = rhs;
        return object1->equals(object2);
    }
};

/// @private
template<typename K>
struct LessThanComparator {
};

/// @private
template<typename K>
struct LessThanComparator<mindroid::sp<K>> {
    bool operator() (const mindroid::sp<K>& lhs, const mindroid::sp<K>& rhs) const {
        const mindroid::sp<mindroid::Object>& object1 = lhs;
        const mindroid::sp<mindroid::Object>& object2 = rhs;
        return object1->hashCode() < object2->hashCode();
    }
};

} /* namespace mindroid */

#endif /* MINDROID_LANG_OBJECT_H_ */
