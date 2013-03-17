#include <stdio.h>
#include "mindroid/os/Message.h"
#include "mindroid/os/LooperThread.h"

using namespace mindroid;

static const int SAY_HELLO = 0;
static const int SAY_WORLD = 1;
static const int QUIT = 2;

class HelloHandler : public Handler {
public:
	HelloHandler(const sp<Handler>& worldHandler) :
			mWorldHandler(worldHandler), mCounter(0) {
	}

	virtual void handleMessage(const sp<Message>& msg) {
		switch (msg->what) {
		case SAY_HELLO:
			mCounter++;
			if (mCounter <= 1000) {
				printf("Hello ");
				sp<Message> sayWorld = mWorldHandler->obtainMessage(SAY_WORLD);
				sayWorld->metaData()->putObject("SayHello", obtainMessage(SAY_HELLO));
				sayWorld->sendToTarget();
			} else {
				mWorldHandler->obtainMessage(QUIT)->sendToTarget();
				Looper::myLooper()->quit();
			}
			break;
		}
	}

private:
	sp<Handler> mWorldHandler;
	int mCounter;
};

class WorldHandler : public Handler {
public:
	virtual void handleMessage(const sp<Message>& msg) {
		switch (msg->what) {
		case SAY_WORLD:
			printf("World!\n");
			msg->metaData()->getObject<Message>("SayHello")->sendToTarget();
			break;
		case QUIT:
			Looper::myLooper()->quit();
			break;
		}
	}
};

int main() {
	sp< LooperThread<WorldHandler> > wlt = new LooperThread<WorldHandler>();
	wlt->start();

	Looper::prepare();
	sp<Handler> hh = new HelloHandler(wlt->getHandler());
	hh->obtainMessage(SAY_HELLO)->sendToTarget();
	Looper::loop();

	return 0;
}
