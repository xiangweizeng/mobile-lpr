/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef PLATE_DETECTORCREATOR_H
#define PLATE_DETECTORCREATOR_H

#include <string>
#include "plate_petector.h"

namespace pr
{
    struct AlignPlateDetectorConfig{
        AlignPlateDetectorConfig(const std::string &param, const std::string &bin, int width, int height,
                                 float iou, float prob);
    public:
        /**
         * O网络模型文件
         */
        std::string align_param;
        std::string align_bin;

        /**
         * 网络输入尺寸
         */
        int width;
        int height;

        /**
         * nms threshold
         */
        float align_iou;

        /**
        * prob threshold
        */
        float align_prob;
    };

    class LFFDPlateDetectorConfig : public PlateDetectorConfigBase {
    public:
        /**
         * 网络模型文件
         */
        std::string param;
        std::string bin;

        /**
         * 网络输入尺寸
         * 目前未使用
         */
        int width;
        int height;

        /**
         * 对齐网络配置
         */
        AlignPlateDetectorConfig align;
    private:
    public:
        LFFDPlateDetectorConfig(const std::string &param, const std::string &bin, int width, int height,
                                const AlignPlateDetectorConfig &align);
        PlateDetectorType type() { return DETECTOR_LFFD; };
    private:
        PlateDetector create_detector();
    };

    class MtcnnPlateDetectorConfig : public PlateDetectorConfigBase{
    public:
        /**
         * P网络模型文件
         */
        std::string proposal_param;
        std::string proposal_bin;

        /**
         * R网络模型文件
         */
        std::string filter_param;
        std::string filter_bin;
        /**
         * IOU threshold
         */
        float proposal_iou;

        /**
         * PNET prob threshold
         */
        float proposal_prob;

        /**
         * RNET prob threshold
         */
        float filter_prob;

        /**
         * align
         */
        AlignPlateDetectorConfig align;
    public:
        MtcnnPlateDetectorConfig(const std::string &pr_param, const std::string &pr_bin,
                                 const std::string &fl_param, const std::string &fl_bin,
                                 float pr_iou, float pr_prob, float fl_prob,
                                 const AlignPlateDetectorConfig &align);
        PlateDetectorType type(){ return DETECTOR_MTCNN;}
    protected:
        PlateDetector create_detector();
    };

    class SSDPlateDetectorConfig : public PlateDetectorConfigBase {
    public:
        /**
         * 网络模型文件
         */
        std::string param;
        std::string bin;

        /**
         * 检测结果过滤阈值
         */
        float detect_score;

        /**
         * 网络输入尺寸
         * 目前未使用
         */
        int width;
        int height;

        /**
         * align
         */
        AlignPlateDetectorConfig align;
    public:
        SSDPlateDetectorConfig(const std::string &param, const std::string &bin, float score_threshold, int width,
                               int height, const AlignPlateDetectorConfig &align);
        PlateDetectorType type() { return DETECTOR_SSD; };
    private:
        PlateDetector create_detector();
    };
}
#endif //PLATE_DETECTORCREATOR_H
