/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef BASE_H
#define BASE_H

#include <cmath>
#include <cstring>
#include "net.h"

namespace pr
{
    void get_affine_matrix(float* src_4pts, const float* dst_4pts, float* M);
    ncnn::Mat warp_affine_matrix(ncnn::Mat src, float *M, int dst_w, int dst_h);
}

#endif
