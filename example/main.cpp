/**
 * @author damone (xiang.wei.zeng@qq.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2020
 *
 */

#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "plate_detectors.h"
#include "plate_recognizers.h"
#include "cvUniText.hpp"

using namespace cv;
using namespace std;

void draw_plate_results(std::vector<pr::PlateInfo> &objects, cv::Mat &image, std::string save_name) {
    uni_text::UniText uniText("../../deps/cv-uni-text/install/wqy-microhei.ttc",22);
    for (auto pi : objects) {
        cv::Rect area(pi.bbox.xmin, pi.bbox.ymin, pi.bbox.xmax - pi.bbox.xmin, pi.bbox.ymax - pi.bbox.ymin);
        cv::rectangle(image, area, cv::Scalar(255, 0, 0));
        int baseLine = 0;
        cv::Size label_size =
                cv::getTextSize(pi.plate_no, cv::FONT_HERSHEY_SIMPLEX, 0.5, 3, &baseLine);

        int x = pi.bbox.xmin;
        int y = pi.bbox.ymin - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > image.cols)
            x = image.cols - label_size.width;

        cv::rectangle(
                image,
                cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
                cv::Scalar(255, 255, 255), cv::FILLED);

        uniText.PutText(image, pi.plate_no, cv::Point(x, y + label_size.height), cv::Scalar(0,255,0), false);
    }

    cv::imwrite(save_name, image);
}

void test_mtcnn_plate() {
    pr::fix_mtcnn_detector("../../models/float", pr::mtcnn_float_detector);
    pr::PlateDetector detector = pr::IPlateDetector::create_plate_detector(pr::mtcnn_float_detector);

    pr::fix_lpr_recognizer("../../models/float", pr::float_lpr_recognizer);
    pr::LPRRecognizer lpr = pr::float_lpr_recognizer.create_recognizer();
    cv::Mat img = imread("../../image/muti-angle.jpg");

    ncnn::Mat sample = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_BGR, img.cols, img.rows);
    std::vector<pr::PlateInfo> objects;
    detector->plate_detect(sample, objects);
    lpr->decode_plate_infos(objects);

    for (auto pi : objects) {
        cout << "plate_no: " << pi.plate_color << pi.plate_no << " box:" << pi.bbox.xmin << ","
             << pi.bbox.ymin << "," << pi.bbox.xmax << "," << pi.bbox.ymax << "," << pi.bbox.score << endl;
    }

    draw_plate_results(objects, img, "mtcnn-plate.png");
}

void test_quantize_mtcnn_plate() {
    pr::fix_mtcnn_detector("../../models/quantize", pr::mtcnn_int8_detector);
    pr::PlateDetector detector = pr::IPlateDetector::create_plate_detector(pr::mtcnn_int8_detector);

    pr::fix_lpr_recognizer("../../models/quantize", pr::int8_lpr_recognizer);
    pr::LPRRecognizer lpr = pr::int8_lpr_recognizer.create_recognizer();
    Mat img = imread("../../image/muti-angle.jpg");

    ncnn::Mat sample = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_BGR, img.cols, img.rows);
    std::vector<pr::PlateInfo> objects;
    detector->plate_detect(sample, objects);
    lpr->decode_plate_infos(objects);

    for (auto pi : objects) {
        cout << "plate_no: " << pi.plate_color << pi.plate_no << " box:" << pi.bbox.xmin << ","
             << pi.bbox.ymin << "," << pi.bbox.xmax << "," << pi.bbox.ymax << "," << pi.bbox.score << endl;
    }

    draw_plate_results(objects, img, "quantize-mtcnn-plate.png");
}


void test_ssd_plate() {
    pr::fix_ssd_detector("../../models/float", pr::ssd_float_detector);
    pr::PlateDetector detector = pr::IPlateDetector::create_plate_detector(pr::ssd_float_detector);

    pr::fix_lpr_recognizer("../../models/float", pr::float_lpr_recognizer);
    pr::LPRRecognizer lpr = pr::float_lpr_recognizer.create_recognizer();
    Mat img = imread("../../image/vehicle-plate3.jpg");

    ncnn::Mat sample = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_BGR, img.cols, img.rows);
    std::vector<pr::PlateInfo> objects;
    detector->plate_detect(sample, objects);
    lpr->decode_plate_infos(objects);

    for (auto pi : objects) {
        cout << "plate_no: " << pi.plate_color << pi.plate_no << " box:" << pi.bbox.xmin << ","
             << pi.bbox.ymin << "," << pi.bbox.xmax << "," << pi.bbox.ymax << "," << pi.bbox.score << endl;
    }

    draw_plate_results(objects, img, "ssd-plate.png");
}

void test_lffd_plate() {
    pr::fix_lffd_detector("../../models/float", pr::lffd_float_detector);
    pr::PlateDetector detector = pr::IPlateDetector::create_plate_detector(pr::lffd_float_detector);

    pr::fix_lpr_recognizer("../../models/float", pr::float_lpr_recognizer);
    pr::LPRRecognizer lpr = pr::float_lpr_recognizer.create_recognizer();
    Mat img = imread("../../image/plate.png");

    ncnn::Mat sample = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_BGR, img.cols, img.rows);
    std::vector<pr::PlateInfo> objects;
    detector->plate_detect(sample, objects);
    lpr->decode_plate_infos(objects);

    for (auto pi : objects) {
        cout << "plate_no: " << pi.plate_color << pi.plate_no << " box:" << pi.bbox.xmin << ","
             << pi.bbox.ymin << "," << pi.bbox.xmax << "," << pi.bbox.ymax << "," << pi.bbox.score << endl;
    }

    draw_plate_results(objects, img, "lffd-plate.png");
}

int main(int argc, char *argv[]) {
    printf("test mtcnn:\n");
    test_mtcnn_plate();
    printf("test quantize mtcnn:\n");
    test_quantize_mtcnn_plate();
    printf("test lffd:\n");
    test_lffd_plate();
    printf("test ssd:\n");
    test_ssd_plate();
    return 0;
}
