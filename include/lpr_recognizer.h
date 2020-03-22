/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#if !defined(LPR_RECOGNIZER_H)
#define LPR_RECOGNIZER_H

#include <memory>
#include <vector>
#include <net.h>
#include "plate_info.h"
#include "lpc_recognizer.h"

namespace pr {

    class ILPRRecognizer;

    typedef std::shared_ptr<ILPRRecognizer> LPRRecognizer;

    class LPRRecognizerConfig {
    public:

        LPRRecognizerConfig(const std::string &param, const std::string &bin, const LPCRecognizerConfig &lpc);

        virtual LPRRecognizer create_recognizer();

    public:
        /**
         * 模型文件
         */
        std::string param;
        std::string bin;

        /**
         * 颜色识别
         */
        LPCRecognizerConfig lpc;
    };

    class ILPRRecognizer {
    public:
        /**
         * 识别车牌， 包括车牌及颜色
         * @param plate_infos
         */
        virtual void decode_plate_infos(std::vector<PlateInfo> &plate_infos) = 0;
        virtual ~ILPRRecognizer() {}
    };


} // namespace pr

#endif // LPR_RECOGNIZER_H
