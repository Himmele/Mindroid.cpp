#include <gtest/gtest.h>
#include <mindroid/app/SharedPreferencesImpl.h>
#include <mindroid/io/File.h>
#include <mindroid/lang/String.h>
#include <mindroid/os/Environment.h>
#include <mindroid/util/ArrayList.h>

using namespace mindroid;

void removeFile(const sp<File>& file) {
    if (file->isDirectory()) {
        sp<ArrayList<sp<File>>> files = file->listFiles();
        if (files != nullptr) {
            auto itr = files->iterator();
            while (itr.hasNext()) {
                sp<File> f = itr.next();
                removeFile(f);
            }
        }
    }
    file->remove();
}

TEST(Mindroid, SharedPreferences) {
    sp<File> tmpDirectory = new File("tmp");
    tmpDirectory->mkdirs();
    Environment::setRootDirectory(tmpDirectory->getAbsolutePath());
    Environment::getPreferencesDirectory()->mkdirs();

    sp<SharedPreferences> sharedPreferences = new SharedPreferencesImpl(new File(Environment::getPreferencesDirectory(), "Test.xml"), 0);
    sp<SharedPreferences::Editor> editor = sharedPreferences->edit();
    editor->clear()->commit();

    ASSERT_EQ(sharedPreferences->getBoolean("Bool", false), false);
    ASSERT_STREQ(sharedPreferences->getString("String", "abc")->c_str(), "abc");
    ASSERT_EQ(sharedPreferences->getStringSet("Set", nullptr), nullptr);

    sp<HashSet<sp<String>>> set = new HashSet<sp<String>>();
    set->add(String::valueOf("1"));
    set->add(String::valueOf("2"));
    set->add(String::valueOf("3"));

    sharedPreferences->edit()
            ->putBoolean("Bool", true)
            ->putString("String", "xyz")
            ->putStringSet("Set", set)
            ->commit();

    ASSERT_EQ(sharedPreferences->getBoolean("Bool", false), true);
    ASSERT_STREQ(sharedPreferences->getString("String", "abc")->c_str(), "xyz");
    sp<HashSet<sp<String>>> s = sharedPreferences->getStringSet("Set", nullptr);
    ASSERT_EQ(set->contains(String::valueOf("1")), true);
    ASSERT_EQ(set->contains(String::valueOf("2")), true);
    ASSERT_EQ(set->contains(String::valueOf("3")), true);

    sharedPreferences->edit()
            ->remove("String")
            ->putStringSet("Set", nullptr)
            ->commit();

    ASSERT_EQ(sharedPreferences->getString("String", nullptr), nullptr);
    ASSERT_EQ(sharedPreferences->getString("Set", nullptr), nullptr);

    removeFile(tmpDirectory);
}
