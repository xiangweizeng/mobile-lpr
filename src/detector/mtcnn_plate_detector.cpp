/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "mtcnn_proposal.h"
#include "align_plate_detector.h"
#include "detector_creator.h"

namespace pr {
    class MtcnnPlateDetector : public AlignPlateDetector {
    public:

        /**
         * 执行检测操作
         * @param input
         * @param plates
         */
        virtual void plate_detect(ncnn::Mat input, std::vector<PlateInfo> &plates);

    protected:
        /**
         * 创建检测器
         * @param config
         */
        MtcnnPlateDetector(MtcnnPlateDetectorConfig &config);

        friend class MtcnnPlateDetectorConfig;

    protected:
        MtcnnProposal proposal;
        MtcnnBase filter;

        int pnet_max_detect_num;
        const float mean_vals[3] = {127.5f, 127.5f, 127.5f};
        const float norm_vals[3] = {0.0078125f, 0.0078125f, 0.0078125f};
    };


    MtcnnPlateDetectorConfig::MtcnnPlateDetectorConfig(
            const std::string &pr_param,
            const std::string &pr_bin,
            const std::string &fl_param,
            const std::string &fl_bin,
            float pr_iou,
            float pr_prob,
            float fl_prob,
            const AlignPlateDetectorConfig &align)
            : proposal_param(pr_param), proposal_bin(pr_bin),
              filter_param(fl_param), filter_bin(fl_bin),
              proposal_iou(pr_iou), proposal_prob(pr_prob),
              filter_prob(fl_prob), align(align) {}

    PlateDetector MtcnnPlateDetectorConfig::create_detector() {
        return PlateDetector(new MtcnnPlateDetector(*this));
    }

    MtcnnPlateDetector::MtcnnPlateDetector(MtcnnPlateDetectorConfig &config) :
            AlignPlateDetector(config.align),
            proposal(config.proposal_param, config.proposal_bin, 120, 48, config.proposal_iou, config.proposal_prob),
            filter(config.filter_param, config.filter_bin, 60, 24, config.proposal_iou, config.filter_prob) {
        pnet_max_detect_num = 5000;
    }

    void MtcnnPlateDetector::plate_detect(ncnn::Mat input, std::vector<PlateInfo> &plates) {

        ncnn::Mat sample = input.clone();
        sample.substract_mean_normalize(mean_vals, norm_vals);

        std::vector<PlateInfo> proposals;
        proposal.proposal(sample, proposals);
        if (pnet_max_detect_num < (int) proposals.size()) {
            proposals.resize(pnet_max_detect_num);
        }

        std::vector<PlateInfo> filters = this->filter.filter(sample, proposals);
        bbox_pad_square(filters, input.w, input.h);

        plates = align.align(sample, filters);
        bbox_pad_square(plates, input.w, input.h);
        bbox_pad(plates, input.w, input.h);

        crop_plates(input, plates);
    }

} // namespace pr
