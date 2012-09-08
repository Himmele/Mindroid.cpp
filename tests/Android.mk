LOCAL_PATH:= $(call my-dir)

# ---- ConcurrencyTest ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	ConcurrencyTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= ConcurrencyTest

include $(BUILD_EXECUTABLE)

# ---- LooperThreadTest ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	LooperThreadTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= LooperThreadTest

include $(BUILD_EXECUTABLE)

# ---- AsyncTaskTest ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	AsyncTaskTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= AsyncTaskTest

include $(BUILD_EXECUTABLE)

# ---- DelegateTest ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	DelegateTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libstlport \
	libmindroid

include external/stlport/libstlport.mk

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= DelegateTest

include $(BUILD_EXECUTABLE)

# ---- RefCountingTest ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	RefCountingTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libstlport \
	libmindroid

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= RefCountingTest

include $(BUILD_EXECUTABLE)

# ---- StringTest ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	StringTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= StringTest

include $(BUILD_EXECUTABLE)

# ---- NetworkTest ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	NetworkTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= NetworkTest

include $(BUILD_EXECUTABLE)

# ---- ListTest ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	ListTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= ListTest

include $(BUILD_EXECUTABLE)

# ---- BundleTest ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	BundleTest.cpp

LOCAL_CFLAGS := -D__ANDROID__ -D__ARM_CPU_ARCH__

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmindroid

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= BundleTest

include $(BUILD_EXECUTABLE)

