/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef PLATE_MTCNNBASE_H
#define PLATE_MTCNNBASE_H

#include <vector>
#include <plate_info.h>
#include <mat.h>
#include <net.h>

namespace pr
{
    std::vector<PlateInfo> NMS(std::vector<PlateInfo> &bboxes, float thresh, char methodType);
    void bbox_regression(std::vector<PlateInfo> &bboxes);
    void bbox_pad(std::vector<PlateInfo> &bboxes, int width, int height);
    void bbox_pad_square(std::vector<PlateInfo> &bboxes, int width, int height) ;

    class MtcnnBase {
    public:
        /**
         * create base
         * @param param
         * @param bin
         * @param width
         * @param height
         * @param iou
         * @param threshold
         */
        MtcnnBase(std::string param, std::string bin, int width = 120, int height = 48, float iou = 0.7, float threshold = 0.9);

        /**
        * Filter Plate
        * @param sample
        * @param bboxs
        * @return
        */
        std::vector<PlateInfo> filter(const ncnn::Mat &sample, std::vector<PlateInfo> &bboxs);

    protected:

        float iou_threhold;
        float prob_threhold;
        ncnn::Net net;

        size_t width;
        size_t height;
    };

}

#endif //PLATE_MTCNNBASE_H
