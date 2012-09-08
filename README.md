## Mindroid is mainly a messaging and concurrency framework for native C++ development ##
A lot of ideas of this framework are based on Google's Java Android messaging and concurrency classes.
This small messaging framework is very handy not only for Android developers :-).

frameworks/base/core/java/os:
- Looper (loops on its MessageQueue) -> Looper.prepare binds the Looper to the calling thread's context by using the thread local storage pthread functions
- Handler (binds to a Looper for message handling) -> may check the thread local storage for the looper
- MessageQueue (message queue with timeout handling mechanism)
- Message (always has a target Handler by which this message is handeled)
- Messenger (extends the message handling framework to a true IPC mechanism using Binder IPC) -> http://developer.android.com/guide/topics/fundamentals/bound-services.html#Messenger
- Binder (Binder remotable objects and proxies) -> Binder IPC normally uses Parcel objects that are sent from one process to another process

To do:
- add the Binder IPC interface by implementing the Messenger class as the Java framework does.
- add Parcels.
- take a look at the Chrome project for doing a Windows port (e.g. condition_variable_win.cc).
- ...

