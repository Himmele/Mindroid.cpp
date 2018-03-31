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

#include <mindroid/lang/Object.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <typeinfo>
#include <mutex>

// Log all reference counting operations
#define PRINT_REFERENCES 0
// Debugging operations
#define DEBUG_REFERENCES 0
#define DEBUG_REFERENCES_TRACK_REFERENCES 0
#define DEBUG_REFERENCES_MEMORIZE_REF_OPERATIONS_DURING_REF_TRACKING 0

namespace mindroid {

#define INITIAL_STRONG_REFERENCE_VALUE (1<<28)

class Object::WeakReferenceImpl : public Object::WeakReference {
public:
    std::atomic<int32_t> mStrongReferenceCounter;
    std::atomic<int32_t> mWeakReferenceCounter;
    Object* const mObject;
    std::atomic<int32_t> mFlags;
    Destroyer* mDestroyer;

#if !DEBUG_REFERENCES
    explicit WeakReferenceImpl(Object* object) :
            mStrongReferenceCounter(INITIAL_STRONG_REFERENCE_VALUE),
            mWeakReferenceCounter(0),
            mObject(object),
            mFlags(0),
            mDestroyer(nullptr) {
    }

    void addStrongReference(const void* id) { }
    void removeStrongReference(const void* id) { }
    void addWeakReference(const void* id) { }
    void removeWeakReference(const void* id) { }
    void printReferences() const { }
    void trackReference(bool, bool) { }

#else
    WeakReferenceImpl(Object* object) :
            mStrongReferenceCounter(INITIAL_STRONG_REFERENCE_VALUE),
            mWeakReferenceCounter(0),
            mObject(object),
            mFlags(0),
            mDestroyer(nullptr),
            mStrongReferences(nullptr),
            mWeakReferences(nullptr),
            mTrackReferences(!!DEBUG_REFERENCES_TRACK_REFERENCES),
            mMemorizeRefOperationsDuringRefTracking(!!DEBUG_REFERENCES_MEMORIZE_REF_OPERATIONS_DURING_REF_TRACKING) {
    }

    ~WeakReferenceImpl() {
        if (!mMemorizeRefOperationsDuringRefTracking && mStrongReferences != nullptr) {
            DEBUG_INFO("Remaining strong references: ");
            mLock.lock();
            Item* item = mStrongReferences;
            while (item) {
                char c = item->referenceCounter >= 0 ? '+' : '-';
                DEBUG_INFO("\t%c ID %p (reference couter is %d):", c, item->id, item->referenceCounter);
                item = item->nextItem;
            }
            mLock.unlock();
        }

        if (!mMemorizeRefOperationsDuringRefTracking && mWeakReferences != nullptr) {
            DEBUG_INFO("Remaining weak references: ");
            mLock.lock();
            Item* item = mWeakReferences;
            while (item) {
                char c = item->referenceCounter >= 0 ? '+' : '-';
                DEBUG_INFO("\t%c ID %p (reference counter is %d):", c, item->id, item->referenceCounter);
                item = item->nextItem;
            }
            mLock.unlock();
        }
    }

    void addStrongReference(const void* id) {
        addItem(&mStrongReferences, id, mStrongReferenceCounter);
    }

    void removeStrongReference(const void* id) {
        if (!mMemorizeRefOperationsDuringRefTracking) {
            removeItem(&mStrongReferences, id);
        } else {
            addItem(&mStrongReferences, id, -mStrongReferenceCounter);
        }
    }

    void addWeakReference(const void* id) {
        addItem(&mWeakReferences, id, mWeakReferenceCounter);
    }

    void removeWeakReference(const void* id) {
        if (!mMemorizeRefOperationsDuringRefTracking) {
            removeItem(&mWeakReferences, id);
        } else {
            addItem(&mWeakReferences, id, -mWeakReferenceCounter);
        }
    }

    void trackReference(bool trackReferences, bool memorizeRefOperationsDuringRefTracking) {
        mTrackReferences = trackReferences;
        mMemorizeRefOperationsDuringRefTracking = memorizeRefOperationsDuringRefTracking;
    }

    void printReferences() const {
        char* buffer = (char*) malloc(1);
        buffer[0] = '\0';

        {
            mLock.lock();
            char tmpBuffer[128];
            sprintf(tmpBuffer, "Strong references on Object %p (WeakReference %p):\n", mObject, this);
            buffer = (char*) realloc(buffer, strlen(buffer) + strlen(tmpBuffer) + 1);
            strcat(buffer, tmpBuffer);
            buffer = printItems(buffer, mStrongReferences);
            sprintf(tmpBuffer, "Weak references on Object %p (WeakReference %p):\n", mObject, this);
            buffer = (char*) realloc(buffer, strlen(buffer) + strlen(tmpBuffer) + 1);
            strcat(buffer, tmpBuffer);
            buffer = printItems(buffer, mWeakReferences);
            mLock.unlock();
        }

        DEBUG_INFO("%s", buffer);

        free(buffer);
    }

private:
    struct Item {
        const void* id;
        int32_t referenceCounter;
        Item* nextItem;
    };

    void addItem(Item** items, const void* id, int32_t refCounter) {
        if (mTrackReferences) {
            mLock.lock();
            Item* item = new Item;
            item->id = id;
            item->referenceCounter = refCounter;
            item->nextItem = *items;
            *items = item;
            mLock.unlock();
        }
    }

    void removeItem(Item** items, const void* id) {
        if (mTrackReferences) {
            mLock.lock();
            Item* item = *items;
            while (item != nullptr) {
                if (item->id == id) {
                    *items = item->nextItem;
                    delete item;
                    mLock.unlock();
                    return;
                }

                items = &item->nextItem;
                item = *items;
            }
            mLock.unlock();

            ERROR_INFO("Removing id %p on Object %p (WeakReference %p) that doesn't exist!\n", id, mObject, this);
        }
    }

    char* printItems(char* buffer, const Item* items) const {
        char tmpBuffer[128];
        while (items) {
            char c = items->referenceCounter >= 0 ? '+' : '-';
            sprintf(tmpBuffer, "\t%c ID %p (reference count: %d):\n", c, items->id, items->referenceCounter);
            buffer = (char*) realloc(buffer, strlen(buffer) + strlen(tmpBuffer) + 1);
            strcat(buffer, tmpBuffer);
            items = items->nextItem;
        }
        return buffer;
    }

    mutable std::mutex mLock;
    Item* mStrongReferences;
    Item* mWeakReferences;
    bool mTrackReferences;
    bool mMemorizeRefOperationsDuringRefTracking;
#endif
};

Object::Object() : mReference(new WeakReferenceImpl(this)) { }

Object::~Object() {
    int32_t flags = mReference->mFlags.load(std::memory_order_relaxed);
    if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_WEAK_REFERENCE) {
        if (mReference->mWeakReferenceCounter.load(std::memory_order_relaxed) == 0) {
            delete mReference;
        }
    } else if (mReference->mStrongReferenceCounter.load(std::memory_order_relaxed) == INITIAL_STRONG_REFERENCE_VALUE) {
        delete mReference;
    }
    // For debugging purposes, clear mReference.
    const_cast<WeakReferenceImpl*&>(mReference) = nullptr;
}

bool Object::equals(const sp<Object>& other) const {
    if (other == nullptr) {
        return false;
    }
    return this == other.getPointer();
}

size_t Object::hashCode() const {
    return (size_t) mReference->mObject;
}

void Object::incStrongReference(const void* id) const {
    WeakReferenceImpl* const reference = mReference;
    reference->incWeakReference(id);
    reference->addStrongReference(id);
    const int32_t oldStrongReferenceCount = reference->mStrongReferenceCounter.fetch_add(1, std::memory_order_relaxed);
    ASSERT(oldStrongReferenceCount > 0, "Object::incStrongReference() called on %p after underflow", reference);
#if PRINT_REFERENCES
    if (oldStrongReferenceCount == INITIAL_STRONG_REFERENCE_VALUE) {
        DEBUG_INFO("Object::incStrongReference() of %p from %p: reference count is %d\n", this, id, 1);
    } else {
        DEBUG_INFO("Object::incStrongReference() of %p from %p: reference count is %d\n", this, id, oldStrongReferenceCount + 1);
    }
#endif
    if (oldStrongReferenceCount != INITIAL_STRONG_REFERENCE_VALUE) {
        return;
    }
    reference->mStrongReferenceCounter.fetch_sub(INITIAL_STRONG_REFERENCE_VALUE, std::memory_order_relaxed);
    const_cast<Object*>(this)->onFirstReference();
}

void Object::decStrongReference(const void* id) const {
    WeakReferenceImpl* const reference = mReference;
    reference->removeStrongReference(id);
    const int32_t oldStrongReferenceCount = reference->mStrongReferenceCounter.fetch_sub(1, std::memory_order_release);
#if PRINT_REFERENCES
    DEBUG_INFO("Object::decStrongReference() of %p from %p: reference count is %d\n", this, id, oldStrongReferenceCount - 1);
#endif
    ASSERT(oldStrongReferenceCount >= 1, "Object::decStrongReference() called on %p too many times", reference);
    if (oldStrongReferenceCount == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        const_cast<Object*>(this)->onLastReference(id);
        int32_t flags = reference->mFlags.load(std::memory_order_relaxed);
        if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG_REFERENCE) {
            if (reference->mDestroyer == nullptr) {
                delete this;
            } else {
                reference->mDestroyer->destroy(const_cast<Object*>(this));
            }
        }
    }
    reference->decWeakReference(id);
}

int32_t Object::getStrongReferenceCount() const {
    return mReference->mStrongReferenceCounter.load(std::memory_order_relaxed);
}

void Object::moveStrongReference(const void* oldId, const void* newId) const {
    WeakReferenceImpl* const reference = mReference;
    reference->removeWeakReference(oldId);
    reference->addWeakReference(newId);
    reference->removeStrongReference(oldId);
    reference->addStrongReference(newId);
}

void Object::setObjectLifetime(int32_t mode) const {
    switch (mode) {
    case OBJECT_LIFETIME_WEAK_REFERENCE:
    case OBJECT_LIFETIME_STRONG_REFERENCE:
        mReference->mFlags.fetch_or(mode, std::memory_order_relaxed);
        break;
    }
}

void Object::onFirstReference() {
}

void Object::onLastReference(const void* id) {
}

void Object::setDestroyer(Object::Destroyer* destroyer) {
    mReference->mDestroyer = destroyer;
}

Object::Destroyer::~Destroyer() {
}

Object* Object::WeakReference::toObject() const {
    return static_cast<const WeakReferenceImpl*>(this)->mObject;
}

void Object::WeakReference::incWeakReference(const void* id) {
    WeakReferenceImpl* const reference = static_cast<WeakReferenceImpl*>(this);
    reference->addWeakReference(id);
    const int32_t oldWeakReferenceCount = reference->mWeakReferenceCounter.fetch_add(1, std::memory_order_relaxed);
    ASSERT(oldWeakReferenceCount >= 0, "Object::WeakReference::incWeakReference() called on %p after underflow", this);
}

void Object::WeakReference::decWeakReference(const void* id) {
    WeakReferenceImpl* const reference = static_cast<WeakReferenceImpl*>(this);
    reference->removeWeakReference(id);
    const int32_t oldWeakReferenceCount = reference->mWeakReferenceCounter.fetch_sub(1, std::memory_order_release);
    ASSERT(oldWeakReferenceCount >= 1, "Object::WeakReference::decWeakReference() called on %p too many times", this);
    if (oldWeakReferenceCount != 1) {
        return;
    }

    std::atomic_thread_fence(std::memory_order_acquire);
    int32_t flags = reference->mFlags.load(std::memory_order_relaxed);
    if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG_REFERENCE) {
        if (reference->mStrongReferenceCounter.load(std::memory_order_relaxed) == INITIAL_STRONG_REFERENCE_VALUE) {
            if (reference->mDestroyer == nullptr) {
                delete reference->mObject;
            } else {
                if (reference->mObject != nullptr) {
                    reference->mDestroyer->destroy(reference->mObject);
                }
            }
        } else {
            delete reference;
        }
    } else {
        if (reference->mDestroyer == nullptr) {
            delete reference->mObject;
        } else {
            if (reference->mObject != nullptr) {
                reference->mDestroyer->destroy(reference->mObject);
            }
        }
    }
}

bool Object::WeakReference::tryIncStrongReference(const void* id) {
    incWeakReference(id);

    WeakReferenceImpl* const reference = static_cast<WeakReferenceImpl*>(this);
    int32_t strongReferenceCount = reference->mStrongReferenceCounter.load(std::memory_order_relaxed);
    ASSERT(strongReferenceCount >= 0, "Object::WeakReference::tryIncStrongReference() called on %p after underflow", this);

    while (strongReferenceCount > 0 && strongReferenceCount != INITIAL_STRONG_REFERENCE_VALUE) {
        if (reference->mStrongReferenceCounter.compare_exchange_weak(strongReferenceCount, strongReferenceCount + 1, std::memory_order_relaxed)) {
            break;
        }
    }

    if (strongReferenceCount <= 0 || strongReferenceCount == INITIAL_STRONG_REFERENCE_VALUE) {
        int32_t flags = reference->mFlags.load(std::memory_order_relaxed);
        if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG_REFERENCE) {
            if (strongReferenceCount <= 0) {
                decWeakReference(id);
                return false;
            }

            while (strongReferenceCount > 0) {
                if (reference->mStrongReferenceCounter.compare_exchange_weak(strongReferenceCount, strongReferenceCount + 1, std::memory_order_relaxed)) {
                    break;
                }
            }

            if (strongReferenceCount <= 0) {
                decWeakReference(id);
                return false;
            }
        } else {
            strongReferenceCount = reference->mStrongReferenceCounter.fetch_add(1, std::memory_order_relaxed);
        }
    }

    reference->addStrongReference(id);

#if PRINT_REFERENCES
    DEBUG_INFO("Object::WeakReference::tryIncStrongReference() of %p from %p: reference count is %d\n", this, id, strongReferenceCount + 1);
#endif

    if (strongReferenceCount == INITIAL_STRONG_REFERENCE_VALUE) {
        reference->mStrongReferenceCounter.fetch_sub(INITIAL_STRONG_REFERENCE_VALUE, std::memory_order_relaxed);
    }

    return true;
}

int32_t Object::WeakReference::getWeakReferenceCount() const {
    return static_cast<const WeakReferenceImpl*>(this)->mWeakReferenceCounter.load(std::memory_order_relaxed);
}

void Object::WeakReference::printReferences() const {
    static_cast<const WeakReferenceImpl*>(this)->printReferences();
}

void Object::WeakReference::trackReference(bool trackReference, bool memorizeRefOperationsDuringRefTracking) {
    static_cast<WeakReferenceImpl*>(this)->trackReference(trackReference, memorizeRefOperationsDuringRefTracking);
}

Object::WeakReference* Object::createWeakReference(const void* id) const {
    mReference->incWeakReference(id);
    return mReference;
}

Object::WeakReference* Object::getWeakReference() const {
    return mReference;
}

void spDataRaceException() {
    printf("Data race exception in sp<> assignment\n");
}

} /* namespace mindroid */
