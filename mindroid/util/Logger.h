#ifndef MINDROID_LOGGER_H_
#define MINDROID_LOGGER_H_

#include <stdint.h>
#include "mindroid/util/Utils.h"

namespace mindroid {

class Logger
{
public:
	Logger() {
	}

	int println(int bufferId, uint8_t priority, const char* tag, const char* msg);
	
private:
	static char mProrities[];

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(Logger)
};

} /* namespace mindroid */

#endif /* MINDROID_LOGGER_H_ */
