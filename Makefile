# GCC, the GNU Compiler Collection
# RELRO: http://tk-blog.blogspot.de/2009/02/relro-not-so-well-known-memory.html
CC := gcc
CXX := g++
LD := g++
AS := gas
AR := ar
RM := rm
INCLUDES := -I. -Isrc
CFLAGS := -c -g -O0 -fPIC -std=c++11 -fexceptions -fstack-protector -Wa,--noexecstack -Werror=format-security -D_FORTIFY_SOURCE=2
LDFLAGS := -pie -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now
OUT_DIR := out

#==== Misc ====

.PHONY: clean

all: tinyxml2 Mindroid.cpp googletest Tests Main

clean:
	$(RM) -rf $(OUT_DIR)

#==== Mindroid.cpp ====

SRCS = \
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
	src/mindroid/runtime/system/ServiceDiscovery.cpp \
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

OBJS = $(SRCS:.cpp=.o)
LIB_OBJS = $(addprefix $(OUT_DIR)/,$(OBJS))

Mindroid.cpp = $(OUT_DIR)/libmindroid.a

Mindroid.cpp: $(Mindroid.cpp)

$(Mindroid.cpp): $(LIB_OBJS) 
	$(AR) -r $@ $^

$(LIB_OBJS): $(OUT_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

#==== TinyXML2 ====

TINYXML2_SRCS := $(wildcard src/tinyxml2/*.cpp)
TINYXML2_OBJS = $(TINYXML2_SRCS:.cpp=.o)
TINYXML2_LIB_OBJS = $(addprefix $(OUT_DIR)/,$(TINYXML2_OBJS))

tinyxml2 = $(OUT_DIR)/libtinyxml2.a

tinyxml2: $(tinyxml2)
 
$(tinyxml2): $(TINYXML2_LIB_OBJS) 
	$(AR) -r $@ $^

$(TINYXML2_LIB_OBJS): $(OUT_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

#==== Google Test ====

GOOGLETEST_SRCS := $(wildcard googletest/src/gtest-all.cc)
GOOGLETEST_OBJS = $(GOOGLETEST_SRCS:.cc=.o)
GOOGLETEST_LIB_OBJS = $(addprefix $(OUT_DIR)/,$(GOOGLETEST_OBJS))

googletest = $(OUT_DIR)/libgoogletest.a

googletest: $(googletest)
 
$(googletest): $(GOOGLETEST_LIB_OBJS) 
	$(AR) -r $@ $^

$(GOOGLETEST_LIB_OBJS): $(OUT_DIR)/%.o : %.cc
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) -Igoogletest -Igoogletest/include -c $< -o $@

#==== Tests ====

TEST_SRCS := $(wildcard tests/mindroid/*/*.cpp)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
TEST_BIN_OBJS = $(addprefix $(OUT_DIR)/,$(TEST_OBJS))

Tests = $(OUT_DIR)/Tests

Tests: $(Tests) Mindroid.cpp tinyxml2 googletest

$(Tests): $(TEST_BIN_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ -Lout -lmindroid -ltinyxml2 -lgoogletest -lpthread -lrt
	
$(TEST_BIN_OBJS): $(OUT_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) -Itests -Igoogletest/include -c -o $@ $<

#==== Main ====

MAIN_INCLUDES := -Iexamples/Concurrency/src -Iexamples/Eliza/src -Iexamples/Eliza/gen -Iexamples/Services/src

MAIN_SRCS := src/main/Main.cpp \
	examples/Concurrency/src/AsyncTaskExample.cpp \
	examples/Concurrency/src/HandlerExample.cpp \
	examples/Concurrency/src/PromiseExample.cpp \
	examples/Eliza/gen/examples/eliza/IEliza.cpp \
	examples/Eliza/gen/examples/eliza/IElizaListener.cpp \
	examples/Eliza/src/Eliza.cpp \
	examples/Eliza/src/ElizaService.cpp \
	examples/Eliza/src/You.cpp \
	examples/Eliza/src/util/Eliza.cpp \
	examples/Services/src/ServiceExample1.cpp
MAIN_OBJS = $(MAIN_SRCS:.cpp=.o)
MAIN_BIN_OBJS = $(addprefix $(OUT_DIR)/,$(MAIN_OBJS))

Main = $(OUT_DIR)/Main

Main: $(Main) Mindroid.cpp tinyxml2

$(Main): $(MAIN_BIN_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ -Lout -lmindroid -ltinyxml2 -lpthread -lrt
	
$(MAIN_BIN_OBJS): $(OUT_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) $(MAIN_INCLUDES) -c -o $@ $<
