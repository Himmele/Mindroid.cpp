# GCC, the GNU Compiler Collection
CC := gcc
CXX := g++
LD := g++
AS := gas
AR := ar
RM := rm
INCLUDES := -I.
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

TINYXML2_SRCS := $(wildcard tinyxml2/*.cpp)
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

SERVICES_SRCS := samples/Services.cpp
SERVICES_OBJS = $(SERVICES_SRCS:.cpp=.o)
SERVICES_BIN_OBJS = $(addprefix $(BIN_DIR)/,$(SERVICES_OBJS))

Services = $(BIN_DIR)/Services

Services: $(Services) Mindroid.cpp tinyxml2

$(Services): $(SERVICES_BIN_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ -Llib -lmindroid -ltinyxml2 -lpthread -lrt
	
$(BIN_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $<
