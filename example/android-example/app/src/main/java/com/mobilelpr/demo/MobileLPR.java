package com.mobilelpr.demo;

import android.content.res.AssetManager;
import android.graphics.Bitmap;

public class MobileLPR extends IDetector {
    static {
        System.loadLibrary("example");
    }

    public static native void init(AssetManager manager, boolean useGPU);
    public static native PlateResult[] detect(Bitmap bitmap);

    @Override
    public String getModelName() {
        return "MobileLPR";
    }

    @Override
    public void initModel(AssetManager manager, boolean useGPU) {
        init(manager, useGPU);
    }

    @Override
    public boolean runDetector(Bitmap image, RunOption option) {
        PlateResult[] results = detect(image);
        if(null == results){
            return false;
        }

        drawPlateResults(image, results);
        return true;
    }
}
