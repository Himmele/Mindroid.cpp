/*
 * Copyright 2008 Google Inc.  All rights reserved.
 * http://code.google.com/p/protobuf/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of Google Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Some adjustments by Daniel Himmelein
 */

/**
 * Runnable* runnable = newRunnable(instance, &Test::test, 17)
 *
 * runnable->run();
 */

#ifndef MINDROID_CLOSURE_H_
#define MINDROID_CLOSURE_H_

#include "mindroid/util/Utils.h"
#include "mindroid/os/Runnable.h"

namespace mindroid {

class FuncClosure0 :
		public Runnable
{
public:
	typedef void (*FuncType)();

	FuncClosure0(FuncType func) :
			mFunc(func) {
	}

	virtual ~FuncClosure0() { }

	virtual void run() {
		mFunc();
	}

private:
	FuncType mFunc;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(FuncClosure0)
};

template<typename Class>
class MethodClosure0 :
		public Runnable
{
public:
	typedef void (Class::*MethodType)();

	MethodClosure0(Class& object, MethodType method) :
			mObject(&object),
			mMethod(method) {
	}

	virtual ~MethodClosure0() { }

	virtual void run() {
		(mObject->*mMethod)();
	}

private:
	Class* mObject;
	MethodType mMethod;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(MethodClosure0)
};

template<typename Arg1>
class FuncClosure1 :
		public Runnable
{
public:
	typedef void (*FuncType)(Arg1 arg1);

	FuncClosure1(FuncType func, Arg1 arg1) :
			mFunc(func),
			mArg1(arg1) {
	}

	virtual ~FuncClosure1() { }

	virtual void run() {
		mFunc(mArg1);
	}

private:
	FuncType mFunc;
	Arg1 mArg1;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(FuncClosure1)
};

template<typename Class, typename Arg1>
class MethodClosure1 :
		public Runnable
{
public:
	typedef void (Class::*MethodType)(Arg1 arg1);

	MethodClosure1(Class& object, MethodType method, Arg1 arg1) :
			mObject(&object),
			mMethod(method),
			mArg1(arg1) {
	}

	virtual ~MethodClosure1() { }

	virtual void run() {
		(mObject->*mMethod)(mArg1);
	}

private:
	Class* mObject;
	MethodType mMethod;
	Arg1 mArg1;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(MethodClosure1)
};

template<typename Arg1, typename Arg2>
class FuncClosure2 :
		public Runnable
{
public:
	typedef void (*FuncType)(Arg1 arg1, Arg2 arg2);

	FuncClosure2(FuncType func, Arg1 arg1, Arg2 arg2) :
			mFunc(func),
			mArg1(arg1),
			mArg2(arg2) {
	}

	virtual ~FuncClosure2() { }

	virtual void run() {
		mFunc(mArg1, mArg2);
	}

private:
	FuncType mFunc;
	Arg1 mArg1;
	Arg2 mArg2;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(FuncClosure2)
};

template<typename Class, typename Arg1, typename Arg2>
class MethodClosure2 :
		public Runnable
{
public:
	typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2);

	MethodClosure2(Class& object, MethodType method, Arg1 arg1, Arg2 arg2) :
			mObject(&object),
			mMethod(method),
			mArg1(arg1),
			mArg2(arg2) {
	}

	virtual ~MethodClosure2() { }

	virtual void run() {
		(mObject->*mMethod)(mArg1, mArg2);
	}

private:
	Class* mObject;
	MethodType mMethod;
	Arg1 mArg1;
	Arg2 mArg2;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(MethodClosure2)
};

template<typename Arg1, typename Arg2, typename Arg3>
class FuncClosure3 :
		public Runnable
{
public:
	typedef void (*FuncType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

	FuncClosure3(FuncType func, Arg1 arg1, Arg2 arg2, Arg3 arg3) :
			mFunc(func),
			mArg1(arg1),
			mArg2(arg2),
			mArg3(arg3) {
	}

	virtual ~FuncClosure3() { }

	virtual void run() {
		mFunc(mArg1, mArg2, mArg3);
	}

private:
	FuncType mFunc;
	Arg1 mArg1;
	Arg2 mArg2;
	Arg3 mArg3;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(FuncClosure3)
};

template<typename Class, typename Arg1, typename Arg2, typename Arg3>
class MethodClosure3 :
		public Runnable
{
public:
	typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

	MethodClosure3(Class& object, MethodType method, Arg1 arg1, Arg2 arg2, Arg3 arg3) :
			mObject(&object),
			mMethod(method),
			mArg1(arg1),
			mArg2(arg2),
			mArg3(arg3) {
	}

	virtual ~MethodClosure3() { }

	virtual void run() {
		(mObject->*mMethod)(mArg1, mArg2, mArg3);
	}

private:
	Class* mObject;
	MethodType mMethod;
	Arg1 mArg1;
	Arg2 mArg2;
	Arg3 mArg3;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(MethodClosure3)
};

template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class FuncClosure4 :
		public Runnable
{
public:
	typedef void (*FuncType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4);

	FuncClosure4(FuncType func, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) :
			mFunc(func),
			mArg1(arg1),
			mArg2(arg2),
			mArg3(arg3),
			mArg4(arg4) {
	}

	virtual ~FuncClosure4() { }

	virtual void run() {
		mFunc(mArg1, mArg2, mArg3, mArg4);
	}

private:
	FuncType mFunc;
	Arg1 mArg1;
	Arg2 mArg2;
	Arg3 mArg3;
	Arg4 mArg4;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(FuncClosure4)
};

template<typename Class, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class MethodClosure4 :
		public Runnable
{
public:
	typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4);

	MethodClosure4(Class& object, MethodType method, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) :
			mObject(&object),
			mMethod(method),
			mArg1(arg1),
			mArg2(arg2),
			mArg3( arg3),
			mArg4(arg4) {
	}

	virtual ~MethodClosure4() { }

	virtual void run() {
		(mObject->*mMethod)(mArg1, mArg2, mArg3, mArg4);
	}

private:
	Class* mObject;
	MethodType mMethod;
	Arg1 mArg1;
	Arg2 mArg2;
	Arg3 mArg3;
	Arg4 mArg4;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(MethodClosure4)
};

inline sp<Runnable> newRunnable(void(*func)()) {
	return new FuncClosure0(func);
}

template<typename Class>
inline sp<Runnable> newRunnable(Class& object, void(Class::*method)()) {
	return new MethodClosure0<Class>(object, method);
}

template<typename Arg1>
inline sp<Runnable> newRunnable(void(*func)(Arg1), Arg1 arg1) {
	return new FuncClosure1<Arg1>(func, arg1);
}

template<typename Class, typename Arg1>
inline sp<Runnable> newRunnable(Class& object, void(Class::*method)(Arg1), Arg1 arg1) {
	return new MethodClosure1<Class, Arg1>(object, method, arg1);
}

template<typename Arg1, typename Arg2>
inline sp<Runnable> newRunnable(void(*func)(Arg1, Arg2), Arg1 arg1, Arg2 arg2) {
	return new FuncClosure2<Arg1, Arg2>(func, arg1, arg2);
}

template<typename Class, typename Arg1, typename Arg2>
inline sp<Runnable> newRunnable(Class& object, void(Class::*method)(Arg1, Arg2), Arg1 arg1, Arg2 arg2) {
	return new MethodClosure2<Class, Arg1, Arg2>(object, method, arg1, arg2);
}

template<typename Arg1, typename Arg2, typename Arg3>
inline sp<Runnable> newRunnable(void(*func)(Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3) {
	return new FuncClosure3<Arg1, Arg2, Arg3>(func, arg1, arg2, arg3);
}

template<typename Class, typename Arg1, typename Arg2, typename Arg3>
inline sp<Runnable> newRunnable(Class& object, void(Class::*method)(Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3) {
	return new MethodClosure3<Class, Arg1, Arg2, Arg3>(object, method, arg1, arg2, arg3);
}

template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline sp<Runnable> newRunnable(void(*func)(Arg1, Arg2, Arg3, Arg4), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
	return new FuncClosure4<Arg1, Arg2, Arg3, Arg4>(func, arg1, arg2, arg3, arg4);
}

template<typename Class, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline sp<Runnable> newRunnable(Class& object, void(Class::*method)(Arg1, Arg2, Arg3, Arg4), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
	return new MethodClosure4<Class, Arg1, Arg2, Arg3, Arg4>(object, method, arg1, arg2, arg3, arg4);
}

} /* namespace mindroid */

#endif /* MINDROID_CLOSURE_H_ */
