/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#if !defined(PLATE_INFO_H)
#define PLATE_INFO_H

#include <string>
#include "mat.h"

namespace pr {

    enum PlateColor {
        BLUE,
        YELLOW,
        WHITE,
        GREEN,
        BLACK,
        UNKNOWN
    };

    enum CharType {
        CHINESE,
        LETTER,
        LETTER_NUMS,
        INVALID
    };

    typedef struct ObjectBox {
        float xmin;
        float ymin;
        float xmax;
        float ymax;
        float score;
        float area;
    } ObjectBox;

    typedef struct PlateInfo {
        ObjectBox bbox;
        std::string plate_no;
        std::string plate_color;

        float bbox_reg[4];
        float landmark_reg[8];
        float landmark[8];

        ncnn::Mat license_plate;
        float confidence = 0;
    } PlateInfo;

} // namespace pr

#endif // PLATE_INFO_H
