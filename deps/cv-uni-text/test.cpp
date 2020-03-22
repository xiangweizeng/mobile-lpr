//
// Created by mini on 2017/5/8.
//

#include <opencv2/opencv.hpp>
#include "cvUniText.hpp"
#include <iostream>

int main(int argc, char** argv) {
    cv::Mat img = cv::imread(argv[1]);
    uni_text::UniText uniText("wqy-microhei.ttc", 22);
    cv::Rect rect = uniText.PutText(img, "Hello, 哈哈哈哈哈嗝", cv::Point(100, 100), cv::Scalar(0,255,0), false);
    std::cout << rect << std::endl;
    cv::imwrite("1.jpg", img);
    return 0;
}
