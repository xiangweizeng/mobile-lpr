package com.mobilelpr.demo;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;

import java.util.Locale;
import java.util.Random;

public abstract class IDetector {
    /**
     * run option
     */
    public static class RunOption{
        private double threshold;
        private double nms_threshold;

        public RunOption(double threshold, double nms_threshold) {
            this.threshold = threshold;
            this.nms_threshold = nms_threshold;
        }

        public double getThreshold() {
            return threshold;
        }

        public void setThreshold(double threshold) {
            this.threshold = threshold;
        }

        public double getNms_threshold() {
            return nms_threshold;
        }

        public void setNms_threshold(double nms_threshold) {
            this.nms_threshold = nms_threshold;
        }
    }

    /**
     * get model name
     * @return
     */
    public abstract String getModelName();

    /**
     * init model
     * @param manager
     * @param useGPU
     */
    public abstract void initModel(AssetManager manager, boolean useGPU);

    /**
     * run detector, and draw results
     * @param image
     * @return
     */
    public abstract boolean runDetector(Bitmap image, RunOption option);

    /**
     * show tools
     * @return
     */
    public boolean isShowTools(){
        return false;
    }


    protected Bitmap drawPlateResults(Bitmap mutableBitmap, PlateResult[] results) {
        if (results == null || results.length <= 0) {
            return mutableBitmap;
        }
        Canvas canvas = new Canvas(mutableBitmap);
        final Paint boxPaint = new Paint();
        boxPaint.setAlpha(200);
        boxPaint.setStyle(Paint.Style.STROKE);
        boxPaint.setStrokeWidth(4 * mutableBitmap.getWidth() / 800.0f);
        boxPaint.setTextSize(30 * mutableBitmap.getWidth() / 800.0f);
        for (PlateResult box : results) {
            boxPaint.setColor(box.getColor());
            boxPaint.setStyle(Paint.Style.FILL);

            canvas.drawText(box.getLabel() + String.format(Locale.CHINESE, " %.3f", box.getScore()), box.x0 + 3, box.y0 + 30 * mutableBitmap.getWidth() / 1000.0f, boxPaint);
            boxPaint.setStyle(Paint.Style.STROKE);
            canvas.drawRect(box.getRect(), boxPaint);
        }
        return mutableBitmap;
    }
}
