/**
 * @file SSDPlateDetection.cpp
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 * @date 2019-09-25
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "align_plate_detector.h"
#include "detector_creator.h"

namespace pr {

    class SSDPlateDetector : public AlignPlateDetector {
    public:
        virtual void plate_detect(ncnn::Mat input, std::vector<PlateInfo> &plates);
    private:
        SSDPlateDetector(SSDPlateDetectorConfig &config);
        friend class SSDPlateDetectorConfig;
    private:

        ncnn::Net net;
        size_t width;
        size_t height;
        float detect_prob;
    };

    PlateDetector SSDPlateDetectorConfig::create_detector() {
        return PlateDetector(new SSDPlateDetector(*this));
    }

    SSDPlateDetectorConfig::SSDPlateDetectorConfig(const std::string &param, const std::string &bin, float score_threshold,
                                                   int width, int height, const AlignPlateDetectorConfig &align)
            : param(param), bin(bin), detect_score(score_threshold), width(width), height(height), align(align) {}

    SSDPlateDetector::SSDPlateDetector(SSDPlateDetectorConfig &config) :
            AlignPlateDetector(config.align) {
        this->width = config.width;
        this->height = config.height;
        this->detect_prob = config.detect_score;

        this->net.load_param(config.param.c_str());
        this->net.load_model(config.bin.c_str());
    }

    void SSDPlateDetector::plate_detect(ncnn::Mat input, std::vector<PlateInfo> &plates) {
        ncnn::Mat in = input.clone();
        //resize_bilinear(input, in, width, height);

        const float mean_vals[3] = {127.5f, 127.5f, 127.5f};
        const float norm_vals[3] = {0.007843f, 0.007843f, 0.007843f};
        in.substract_mean_normalize(mean_vals, norm_vals);

        ncnn::Extractor ex = net.create_extractor();
        ex.set_light_mode(true);
        ex.input("data", in);
        ncnn::Mat objects;
        ex.extract("detection_out", objects);

        float confidence_threshold = detect_prob;
        std::vector<PlateInfo> detects;
        for (int i = 0; i < objects.h; i++) {
            float confidence = objects.row(i)[1];
            if (confidence > confidence_threshold) {
                float tl_x = objects.row(i)[2] * input.w;
                float tl_y = objects.row(i)[3] * input.h;
                float br_x = objects.row(i)[4] * input.w;
                float br_y = objects.row(i)[5] * input.h;

                float rect_width = br_x - tl_x;
                float rect_height = br_y - tl_y;

                PlateInfo plate_info;
                plate_info.bbox.xmin = tl_x - 0.1 * rect_width;
                plate_info.bbox.ymin = tl_y - 0.1 * rect_height;
                plate_info.bbox.xmax = br_x + 0.1 * rect_width;
                plate_info.bbox.ymax = br_y + 0.1 * rect_height;
                plate_info.bbox.score = confidence;
                detects.push_back(plate_info);
            }
        }

        plates = align_plates(input, detects);
        crop_plates(input, plates);
    }

} // namespace pr
