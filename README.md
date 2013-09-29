## Mindroid.cpp is mainly a messaging and concurrency framework for C++ development ##

### [Mindroid would even make Alan Kay comfortable with C++ :-)](http://himmele.blogspot.com/2010/11/alan-kay-on-object-oriented-programming.html) ###

Most ideas for Mindroid come from Google's Android messaging and concurrency classes.
Take a look at the tests subdirectory for examples how to use Mindroid.

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
For more information about this class see Google's "Painless Threading" article. (Source: Google Android Developer Reference).

Links:
- [Android messaging and concurrency for native C++ code development](http://himmele.blogspot.com/2011/08/android-messaging-and-concurrency-for.html)
- [Alan Kay on Object-Oriented Programming](http://himmele.blogspot.com/2010/11/alan-kay-on-object-oriented-programming.html)

