cmake_minimum_required(VERSION 2.8)
project(tests)

set(ROOT_PATH ${PROJECT_SOURCE_DIR}/..)
set(MINDROID_LIBS ${ROOT_PATH}/prebuilt/lib/libmindroid.so)
set(MINDROID_INCLUDE_DIRS ${ROOT_PATH}/prebuilt/include)
include_directories( ${MINDROID_INCLUDE_DIRS})

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Helloworld
add_executable( HelloWorld HelloWorld.cpp)
target_link_libraries( HelloWorld ${MINDROID_LIBS} Threads::Threads)

# Bundletest
add_executable(BundleTest BundleTest.cpp)
target_link_libraries(BundleTest ${MINDROID_LIBS} Threads::Threads)

# Networktest
add_executable(NetworkTest NetworkTest.cpp)
target_link_libraries(NetworkTest ${MINDROID_LIBS} Threads::Threads)

# ListTest
add_executable(ListTest ListTest.cpp)
target_link_libraries(ListTest ${MINDROID_LIBS} Threads::Threads)

# AsyncTaskTest
add_executable(AsyncTaskTest AsyncTaskTest.cpp)
target_link_libraries(AsyncTaskTest ${MINDROID_LIBS} Threads::Threads)

# RefCountingTest
add_executable(RefCountingTest RefCountingTest.cpp)
target_link_libraries(RefCountingTest ${MINDROID_LIBS} Threads::Threads)

# StringTest
add_executable(StringTest StringTest.cpp)
target_link_libraries(StringTest ${MINDROID_LIBS} Threads::Threads)

# ConcurrencyTest
add_executable(ConcurrencyTest ConcurrencyTest.cpp)
target_link_libraries(ConcurrencyTest ${MINDROID_LIBS} Threads::Threads)

# ConcurrencyTest
add_executable(LooperThreadTest LooperThreadTest.cpp)
target_link_libraries(LooperThreadTest ${MINDROID_LIBS} Threads::Threads)
