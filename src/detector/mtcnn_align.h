//
/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef PLATE_MTCNNALIGN_H
#define PLATE_MTCNNALIGN_H

#include "mtcnn_base.h"

namespace pr
{
    class MtcnnAlign : public MtcnnBase {
    public:
        MtcnnAlign(std::string param, std::string bin, int width = 120, int height = 32, float iou = 0.7, float threshold = 0.9);

        /**
         * Align Plate
         * @param sample
         * @param bbox
         * @return
         */
        std::vector<PlateInfo> align(const ncnn::Mat &sample, std::vector<PlateInfo> &bboxs);
    };

}

#endif //PLATE_MTCNNALIGN_H
