
find_package(OpenCV 3.0.0 REQUIRED COMPONENTS core imgcodecs imgproc highgui)

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(../deps/cv-uni-text/install)
link_directories(../deps/cv-uni-text/install)
add_executable(run_plate main.cpp)
target_link_libraries(run_plate mlpr cvunitext freetype ncnn  ${OpenCV_LIBS})
