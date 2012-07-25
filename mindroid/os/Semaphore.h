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

#ifndef MINDROID_SEMAPHORE_H_
#define MINDROID_SEMAPHORE_H_

#include <stdint.h>
#include <semaphore.h>
#include "mindroid/util/Utils.h"

namespace mindroid {

class Semaphore
{
public:
	Semaphore(uint32_t value = 0);
	~Semaphore();
	void signal();
	bool wait();
	bool wait(uint32_t timeout);

private:
	sem_t mSemaphore;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(Semaphore)
};

} /* namespace mindroid */

#endif /* MINDROID_SEMAPHORE_H_ */
