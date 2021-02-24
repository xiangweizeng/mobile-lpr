/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#include <tgmath.h>
#include "mtcnn_proposal.h"

namespace pr
{
    MtcnnProposal::MtcnnProposal(AAssetManager* mgr, std::string param, std::string bin, int width, int height, float iou,
                                 float threshold) : MtcnnBase(mgr, param, bin, width, height, iou, threshold) {
        min_size = 80;
        factor = 0.709f;
        pnet_stride = 2;
        pnet_cell_size_width = 30;
        pnet_cell_size_height = 12;
    }

    void MtcnnProposal::proposal(const ncnn::Mat &sample, std::vector<PlateInfo> &bboxs) {
        int width = sample.w;
        int height = sample.h;
        float scale = 12.0f / min_size;
        float minWH = std::min(height, width) * scale;
        std::vector<float> scales;
        while (minWH >= 30) {
            scales.push_back(scale);
            minWH *= factor;
            scale *= factor;
        }

        std::vector<PlateInfo> total_boxes;
        std::vector<PlateInfo> candidate_boxes;

        for (int i = 0; i < scales.size(); i++) {
            int ws = (int) std::ceil(width * scales[i]);
            int hs = (int) std::ceil(height * scales[i]);

            ncnn::Mat in;
            resize_bilinear(sample, in, ws, hs);
            ncnn::Extractor ex = net.create_extractor();
            ex.set_light_mode(true);
            ex.input("data", in);

            ncnn::Mat score;
            ncnn::Mat location;
            ex.extract("prob1", score);
            ex.extract("conv4-2", location);

            generate_bboxs(score, location, scales[i], prob_threhold, candidate_boxes);
            std::vector<PlateInfo> bboxes_nms = NMS(candidate_boxes, 0.5f, 'u');

            if (bboxes_nms.size() > 0) {
                total_boxes.insert(total_boxes.end(), bboxes_nms.begin(), bboxes_nms.end());
            }
        }

        int num_box = (int) total_boxes.size();
        bboxs.clear();

        if (num_box != 0) {
            bboxs = NMS(total_boxes, iou_threhold, 'u');
            bbox_regression(bboxs);
            bbox_pad_square(bboxs, width, height);
        }
    }

    void MtcnnProposal::generate_bboxs(ncnn::Mat score, ncnn::Mat boxs_mat, float scale, float thresh,
                                       std::vector<PlateInfo> &boxes) {
        boxes.clear();
        float *prob1_confidence = score.channel(1);
        for (int h = 0; h < score.h; h++) {
            for (int w = 0; w < score.w; w++, prob1_confidence++) {
                if (*prob1_confidence > thresh) {
                    PlateInfo plate_info;
                    ObjectBox &bbox = plate_info.bbox;
                    bbox.score = *prob1_confidence;
                    bbox.xmin = w * pnet_stride / scale;
                    bbox.ymin = h * pnet_stride / scale;
                    bbox.xmax = (w * pnet_stride + pnet_cell_size_width + 1 - 1.f) / scale;
                    bbox.ymax = (h * pnet_stride + pnet_cell_size_height + 1 - 1.f) / scale;

                    int index = h * score.w + w;
                    plate_info.bbox_reg[0] = boxs_mat.channel(0)[index];
                    plate_info.bbox_reg[1] = boxs_mat.channel(1)[index];
                    plate_info.bbox_reg[2] = boxs_mat.channel(2)[index];
                    plate_info.bbox_reg[3] = boxs_mat.channel(3)[index];
                    boxes.push_back(plate_info);
                }
            }
        }
    }
}