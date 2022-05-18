cmake_minimum_required(VERSION 3.18.1)

################# civetweb #################

set(
        civetweb_src
        ../libs/civetweb
)

set(
        civetweb_prebuilt
        ../libs/civetweb/prebuilt
)

add_subdirectory(
        ${civetweb_src}
        ${civetweb_prebuilt}
)

################# rapidjson #################

set(
        rapidjson_src
        ../libs/rapidjson
)

set(
        rapidjson_prebuilt
        ../libs/rapidjson/prebuilt
)

add_subdirectory(
        ${rapidjson_src}
        ${rapidjson_prebuilt}
)

################# snakat #################

include_directories(
        ${civetweb_src}
        ${rapidjson_src}/rapidjson/include
        ${openssl_src}/openssl/include/android
)

add_library(
        snakat-lib
        STATIC
        snakat/core/AutoreleaseObject.cpp
        snakat/handler/ApiHandler.cpp
        snakat/handler/FileHandler.cpp
        snakat/handler/HttpHandler.cpp
        snakat/Proxy.cpp
        snakat/util/FileUtil.cpp
        snakat/util/JniUtil.cpp
        snakat/util/Util.cpp
        snakat/util/Util-android.cpp
)

target_link_libraries(
        snakat-lib
        civetweb-lib
)

