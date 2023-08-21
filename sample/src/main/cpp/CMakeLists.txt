cmake_minimum_required(VERSION 3.18.1)
project(sample)

if(DEPENDENCY_ON_LOCAL_LIBRARY)
    set(LANTERN_BASE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../lantern)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(LANTERN_LIB_BASE ${LANTERN_BASE}/build/intermediates/merged_native_libs/debug)
    else()
        set(LANTERN_LIB_BASE ${LANTERN_BASE}/build/intermediates/stripped_native_libs/release)
    endif()

    add_library(lantern::lantern SHARED IMPORTED)
    set_target_properties(lantern::lantern PROPERTIES
            IMPORTED_LOCATION ${LANTERN_LIB_BASE}/out/lib/${ANDROID_ABI}/liblantern.so
            INTERFACE_INCLUDE_DIRECTORIES ${LANTERN_BASE}/src/main/cpp/include)
else()
    find_package(lantern REQUIRED CONFIG)
endif()

# liblantern_jni.so
add_library(lantern_jni SHARED lantern_jni.cpp)
target_compile_features(lantern_jni PUBLIC cxx_std_11)
target_link_libraries(lantern_jni log)
target_link_libraries(lantern_jni lantern::lantern)
