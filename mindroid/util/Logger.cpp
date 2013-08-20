#include "mindroid/util/Logger.h"
#include <stdio.h>

namespace mindroid {

char Logger::mProrities[] = { 'V', 'D', 'I', 'W', 'E', 'A' };

int Logger::println(int bufferId, uint8_t priority, const char* tag, const char* msg) {
	printf("%c/%s: %s\r\n", mProrities[priority], tag, msg);
	return 0;
}

} /* namespace mindroid */
