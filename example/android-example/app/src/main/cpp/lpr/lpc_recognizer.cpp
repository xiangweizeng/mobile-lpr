/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "lpc_recognizer.h"

namespace pr {

    class LPCRecognizerImpl : public ILPCRecognizer{
    public:
        LPCRecognizerImpl(LPCRecognizerConfig &config);

        ~LPCRecognizerImpl();

        virtual void predict_plate_color(PlateInfo &plate_info);

    private:
        ncnn::Net net;
        std::vector<std::string> plate_color_dict;
    };

    LPCRecognizerConfig::LPCRecognizerConfig(AAssetManager* mgr, const std::string &param, const std::string &bin)
            : mgr(mgr), param(param), bin(bin) {}

    LPCRecognizer LPCRecognizerConfig::create_recognizer() {
        return LPCRecognizer(new LPCRecognizerImpl(*this));
    }

    LPCRecognizerImpl::LPCRecognizerImpl(LPCRecognizerConfig &config) {
        this->net.load_param(config.mgr, config.param.c_str());
        this->net.load_model(config.mgr, config.bin.c_str());

        plate_color_dict.push_back("白");
        plate_color_dict.push_back("黄");
        plate_color_dict.push_back("蓝");
        plate_color_dict.push_back("黑");
        plate_color_dict.push_back("绿");
    }

    LPCRecognizerImpl::~LPCRecognizerImpl() {
    }

    void LPCRecognizerImpl::predict_plate_color(PlateInfo &plate_info) {
        const float mean_vals[3] = {89.9372, 81.1989, 73.6352};
        const float norm_vals[3] = {1.0f, 1.0f, 1.0f};

        ncnn::Mat in;
        resize_bilinear(plate_info.license_plate, in, 110, 22);
        in.substract_mean_normalize(mean_vals, norm_vals);

        ncnn::Extractor ex = net.create_extractor();
        ex.set_light_mode(true);
        ex.input("data", in);
        ncnn::Mat probs;
        ex.extract("prob", probs);

        float max = probs.channel(0)[0];
        int clsId = 0;
        for (int i = 1; i < 5; i++) {
            if (probs.channel(i)[0] > max) {
                max = probs.channel(i)[0];
                clsId = i;
            }
        }

        plate_info.plate_color = plate_color_dict[clsId];
    }
} // namespace pr
