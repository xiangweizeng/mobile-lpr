
//
// Created by damone on 2020/3/22.
//

#ifndef MOBILE_LPR_RECOGNIZERS_H
#define MOBILE_LPR_RECOGNIZERS_H

#include "lpc_recognizer.h"
#include "lpr_recognizer.h"

namespace pr {
    static LPCRecognizerConfig float_lpc_recognizer(nullptr, "lpc.param", "lpc.bin");
    static LPCRecognizerConfig int8_lpc_recognizer(nullptr, "lpc-int8.param", "lpc-int8.bin");
    static LPCRecognizerConfig opt_lpc_recognizer(nullptr, "lpc-opt.param", "lpc-opt.bin");

    static void fix_lpc_recognizer(AAssetManager* mgr, const std::string &path, LPCRecognizerConfig &config)
    {
        config.mgr = mgr;
        config.param = path + "/" + config.param;
        config.bin = path + "/" + config.bin;
    }

    static LPRRecognizerConfig float_lpr_recognizer(nullptr, "lpr.param","lpr.bin", float_lpc_recognizer);
    static LPRRecognizerConfig int8_lpr_recognizer(nullptr, "lpr-int8.param", "lpr-int8.bin", int8_lpc_recognizer);
    static LPRRecognizerConfig opt_lpr_recognizer(nullptr, "lpr-opt.param", "lpr-opt.bin", opt_lpc_recognizer);

    static void fix_lpr_recognizer(AAssetManager* mgr, const std::string &path, LPRRecognizerConfig &config)
    {
        config.mgr = mgr;
        config.param = path + "/" + config.param;
        config.bin = path + "/" + config.bin;
        fix_lpc_recognizer(mgr, path, config.lpc);
    }
}

#endif //MOBILE_LPR_RECOGNIZERS_H
