# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.
project("QSCqeeplib")

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
# include the header path for search header files (here it is for 'qsc.h')
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../QiSpace_SDK_Lib/prebuilt/)

# define the varaible for the native shared library name
set(
        QEEP_ANDROID_SO
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../QiSpace_SDK_Lib/prebuilt/libqeep-android-clang-${CMAKE_ANDROID_ARCH_ABI}.so)

# add the shared library into the project to be used and packaged into Android package (APK or AAB)
add_library(
        qeep-android # it is for libqeep-android.so
        SHARED
        IMPORTED)

# IMPORTED_LOCATION specifies the location of the library file on disk
set_target_properties(
        qeep-android
        PROPERTIES
        IMPORTED_LOCATION ${QEEP_ANDROID_SO})

# Add the library to this project
target_link_libraries(
        native-lib
        qeep-android)
