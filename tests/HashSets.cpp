#include <gtest/gtest.h>
#include "mindroid/util/HashSet.h"
#include "mindroid/lang/String.h"

using namespace mindroid;

TEST(Mindroid, SetOfInteger) {
    sp<HashSet<int>> set = new HashSet<int>();
    set->add(1);
    ASSERT_EQ(set->contains(1), true);
    ASSERT_EQ(set->contains(2), false);
    set->add(2);
    ASSERT_EQ(set->contains(1), true);
    ASSERT_EQ(set->contains(2), true);
    set->add(3);
    ASSERT_EQ(set->contains(1), true);
    ASSERT_EQ(set->contains(2), true);
    ASSERT_EQ(set->contains(3), true);
    set->remove(3);
    ASSERT_EQ(set->contains(1), true);
    ASSERT_EQ(set->contains(2), true);
    ASSERT_EQ(set->contains(3), false);
    auto itr = set->iterator();
    while (itr.hasNext()) {
        itr.next();
        itr.remove();
    }
    ASSERT_EQ(set->size(), 0);
}

TEST(Mindroid, SetOfString) {
    sp<HashSet<sp<String>>> set = new HashSet<sp<String>>();
    set->add(String::valueOf("1"));
    ASSERT_EQ(set->contains(String::valueOf("1")), true);
    ASSERT_EQ(set->contains(String::valueOf("2")), false);
    set->add(String::valueOf("2"));
    ASSERT_EQ(set->contains(String::valueOf("1")), true);
    ASSERT_EQ(set->contains(String::valueOf("2")), true);
    set->add(String::valueOf("3"));
    ASSERT_EQ(set->contains(String::valueOf("1")), true);
    ASSERT_EQ(set->contains(String::valueOf("2")), true);
    ASSERT_EQ(set->contains(String::valueOf("3")), true);
    set->remove(String::valueOf("3"));
    ASSERT_EQ(set->contains(String::valueOf("1")), true);
    ASSERT_EQ(set->contains(String::valueOf("2")), true);
    ASSERT_EQ(set->contains(String::valueOf("3")), false);
    auto itr = set->iterator();
    while (itr.hasNext()) {
        itr.next();
        itr.remove();
    }
    ASSERT_EQ(set->size(), 0);
}
