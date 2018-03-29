#include <gtest/gtest.h>
#include <mindroid/os/Looper.h>
#include <mindroid/os/Handler.h>
#include <mindroid/util/function/Function.h>
#include <mindroid/util/concurrent/Promise.h>

using namespace mindroid;

TEST(Mindroid, Handler1) {
    sp<HandlerThread> thread = new HandlerThread();
    thread->start();
    sp<Handler> handler = new Handler(thread->getLooper());

    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    handler->post([promise] { promise->complete(123); });
    ASSERT_EQ(promise->get(), 123);

    thread->quit();
}

TEST(Mindroid, Handler2) {
    sp<HandlerThread> thread = new HandlerThread();
    thread->start();
    sp<Handler> handler = new Handler(thread->getLooper());

    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    handler->postDelayed([promise] { promise->complete(123); }, 100);
    ASSERT_EQ(promise->get(), 123);

    thread->quit();
}

TEST(Mindroid, Handler3) {
    sp<HandlerThread> thread = new HandlerThread();
    thread->start();
    sp<Handler> handler = new Handler(thread->getLooper());

    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    sp<Function<void, void>> function = new Function<void, void>([promise] { promise->complete(42); });
    handler->postDelayed(function, 100);
    ASSERT_EQ(promise->get(), 42);

    thread->quit();
}

TEST(Mindroid, Handler4) {
    sp<HandlerThread> thread = new HandlerThread();
    thread->start();
    sp<Handler> handler = new Handler(thread->getLooper());

    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    sp<Function<void, void>> function = new Function<void, void>([promise] { promise->complete(42); });
    handler->postDelayed(function, 1000);
    ASSERT_EQ(handler->removeCallbacks(function), true);

    thread->quit();
}
