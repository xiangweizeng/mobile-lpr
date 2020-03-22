
add_library(mlpr
        base.cpp
        lpc_recognizer.cpp
        lpr_recognizer.cpp
        detector/mtcnn_align.cpp
        detector/mtcnn_proposal.cpp
        detector/mtcnn_base.cpp
        detector/mtcnn_plate_detector.cpp
        detector/ssd_plate_detector.cpp
        detector/lffd.cpp
        detector/lffd_plate_detector.cpp
        detector/align_plate_detector.cpp
        plate_petector.cpp
        )

if(MLPR_INSTALL_SDK)
    install(TARGETS mlpr EXPORT mlpr ARCHIVE DESTINATION lib)
endif()

target_link_libraries(mlpr)