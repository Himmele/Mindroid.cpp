# GCC, the GNU Compiler Collection
CC := gcc
CXX := g++
LD := g++
AS := gas
AR := ar
RM := rm
INCLUDES := -I. -Isrc
CFLAGS := -c -g -O0 -fPIC -std=c++11 -fexceptions
LDFLAGS := 
LIB_DIR := lib
BIN_DIR := bin

#==== Misc ====
	
.PHONY: clean

all: tinyxml2 Mindroid.cpp Tests Services

clean:
	$(RM) -rf $(LIB_DIR)
	$(RM) -rf $(BIN_DIR)

#==== Mindroid.cpp ====

SRCS = \
	src/mindroid/app/SharedPreferencesImpl.cpp \
	src/mindroid/app/IOnSharedPreferenceChangeListener.cpp \
	src/mindroid/app/Service.cpp \
	src/mindroid/app/ContextImpl.cpp \
	src/mindroid/content/Context.cpp \
	src/mindroid/content/ComponentName.cpp \
	src/mindroid/content/ContextWrapper.cpp \
	src/mindroid/content/pm/IPackageManager.cpp \
	src/mindroid/content/pm/PackageManagerService.cpp \
	src/mindroid/content/Intent.cpp \
	src/mindroid/io/File.cpp \
	src/mindroid/lang/Object.cpp \
	src/mindroid/lang/Thread.cpp \
	src/mindroid/lang/String.cpp \
	src/mindroid/lang/Closure.cpp \
	src/mindroid/lang/Class.cpp \
	src/mindroid/net/DatagramSocket.cpp \
	src/mindroid/net/SocketAddress.cpp \
	src/mindroid/net/Socket.cpp \
	src/mindroid/net/ServerSocket.cpp \
	src/mindroid/os/Environment.cpp \
	src/mindroid/os/Handler.cpp \
	src/mindroid/os/Message.cpp \
	src/mindroid/os/Binder.cpp \
	src/mindroid/os/IRemoteCallback.cpp \
	src/mindroid/os/Process.cpp \
	src/mindroid/os/Bundle.cpp \
	src/mindroid/os/HandlerThread.cpp \
	src/mindroid/os/IProcess.cpp \
	src/mindroid/os/IServiceManager.cpp \
	src/mindroid/os/ServiceManager.cpp \
	src/mindroid/os/Looper.cpp \
	src/mindroid/os/AsyncTask.cpp \
	src/mindroid/os/SystemClock.cpp \
	src/mindroid/os/MessageQueue.cpp \
	src/mindroid/util/Assert.cpp \
	src/mindroid/util/Variant.cpp \
	src/mindroid/util/concurrent/SerialExecutor.cpp \
	src/mindroid/util/concurrent/locks/ReentrantLock.cpp \
	src/mindroid/util/concurrent/locks/ConditionImpl.cpp \
	src/mindroid/util/concurrent/ThreadPoolExecutor.cpp \
	src/mindroid/util/concurrent/Semaphore.cpp \
	src/mindroid/util/Log.cpp \
	src/mindroid/util/Logger.cpp

OBJS = $(SRCS:.cpp=.o)
LIB_OBJS = $(addprefix $(LIB_DIR)/,$(OBJS))

Mindroid.cpp = $(LIB_DIR)/libmindroid.a

Mindroid.cpp: $(Mindroid.cpp)

$(Mindroid.cpp): $(LIB_OBJS) 
	$(AR) -r $@ $^

$(LIB_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

#==== TinyXML2 ====

TINYXML2_SRCS := $(wildcard src/tinyxml2/*.cpp)
TINYXML2_OBJS = $(TINYXML2_SRCS:.cpp=.o)
TINYXML2_LIB_OBJS = $(addprefix $(LIB_DIR)/,$(TINYXML2_OBJS))

tinyxml2 = $(LIB_DIR)/libtinyxml2.a

tinyxml2: $(tinyxml2)
 
$(tinyxml2): $(TINYXML2_LIB_OBJS) 
	$(AR) -r $@ $^

$(LIB_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

#==== Tests ====

TEST_SRCS := $(wildcard tests/*.cpp)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
TEST_BIN_OBJS = $(addprefix $(BIN_DIR)/,$(TEST_OBJS))

Tests = $(BIN_DIR)/Tests

Tests: $(Tests)

$(Tests): $(TEST_BIN_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ -Llib -Lgtest -lmindroid -ltinyxml2 -lgtest -lpthread -lrt
	
$(BIN_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $<

#==== Services ====

SERVICES_SRCS := src/examples/Services.cpp
SERVICES_OBJS = $(SERVICES_SRCS:.cpp=.o)
SERVICES_BIN_OBJS = $(addprefix $(BIN_DIR)/,$(SERVICES_OBJS))

Services = $(BIN_DIR)/Services

Services: $(Services) Mindroid.cpp tinyxml2

$(Services): $(SERVICES_BIN_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ -Llib -lmindroid -ltinyxml2 -lpthread -lrt
	
$(BIN_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $<
