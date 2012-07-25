#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "mindroid/os/Looper.h"
#include "mindroid/os/Handler.h"
#include "mindroid/os/Thread.h"
#include "mindroid/os/Message.h"
#include "mindroid/os/Closure.h"
#include "mindroid/os/LooperThread.h"

using namespace mindroid;

class ExampleHandler : public Handler
{
public:
	virtual void handleMessage(const sp<Message>& message) {
		printf("ExampleHandler::handleMessage with ID %d by Looper %p\n",
				message->what, Looper::myLooper());
	}
};

class Test
{
public:
	void test(int32_t value) {
		printf("Test::test() called with value %d\n", value);
	}
};

static sp< LooperThread<ExampleHandler> > sLooperThread = new LooperThread<ExampleHandler>();
static Test sTest;

int main() {
	sLooperThread->start();
	sp<Handler> handler = sLooperThread->getHandler();

	handler->obtainMessage(1234)->sendToTarget();
	handler->postDelayed(newRunnable(sTest, &Test::test, 42), 1000);

	Thread::sleep(2000);

	sLooperThread->getLooper()->quit();
	sLooperThread->join();

	return 0;
}
