/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "lpr_recognizer.h"
#include "label.h"

namespace pr {

    class LPRRecognizerImpl : public ILPRRecognizer {
    public:
        LPRRecognizerImpl(LPRRecognizerConfig &config);

        ~LPRRecognizerImpl();

        void decode_plate_infos(std::vector<PlateInfo> &plate_infos);

    private:
        void ctc_decode(ncnn::Mat &output, std::vector<int> &codes);

        std::string decode_plate_no(const std::vector<int> &codes);

        void predict_plate_license_no(PlateInfo &plate_info);

    private:
        ncnn::Net net;
        LPCRecognizer lpc_recognizer;
    };

    LPRRecognizerConfig::LPRRecognizerConfig(AAssetManager* mgr, const std::string &param, const std::string &bin,
                                             const LPCRecognizerConfig &lpc) : mgr(mgr), param(param), bin(bin), lpc(lpc) {}

    LPRRecognizer LPRRecognizerConfig::create_recognizer() {
        return LPRRecognizer(new LPRRecognizerImpl(*this));
    }

    LPRRecognizerImpl::LPRRecognizerImpl(LPRRecognizerConfig &config)
            : lpc_recognizer(config.lpc.create_recognizer()) {
        this->net.load_param(config.mgr, config.param.c_str());
        this->net.load_model(config.mgr, config.bin.c_str());
    }

    LPRRecognizerImpl::~LPRRecognizerImpl() {
    }

    void LPRRecognizerImpl::decode_plate_infos(std::vector<PlateInfo> &plate_infos) {
        for (auto &plateInfo : plate_infos) {
            predict_plate_license_no(plateInfo);
            lpc_recognizer->predict_plate_color(plateInfo);
        }
    }

    void LPRRecognizerImpl::ctc_decode(ncnn::Mat &output, std::vector<int> &codes) {
        int prev_class_idx = -1;
        for (int t = 0; t < output.c; t++) {
            int max_class_idx = 0;
            float *current_code = output.channel(t);
            float max_prob = *current_code;
            current_code++;
            for (int c = 1; c < output.h; c++, current_code++) {
                if (*current_code > max_prob) {
                    max_prob = *current_code;
                    max_class_idx = c;
                }
            }

            if (max_class_idx != 0 && max_class_idx != prev_class_idx) {
                codes.push_back(max_class_idx);
            }
            prev_class_idx = max_class_idx;
        }
    }

    std::string LPRRecognizerImpl::decode_plate_no(const std::vector<int> &codes) {
        std::string plateNo = "";
        for (auto it = codes.begin(); it != codes.end(); ++it) {
            plateNo += label[*it];
        }
        return plateNo;
    }

    void LPRRecognizerImpl::predict_plate_license_no(PlateInfo &plate_info) {
        std::vector<int> codes = {};

        const float mean_vals[3] = {116.407, 133.722, 124.187};
        const float norm_vals[3] = {1.0f, 1.0f, 1.0f};

        ncnn::Mat in = plate_info.license_plate.clone();
        //resize_bilinear(Plate, in, 128, 32);
        in.substract_mean_normalize(mean_vals, norm_vals);

        ncnn::Extractor ex = net.create_extractor();
        ex.set_light_mode(true);
        ex.input("data", in);
        ncnn::Mat probs;
        ex.extract("fc1x", probs);

        ctc_decode(probs, codes);
        plate_info.plate_no = decode_plate_no(codes);
    }
} // namespace pr
