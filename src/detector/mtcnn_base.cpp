/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#include <algorithm>
#include "mtcnn_base.h"

namespace pr
{
    bool CompareBBox(const PlateInfo &a, const PlateInfo &b) {
        return a.bbox.score > b.bbox.score;
    }

    float IoU(float xmin, float ymin, float xmax, float ymax, float xmin_,
                     float ymin_, float xmax_, float ymax_, bool is_iom) {
        float iw = std::min(xmax, xmax_) - std::max(xmin, xmin_) + 1;
        float ih = std::min(ymax, ymax_) - std::max(ymin, ymin_) + 1;
        if (iw <= 0 || ih <= 0)
            return 0;
        float s = iw * ih;
        if (is_iom) {
            float ov = s / std::min((xmax - xmin + 1) * (ymax - ymin + 1), (xmax_ - xmin_ + 1) * (ymax_ - ymin_ + 1));
            return ov;
        } else {
            float ov = s / ((xmax - xmin + 1) * (ymax - ymin + 1) + (xmax_ - xmin_ + 1) * (ymax_ - ymin_ + 1) - s);
            return ov;
        }
    }

    std::vector<PlateInfo> NMS(std::vector<PlateInfo> &bboxes, float thresh,
                               char methodType) {
        std::vector<PlateInfo> bboxes_nms;
        if (bboxes.size() == 0) {
            return bboxes_nms;
        }
        std::sort(bboxes.begin(), bboxes.end(), CompareBBox);

        int32_t select_idx = 0;
        int32_t num_bbox = static_cast<int32_t>(bboxes.size());
        std::vector<int32_t> mask_merged(num_bbox, 0);
        bool all_merged = false;

        while (!all_merged) {
            while (select_idx < num_bbox && mask_merged[select_idx] == 1)
                select_idx++;
            if (select_idx == num_bbox) {
                all_merged = true;
                continue;
            }
            bboxes_nms.push_back(bboxes[select_idx]);
            mask_merged[select_idx] = 1;

            ObjectBox select_bbox = bboxes[select_idx].bbox;
            float area1 = static_cast<float>((select_bbox.xmax - select_bbox.xmin + 1) *
                                             (select_bbox.ymax - select_bbox.ymin + 1));
            float x1 = static_cast<float>(select_bbox.xmin);
            float y1 = static_cast<float>(select_bbox.ymin);
            float x2 = static_cast<float>(select_bbox.xmax);
            float y2 = static_cast<float>(select_bbox.ymax);

            select_idx++;
            for (int32_t i = select_idx; i < num_bbox; i++) {
                if (mask_merged[i] == 1)
                    continue;

                ObjectBox &bbox_i = bboxes[i].bbox;
                float x = std::max<float>(x1, static_cast<float>(bbox_i.xmin));
                float y = std::max<float>(y1, static_cast<float>(bbox_i.ymin));
                float w = std::min<float>(x2, static_cast<float>(bbox_i.xmax)) - x + 1;
                float h = std::min<float>(y2, static_cast<float>(bbox_i.ymax)) - y + 1;
                if (w <= 0 || h <= 0)
                    continue;

                float area2 = static_cast<float>((bbox_i.xmax - bbox_i.xmin + 1) * (bbox_i.ymax - bbox_i.ymin + 1));
                float area_intersect = w * h;

                switch (methodType) {
                    case 'u':
                        if (static_cast<float>(area_intersect) / (area1 + area2 - area_intersect) > thresh)
                            mask_merged[i] = 1;
                        break;
                    case 'm':
                        if (static_cast<float>(area_intersect) / std::min(area1, area2) > thresh)
                            mask_merged[i] = 1;
                        break;
                    default:
                        break;
                }
            }
        }
        return bboxes_nms;
    }

    void bbox_regression(std::vector<PlateInfo> &bboxes) {
        for (int i = 0; i < bboxes.size(); ++i) {
            ObjectBox &bbox = bboxes[i].bbox;
            float *bbox_reg = bboxes[i].bbox_reg;
            float w = bbox.xmax - bbox.xmin + 1;
            float h = bbox.ymax - bbox.ymin + 1;
            bbox.xmin += bbox_reg[0] * w;
            bbox.ymin += bbox_reg[1] * h;
            bbox.xmax = bbox.xmin + bbox_reg[2] * w;
            bbox.ymax = bbox.ymin + bbox_reg[3] * h;
        }
    }

    void bbox_pad(std::vector<PlateInfo> &bboxes, int width, int height) {
        for (int i = 0; i < bboxes.size(); ++i) {
            ObjectBox &bbox = bboxes[i].bbox;
            bbox.xmin = round(std::max(bbox.xmin, 1.0f));
            bbox.ymin = round(std::max(bbox.ymin, 1.0f));
            bbox.xmax = round(std::min(bbox.xmax, width - 1.f));
            bbox.ymax = round(std::min(bbox.ymax, height - 1.f));
        }
    }

    void bbox_pad_square(std::vector<PlateInfo> &bboxes, int width, int height) {
        for (int i = 0; i < bboxes.size(); ++i) {

            ObjectBox &bbox = bboxes[i].bbox;
            float w = bbox.xmax - bbox.xmin + 1;
            float h = bbox.ymax - bbox.ymin + 1;
            float side = h > w ? h : w;
            float side_w = h > w ? h : w;
            float side_h = 12 / 30.0 * side_w; // 24,60, 48,120

            bbox.xmin = round(std::max(bbox.xmin + (w - side_w) * 0.5f, 0.f));
            bbox.ymin = round(std::max(bbox.ymin + (h - side_h) * 0.5f, 0.f));
            bbox.xmax = round(std::min(bbox.xmin + side_w - 1, width - 1.f));
            bbox.ymax = round(std::min(bbox.ymin + side_h - 1, height - 1.f));
        }
    }

    MtcnnBase::MtcnnBase(std::string param, std::string bin, int width, int height, float iou, float threshold) {
        this->width = width;
        this->height = height;
        this->iou_threhold = iou;
        this->prob_threhold = threshold;

        if(0 != net.load_param(param.c_str())){
            printf("Load param failed for mtcnnbase\n");
            return ;
        }

        if(0 != net.load_model(bin.c_str())){
            printf("Load model failed for mtcnnbase\n");
            return ;
        }
    }

    std::vector<PlateInfo> MtcnnBase::filter(const ncnn::Mat &sample, std::vector <PlateInfo> &bbox) {
        std::vector<PlateInfo> filters;
        int batch_size = bbox.size();

        for (int n = 0; n < batch_size; ++n) {
            ObjectBox &box = bbox[n].bbox;
            ncnn::Mat img_t, in;;

            copy_cut_border(sample, img_t, box.ymin, sample.h - box.ymax, box.xmin, sample.w - box.xmax);
            resize_bilinear(img_t, in, width, height);

            ncnn::Extractor ex = net.create_extractor();
            ex.set_light_mode(true);
            ex.input("data", in);

            ncnn::Mat score, bbox;
            ex.extract("prob1", score);
            ex.extract("conv5-2", bbox);

            float conf = score[1];
            if (conf >= prob_threhold) {

                PlateInfo info;
                info.bbox.score = conf;
                info.bbox.xmin = box.xmin;
                info.bbox.ymin = box.ymin;
                info.bbox.xmax = box.xmax;
                info.bbox.ymax = box.ymax;

                for (int i = 0; i < 4; ++i) {
                    info.bbox_reg[i] = bbox[i];
                }
                filters.push_back(info);
            }
        }

        bbox_regression(filters);
        return NMS(filters, iou_threhold, 'u');
    }
}