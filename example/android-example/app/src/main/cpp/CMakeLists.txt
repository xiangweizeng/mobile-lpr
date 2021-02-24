# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
if(DEFINED ANDROID_NDK_MAJOR AND ${ANDROID_NDK_MAJOR} GREATER 20)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-openmp")
endif()

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library(log-lib log)
find_library(android-lib android)
find_library(vulkan-lib vulkan)
find_library(jnigraphics-lib jnigraphics)
add_library( ncnn STATIC IMPORTED )
set_target_properties( # Specifies the target library.
        ncnn
        # Specifies the parameter you want to define.
        PROPERTIES IMPORTED_LOCATION
        # Provides the path to the library you want to import.
        ${CMAKE_SOURCE_DIR}/ncnnvulkan/${ANDROID_ABI}/libncnn.a )

# ncnnvulkan
add_library(glslang STATIC IMPORTED)
add_library(OGLCompiler STATIC IMPORTED)
add_library(OSDependent STATIC IMPORTED)
add_library(SPIRV STATIC IMPORTED)
set_target_properties(glslang PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ncnnvulkan/${ANDROID_ABI}/libglslang.a)
set_target_properties(OGLCompiler PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ncnnvulkan/${ANDROID_ABI}/libOGLCompiler.a)
set_target_properties(OSDependent PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ncnnvulkan/${ANDROID_ABI}/libOSDependent.a)
set_target_properties(SPIRV PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ncnnvulkan/${ANDROID_ABI}/libSPIRV.a)

include_directories(ncnnvulkan/include ocr)
include_directories(lpr lpr/include)

aux_source_directory(. SRC_LIST)
set(MLPR_SRCS
        lpr/base.cpp
        lpr/lpc_recognizer.cpp
        lpr/lpr_recognizer.cpp
        lpr/detector/mtcnn_align.cpp
        lpr/detector/mtcnn_proposal.cpp
        lpr/detector/mtcnn_base.cpp
        lpr/detector/mtcnn_plate_detector.cpp
        lpr/detector/ssd_plate_detector.cpp
        lpr/detector/lffd.cpp
        lpr/detector/lffd_plate_detector.cpp
        lpr/detector/align_plate_detector.cpp
        lpr/plate_detector.cpp
        lpr/LPRExport.cpp
        )

add_library(
        example
        SHARED
        ${SRC_LIST}
        ${MLPR_SRCS}
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
target_link_libraries( # Specifies the target library.
        example
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${vulkan-lib}
        ${android-lib}
        ${jnigraphics-lib}
        ncnn glslang SPIRV OGLCompiler OSDependent)