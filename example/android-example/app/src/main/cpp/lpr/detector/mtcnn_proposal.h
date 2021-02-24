/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef PLATE_MTCNNPROPOSAL_H
#define PLATE_MTCNNPROPOSAL_H

#include "mtcnn_base.h"

namespace pr
{
    class MtcnnProposal : public MtcnnBase {
    public:
        MtcnnProposal(AAssetManager* mgr, std::string param, std::string bin, int width = 120, int height = 32, float iou = 0.7, float threshold = 0.9);
        void proposal(const ncnn::Mat &sample, std::vector<PlateInfo> &bboxs);
    private:
        void generate_bboxs(ncnn::Mat score, ncnn::Mat boxs_mat, float scale, float thresh, std::vector<PlateInfo> &boxes);
    private:
        float min_size;
        float factor;
        float pnet_stride;
        float pnet_cell_size_width;
        float pnet_cell_size_height;
    };

}

#endif //PLATE_MTCNNPROPOSAL_H
