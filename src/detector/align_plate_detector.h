/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef PLATE_ALIGNPLATEDETECTOR_H
#define PLATE_ALIGNPLATEDETECTOR_H

#include "detector_creator.h"
#include "plate_info.h"
#include "mtcnn_align.h"

namespace pr {

    /**
     * 封装车牌对齐
     */
    class AlignPlateDetector : public IPlateDetector {
    protected:
        AlignPlateDetector(AlignPlateDetectorConfig &config);
        std::vector<PlateInfo> align_plates(ncnn::Mat input, std::vector<PlateInfo> &filters);
        void crop_plates(ncnn::Mat input, std::vector<PlateInfo> &filters);
    protected:
        MtcnnAlign align;
    };
}

#endif //PLATE_ALIGNPLATEDETECTOR_H
