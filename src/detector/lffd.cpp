
#include "lffd.h"

LFFD::LFFD(std::string param, std::string bin, int num_thread_) {
    num_thread = num_thread_;
    num_output_scales = 3;
    param_file_name = param;
    bin_file_name = bin;
    receptive_field_list = {128, 256, 512};
    receptive_field_stride = {16, 32, 64};
    bbox_small_list = {64, 128, 256};
    bbox_large_list = {128, 256, 512};
    receptive_field_center_start = {15, 31, 63};

    for (int i = 0; i < receptive_field_list.size(); i++) {
        constant.push_back(receptive_field_list[i] / 2);
    }

    output_blob_names = {"softmax0", "conv10_3_bbox",
                         "softmax1", "conv13_3_bbox",
                         "softmax2", "conv16_3_bbox"};

    lffd.load_param(param_file_name.data());
    lffd.load_model(bin_file_name.data());
}

LFFD::~LFFD() {
    lffd.clear();
}

int LFFD::detect(ncnn::Mat &img, std::vector<pr::ObjectBox> &face_list, int resize_h, int resize_w,
                 float score_threshold, float nms_threshold, int top_k, std::vector<int> skip_scale_branch_list) {

    if (img.empty()) {
        std::cout << "image is empty ,please check!" << std::endl;
        return -1;
    }

    image_h = img.h;
    image_w = img.w;

    ncnn::Mat in;
    ncnn::resize_bilinear(img, in, resize_w, resize_h);

    ncnn::Mat ncnn_img = in;
    std::vector<pr::ObjectBox> bbox_collection;
    ncnn::Extractor ex = lffd.create_extractor();
    ex.set_num_threads(num_thread);
    ex.input("data", ncnn_img);

    for (int i = 0; i < num_output_scales; i++) {
        ncnn::Mat conf;
        ncnn::Mat reg;

        ex.extract(output_blob_names[2 * i].c_str(), conf);
        ex.extract(output_blob_names[2 * i + 1].c_str(), reg);

        generateBBox(bbox_collection, conf, reg, score_threshold, conf.w, conf.h, in.w, in.h, i);
    }
    std::vector<pr::ObjectBox> valid_input;
    get_topk_bbox(bbox_collection, valid_input, top_k);
    nms(valid_input, face_list, nms_threshold);

    return 0;
}

void LFFD::generateBBox(std::vector<pr::ObjectBox> &bbox_collection, ncnn::Mat score_map, ncnn::Mat box_map,
                        float score_threshold, int fea_w, int fea_h, int cols, int rows, int scale_id) {
    float *RF_center_Xs = new float[fea_w];
    float *RF_center_Xs_mat = new float[fea_w * fea_h];
    float *RF_center_Ys = new float[fea_h];
    float *RF_center_Ys_mat = new float[fea_h * fea_w];

    for (int x = 0; x < fea_w; x++) {
        RF_center_Xs[x] = receptive_field_center_start[scale_id] + receptive_field_stride[scale_id] * x;
    }
    for (int x = 0; x < fea_h; x++) {
        for (int y = 0; y < fea_w; y++) {
            RF_center_Xs_mat[x * fea_w + y] = RF_center_Xs[y];
        }
    }

    for (int x = 0; x < fea_h; x++) {
        RF_center_Ys[x] = receptive_field_center_start[scale_id] + receptive_field_stride[scale_id] * x;
        for (int y = 0; y < fea_w; y++) {
            RF_center_Ys_mat[x * fea_w + y] = RF_center_Ys[x];
        }
    }

    float *x_lt_mat = new float[fea_h * fea_w];
    float *y_lt_mat = new float[fea_h * fea_w];
    float *x_rb_mat = new float[fea_h * fea_w];
    float *y_rb_mat = new float[fea_h * fea_w];



    //x-left-top
    float mid_value = 0;
    for (int j = 0; j < fea_h * fea_w; j++) {
        mid_value = RF_center_Xs_mat[j] - box_map.channel(0)[j] * constant[scale_id];
        x_lt_mat[j] = mid_value < 0 ? 0 : mid_value;
    }
    //y-left-top
    for (int j = 0; j < fea_h * fea_w; j++) {
        mid_value = RF_center_Ys_mat[j] - box_map.channel(1)[j] * constant[scale_id];
        y_lt_mat[j] = mid_value < 0 ? 0 : mid_value;
    }
    //x-right-bottom
    for (int j = 0; j < fea_h * fea_w; j++) {
        mid_value = RF_center_Xs_mat[j] - box_map.channel(2)[j] * constant[scale_id];
        x_rb_mat[j] = mid_value > cols - 1 ? cols - 1 : mid_value;
    }
    //y-right-bottom
    for (int j = 0; j < fea_h * fea_w; j++) {
        mid_value = RF_center_Ys_mat[j] - box_map.channel(3)[j] * constant[scale_id];
        y_rb_mat[j] = mid_value > rows - 1 ? rows - 1 : mid_value;
    }

    for (int k = 0; k < fea_h * fea_w; k++) {
        if (score_map.channel(0)[k] > score_threshold) {
            pr::ObjectBox faceinfo;
            faceinfo.xmin = x_lt_mat[k];
            faceinfo.ymin = y_lt_mat[k];
            faceinfo.xmax = x_rb_mat[k];
            faceinfo.ymax = y_rb_mat[k];
            faceinfo.score = score_map[k];
            faceinfo.area = (faceinfo.xmax - faceinfo.xmin) * (faceinfo.ymax - faceinfo.ymin);
            bbox_collection.push_back(faceinfo);
        }
    }

    delete[] RF_center_Xs;
    RF_center_Xs = NULL;
    delete[] RF_center_Ys;
    RF_center_Ys = NULL;
    delete[] RF_center_Xs_mat;
    RF_center_Xs_mat = NULL;
    delete[] RF_center_Ys_mat;
    RF_center_Ys_mat = NULL;
    delete[] x_lt_mat;
    x_lt_mat = NULL;
    delete[] y_lt_mat;
    y_lt_mat = NULL;
    delete[] x_rb_mat;
    x_rb_mat = NULL;
    delete[] y_rb_mat;
    y_rb_mat = NULL;
}

void LFFD::get_topk_bbox(std::vector<pr::ObjectBox> &input, std::vector<pr::ObjectBox> &output, int top_k) {
    std::sort(input.begin(), input.end(),
              [](const pr::ObjectBox &a, const pr::ObjectBox &b) {
                  return a.score > b.score;
              });

    if (input.size() > top_k) {
        for (int k = 0; k < top_k; k++) {
            output.push_back(input[k]);
        }
    } else {
        output = input;
    }
}

void LFFD::nms(std::vector<pr::ObjectBox> &input, std::vector<pr::ObjectBox> &output, float threshold, int type) {
    if (input.empty())
        return;

    std::sort(input.begin(), input.end(),
              [](const pr::ObjectBox &a, const pr::ObjectBox &b) {
                  return a.score > b.score;
              });

    int box_num = input.size();

    std::vector<int> merged(box_num, 0);

    for (int i = 0; i < box_num; i++) {
        if (merged[i])
            continue;

        output.push_back(input[i]);

        float h0 = input[i].ymax - input[i].ymin + 1;
        float w0 = input[i].xmax - input[i].xmin + 1;

        float area0 = h0 * w0;


        for (int j = i + 1; j < box_num; j++) {
            if (merged[j])
                continue;

            float inner_x0 = input[i].xmin > input[j].xmin ? input[i].xmin
                                                           : input[j].xmin;//std::max(input[i].xmin, input[j].xmin);
            float inner_y0 = input[i].ymin > input[j].ymin ? input[i].ymin : input[j].ymin;

            float inner_x1 = input[i].xmax < input[j].xmax ? input[i].xmax : input[j].xmax;  //bug fixed ,sorry
            float inner_y1 = input[i].ymax < input[j].ymax ? input[i].ymax : input[j].ymax;

            float inner_h = inner_y1 - inner_y0 + 1;
            float inner_w = inner_x1 - inner_x0 + 1;


            if (inner_h <= 0 || inner_w <= 0)
                continue;

            float inner_area = inner_h * inner_w;

            float h1 = input[j].ymax - input[j].ymin + 1;
            float w1 = input[j].xmax - input[j].xmin + 1;

            float area1 = h1 * w1;

            float score = inner_area / area1;

            if (score > threshold)
                merged[j] = 1;
        }

    }
}
