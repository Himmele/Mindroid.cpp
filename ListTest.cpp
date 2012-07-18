#include "android/util/List.h"
#include "android/lang/String.h"
#include <stdio.h>
#include <assert.h>

using namespace android::lang;
using namespace android::util;

int main() {
	String s1("abc\n\n\ndef\r\nyxz");
	List<String> strings = s1.split("\n");
	List<String>::iterator itr = strings.begin();
	while (itr != strings.end()) {
		printf("%s\n", itr->trim().c_str());
		++itr;
	}

	return 0;
}
