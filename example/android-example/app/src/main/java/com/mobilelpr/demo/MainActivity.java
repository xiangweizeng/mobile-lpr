package com.mobilelpr.demo;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.media.ExifInterface;
import android.media.MediaMetadataRetriever;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Rational;
import android.util.Size;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.camera.core.CameraX;
import androidx.camera.core.ImageAnalysis;
import androidx.camera.core.ImageAnalysisConfig;
import androidx.camera.core.ImageProxy;
import androidx.camera.core.Preview;
import androidx.camera.core.PreviewConfig;
import androidx.camera.core.UseCase;
import androidx.core.app.ActivityCompat;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Locale;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;

public class MainActivity extends AppCompatActivity {

    public static boolean USE_GPU = false;
    public static CameraX.LensFacing CAMERA_ID = CameraX.LensFacing.FRONT;
    public static IDetector detector;

    private static final int REQUEST_CAMERA = 1;
    private static final int REQUEST_PICK_IMAGE = 2;
    private static final int REQUEST_PICK_VIDEO = 3;
    private static String[] PERMISSIONS_CAMERA = {
            Manifest.permission.CAMERA
    };
    private Toolbar toolbar;
    private ImageView resultImageView;
    private TextView tvInfo;
    private Button btnPhoto;
    private Button btnVideo;
    private Button btnCamera;
    private TextureView viewFinder;
    private SeekBar sbVideo;
    private SeekBar sbVideoSpeed;

    protected float videoSpeed = 1.0f;
    protected long videoCurFrameLoc = 0;
    public static int VIDEO_SPEED_MAX = 20 + 1;
    public static int VIDEO_SPEED_MIN = 1;

    private AtomicBoolean detectCamera = new AtomicBoolean(false);
    private AtomicBoolean detectPhoto = new AtomicBoolean(false);
    private AtomicBoolean detectVideo = new AtomicBoolean(false);

    private long startTime = 0;
    private long endTime = 0;
    private int width;
    private int height;

    double total_fps = 0;
    int fps_count = 0;

    protected Bitmap mutableBitmap;

    ExecutorService detectService = Executors.newSingleThreadExecutor();

    MediaMetadataRetriever mmr;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        int permission = ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA);
        if (permission != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(
                    this,
                    PERMISSIONS_CAMERA,
                    REQUEST_CAMERA
            );
            finish();
        }

        detector = new MobileLPR();
        detector.initModel(getAssets(), USE_GPU);
        initViewID();
        initViewListener();
    }

    protected void initViewListener() {
        toolbar.setNavigationIcon(R.drawable.actionbar_dark_back_icon);
        toolbar.setNavigationOnClickListener(v -> finish());
        btnPhoto.setOnClickListener(view -> {
            int permission = ActivityCompat.checkSelfPermission(MainActivity.this, Manifest.permission.READ_EXTERNAL_STORAGE);
            if (permission != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(MainActivity.this,
                        new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 777
                );
            } else {
                Intent intent = new Intent(Intent.ACTION_PICK);
                intent.setType("image/*");
                startActivityForResult(intent, REQUEST_PICK_IMAGE);
            }
        });

        btnVideo.setOnClickListener(view -> {
            int permission = ActivityCompat.checkSelfPermission(MainActivity.this, Manifest.permission.READ_EXTERNAL_STORAGE);
            if (permission != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(MainActivity.this,
                        new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 777
                );
            } else {
                Intent intent = new Intent(Intent.ACTION_PICK);
                intent.setType("video/*");
                startActivityForResult(intent, REQUEST_PICK_VIDEO);
            }
        });


        btnCamera.setOnClickListener( view -> {
            CAMERA_ID = CAMERA_ID ==  CameraX.LensFacing.BACK ?  CameraX.LensFacing.FRONT :  CameraX.LensFacing.BACK;
            btnCamera.setText(CAMERA_ID == CameraX.LensFacing.BACK ? "Front" : "Backend");
            detectPhoto.set(false);
            detectVideo.set(false);
            detectCamera.set(false);
            sbVideo.setVisibility(View.GONE);
            sbVideoSpeed.setVisibility(View.GONE);
            startCamera();
        });

        resultImageView.setOnClickListener(v -> {
            if (detectVideo.get() || detectPhoto.get()) {
                detectPhoto.set(false);
                detectVideo.set(false);
                sbVideo.setVisibility(View.GONE);
                sbVideoSpeed.setVisibility(View.GONE);
                startCamera();
            }
        });

        viewFinder.addOnLayoutChangeListener((view, i, i1, i2, i3, i4, i5, i6, i7) -> updateTransform());

        viewFinder.post(() -> startCamera());

        sbVideoSpeed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                videoSpeed = i;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                Toast.makeText(MainActivity.this, "Video Speed:" + seekBar.getProgress(), Toast.LENGTH_SHORT).show();
            }
        });

        sbVideo.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                videoCurFrameLoc = i;
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                videoCurFrameLoc = seekBar.getProgress();
            }
        });
    }

    protected void initViewID() {
        toolbar = findViewById(R.id.tool_bar);
        resultImageView = findViewById(R.id.imageView);
        tvInfo = findViewById(R.id.tv_info);
        btnPhoto = findViewById(R.id.button);
        btnCamera = findViewById(R.id.btn_camera);
        btnVideo = findViewById(R.id.btn_video);
        viewFinder = findViewById(R.id.view_finder);
        sbVideo = findViewById(R.id.sb_video);
        sbVideo.setVisibility(View.GONE);
        sbVideoSpeed = findViewById(R.id.sb_video_speed);
        sbVideoSpeed.setMin(VIDEO_SPEED_MIN);
        sbVideoSpeed.setMax(VIDEO_SPEED_MAX);
        sbVideoSpeed.setVisibility(View.GONE);
    }

    private void updateTransform() {
        Matrix matrix = new Matrix();
        // Compute the center of the view finder
        float centerX = viewFinder.getWidth() / 2f;
        float centerY = viewFinder.getHeight() / 2f;

        float[] rotations = {0, 90, 180, 270};
        // Correct preview output to account for display rotation
        float rotationDegrees = rotations[viewFinder.getDisplay().getRotation()];

        matrix.postRotate(-rotationDegrees, centerX, centerY);

        // Finally, apply transformations to our TextureView
        viewFinder.setTransform(matrix);
    }

    private void startCamera() {
        CameraX.unbindAll();

        // 1. preview
        PreviewConfig previewConfig = new PreviewConfig.Builder()
                .setLensFacing(CAMERA_ID)
                .setTargetAspectRatio(Rational.NEGATIVE_INFINITY)
                .setTargetResolution(new Size(480, 640))
                .build();

        Preview preview = new Preview(previewConfig);
        preview.setOnPreviewOutputUpdateListener(output -> {
            ViewGroup parent = (ViewGroup) viewFinder.getParent();
            parent.removeView(viewFinder);
            parent.addView(viewFinder, 0);

            viewFinder.setSurfaceTexture(output.getSurfaceTexture());
            updateTransform();
        });

        DetectAnalyzer detectAnalyzer = new DetectAnalyzer();
        CameraX.bindToLifecycle(this, preview, gainAnalyzer(detectAnalyzer));
    }


    private UseCase gainAnalyzer(DetectAnalyzer detectAnalyzer) {
        ImageAnalysisConfig.Builder analysisConfigBuilder = new ImageAnalysisConfig.Builder();
        analysisConfigBuilder.setLensFacing(CAMERA_ID);
        analysisConfigBuilder.setImageReaderMode(ImageAnalysis.ImageReaderMode.ACQUIRE_LATEST_IMAGE);
        analysisConfigBuilder.setTargetResolution(new Size(480, 640));
        ImageAnalysisConfig config = analysisConfigBuilder.build();
        ImageAnalysis analysis = new ImageAnalysis(config);
        analysis.setAnalyzer(detectAnalyzer);
        return analysis;
    }

    private Bitmap imageToBitmap(ImageProxy image) {
        byte[] nv21 = imageToNV21(image);

        YuvImage yuvImage = new YuvImage(nv21, ImageFormat.NV21, image.getWidth(), image.getHeight(), null);
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        yuvImage.compressToJpeg(new Rect(0, 0, yuvImage.getWidth(), yuvImage.getHeight()), 100, out);
        byte[] imageBytes = out.toByteArray();
        try {
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
    }

    private byte[] imageToNV21(ImageProxy image) {
        ImageProxy.PlaneProxy[] planes = image.getPlanes();
        ImageProxy.PlaneProxy y = planes[0];
        ImageProxy.PlaneProxy u = planes[1];
        ImageProxy.PlaneProxy v = planes[2];
        ByteBuffer yBuffer = y.getBuffer();
        ByteBuffer uBuffer = u.getBuffer();
        ByteBuffer vBuffer = v.getBuffer();
        int ySize = yBuffer.remaining();
        int uSize = uBuffer.remaining();
        int vSize = vBuffer.remaining();
        byte[] nv21 = new byte[ySize + uSize + vSize];
        // U and V are swapped
        yBuffer.get(nv21, 0, ySize);
        vBuffer.get(nv21, ySize, vSize);
        uBuffer.get(nv21, ySize + vSize, uSize);

        return nv21;
    }

    private class DetectAnalyzer implements ImageAnalysis.Analyzer {

        @Override
        public void analyze(ImageProxy image, final int rotationDegrees) {
            detectOnModel(image, rotationDegrees);
        }
    }


    private void detectOnModel(ImageProxy image, final int rotationDegrees) {
        if (detectCamera.get() || detectPhoto.get() || detectVideo.get()) {
            return;
        }

        detectCamera.set(true);
        startTime = System.currentTimeMillis();
        final Bitmap bitmapSrc = imageToBitmap(image);
        if (detectService == null) {
            detectCamera.set(false);
            return;
        }

        detectService.execute(() -> {
            Matrix matrix = new Matrix();
            matrix.postRotate(rotationDegrees);
            width = bitmapSrc.getWidth();
            height = bitmapSrc.getHeight();
            Bitmap bitmap = Bitmap.createBitmap(bitmapSrc, 0, 0, width, height, matrix, false);

            mutableBitmap = detectAndDraw(bitmap);
            showResultOnUI();
        });
    }

    protected void showResultOnUI() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                detectCamera.set(false);
                resultImageView.setImageBitmap(mutableBitmap);
                endTime = System.currentTimeMillis();
                long dur = endTime - startTime;
                float fps = (float) (1000.0 / dur);
                total_fps = (total_fps == 0) ? fps : (total_fps + fps);
                fps_count++;
                String modelName = getModelName();

                tvInfo.setText(String.format(Locale.CHINESE,
                        "%s\nSize: %dx%d\nTime: %.3f s\nFPS: %.3f\nAVG_FPS: %.3f",
                        modelName, height, width, dur / 1000.0, fps, (float) total_fps / fps_count));
            }
        });
    }

    protected Bitmap detectAndDraw(Bitmap image) {

        IDetector.RunOption runOption = new IDetector.RunOption(0, 0);
        if(!detector.runDetector(image, runOption)){
            detectCamera.set(false);
        }
        return image;
    }

    protected String getModelName() {
        String modelName = detector.getModelName();
        return USE_GPU ? "[ GPU ] " + modelName : "[ CPU ] " + modelName;
    }

    @Override
    protected void onDestroy() {
        detectCamera.set(false);
        detectVideo.set(false);
        if (detectService != null) {
            detectService.shutdown();
            detectService = null;
        }
        if (mmr != null) {
            mmr.release();
        }
        CameraX.unbindAll();
        super.onDestroy();
    }


    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        for (int result : grantResults) {
            if (result != PackageManager.PERMISSION_GRANTED) {
                Toast.makeText(this, "Camera Permission!", Toast.LENGTH_SHORT).show();
                this.finish();
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (data == null) {
            return;
        }
        if (requestCode == REQUEST_PICK_IMAGE) {
            // photo
            runByPhoto(requestCode, resultCode, data);
        } else if (requestCode == REQUEST_PICK_VIDEO) {
            // video
            runByVideo(requestCode, resultCode, data);
        } else {
            Toast.makeText(this, "Error", Toast.LENGTH_SHORT).show();
        }
    }

    public void runByPhoto(int requestCode, int resultCode, @Nullable Intent data) {
        if (resultCode != RESULT_OK || data == null) {
            Toast.makeText(this, "Photo error", Toast.LENGTH_SHORT).show();
            return;
        }
        if (detectVideo.get()) {
            Toast.makeText(this, "Video is running", Toast.LENGTH_SHORT).show();
            return;
        }

        detectPhoto.set(true);
        final Bitmap image = getPicture(data.getData());
        if (image == null) {
            Toast.makeText(this, "Photo is null", Toast.LENGTH_SHORT).show();
            return;
        }

        CameraX.unbindAll();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                long start = System.currentTimeMillis();
                mutableBitmap = image.copy(Bitmap.Config.ARGB_8888, true);
                width = image.getWidth();
                height = image.getHeight();

                mutableBitmap = detectAndDraw(mutableBitmap);
                final long dur = System.currentTimeMillis() - start;
                runOnUiThread(() -> {
                    String modelName = getModelName();
                    resultImageView.setImageBitmap(mutableBitmap);
                    tvInfo.setText(String.format(Locale.CHINESE, "%s\nSize: %dx%d\nTime: %.3f s\nFPS: %.3f",
                            modelName, height, width, dur / 1000.0, 1000.0f / dur));
                });
            }
        }, "photo detect");
        thread.start();
    }

    public void runByVideo(int requestCode, int resultCode, @Nullable Intent data) {
        if (resultCode != RESULT_OK || data == null) {
            Toast.makeText(this, "Video error", Toast.LENGTH_SHORT).show();
            return;
        }

        try {
            Uri uri = data.getData();
            detectOnVideo(uri);
        } catch (Exception e) {
            e.printStackTrace();
            Toast.makeText(this, "Video is null", Toast.LENGTH_SHORT).show();
        }
    }

    public void detectOnVideo(final Uri path) {
        if (detectVideo.get()) {
            Toast.makeText(this, "Video is running", Toast.LENGTH_SHORT).show();
            return;
        }

        detectVideo.set(true);
        Toast.makeText(MainActivity.this, "FPS is not accurate!", Toast.LENGTH_SHORT).show();
        sbVideo.setVisibility(View.VISIBLE);
        sbVideoSpeed.setVisibility(View.VISIBLE);


        CameraX.unbindAll();
        Thread thread = new Thread(() -> {
            mmr = new MediaMetadataRetriever();
            mmr.setDataSource(UriTools.getFilePathByUri(getApplicationContext(), path));

            String dur = mmr.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION);  // ms
            String sfps = mmr.extractMetadata(MediaMetadataRetriever.METADATA_KEY_CAPTURE_FRAMERATE);  // fps
            String total = mmr.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_FRAME_COUNT);  // frame count
            String rota = mmr.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_ROTATION);  // rotation

            float fps = 25;
            int duration = Integer.parseInt(dur);
            if(null != sfps) {
                fps = Float.parseFloat(sfps);
            }else if(null != total){
                fps = Float.parseFloat(total) / (duration / 1000.f);
            }

            float rotate = 0;
            if (rota != null) {
                rotate = Float.parseFloat(rota);
            }

            sbVideo.setMax(duration * 1000);
            float frameDis = 1.0f / fps * 1000 * 1000 * videoSpeed;
            videoCurFrameLoc = 0;
            while (detectVideo.get() && (videoCurFrameLoc) < (duration * 1000)) {
                videoCurFrameLoc = (long) (videoCurFrameLoc + frameDis);
                sbVideo.setProgress((int) videoCurFrameLoc);
                final Bitmap b = mmr.getFrameAtTime(videoCurFrameLoc, MediaMetadataRetriever.OPTION_CLOSEST);
                if (b == null) {
                    continue;
                }

                Matrix matrix = new Matrix();
                matrix.postRotate(rotate);
                width = b.getWidth();
                height = b.getHeight();
                final Bitmap bitmap = Bitmap.createBitmap(b, 0, 0, width, height, matrix, false);
                startTime = System.currentTimeMillis();
                mutableBitmap = detectAndDraw(bitmap.copy(Bitmap.Config.ARGB_8888, true));
                showResultOnUI();
                frameDis = 1.0f / fps * 1000 * 1000 * videoSpeed;
            }
            
            mmr.release();
            if (detectVideo.get()) {
                runOnUiThread(() -> {
                    sbVideo.setVisibility(View.GONE);
                    sbVideoSpeed.setVisibility(View.GONE);
                    Toast.makeText(MainActivity.this, "Video end!", Toast.LENGTH_LONG).show();
                });
            }
            detectVideo.set(false);
        }, "video detect");
        thread.start();
    }

    public Bitmap getPicture(Uri selectedImage) {
        String[] filePathColumn = {MediaStore.Images.Media.DATA};
        Cursor cursor = this.getContentResolver().query(selectedImage, filePathColumn, null, null, null);
        if (cursor == null) {
            return null;
        }
        cursor.moveToFirst();
        int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
        String picturePath = cursor.getString(columnIndex);
        cursor.close();
        Bitmap bitmap = BitmapFactory.decodeFile(picturePath);
        if (bitmap == null) {
            return null;
        }
        int rotate = readPictureDegree(picturePath);
        return rotateBitmapByDegree(bitmap, rotate);
    }

    public int readPictureDegree(String path) {
        int degree = 0;
        try {
            ExifInterface exifInterface = new ExifInterface(path);
            int orientation = exifInterface.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);
            switch (orientation) {
                case ExifInterface.ORIENTATION_ROTATE_90:
                    degree = 90;
                    break;
                case ExifInterface.ORIENTATION_ROTATE_180:
                    degree = 180;
                    break;
                case ExifInterface.ORIENTATION_ROTATE_270:
                    degree = 270;
                    break;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return degree;
    }

    public Bitmap rotateBitmapByDegree(Bitmap bm, int degree) {
        Bitmap returnBm = null;
        Matrix matrix = new Matrix();
        matrix.postRotate(degree);
        try {
            returnBm = Bitmap.createBitmap(bm, 0, 0, bm.getWidth(),
                    bm.getHeight(), matrix, true);
        } catch (OutOfMemoryError e) {
            e.printStackTrace();
        }
        if (returnBm == null) {
            returnBm = bm;
        }
        if (bm != returnBm) {
            bm.recycle();
        }
        return returnBm;
    }
}
