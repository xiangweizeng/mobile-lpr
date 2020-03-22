
//
// Created by damone on 2020/3/22.
//

#ifndef MOBILE_LPR_RECOGNIZERS_H
#define MOBILE_LPR_RECOGNIZERS_H

#include "lpc_recognizer.h"
#include "lpr_recognizer.h"

namespace pr {
    static LPCRecognizerConfig float_lpc_recognizer = {
        "lpc.param",
        "lpc.bin"
    };

    static LPCRecognizerConfig int8_lpc_recognizer = {
            "lpc-int8.param",
            "lpc-int8.bin"
    };

    static LPCRecognizerConfig opt_lpc_recognizer = {
            "lpc-opt.param",
            "lpc-opt.bin"
    };

    static void fix_lpc_recognizer(std::string path, LPCRecognizerConfig &config)
    {
        config.param = path + "/" + config.param;
        config.bin = path + "/" + config.bin;
    }

    static LPRRecognizerConfig float_lpr_recognizer = {
            "lpr.param",
            "lpr.bin",
            float_lpc_recognizer
    };

    static LPRRecognizerConfig int8_lpr_recognizer = {
            "lpr-int8.param",
            "lpr-int8.bin",
            int8_lpc_recognizer
    };

    static LPRRecognizerConfig opt_lpr_recognizer = {
            "lpr-opt.param",
            "lpr-opt.bin",
            opt_lpc_recognizer
    };

    static void fix_lpr_recognizer(std::string path, LPRRecognizerConfig &config)
    {
        config.param = path + "/" + config.param;
        config.bin = path + "/" + config.bin;
        fix_lpc_recognizer(path, config.lpc);
    }
}

#endif //MOBILE_LPR_RECOGNIZERS_H
