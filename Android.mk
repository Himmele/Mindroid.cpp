LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mindroid/os/Thread.cpp \
	mindroid/os/Semaphore.cpp \
	mindroid/os/MessageQueue.cpp \
	mindroid/os/Message.cpp \
	mindroid/os/Looper.cpp \
	mindroid/os/Lock.cpp \
	mindroid/os/CondVar.cpp \
	mindroid/os/Handler.cpp \
	mindroid/os/Clock.cpp \
	mindroid/os/AsyncTask.cpp \
	mindroid/os/SerialExecutor.cpp \
	mindroid/os/ThreadPoolExecutor.cpp \
	mindroid/os/AtomicInteger.cpp.arm \
	mindroid/os/Ref.cpp \
	mindroid/os/Bundle.cpp \
	mindroid/util/Buffer.cpp \
	mindroid/lang/String.cpp \
	mindroid/lang/StringWrapper.cpp \
	mindroid/net/SocketAddress.cpp \
	mindroid/net/ServerSocket.cpp \
	mindroid/net/Socket.cpp \
	mindroid/net/DatagramSocket.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= libmindroid

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))

