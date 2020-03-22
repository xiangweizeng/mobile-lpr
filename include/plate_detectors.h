/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef PLATE_PLATE_DETECTORS_H
#define PLATE_PLATE_DETECTORS_H

#include "detector_creator.h"

namespace pr{
    static AlignPlateDetectorConfig float_align = {
            "det3.param",
            "det3.bin",
            120,
            48,
            0.5,
            0.9,
    };

    static AlignPlateDetectorConfig int8_align = {
            "det3-int8.param",
            "det3-int8.bin",
            120,
            48,
            0.5,
            0.9,
    };

    static AlignPlateDetectorConfig opt_align = {
            "det3-opt.param",
            "det3-opt.bin",
            120,
            48,
            0.5,
            0.9,
    };

    static void fix_align_detector(std::string path, AlignPlateDetectorConfig &config)
    {
        config.align_param = path + "/" + config.align_param;
        config.align_bin = path + "/" + config.align_bin;
    }

    static MtcnnPlateDetectorConfig mtcnn_float_detector = {
            "det1.param",
            "det1.bin",
            "det2.param",
            "det2.bin",
            0.5,
            0.7,
            0.8,
            float_align,
    };

    static MtcnnPlateDetectorConfig mtcnn_int8_detector = {
            "det1-int8.param",
            "det1-int8.bin",
            "det2-int8.param",
            "det2-int8.bin",
            0.5,
            0.6,
            0.7,
            int8_align,
    };

    static void fix_mtcnn_detector(std::string path, MtcnnPlateDetectorConfig &config){
        config.filter_param = path + "/" + config.filter_param;
        config.filter_bin = path + "/" + config.filter_bin;
        config.proposal_param = path + "/" + config.proposal_param;
        config.proposal_bin = path + "/" + config.proposal_bin;
        fix_align_detector(path, config.align);
    }

    static SSDPlateDetectorConfig ssd_float_detector = {
            "mssd512_voc.param",
            "mssd512_voc.bin",
            0.05,
            512,
            512,
            float_align
    };

    static void fix_ssd_detector(std::string path, SSDPlateDetectorConfig &config){
        config.param = path + "/" + config.param;
        config.bin = path + "/" + config.bin;
        fix_align_detector(path, config.align);
    }

    static LFFDPlateDetectorConfig lffd_float_detector = {
            "lffd.param",
            "lffd.bin",
            640,
            640,
            float_align
    };

    static void fix_lffd_detector(std::string path, LFFDPlateDetectorConfig &config){
        config.param = path + "/" + config.param;
        config.bin = path + "/" + config.bin;
        fix_align_detector(path, config.align);
    }
}

#endif //PLATE_PLATE_DETECTORS_H
