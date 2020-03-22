/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "base.h"
#include "align_plate_detector.h"

namespace pr{
    AlignPlateDetectorConfig::AlignPlateDetectorConfig(const std::string &param, const std::string &bin,
                                                       int width, int height, float iou, float prob)
            : align_param(param), align_bin(bin), width(width), height(height), align_iou(iou),
              align_prob(prob) {}

    AlignPlateDetector::AlignPlateDetector(AlignPlateDetectorConfig &config):
    align(config.align_param, config.align_bin, config.width, config.height, config.align_iou, config.align_prob)
    {}

    std::vector<PlateInfo> AlignPlateDetector::align_plates(ncnn::Mat input, std::vector<PlateInfo> &filters) {
        const float mean_vals[3] = {127.5f, 127.5f, 127.5f};
        const float norm_vals[3] = {0.0078125f, 0.0078125f, 0.0078125f};

        ncnn::Mat sample = input.clone();
        sample.substract_mean_normalize(mean_vals, norm_vals);

        std::vector<PlateInfo> plates = align.align(sample, filters);
        bbox_pad_square(plates, input.w, input.h);
        bbox_pad(plates, input.w, input.h);

        return  plates;
    }

    static ncnn::Mat plate_preprocess(ncnn::Mat img, PlateInfo &info) {
        int image_w = 128;
        int image_h = 32;

        float dst[8];
        float src[8];

        dst[0] = 0;
        dst[4] = 0;
        dst[1] = 128;
        dst[5] = 0;
        dst[2] = 128;
        dst[6] = 32;
        dst[3] = 0;
        dst[7] = 32;

        src[0] = info.landmark[0];
        src[4] = info.landmark[1];
        src[1] = info.landmark[2];
        src[5] = info.landmark[3];
        src[2] = info.landmark[4];
        src[6] = info.landmark[5];
        src[3] = info.landmark[6];
        src[7] = info.landmark[7];

        float M[6];
        get_affine_matrix(src, dst, M);
        return warp_affine_matrix(img, M, image_w, image_h);
    }

    void AlignPlateDetector::crop_plates(ncnn::Mat input, std::vector<PlateInfo> &filters) {
        for(auto &info : filters){
            info.license_plate = plate_preprocess(input, info);
        }
    }
}
