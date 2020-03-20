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
	src/mindroid/app/ContextImpl.cpp \
	src/mindroid/app/IOnSharedPreferenceChangeListener.cpp \
	src/mindroid/app/Service.cpp \
	src/mindroid/app/SharedPreferencesImpl.cpp \
	src/mindroid/content/ComponentName.cpp \
	src/mindroid/content/Context.cpp \
	src/mindroid/content/ContextWrapper.cpp \
	src/mindroid/content/Intent.cpp \
	src/mindroid/content/pm/IPackageManager.cpp \
	src/mindroid/content/pm/PackageManager.cpp \
	src/mindroid/content/pm/PackageManagerService.cpp \
	src/mindroid/io/ByteArrayInputStream.cpp \
	src/mindroid/io/ByteArrayOutputStream.cpp \
	src/mindroid/io/DataInputStream.cpp \
	src/mindroid/io/DataOutputStream.cpp \
	src/mindroid/io/File.cpp \
	src/mindroid/io/FileInputStream.cpp \
	src/mindroid/io/FileOutputStream.cpp \
	src/mindroid/io/InputStream.cpp \
	src/mindroid/io/OutputStream.cpp \
	src/mindroid/lang/Boolean.cpp \
	src/mindroid/lang/ByteArray.cpp \
	src/mindroid/lang/Byte.cpp \
	src/mindroid/lang/Character.cpp \
	src/mindroid/lang/Class.cpp \
	src/mindroid/lang/Double.cpp \
	src/mindroid/lang/Float.cpp \
	src/mindroid/lang/Integer.cpp \
	src/mindroid/lang/Long.cpp \
	src/mindroid/lang/Math.cpp \
	src/mindroid/lang/Object.cpp \
	src/mindroid/lang/Short.cpp \
	src/mindroid/lang/StringArray.cpp \
	src/mindroid/lang/StringBuilder.cpp \
	src/mindroid/lang/String.cpp \
	src/mindroid/lang/System.cpp \
	src/mindroid/lang/Thread.cpp \
	src/mindroid/net/DatagramSocket.cpp \
	src/mindroid/net/Inet4Address.cpp \
	src/mindroid/net/Inet6Address.cpp \
	src/mindroid/net/InetAddress.cpp \
	src/mindroid/net/InetSocketAddress.cpp \
	src/mindroid/net/NetworkInterface.cpp \
	src/mindroid/net/ServerSocket.cpp \
	src/mindroid/net/Socket.cpp \
	src/mindroid/net/StandardSocketOptions.cpp \
	src/mindroid/net/URI.cpp \
	src/mindroid/nio/Buffer.cpp \
	src/mindroid/nio/ByteArrayBuffer.cpp \
	src/mindroid/nio/ByteBuffer.cpp \
	src/mindroid/os/AsyncTask.cpp \
	src/mindroid/os/Binder.cpp \
	src/mindroid/os/Bundle.cpp \
	src/mindroid/os/Environment.cpp \
	src/mindroid/os/Handler.cpp \
	src/mindroid/os/HandlerThread.cpp \
	src/mindroid/os/IProcess.cpp \
	src/mindroid/os/IRemoteCallback.cpp \
	src/mindroid/os/IServiceManager.cpp \
	src/mindroid/os/Looper.cpp \
	src/mindroid/os/Message.cpp \
	src/mindroid/os/MessageQueue.cpp \
	src/mindroid/os/Parcel.cpp \
	src/mindroid/os/Process.cpp \
	src/mindroid/os/ServiceManager.cpp \
	src/mindroid/os/SystemClock.cpp \
	src/mindroid/runtime/inspection/Console.cpp \
	src/mindroid/runtime/inspection/ConsoleService.cpp \
	src/mindroid/runtime/inspection/ICommandHandler.cpp \
	src/mindroid/runtime/inspection/IConsole.cpp \
	src/mindroid/runtime/system/Mindroid.cpp \
	src/mindroid/runtime/system/Plugin.cpp \
	src/mindroid/runtime/system/Runtime.cpp \
	src/mindroid/runtime/system/ServiceDiscoveryConfigurationReader.cpp \
	src/mindroid/runtime/system/io/AbstractClient.cpp \
	src/mindroid/runtime/system/io/AbstractServer.cpp \
	src/mindroid/util/Assert.cpp \
	src/mindroid/util/Base64.cpp \
	src/mindroid/util/EventLog.cpp \
	src/mindroid/util/Log.cpp \
	src/mindroid/util/Properties.cpp \
	src/mindroid/util/Variant.cpp \
	src/mindroid/util/concurrent/AsyncAwait.cpp \
	src/mindroid/util/concurrent/Executors.cpp \
	src/mindroid/util/concurrent/SerialExecutor.cpp \
	src/mindroid/util/concurrent/Thenable.cpp \
	src/mindroid/util/concurrent/ThreadPoolExecutor.cpp \
	src/mindroid/util/concurrent/atomic/AtomicBoolean.cpp \
	src/mindroid/util/concurrent/atomic/AtomicInteger.cpp \
	src/mindroid/util/concurrent/locks/ConditionImpl.cpp \
	src/mindroid/util/concurrent/locks/ReentrantLock.cpp \
	src/mindroid/util/logging/ConsoleHandler.cpp \
	src/mindroid/util/logging/FileHandler.cpp \
	src/mindroid/util/logging/ILogger.cpp \
	src/mindroid/util/logging/LogBuffer.cpp \
	src/mindroid/util/logging/Logger.cpp \
	src/mindroid/util/logging/LoggerService.cpp

LOCAL_CFLAGS := -DANDROID -Wno-unused-parameter -fexceptions
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= libmindroid
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src
LOCAL_RTTI_FLAG := -frtti
include $(BUILD_STATIC_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
