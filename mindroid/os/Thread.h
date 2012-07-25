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

#include <pthread.h>
#include "mindroid/util/Utils.h"
#include "mindroid/os/Runnable.h"

namespace mindroid {

class Thread :
		public Runnable
{
public:
	Thread();
	Thread(const sp<Runnable>& runnable);
	virtual ~Thread() { }
	virtual void run() { }
	bool start();
	static void sleep(uint32_t milliseconds);
	void interrupt();
	bool isInterrupted() const;
	void join() const;
	void setSchedulingParams(int32_t policy, int32_t priority);
	static sp<Thread> currentThread();

private:
	Thread(pthread_t thread);
	static void* exec(void* args);

	sp<Runnable> mRunnable;
	sp<Thread> mThreadKeeper;
	pthread_t mThread;
	bool mStarted;
	bool mInterrupted;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(Thread)
};

} /* namespace mindroid */

#endif /* MINDROID_THREAD_H_ */
