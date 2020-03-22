/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "lffd.h"
#include "align_plate_detector.h"
#include "detector_creator.h"

namespace pr {

    class LFFDPlateDetector : public AlignPlateDetector {
    public:
        virtual void plate_detect(ncnn::Mat input, std::vector<PlateInfo> &plates);

    private:
        LFFDPlateDetector(LFFDPlateDetectorConfig &config);

        friend class LFFDPlateDetectorConfig;

    private:
        LFFD lffd;
        size_t width;
        size_t height;
    };

    PlateDetector LFFDPlateDetectorConfig::create_detector() {
        return PlateDetector(new LFFDPlateDetector(*this));
    }

    LFFDPlateDetectorConfig::LFFDPlateDetectorConfig(
            const std::string &param,
            const std::string &bin,
            int width,
            int height,
            const AlignPlateDetectorConfig &align) :
            param(param),
            bin(bin),
            width(width),
            height(height),
            align(align) {}

    LFFDPlateDetector::LFFDPlateDetector(LFFDPlateDetectorConfig &config) :
            AlignPlateDetector(config.align),
            lffd(config.param, config.bin, 4) {
        this->width = width;
        this->height = height;
    }

    void LFFDPlateDetector::plate_detect(ncnn::Mat input, std::vector<PlateInfo> &plates) {
        std::vector<pr::ObjectBox> detects;
        lffd.detect(input, detects, input.h, input.w);

        std::vector<PlateInfo> detect_plates;
        for (int i = 0; i < detects.size(); i++) {

            pr::ObjectBox box = detects[i];
            float rect_width = box.xmax - box.xmin;
            float rect_height = box.ymax - box.ymin;

            PlateInfo plateInfo;
            plateInfo.bbox.xmin = box.xmin - 0.1 * rect_width;
            plateInfo.bbox.ymin = box.ymin - 0.1 * rect_height;
            plateInfo.bbox.xmax = box.xmax + 0.1 * rect_width;
            plateInfo.bbox.ymax = box.ymax + 0.1 * rect_height;
            plateInfo.bbox.score = box.score;
            detect_plates.push_back(plateInfo);
        }

        plates = align_plates(input, detect_plates);
        crop_plates(input, plates);
    }
}