cmake_minimum_required(VERSION 3.0)
project(mobile-lpr CXX)

option(MLPR_INSTALL_SDK "install mobile-opr library and headers" ON)
option(MLPR_EXAMPLE "build examples for mobile-pr" ON)

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory")
endif()
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "Choose the type of build" FORCE)
endif()

include_directories(deps/ncnn/include/ncnn include src)
link_directories(deps/ncnn/lib)

add_subdirectory(src)

if(MLPR_EXAMPLE)
    add_subdirectory(example)
endif()

if(MLPR_INSTALL_SDK)
    install(FILES
            include/plate_info.h
            include/plate_petector.h
            include/plate_detectors.h
            include/detector_creator.h
            include/lpc_recognizer.h
            include/lpc_recognizer.h
            DESTINATION include/mpr
            )
endif()



