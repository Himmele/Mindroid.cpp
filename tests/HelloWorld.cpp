#include <stdio.h>
#include "mindroid/os/Message.h"
#include "mindroid/os/LooperThread.h"

using namespace mindroid;

static const int SAY_HELLO = 0;
static const int SAY_WORLD = 1;

class HelloHandler : public Handler {
public:
	HelloHandler(const sp<Handler>& worldHandler) :
			mWorldHandler(worldHandler) {
	}

	virtual void handleMessage(const sp<Message>& msg) {
		switch (msg->what) {
		case SAY_HELLO:
			printf("Hello ");
			sp<Message> message = mWorldHandler->obtainMessage(SAY_WORLD);
			message->metaData()->putObject("Handler", this);
			message->sendToTarget();
			break;
		}
	}

private:
	sp<Handler> mWorldHandler;
};

class WorldHandler : public Handler {
public:
	WorldHandler() {
	}

	virtual void handleMessage(const sp<Message>& msg) {
		switch (msg->what) {
		case SAY_WORLD:
			printf("World!\n");
			sp<Handler> helloHandler = msg->metaData()->getObject<Handler>("Handler");
			sp<Message> message = helloHandler->obtainMessage(SAY_HELLO);
			helloHandler->sendMessageDelayed(message, 1000);
			break;
		}
	}
};

int main() {
	Looper::prepare();
	sp<Handler> worldHandler = new WorldHandler();
	sp<Handler> helloHandler = new HelloHandler(worldHandler);
	helloHandler->obtainMessage(SAY_HELLO)->sendToTarget();
	Looper::loop();

	return 0;
}
