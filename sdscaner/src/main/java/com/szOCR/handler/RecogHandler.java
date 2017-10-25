package com.szOCR.handler;

import android.graphics.Bitmap;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import com.szOCR.R;
import com.szOCR.activity.ScanView;
import com.szOCR.general.CGlobal;
import com.szOCR.general.RecogResult;

public final class RecogHandler extends Handler {

    private static final String TAG = RecogHandler.class.getSimpleName();

    private final ScanView mActivity;

    RecogHandler(ScanView activity) {
        this.mActivity = activity;
    }

    @Override
    public void handleMessage(Message message) {
        if (message.what == R.id.recog_start) {// Log.d(TAG, "Got decode message");
            decode((byte[]) message.obj, message.arg1, message.arg2);

        } else if (message.what == R.id.quit) {
            Looper.myLooper().quit();

        }
    }

    /**
     * @param data:   The YUV preview frame.
     * @param width:  The width of the preview frame.
     * @param height: The height of the preview frame.
     */
    private void decode(byte[] data, int width, int height) {
        mActivity.setIsAvailable(false);
        int rot = 90;
        long start = System.currentTimeMillis();
        Rect orgRect = CGlobal.getOrgCropRect(width, height, rot, CGlobal.g_rectCrop);
        RecogResult rawResult = CGlobal.myEngine.RecogPhoneNumber_data(data, width, height, rot, orgRect);
        long end = System.currentTimeMillis();
        rawResult.m_nRecogTime = (end - start);
        if (rawResult.m_nResultCount > 0) {

            Log.d("RecogResult - ", rawResult.m_szNumber.toString());
            Bitmap recogBitmap = CGlobal.makeCropedGrayBitmap(data, width, height, rot, orgRect);
            Message message = Message.obtain(mActivity.getHandler(), R.id.recog_succeeded, rawResult);
            Bundle bundle = new Bundle();
            bundle.putParcelable(CGlobal.PHONENUMBER_BITMAP, recogBitmap);
            message.setData(bundle);
            Log.d(TAG, "Sending recog succeeded message...");
            message.sendToTarget();
        } else {
            Message message = Message.obtain(mActivity.getHandler(), R.id.recog_failed);
            message.sendToTarget();
        }
        mActivity.setIsAvailable(true);
    }

}
