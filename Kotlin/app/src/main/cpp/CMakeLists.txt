# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 4.1.1)

# Declares and names the project.
project("QiSpace_SDK")

add_compile_options(-std=c11 -std=c++11)

# This is the shared library for the jni wrapper. File name in apk will be libnative-lib.so
add_library( # Sets the name of the library.
             native-lib
             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
             native-lib.cpp )

find_library( # Sets the name of the path variable.
              log-lib
              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

target_link_libraries( # Specifies the target library.
                       native-lib
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

# include the header path for search header files
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../QiSpace_SDK_Lib/qeep/lib/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../QiSpace_SDK_Lib/ds/lib_ghppk/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../QiSpace_SDK_Lib/kem/lib_hppk/
)

# define the variable for the native library name
set(
        QEEP_ANDROID_A
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../QiSpace_SDK_Lib/qeep/lib/libqeep-android-clang-${CMAKE_ANDROID_ARCH_ABI}.a)

set(
        MASQ_BN_ANDROID_A
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../QiSpace_SDK_Lib/kem/lib_hppk/libmasq_bn_util_qtp-android-clang-${CMAKE_ANDROID_ARCH_ABI}.a)

set(
        MASQ_DS_ANDROID_A
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../QiSpace_SDK_Lib/ds/lib_ghppk/libmasq_ds_ghppk-android-clang-${CMAKE_ANDROID_ARCH_ABI}.a)

set(
        MASQ_KEM_ANDROID_A
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../QiSpace_SDK_Lib/kem/lib_hppk/libmasq_kem_hppk-android-clang-${CMAKE_ANDROID_ARCH_ABI}.a)


# add the shared library into the project to be used and packaged into Android package (APK or AAB)
add_library(
        qeep-android
        STATIC
        IMPORTED)
add_library(
        masq-bn-android
        STATIC
        IMPORTED)
add_library(
        masq-ds-android
        STATIC
        IMPORTED)
add_library(
        masq-kem-android
        STATIC
        IMPORTED)

# IMPORTED_LOCATION specifies the location of the library file on disk
set_target_properties(
        qeep-android
        PROPERTIES
        IMPORTED_LOCATION ${QEEP_ANDROID_A})

set_target_properties(
        masq-bn-android
        PROPERTIES
        IMPORTED_LOCATION ${MASQ_BN_ANDROID_A})

set_target_properties(
        masq-ds-android
        PROPERTIES
        IMPORTED_LOCATION ${MASQ_DS_ANDROID_A})

set_target_properties(
        masq-kem-android
        PROPERTIES
        IMPORTED_LOCATION ${MASQ_KEM_ANDROID_A})

# Add the library to this project
target_link_libraries(
        native-lib
        qeep-android
        masq-bn-android
        masq-ds-android
        masq-kem-android
        ${log-lib})
