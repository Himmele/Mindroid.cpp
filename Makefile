CC       := gcc
CXX      := g++ 
CFLAGS   := -D__X86_CPU_ARCH__
INCLUDES := -I.
LDFLAGS  := -lpthread -lrt

SRC_FILES := \
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
	mindroid/os/AtomicInteger.cpp \
	mindroid/os/Ref.cpp \
	mindroid/os/Bundle.cpp \
	mindroid/util/Buffer.cpp \
	mindroid/lang/String.cpp \
	mindroid/net/SocketAddress.cpp \
	mindroid/net/ServerSocket.cpp \
	mindroid/net/Socket.cpp \
	mindroid/net/DatagramSocket.cpp

OBJS += $(filter %.o,$(SRC_FILES:.cpp=.o))

%.o: %.cpp
	@rm -f $@ 
	$(CXX) $(CFLAGS) $(INCLUDES) -shared -fPIC -c $< -o $@

all: libmindroid

libmindroid: $(OBJS)
	$(CXX) $(CFLAGS) $(INCLUDES) -o libmindroid.so -shared -fPIC $(OBJS) $(LDFLAGS)

Test:
	$(CXX) $(CFLAGS) $(INCLUDES) Test.cpp -L. -lmindroid $(LDFLAGS) -o Test

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f libmindroid.so	
	@rm -f Test

