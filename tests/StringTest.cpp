#include "mindroid/lang/String.h"
#include "mindroid/lang/StringWrapper.h"
#include <stdio.h>
#include <assert.h>

using namespace mindroid;

int main() {
	StringWrapper s1("Hello");
	printf("%s\n", s1.c_str());
	s1 += " World";
	printf("%s\n", s1.c_str());
	s1 = s1 + "!";
	printf("%s\n", s1.c_str());
	s1 = "Test";
	printf("%s\n", s1.c_str());
	sp<String> tmp = s1.getString();
	printf("%s\n", tmp->c_str());

	sp<String> s2 = String::format("%d", 123);
	printf("%s\n", s2->c_str());
	s2 = s2->appendFormatted("%s%d", "456", 789);
	printf("%s\n", s2->c_str());

	sp<String> s3 = s2;
	printf("%s\n", s3->c_str());
	s2 = NULL;
	printf("%s\n", s3->c_str());

	if (s3->equals("123456789")) {
		printf("OK\n");
	} else {
		printf("Error\n");
	}

	sp<String> s4 = new String("    1234    ");
	assert(s4->size() == 12);
	assert(s4->equals("    1234    "));
	s4 = s4->trim();
	assert(s4->size() == 4);
	assert(s4->equals("1234"));

	s4 = new String("\r\n");
	assert(s4->size() == 2);
	assert(s4->equals("\r\n"));
	s4 = s4->trim();
	assert(s4->size() == 0);
	assert(s4->equals(""));
	assert(s4 == String::EMPTY_STRING);

	sp<String> s5 = new String();
	assert(s5->isEmpty());

	sp<String> s6 = new String("Test");
	assert(s6->startsWith("Te"));
	assert(!s6->startsWith("abc"));
	assert(s6->substr(1)->equals("est"));
	assert(s6->substr(1, 3)->equals("es"));
	assert(s6->indexOf("es") == 1);

	sp<String> s7 = new String("Test");
	assert(s7->endsWith("st"));

	assert(s7->left(2)->equals("Te"));
	assert(s7->right(2)->equals("st"));
	assert(s7->right(20)->equals("Test"));

	sp<String> s8 = new String("abc\n\n\ndef\r\nyxz");
	sp< List< sp<String> > > strings = s8->split("\n");
	List< sp<String> >::iterator itr = strings->begin();
	while (itr != strings->end()) {
		printf("%s\n", (*itr)->trim()->c_str());
		++itr;
	}

	return 0;
}
