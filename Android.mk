#  Copyright (c) 2016 E.S.R. Labs. All rights reserved.
#
#  NOTICE:  All information contained herein is, and remains
#  the property of E.S.R.Labs and its suppliers, if any.
#  The intellectual and technical concepts contained herein are
#  proprietary to E.S.R.Labs and its suppliers and may be covered
#  by German and Foreign Patents, patents in process, and are protected
#  by trade secret or copyright law.
#  Dissemination of this information or reproduction of this material
#  is strictly forbidden unless prior written permission is obtained
#  from E.S.R.Labs.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mindroid/app/SharedPreferencesImpl.cpp \
	mindroid/app/IOnSharedPreferenceChangeListener.cpp \
	mindroid/app/Service.cpp \
	mindroid/app/ContextImpl.cpp \
	mindroid/content/Context.cpp \
	mindroid/content/ComponentName.cpp \
	mindroid/content/ContextWrapper.cpp \
	mindroid/content/pm/IPackageManager.cpp \
	mindroid/content/pm/PackageManagerService.cpp \
	mindroid/content/Intent.cpp \
	mindroid/io/File.cpp \
	mindroid/lang/Object.cpp \
	mindroid/lang/Thread.cpp \
	mindroid/lang/String.cpp \
	mindroid/lang/Closure.cpp \
	mindroid/lang/Class.cpp \
	mindroid/net/DatagramSocket.cpp \
	mindroid/net/SocketAddress.cpp \
	mindroid/net/Socket.cpp \
	mindroid/net/ServerSocket.cpp \
	mindroid/os/Environment.cpp \
	mindroid/os/Handler.cpp \
	mindroid/os/Message.cpp \
	mindroid/os/Binder.cpp \
	mindroid/os/IRemoteCallback.cpp \
	mindroid/os/Process.cpp \
	mindroid/os/Bundle.cpp \
	mindroid/os/HandlerThread.cpp \
	mindroid/os/IProcess.cpp \
	mindroid/os/IServiceManager.cpp \
	mindroid/os/ServiceManager.cpp \
	mindroid/os/Looper.cpp \
	mindroid/os/AsyncTask.cpp \
	mindroid/os/SystemClock.cpp \
	mindroid/os/MessageQueue.cpp \
	mindroid/util/Assert.cpp \
	mindroid/util/Variant.cpp \
	mindroid/util/concurrent/SerialExecutor.cpp \
	mindroid/util/concurrent/locks/ReentrantLock.cpp \
	mindroid/util/concurrent/locks/ConditionImpl.cpp \
	mindroid/util/concurrent/ThreadPoolExecutor.cpp \
	mindroid/util/concurrent/Semaphore.cpp \
	mindroid/util/Log.cpp \
	mindroid/util/Logger.cpp

LOCAL_CFLAGS := -D__ANDROID__ -Wno-unused-parameter -fexceptions
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= libmindroid

include $(BUILD_STATIC_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
