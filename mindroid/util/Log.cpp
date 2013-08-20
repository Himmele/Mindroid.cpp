#include "mindroid/util/Log.h"

namespace mindroid {

Logger Log::sLogger;

int Log::v(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, VERBOSE, tag, msg);
}

int Log::d(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, DEBUG, tag, msg);
}

int Log::i(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, INFO, tag, msg);
}

int Log::w(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, WARN, tag, msg);
}

int Log::e(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, ERROR, tag, msg);
}

int Log::wtf(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, WTF, tag, msg);
}

} /* namespace mindroid */
