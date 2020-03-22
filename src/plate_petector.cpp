/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "plate_petector.h"

namespace pr{
    PlateDetector IPlateDetector::create_plate_detector(PlateDetectorConfigBase &config) {
        return config.create_detector();
    }
}