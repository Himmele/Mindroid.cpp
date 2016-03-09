/*
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

#ifndef MINDROID_THREAD_H_
#define MINDROID_THREAD_H_

#include "mindroid/lang/String.h"
#include "mindroid/lang/Runnable.h"
#include <pthread.h>

namespace mindroid {

class Thread :
		public Runnable {
public:
	Thread() :
			Thread(nullptr, nullptr) {
	}
	Thread(const char* name) :
			Thread(nullptr, String::valueOf(name)) {
	}
	Thread(const sp<String>& name) :
			Thread(nullptr, name) {
	}
	Thread(const sp<Runnable>& runnable) :
			Thread(runnable, nullptr) {
	}
	Thread(const sp<Runnable>& runnable, const char* name) :
			Thread(runnable, String::valueOf(name)) {
	}
	Thread(const sp<Runnable>& runnable, const sp<String>& name);
	virtual ~Thread() = default;
	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	sp<String> getName() { return mName; }
	virtual void run() { }
	bool start();
	static void sleep(uint32_t milliseconds);
	void interrupt();
	bool isInterrupted() const;
	void join() const;
	static sp<Thread> currentThread();
	bool isAlive() const;

	void setSchedulingParams(int32_t policy, int32_t priority);

private:
	Thread(pthread_t thread);
	static void* exec(void* args);

	sp<Thread> mSelf;
	sp<String> mName;
	sp<Runnable> mRunnable;
	pthread_t mThread;
	bool mStarted;
	bool mInterrupted;

	friend class Looper;
};

} /* namespace mindroid */

#endif /* MINDROID_THREAD_H_ */
