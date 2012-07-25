LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
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
	mindroid/net/SocketAddress.cpp \
	mindroid/net/ServerSocket.cpp \
	mindroid/net/Socket.cpp \
	mindroid/net/DatagramSocket.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= libmindroid

include $(BUILD_SHARED_LIBRARY)

# ----

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	Test.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid
	
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= AndroidMessagingAndConcurrencyTest

include $(BUILD_EXECUTABLE)

# ----

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	LooperThreadExample.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid
	
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= LooperThreadExample

include $(BUILD_EXECUTABLE)

# ----

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	AsyncTaskExample.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid
	
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= AsyncTaskExample

include $(BUILD_EXECUTABLE)

# ----

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	DelegateExample.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libstlport \
	libmindroid
	
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

include external/stlport/libstlport.mk

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= DelegateExample

include $(BUILD_EXECUTABLE)

# ----

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	RefCountingTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libstlport \
	libmindroid
	
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= RefCountingTest

include $(BUILD_EXECUTABLE)

# ----

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	StringTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid
	
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= StringTest

include $(BUILD_EXECUTABLE)

# ----

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	NetworkTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid
	
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= NetworkTest

include $(BUILD_EXECUTABLE)

