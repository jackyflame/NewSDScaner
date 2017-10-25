package com.szOCR.camera;

import android.content.res.Configuration;
import android.graphics.Point;
import android.graphics.Rect;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;
import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.RelativeLayout;
import android.widget.Toast;

import com.szOCR.activity.ScanView;
import com.szOCR.general.CGlobal;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * This class assumes the parent layout is RelativeLayout.LayoutParams.
 */
public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback, Camera.AutoFocusCallback, PreviewCallback {
    private static boolean DEBUGGING = true;
    private static final String LOG_TAG = "CameraPreviewSample";
    private static final String CAMERA_PARAM_ORIENTATION = "orientation";
    private static final String CAMERA_PARAM_LANDSCAPE = "landscape";
    private static final String CAMERA_PARAM_PORTRAIT = "portrait";
    protected ScanView mActivity;

    private SurfaceHolder mHolder;
    public Camera mCamera;
    protected List<Size> mPreviewSizeList;
    protected List<Size> mPictureSizeList;
    protected Size mPreviewSize;
    protected Size mPictureSize;
    private int mSurfaceChangedCallDepth = 0;
    private int mCameraId;
    private LayoutMode mLayoutMode;
    private int mCenterPosX = -1;
    private int mCenterPosY;
    PreviewReadyCallback mPreviewReadyCallback = null;

    public boolean bIsFocusSuccessed = false;

    public int nLayoutWidth = 0;
    public int nLayoutHeight = 0;

    private boolean mbHasSurface;

    private Handler previewHandler;
    private Handler autofocusHandler;
    private int previewMessage;
    private int autofocusMessage;
    private static final long AUTOFOCUS_INTERVAL_MS = 1500L;

    public boolean bIsRecoging = true;
    public boolean bIsCameraReleased = false;
    public boolean bInitialized = false;
    public boolean bIsStateAutoFocusing = false;

    public static enum LayoutMode {
        FitToParent, // Scale to the size that no side is larger than the parent
        NoBlank // Scale to the size that no side is smaller than the parent
    }

    ;

    public interface PreviewReadyCallback {
        public void onPreviewReady();
    }

    /**
     * State flag: true when surface's layout size is set and surfaceChanged()
     * process has not been completed.
     */
    protected boolean mSurfaceConfiguring = false;

    public CameraPreview(ScanView activity, int cameraId, LayoutMode mode) {
        super(activity.getActivityContext()); // Always necessary
        mActivity = activity;
        mLayoutMode = mode;
        mHolder = getHolder();
        mHolder.addCallback(this);
        mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD) {
            if (Camera.getNumberOfCameras() > cameraId) {
                mCameraId = cameraId;
            } else {
                mCameraId = 0;
            }
        } else {
            mCameraId = 0;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD) {
            mCamera = Camera.open(mCameraId);
        } else {
            mCamera = Camera.open();
        }
        Camera.Parameters cameraParams = mCamera.getParameters();
        mPreviewSizeList = cameraParams.getSupportedPreviewSizes();
        mPictureSizeList = cameraParams.getSupportedPictureSizes();
        mbHasSurface = false;
        bIsCameraReleased = true;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        try {
            mCamera.setPreviewDisplay(mHolder);
            if (mbHasSurface == false) {
                mbHasSurface = true;
            }

        } catch (IOException e) {
            //mCamera.setPreviewCallback(null);
            mCamera.release();
            mCamera = null;
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        stop();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

        int gap = width / 16;
        int hei = (int) ((width - gap * 2) * 45 / 100);
        CGlobal.g_rectPreview = new Rect(gap, gap, width - gap, hei);
        CGlobal.g_screenSize = new Point(width, height);

        mSurfaceChangedCallDepth++;
        doSurfaceChanged(width, height);
        mSurfaceChangedCallDepth--;

    }

    private void doSurfaceChanged(int width, int height) {
        mCamera.stopPreview();

        Camera.Parameters cameraParams = mCamera.getParameters();

        submitFocusAreaRect(cameraParams);

        boolean portrait = isPortrait();

        // The code in this if-statement is prevented from executed again when surfaceChanged is
        // called again due to the change of the layout size in this if-statement.
        if (!mSurfaceConfiguring) {
            Size previewSize = determinePreviewSize(portrait, width, height);
            Size pictureSize = determinePictureSize(previewSize);
            if (DEBUGGING) {
                Log.v(LOG_TAG, "Desired Preview Size - w: " + width + ", h: " + height);
            }
            mPreviewSize = previewSize;
            mPictureSize = pictureSize;
            mSurfaceConfiguring = adjustSurfaceLayoutSize(previewSize, portrait, width, height);

            double wrate = (double) mPreviewSize.width / CGlobal.g_screenSize.y;

            int gap = (int) (CGlobal.g_rectPreview.left * wrate);
            int hei = (int) ((mPreviewSize.height - gap * 2) * 45 / 100);
            CGlobal.g_rectCrop = new Rect(gap, gap, mPreviewSize.height - gap, hei);

            mActivity.setAndshowPreviewSize();

            // Continue executing this method if this method is called recursively.
            // Recursive call of surfaceChanged is very special case, which is a path from
            // the catch clause at the end of this method.
            // The later part of this method should be executed as well in the recursive
            // invocation of this method, because the layout change made in this recursive
            // call will not trigger another invocation of this method.
            if (mSurfaceConfiguring && (mSurfaceChangedCallDepth <= 1)) {
                mCamera.startPreview();
                //setPreviewCallback(this);
                mCamera.setOneShotPreviewCallback(this);
                return;
            }
        }

        configureCameraParameters(cameraParams, portrait);
        mSurfaceConfiguring = false;

        try {
            mCamera.startPreview();
            //setPreviewCallback(this);
            mCamera.setOneShotPreviewCallback(this);
        } catch (Exception e) {
            Log.w(LOG_TAG, "Failed to start preview: " + e.getMessage());

            // Remove failed size
            mPreviewSizeList.remove(mPreviewSize);
            mPreviewSize = null;

            // Reconfigure
            if (mPreviewSizeList.size() > 0) { // prevent infinite loop
                surfaceChanged(null, 0, width, height);
            } else {
                Toast.makeText(mActivity.getActivityContext(), "Can't start preview", Toast.LENGTH_LONG).show();
                Log.w(LOG_TAG, "Gave up starting preview");
            }
        }

        if (null != mPreviewReadyCallback) {
            mPreviewReadyCallback.onPreviewReady();
        }
    }

    private void submitFocusAreaRect(Camera.Parameters cameraParameters) {
        if (cameraParameters.getMaxNumFocusAreas() == 0) {
            return;
        }

        // Convert from View's width and height to +/- 1000

        Rect focusArea = CGlobal.g_rectPreview;

        // Submit focus area to camera

        ArrayList<Camera.Area> focusAreas = new ArrayList<Camera.Area>();
        if (focusAreas.size() > 0) {
            focusAreas.add(new Camera.Area(focusArea, 1000));
            cameraParameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
            cameraParameters.setFocusAreas(focusAreas);
            mCamera.setParameters(cameraParameters);
        }

        // Start the autofocus operation
    }

    /**
     * @param portrait
     * @param reqWidth     must be the value of the parameter passed in surfaceChanged
     * @param reqHeight    must be the value of the parameter passed in surfaceChanged
     * @return Camera.Size object that is an element of the list returned from Camera.Parameters.getSupportedPreviewSizes.
     */
    protected Size determinePreviewSize(boolean portrait, int reqWidth, int reqHeight) {
        // Meaning of width and height is switched for preview when portrait,
        // while it is the same as user's view for surface and metrics.
        // That is, width must always be larger than height for setPreviewSize.
        int reqPreviewWidth; // requested width in terms of camera hardware
        int reqPreviewHeight; // requested height in terms of camera hardware
        if (portrait) {
            reqPreviewWidth = reqHeight;
            reqPreviewHeight = reqWidth;
        } else {
            reqPreviewWidth = reqWidth;
            reqPreviewHeight = reqHeight;
        }

        if (DEBUGGING) {
            Log.v(LOG_TAG, "Listing all supported preview sizes");
            for (Size size : mPreviewSizeList) {
                Log.v(LOG_TAG, "  w: " + size.width + ", h: " + size.height);
            }
            Log.v(LOG_TAG, "Listing all supported picture sizes");
            for (Size size : mPictureSizeList) {
                Log.v(LOG_TAG, "  w: " + size.width + ", h: " + size.height);
            }
        }

        // Adjust surface size with the closest aspect-ratio
        double reqRatio = ((double) 800);//640);//1280);//reqPreviewWidth);// ((double) reqPreviewWidth) / reqPreviewHeight;
        double curRatio, deltaRatio;
        double deltaRatioMin = Float.MAX_VALUE;
        Size retSize = null;
        for (Size size : mPreviewSizeList) {
            //curRatio = ((double) size.width) / size.height;
            //deltaRatio = Math.abs(reqRatio - curRatio);
            curRatio = ((double) size.width);
            deltaRatio = Math.abs(reqRatio - curRatio);
            if (deltaRatio < deltaRatioMin) {
                deltaRatioMin = deltaRatio;
                retSize = size;
            }
        }
//        float reqRatio = ((float) reqPreviewWidth) / reqPreviewHeight;
//        float curRatio, deltaRatio;
//        float deltaRatioMin = Float.MAX_VALUE;
//        Camera.Size retSize = null;
//        for (Camera.Size size : mPreviewSizeList) {
//            curRatio = ((float) size.width) / size.height;
//            deltaRatio = Math.abs(reqRatio - curRatio);
//            if (deltaRatio < deltaRatioMin) {
//                deltaRatioMin = deltaRatio;
//                retSize = size;
//            }
//        }

        return retSize;
    }

    protected Size determinePictureSize(Size previewSize) {
        Size retSize = null;
        for (Size size : mPictureSizeList) {
            if (size.equals(previewSize)) {
                return size;
            }
        }

        if (DEBUGGING) {
            Log.v(LOG_TAG, "Same picture size not found.");
        }

        // if the preview size is not supported as a picture size
        float reqRatio = ((float) previewSize.width) / previewSize.height;
        float curRatio, deltaRatio;
        float deltaRatioMin = Float.MAX_VALUE;
        for (Size size : mPictureSizeList) {
            curRatio = ((float) size.width) / size.height;
            deltaRatio = Math.abs(reqRatio - curRatio);
            if (deltaRatio < deltaRatioMin) {
                deltaRatioMin = deltaRatio;
                retSize = size;
            }
        }

        return retSize;
    }

    protected boolean adjustSurfaceLayoutSize(Size previewSize, boolean portrait,
                                              int availableWidth, int availableHeight) {
        float tmpLayoutHeight, tmpLayoutWidth;
        if (portrait) {
            tmpLayoutHeight = previewSize.width;
            tmpLayoutWidth = previewSize.height;
        } else {
            tmpLayoutHeight = previewSize.height;
            tmpLayoutWidth = previewSize.width;
        }

        float factH, factW, fact;
        factH = availableHeight / tmpLayoutHeight;
        factW = availableWidth / tmpLayoutWidth;
        if (mLayoutMode == LayoutMode.FitToParent) {
            // Select smaller factor, because the surface cannot be set to the size larger than display metrics.
            if (factH < factW) {
                fact = factH;
            } else {
                fact = factW;
            }
        } else {
            if (factH < factW) {
                fact = factW;
            } else {
                fact = factH;
            }
        }

        RelativeLayout.LayoutParams layoutParams = (RelativeLayout.LayoutParams) this.getLayoutParams();

        int layoutHeight = (int) (tmpLayoutHeight * fact);
        int layoutWidth = (int) (tmpLayoutWidth * fact);
        if (DEBUGGING) {
            Log.v(LOG_TAG, "Preview Layout Size - w: " + layoutWidth + ", h: " + layoutHeight);
            Log.v(LOG_TAG, "Scale factor: " + fact);
        }

        boolean layoutChanged;
        if ((layoutWidth != this.getWidth()) || (layoutHeight != this.getHeight())) {
            layoutParams.height = layoutHeight;
            layoutParams.width = layoutWidth;
            if (mCenterPosX >= 0) {
                layoutParams.topMargin = mCenterPosY - (layoutHeight / 2);
                layoutParams.leftMargin = mCenterPosX - (layoutWidth / 2);
            }
            this.setLayoutParams(layoutParams); // this will trigger another surfaceChanged invocation.
            layoutChanged = true;
        } else {
            layoutChanged = false;
        }

        nLayoutWidth = layoutWidth;
        nLayoutHeight = layoutHeight;

        return layoutChanged;
    }

    /**
     * @param x X coordinate of center position on the screen. Set to negative value to unset.
     * @param y Y coordinate of center position on the screen.
     */
    public void setCenterPosition(int x, int y) {
        mCenterPosX = x;
        mCenterPosY = y;
    }

    protected void configureCameraParameters(Camera.Parameters cameraParams, boolean portrait) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.FROYO) { // for 2.1 and before
            if (portrait) {
                cameraParams.set(CAMERA_PARAM_ORIENTATION, CAMERA_PARAM_PORTRAIT);
            } else {
                cameraParams.set(CAMERA_PARAM_ORIENTATION, CAMERA_PARAM_LANDSCAPE);
            }
        } else { // for 2.2 and later
            int angle;
            Display display = mActivity.getActivityContext().getWindowManager().getDefaultDisplay();
            switch (display.getRotation()) {
                case Surface.ROTATION_0: // This is display orientation
                    angle = 90; // This is camera orientation
                    break;
                case Surface.ROTATION_90:
                    angle = 0;
                    break;
                case Surface.ROTATION_180:
                    angle = 270;
                    break;
                case Surface.ROTATION_270:
                    angle = 180;
                    break;
                default:
                    angle = 90;
                    break;
            }
            Log.v(LOG_TAG, "angle: " + angle);

            mCamera.setDisplayOrientation(90);
        }

        cameraParams.setPreviewSize(mPreviewSize.width, mPreviewSize.height);
        cameraParams.setPictureSize(mPictureSize.width, mPictureSize.height);
        if (DEBUGGING) {
            Log.v(LOG_TAG, "Preview Actual Size - w: " + mPreviewSize.width + ", h: " + mPreviewSize.height);
            Log.v(LOG_TAG, "Picture Actual Size - w: " + mPictureSize.width + ", h: " + mPictureSize.height);
        }

        cameraParams.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);

        int maxZoom = cameraParams.getMaxZoom();
        int curZoom = cameraParams.getZoom();

        if (cameraParams.isZoomSupported()) {
            int zoom = (maxZoom * 3) / 10;
            if (zoom < maxZoom && zoom > 0) {
                cameraParams.setZoom(zoom);
            }
        }
        cameraParams.setWhiteBalance(Camera.Parameters.WHITE_BALANCE_AUTO);
        mCamera.setParameters(cameraParams);

    }

    public void stop() {
        if (null == mCamera) {
            return;
        }
        stopPreview();
        mCamera.release();
        mCamera = null;
        mbHasSurface = false;
    }

    public void stopCamera() {
        if (null == mCamera) {
            return;
        }
        stopPreview();
    }

    public boolean isPortrait() {
        return (mActivity.getActivityContext().getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT);
    }

    public void setOneShotPreviewCallback(PreviewCallback callback) {
        if (null == mCamera) {
            return;
        }
        mCamera.setOneShotPreviewCallback(callback);
    }

    public void setPreviewCallback(PreviewCallback callback) {
        if (null == mCamera) {
            return;
        }
        //mCamera.setPreviewCallback(callback);
        mCamera.setOneShotPreviewCallback(this);
    }

    public Size getPreviewSize() {
        return mPreviewSize;
    }


    public void setOnPreviewReady(PreviewReadyCallback cb) {
        mPreviewReadyCallback = cb;
    }

    public void stopPreview() {
        if (mCamera != null) {
            mCamera.stopPreview();
            setPreviewHandler(null, 0);
            setAutofocusHandler(null, 0);
            //previewing = false;
        }
    }

    public void autoCameraFocuse() {
        CGlobal.g_bAutoFocused = false;
        bIsStateAutoFocusing = true;
        mCamera.autoFocus(this);
        //imgAim.setImageResource(R.drawable.target_red);
    }

    @Override
    public void onAutoFocus(boolean success, Camera camera) {
        // TODO Auto-generated method stub

        bIsStateAutoFocusing = false;
        // TODO Auto-generated method stub
        if (success == true) {
            CGlobal.g_bAutoFocused = true;
            //imgAim.setImageResource(R.drawable.target);
            //restartPreviewFrame();
        } else {
            //autoCameraFocuse();
        }
    }

    @Override
    public void onPreviewFrame(byte[] arg0, Camera arg1) {
        if (arg0 == null || arg1 == null) {
            Log.w(LOG_TAG, "frame is null! skipping");
            return;
        }

        //ScanActivity act = (ScanActivity)mActivity;

        if (mCamera == null)
            return;


        int nWidth = mCamera.getParameters().getPreviewSize().width;
        int nHeight = mCamera.getParameters().getPreviewSize().height;
        //act.onRecogFromFrame(arg0, nWidth, nHeight);

        if (previewHandler != null) {
            //if (bIsAvailable == true && CGlobal.g_bAutoFocused && !bIsStateAutoFocusing)
            if (mActivity.isAvailable() == true)// && CGlobal.g_bAutoFocused && !bIsStateAutoFocusing)
            {
                //Log.d(LOG_TAG, " preview callback");
                Message message = previewHandler.obtainMessage(previewMessage, nWidth, nHeight, arg0);
                message.sendToTarget();
                previewHandler = null;
            }
        } else {
            Log.d(LOG_TAG, "Got preview callback, but no handler for it");
        }
    }

    public void cancelAutoFocus() {
        if (mCamera != null)
            mCamera.cancelAutoFocus();
    }

    void setPreviewHandler(Handler previewHandler, int previewMessage) {
        this.previewHandler = previewHandler;
        this.previewMessage = previewMessage;
    }

    void setAutofocusHandler(Handler autofocusHandler, int autofocusMessage) {
        this.autofocusHandler = autofocusHandler;
        this.autofocusMessage = autofocusMessage;
    }

    public void requestPreviewFrame(Handler handler, int message) {
        if (mCamera != null) {
            setPreviewHandler(handler, message);
            mCamera.setOneShotPreviewCallback(this);
        }
    }

    public void requestAutoFocus(Handler handler, int message) {
        if (mCamera != null) {
            setAutofocusHandler(handler, message);
            mCamera.autoFocus(this);
        }
    }
}
