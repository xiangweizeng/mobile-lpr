/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef PLATE_IPLATEDETECTOR_H
#define PLATE_IPLATEDETECTOR_H

#include <memory>
#include <mat.h>
#include "plate_info.h"

namespace pr{

    enum PlateDetectorType{
        /**
         * mtcnn 检测器
         */
        DETECTOR_MTCNN = 1,

        /**
         * lffd 检测器
         */
        DETECTOR_LFFD,

        /**
         * ssd 检测器
         */
        DETECTOR_SSD,
    };

    class PlateDetectorConfigBase;
    class IPlateDetector;
    typedef std::shared_ptr<IPlateDetector> PlateDetector;
    class IPlateDetector{
    public:
        /**
         * 检测车牌，并进行车牌对齐
         * @param input 输入原始数据
         * @param plates 检测的车牌输出结果
         */
        virtual void plate_detect(ncnn::Mat input, std::vector<PlateInfo> &plates) = 0;
        virtual ~IPlateDetector(){}

        /**
         * 创建目标检测器
         * @param config
         * @return
         */
        static PlateDetector create_plate_detector(PlateDetectorConfigBase &config);
    };

    /**
     * Plate Detector 配置基类
     */
    class PlateDetectorConfigBase{
    public:
        /**
         * 返回类型
         * @return
         */
        virtual PlateDetectorType type() = 0;
        virtual ~PlateDetectorConfigBase(){}
    private:
        /**
         * 创建detector 实例
         * @return
         */
        virtual PlateDetector create_detector() = 0;

        friend class IPlateDetector;
    };
}

#endif //PLATE_IPLATEDETECTOR_H
