project(mindroid)

cmake_minimum_required(VERSION 2.8)

file(GLOB TARGET_OS_SRC
    mindroid/os/*.cpp
)

file(GLOB TARGET_UTIL_SRC
    mindroid/util/*.cpp
)

file(GLOB TARGET_LANG_SRC
    mindroid/lang/*.cpp
)

file(GLOB TARGET_NET_SRC
    mindroid/net/*.cpp
)

include_directories( .)
add_library(mindroid SHARED ${TARGET_OS_SRC} ${TARGET_UTIL_SRC} ${TARGET_LANG_SRC} ${TARGET_NET_SRC})

#----------------------------------------------------------------------------
# Install instructions.
#----------------------------------------------------------------------------
install(
  TARGETS mindroid
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)

file(GLOB TARGET_OS_HDR
    mindroid/os/*.h
)

file(GLOB TARGET_UTIL_HDR
    mindroid/util/*.h
)

file(GLOB TARGET_LANG_HDR
    mindroid/lang/*.h
)

file(GLOB TARGET_NET_HDR
    mindroid/net/*.h
)

install(FILES ${TARGET_OS_HDR} DESTINATION include/mindroid/os)
install(FILES ${TARGET_UTIL_HDR} DESTINATION include/mindroid/util)
install(FILES ${TARGET_LANG_HDR} DESTINATION include/mindroid/lang)
install(FILES ${TARGET_NET_HDR} DESTINATION include/mindroid/net)
