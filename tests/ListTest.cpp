#include "mindroid/util/List.h"
#include "mindroid/lang/String.h"
#include <stdio.h>
#include <assert.h>

using namespace mindroid;

int main() {
	sp<String> s1 = new String("abc\n\n\ndef\r\nyxz");
	sp< List< sp<String> > > strings = s1->split("\n");
	List< sp<String> >::iterator itr = strings->begin();
	while (itr != strings->end()) {
		printf("%s\n", (*itr)->trim()->c_str());
		++itr;
	}

	return 0;
}
