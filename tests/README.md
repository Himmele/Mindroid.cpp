## Build test
* Linux/MacOS/Windows
`cmake  .`

* Android
cmake -DCMAKE_TOOLCHAIN_FILE=../android-cmake/android.toolchain.cmake -DANDROID_NDK=$ANDROID_NDK_HOME -DANDROID_ABI="armeabi-v7a with NEON" .
