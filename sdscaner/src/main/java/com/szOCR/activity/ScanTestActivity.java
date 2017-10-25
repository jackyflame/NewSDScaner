package com.szOCR.activity;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.hardware.Camera;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.os.Vibrator;
import android.support.v7.app.AppCompatActivity;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.ForegroundColorSpan;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;
import android.widget.TextView;

import com.recogEngine.RecogEngine;
import com.szOCR.R;
import com.szOCR.camera.CameraPreview;
import com.szOCR.general.CGlobal;
import com.szOCR.general.RecogResult;
import com.szOCR.handler.ScanHandler;

public class ScanTestActivity extends AppCompatActivity implements SensorEventListener, OnClickListener, ScanView {

    private CameraPreview mCameraPreview;
    private RelativeLayout mHomeLayout;
    private Vibrator vibrator;

    private ImageView mImageView;
    private EditText mEditPhoneNumber1;
    private EditText mEditPhoneNumber2;
    private Button mBtnScanStart;
    private Button mBtnSaveImage;
    private TextView mTxtViewPreviewSize;
    private TextView mTxtViewRecogTime;

    public ScanHandler m_scanHandler;
    PowerManager.WakeLock wakeLock;

    private long lastTime;
    private float lastX;
    private float lastY;
    private float lastZ;
    private SensorManager sensorManager;
    private Sensor accelerormeterSensor;

    public boolean bIsAvailable;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scan_test);

        try {
            if (CGlobal.myEngine == null) {
                CGlobal.myEngine = new RecogEngine();
                CGlobal.myEngine.initEngine();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        bIsAvailable = true;
        mHomeLayout = (RelativeLayout) findViewById(R.id.previewLayout);
        mImageView = (ImageView) findViewById(R.id.imageView1);
        mEditPhoneNumber1 = (EditText) findViewById(R.id.editPhoneNumber1);
        mEditPhoneNumber2 = (EditText) findViewById(R.id.editPhoneNumber2);
        mBtnScanStart = (Button) findViewById(R.id.btnScanstart);
        mBtnSaveImage = (Button) findViewById(R.id.btnSaveImage);
        mTxtViewPreviewSize = (TextView) findViewById(R.id.txtViewPreviewSize);
        mTxtViewRecogTime = (TextView) findViewById(R.id.txtViewRecogTime);

        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        accelerormeterSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

        vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
        wakeLock = ((PowerManager) getSystemService(POWER_SERVICE))
                .newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK | PowerManager.ON_AFTER_RELEASE, "WakeLockActivity");

    }

    public Handler getHandler() {
        return m_scanHandler;
    }

    @Override
    protected void onResume() {
        super.onResume();

        mCameraPreview = new CameraPreview(this, 0, CameraPreview.LayoutMode.FitToParent);
        LayoutParams previewLayoutParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
        //previewLayoutParams.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
        mHomeLayout.addView(mCameraPreview, 0, previewLayoutParams);

        if (accelerormeterSensor != null)
            sensorManager.registerListener(this, accelerormeterSensor, SensorManager.SENSOR_DELAY_GAME);


        if (m_scanHandler == null)
            m_scanHandler = new ScanHandler(this, mCameraPreview);
        m_scanHandler.sendEmptyMessageDelayed(R.id.auto_focus, 1000);


        if (wakeLock != null)
            wakeLock.acquire();
        ShowResultCtrls(false);
        mBtnScanStart.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                ShowResultCtrls(false);
                bIsAvailable = true;
                m_scanHandler.sendEmptyMessage(R.id.restart_preview);
            }
        });
        mBtnSaveImage.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                CGlobal.SaveRecogImage("1.jpg", CGlobal.g_bitmapPhoneNumber);
                bIsAvailable = true;
                m_scanHandler.sendEmptyMessage(R.id.restart_preview);
            }
        });
    }

    @Override
    protected void onPause() {
        super.onPause();
        mCameraPreview.cancelAutoFocus();
        if (m_scanHandler != null) {
            m_scanHandler.quitSynchronously();
            m_scanHandler = null;
        }

        mCameraPreview.stop();
        mHomeLayout.removeView(mCameraPreview); // This is necessary.
        mCameraPreview = null;

        if (sensorManager != null)
            sensorManager.unregisterListener(this);

        if (wakeLock != null) {
            wakeLock.release();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (CGlobal.myEngine != null) {
            CGlobal.myEngine.endEngine();
            CGlobal.myEngine = null;
        }
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {

            long currentTime = System.currentTimeMillis();
            long gabOfTime = (currentTime - lastTime);
            if (gabOfTime > 1000 && bIsAvailable == true) {
                lastTime = currentTime;
                // TODO Auto-generated method stub
                float x = event.values[0];
                float y = event.values[1];
                float z = event.values[2];

                if (!mCameraPreview.bInitialized) {
                    lastX = x;
                    lastY = y;
                    lastZ = z;
                    mCameraPreview.bInitialized = true;
                }
                float deltaX = Math.abs(lastX - x);
                float deltaY = Math.abs(lastY - y);
                float deltaZ = Math.abs(lastZ - z);

                if (mCameraPreview.mCamera != null && deltaX > .2 && !mCameraPreview.bIsStateAutoFocusing) { //AUTOFOCUS (while it is not autofocusing)
                    mCameraPreview.autoCameraFocuse();
                }
                if (mCameraPreview.mCamera != null && deltaY > .2 && !mCameraPreview.bIsStateAutoFocusing) { //AUTOFOCUS (while it is not autofocusing)
                    mCameraPreview.autoCameraFocuse();

                }
                if (mCameraPreview.mCamera != null && deltaZ > 0.2 && !mCameraPreview.bIsStateAutoFocusing) { //AUTOFOCUS (while it is not autofocusing) */
                    mCameraPreview.autoCameraFocuse();

                }

                lastX = x;
                lastY = y;
                lastZ = z;

            }
        }
    }

    public void setAndshowPreviewSize() {
        Camera.Size previewSize = mCameraPreview.getPreviewSize();
        String strPreviewSize = String.valueOf(previewSize.width) + " x " + String.valueOf(previewSize.height);
        mTxtViewPreviewSize.setText(strPreviewSize);
    }

    @Override
    public Activity getActivityContext() {
        return this;
    }

    public void returnRecogedData(RecogResult result, Bitmap bmImage) {
        //playBeepSoundAndVibrate();
        if (vibrator != null)
            vibrator.vibrate(200L);
        CGlobal.g_RecogResult = result;
        CGlobal.g_bitmapPhoneNumber = bmImage;
        mImageView.setImageBitmap(bmImage);
        String strNumber1 = CGlobal.MakePhoneNumberTypeString(result.m_szNumber);
        mEditPhoneNumber1.setText(strNumber1);
        if (result.m_nResultCount == 1) {
            mEditPhoneNumber2.setText("");
        } else {
            int i, i1;

            mEditPhoneNumber2.setText("");
            String strNumber2 = CGlobal.MakePhoneNumberTypeString(result.m_szNumber1);
            SpannableString spanString = new SpannableString(strNumber2);
            for (i = 0; i < 11; i++) {
                if (result.m_diffPos[i] == 1) {
                    if (i < 3) i1 = i;
                    else if (i < 7) i1 = i + 1;
                    else i1 = i + 2;
                    ForegroundColorSpan span = new ForegroundColorSpan(Color.RED);
                    spanString.setSpan(span, i1, i1 + 1, Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
                }
                //else							text = text + "<font color='red'>result.m_szNumber1[i]</font>";
            }
            mEditPhoneNumber2.append(spanString);
        }
        mTxtViewRecogTime.setText(String.valueOf(result.m_nRecogTime));
        ShowResultCtrls(true);

    }

    @Override
    public void setIsAvailable(boolean isAvailable) {
        this.bIsAvailable = isAvailable;
    }

    @Override
    public boolean isAvailable() {
        return bIsAvailable;
    }

    private void ShowResultCtrls(boolean bShow) {
        if (bShow == false) {
            mImageView.setVisibility(View.GONE);
            mEditPhoneNumber1.setVisibility(View.GONE);
            mEditPhoneNumber2.setVisibility(View.GONE);
            mBtnScanStart.setVisibility(View.GONE);
            mBtnSaveImage.setVisibility(View.GONE);
            bIsAvailable = true;

        } else {
            mImageView.setVisibility(View.VISIBLE);
            mEditPhoneNumber1.setVisibility(View.VISIBLE);
            mEditPhoneNumber2.setVisibility(View.VISIBLE);
            mBtnScanStart.setVisibility(View.VISIBLE);
            mBtnSaveImage.setVisibility(View.VISIBLE);
            bIsAvailable = false;
        }
    }

    @Override
    public void onAccuracyChanged(Sensor arg0, int arg1) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onClick(View arg0) {
        // TODO Auto-generated method stub
        if (mCameraPreview != null) {
            mCameraPreview.autoCameraFocuse();
        }
    }
}