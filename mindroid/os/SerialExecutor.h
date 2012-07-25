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

#ifndef MINDROID_SERIALEXECUTOR_H_
#define MINDROID_SERIALEXECUTOR_H_

#include "mindroid/os/Executor.h"
#include "mindroid/os/LooperThread.h"

namespace mindroid {

class Handler;

class SerialExecutor :
	public Executor
{
public:
	SerialExecutor();
	virtual ~SerialExecutor();

	virtual void execute(const sp<Runnable>& runnable);
	virtual bool cancel(const sp<Runnable>& runnable);

private:
	sp< LooperThread<Handler> > mLooperThread;
	sp<Handler> mHandler;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(SerialExecutor)
};

} /* namespace mindroid */

#endif /* MINDROID_SERIALEXECUTOR_H_ */
