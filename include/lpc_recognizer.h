/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#if !defined(LPC_RECOGNIZER_H)
#define LPC_RECOGNIZER_H

#include <memory>
#include <vector>
#include <net.h>
#include "plate_info.h"

namespace pr {

    class ILPCRecognizer;
    typedef std::shared_ptr<ILPCRecognizer> LPCRecognizer;

    class LPCRecognizerConfig{
    public:

        LPCRecognizerConfig(const std::string &param, const std::string &bin);
        virtual LPCRecognizer create_recognizer();
    public:
        /**
         * 模型文件
         */
        std::string param;
        std::string bin;
    };

    class ILPCRecognizer {
    public:
        virtual ~ILPCRecognizer(){}

        /**
         * 车牌颜色识别
         * @param plate_info
         */
        virtual void predict_plate_color(PlateInfo &plate_info) = 0;
    };
} // namespace pr

#endif // LPC_RECOGNIZER_H
