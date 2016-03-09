#include <gtest/gtest.h>
#include "mindroid/util/ArrayList.h"
#include "mindroid/lang/String.h"

using namespace mindroid;

TEST(Mindroid, ArrayListOfInteger) {
	sp<ArrayList<int>> list = new ArrayList<int>();
	list->add(1);
	list->add(2);
	list->add(17);
	list->add(42);

	ASSERT_EQ(list->get(0), 1);
	ASSERT_EQ(list->get(1), 2);
	ASSERT_EQ(list->get(2), 17);
	ASSERT_EQ(list->get(3), 42);

	ASSERT_EQ(list->set(0, 2), 1);
	ASSERT_EQ(list->set(1, 3), 2);

	ASSERT_EQ(list->size(), 4);

	size_t i = 0;
	auto itr = list->iterator();
	while (itr.hasNext()) {
		switch (i) {
		case 0:
			ASSERT_EQ(itr.next(), 2);
			break;
		case 1:
			ASSERT_EQ(itr.next(), 3);
			break;
		case 2:
			ASSERT_EQ(itr.next(), 17);
			break;
		case 3:
			ASSERT_EQ(itr.next(), 42);
			break;
		}
		i++;
		itr.remove();
	}
	ASSERT_EQ(list->size(), 0);
}

TEST(Mindroid, ArrayListOfString) {
	sp<ArrayList<sp<String>>> list = new ArrayList<sp<String>>();
	list->add(new String("ABC"));
	list->add(new String("DEF"));
	list->add(new String("XYZ"));

	ASSERT_STREQ(list->get(0)->c_str(), "ABC");
	ASSERT_STREQ(list->get(1)->c_str(), "DEF");
	ASSERT_STREQ(list->get(2)->c_str(), "XYZ");

	ASSERT_STREQ(list->set(0, String::valueOf("Test"))->c_str(), "ABC");
	ASSERT_STREQ(list->set(1, String::valueOf("abc"))->c_str(), "DEF");

	ASSERT_EQ(list->size(), 3);

	ASSERT_STREQ(list->set(0, String::valueOf("ABC"))->c_str(), "Test");
	ASSERT_STREQ(list->set(1, String::valueOf("DEF"))->c_str(), "abc");

	ASSERT_EQ(list->size(), 3);

	size_t i = 0;
	auto itr = list->iterator();
	while (itr.hasNext()) {
		switch (i) {
		case 0:
			ASSERT_STREQ(itr.next()->c_str(), "ABC");
			break;
		case 1:
			ASSERT_STREQ(itr.next()->c_str(), "DEF");
			break;
		case 2:
			ASSERT_STREQ(itr.next()->c_str(), "XYZ");
			break;
		}
		i++;
	}
	ASSERT_EQ(list->size(), 3);

	list->remove(new String("XYZ"));
	ASSERT_EQ(list->size(), 2);
	itr = list->iterator();
	while (itr.hasNext()) {
		itr.next();
		itr.remove();
	}
	ASSERT_EQ(list->size(), 0);

	list->add(0, new String("abc"));
	list->add(0, new String("def"));
	list->add(0, new String("xyz"));
	ASSERT_EQ(list->size(), 3);
	ASSERT_EQ(list->indexOf(new String("def")), 1);

	sp<String> string = list->remove(1);
	ASSERT_STREQ(string->c_str(), "def");

	ASSERT_STREQ(list->get(0)->c_str(), "xyz");
	ASSERT_STREQ(list->get(1)->c_str(), "abc");
	ASSERT_EQ(list->size(), 2);

	list->set(0, new String("Test"));
	ASSERT_STREQ(list->get(0)->c_str(), "Test");
	ASSERT_STREQ(list->get(1)->c_str(), "abc");
	ASSERT_EQ(list->size(), 2);
}
