## Mindroid.cpp application framework for C++ ##

Mindroid is an application framework (with focus on messaging and concurrency) that lets you create applications using a set of reusable components - just like Android.
The name Mindroid has two different meanings. On one hand Mindroid is a minimal set of core Android classes and on the other hand these classes also form Android's mind (at least in my opinion).

Blog articles:
- [On Building Reliable Automotive Software Systems](https://himmele.blogspot.de/2017/04/on-building-reliable-automotive.html)
- [Mindroid - Android everywhere](https://himmele.blogspot.de/2013/10/mindroid-android-everywhere.html)
- [Android messaging and concurrency for native C++ code development](https://himmele.blogspot.com/2011/08/android-messaging-and-concurrency-for.html)
- [Alan Kay on Object-Oriented Programming](https://himmele.blogspot.com/2010/11/alan-kay-on-object-oriented-programming.html)

### Ideas from Google Android ###

Most ideas of Mindroid come from Google's Android messaging and concurrency classes.

Android's messaging and concurrency framework (together with the Binder IPC mechanism) forms the basis of all Android applications and services.
The messaging and concurrency framework is mainly based on the Thread, Looper, Message, MessageQueue and Handler classes.
For convenience there is also the AsyncTask class and for inter-process communication there are some other classes like Binder, Parcel and Messenger.

A Looper is used to run a message loop for a thread. Threads by default do not have a message loop associated with them; to create one,
call Looper.prepare() in the thread that is to run the loop, and then Looper.loop() to have it process messages until the loop is stopped.
Most interaction with a message loop is through the Handler class. A Handler allows you to send and process Message and Runnable objects associated with a thread's MessageQueue.
Each Handler instance is associated with a single thread and that thread's message queue. When you create a new Handler,
it is bound to the thread / message queue of the thread that is creating it - from that point on,
it will deliver messages and runnables to that message queue and execute them as they come out of the message queue.
Furthermore, one can bind as many Handlers as he or she wants to a single thread / message queue for message handling.
There are two main uses for a Handler. First a Handler allows you to enqueue an action to be performed on a different thread than your own
and furthermore it also enables you to schedule messages and runnables to be executed at some point in the future.
The AsyncTask class enables proper and easy use of concurrency in a multithread environment.
For more information about this class see Google's "Painless Threading" article.

Source: Google Android Developer Reference

### Ideas from Erlang ###

Process (component) isolation is central to understanding “Concurrency Oriented Programming” (COP),
and to the construction of fault-tolerant sodware.

1. Processes (components) have “share nothing” semantics.
This is obvious since they are imagined to run on physically separated machines.
2. Message passing is the only way to pass data between processes (components).
Again since nothing is shared this is the only means possible to exchange data.
3. Isolation implies that message passing is asynchronous. If process communication is synchronous
then a software error in the receiver of a message could indefinitely block the sender of the
message destroying the property of isolation.
4. Since nothing is shared, everything necessary to perform a distributed computation must be copied. Since nothing is shared, and
the only way to communicate between processes (components) is by message passing, then we will never know if our messages arrive (remember we
said that message passing is inherently unreliable.) The only way to know if a message has been correctly sent is to send a confirmation
message back.

Source: [Joe Armstrong's PhD Thesis](http://erlang.org/download/armstrong_thesis_2003.pdf)

### Ideas from Alan Kay on Object-Oriented Programming ###

"So I thought of objects being like biological cells, only able to communicate with messages (so messaging came at the very beginning - it took a while to see how to do messaging in a programming language efficiently enough to be useful)."

"Object-oriented programming to me means only messaging, encapsulating and hiding state, and extreme late-binding of all things. It could be done in Smalltalk and in LISP. There are possibly other systems in which this is possible, but I'm not aware of them."

"The big idea is "messaging" - that is what the kernel of Smalltalk/Squeak is all about (and it's something that was never quite completed in our Xerox PARC phase). The key in making great and growable systems is much more to design how its modules communicate rather than what their internal properties and behaviors should be."

Source: [Alan Kay on Object-Oriented Programming](http://himmele.blogspot.com/2010/11/alan-kay-on-object-oriented-programming.html)

### Ideas from the [Reactive Manifesto](https://www.reactivemanifesto.org/) ###

Reactive Systems are responsive, resilient, elastic and message-driven.

