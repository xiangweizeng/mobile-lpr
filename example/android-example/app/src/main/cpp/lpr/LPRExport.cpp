//
// Created by Administrator on 2021/2/16.
//

#include <vector>
#include <string>
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <ncnn/gpu.h>

#include "plate_detectors.h"
#include "plate_recognizers.h"

static pr::PlateDetector detector;
static pr::LPRRecognizer lpr;
static bool fixed = false;

extern "C" JNIEXPORT void JNICALL
Java_com_mobilelpr_demo_MobileLPR_init(_JNIEnv *env, _jclass *clazz, _jobject* assetManager, uint8_t useGPU) {

    if (detector != nullptr) {
        detector = nullptr;
        lpr = nullptr;
    }

    if(!fixed){
        struct AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
        pr::fix_lffd_detector(mgr, "lpr", pr::lffd_float_detector);
        pr::fix_lpr_recognizer(mgr, "lpr", pr::opt_lpr_recognizer);
        fixed = true;
    }

    detector = pr::IPlateDetector::create_plate_detector(pr::lffd_float_detector);
    lpr = pr::opt_lpr_recognizer.create_recognizer();
}

static _jstring *str2jstring(_JNIEnv *env, const char *pat) {
    //定义java String类 strClass
    _jclass *strClass = (env)->FindClass("java/lang/String");
    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
    struct _jmethodID *ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    //建立byte数组
    _jbyteArray *bytes = (env)->NewByteArray(strlen(pat));
    //将char* 转换为byte数组
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (int8_t *) pat);
    // 设置String, 保存语言类型,用于byte数组转换至String时的参数
    _jstring *encoding = (env)->NewStringUTF("UTF-8");
    //将byte数组转换为java String,并输出
    return (_jstring * ) (env)->NewObject(strClass, ctorID, bytes, encoding);
}

extern "C" JNIEXPORT _jobjectArray* JNICALL
Java_com_mobilelpr_demo_MobileLPR_detect(_JNIEnv * env, _jclass * clazz, _jobject * bitmap) {

    AndroidBitmapInfo img_size;
    AndroidBitmap_getInfo(env, bitmap, &img_size);

    int input_width = 0;
    int input_height = 0;
    float width_ratio = (float) img_size.width / (float) pr::lffd_float_detector.width;
    float height_ratio = (float) img_size.height / (float) pr::lffd_float_detector.height;
    float ratio = width_ratio > height_ratio ?  width_ratio : height_ratio;

    if(ratio < 1){
        ratio = 1;
        input_width = img_size.width;
        input_height = img_size.height;
    }else{
        input_width = img_size.width/ratio;
        input_height = img_size.height/ratio;
    }

    ncnn::Mat sample  = ncnn::Mat::from_android_bitmap_resize(env, bitmap,
            ncnn::Mat::PIXEL_RGBA2BGR, input_width, input_height);

    std::vector<pr::PlateInfo> objects;
    detector->plate_detect(sample, objects);
    lpr->decode_plate_infos(objects);

    auto box_cls = env->FindClass("com/mobilelpr/demo/PlateResult");
    auto cid = env->GetMethodID(box_cls, "<init>", "(FFFFLjava/lang/String;F)V");
    jobjectArray ret = env->NewObjectArray(objects.size(), box_cls, nullptr);
    int i = 0;
    for (auto &box: objects) {
        env->PushLocalFrame(1);
        jobject obj = env->NewObject(box_cls, cid, box.bbox.xmin*ratio, box.bbox.ymin*ratio,
                box.bbox.xmax*ratio, box.bbox.ymax*ratio, str2jstring(env, box.plate_no.c_str()), box.bbox.score);
        obj = env->PopLocalFrame(obj);
        env->SetObjectArrayElement(ret, i++, obj);
    }

    return ret;
}