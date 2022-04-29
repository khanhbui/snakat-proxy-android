cmake_minimum_required(VERSION 3.18.1)

################# openssl #################

set(
        openssl_src
        ../openssl
)

set(
        openssl_prebuilt
        ../openssl/openssl/prebuilt/android/${ANDROID_ABI}
)

add_subdirectory(
        ${openssl_src}
        ${openssl_prebuilt}
)

################# civetweb #################

include_directories(
        civetweb/include
        ${openssl_src}/openssl/include/android
)

add_library(
        civetweb-lib
        STATIC
        civetweb/src/civetweb.c
        civetweb/src/CivetServer.cpp
)

target_link_libraries(
        civetweb-lib
        ssl-lib
        crypto-lib
)
