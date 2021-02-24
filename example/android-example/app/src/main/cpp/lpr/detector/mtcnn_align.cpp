/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "mtcnn_align.h"

namespace pr {
    MtcnnAlign::MtcnnAlign(AAssetManager* mgr, std::string param, std::string bin, int width, int height, float iou, float threshold)
            : MtcnnBase(mgr, param, bin, width, height, iou, threshold) {
    }

    std::vector<PlateInfo> MtcnnAlign::align(const ncnn::Mat &sample, std::vector<PlateInfo> &bbox) {
        std::vector<PlateInfo> aligns;
        int batch_size = bbox.size();

        for (int n = 0; n < batch_size; ++n) {
            ObjectBox &box = bbox[n].bbox;

            ncnn::Mat img_t, in;
            box.xmin = box.xmin < 0 ? 0 : box.xmin;
	        box.xmax = box.xmax > sample.w ? sample.w : box.xmax;
	        box.ymin = box.ymin < 0 ? 0 : box.ymin;
	        box.ymax = box.ymax > sample.h ? sample.h : box.ymax;
            copy_cut_border(sample, img_t, box.ymin, sample.h - box.ymax, box.xmin, sample.w - box.xmax);
            resize_bilinear(img_t, in, width, height);

            ncnn::Extractor ex = net.create_extractor();
            ex.set_light_mode(true);
            ex.input("data", in);

            ncnn::Mat score, bbox, point;
            ex.extract("prob1", score);
            ex.extract("conv6-2", bbox);
            ex.extract("conv6-3", point);

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

                float w = info.bbox.xmax - info.bbox.xmin + 1.f;
                float h = info.bbox.ymax - info.bbox.ymin + 1.f;
                // x x x x y y y y to x y x y x y x y
                for (int i = 0; i < 4; ++i) {
                    info.landmark[2 * i] = point[2 * i] * w + info.bbox.xmin;
                    info.landmark[2 * i + 1] = point[2 * i + 1] * h + info.bbox.ymin;
                }
                aligns.push_back(info);
            }
        }

        return NMS(aligns, iou_threhold, 'm');
    }
}
