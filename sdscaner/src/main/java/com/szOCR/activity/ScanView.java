package com.szOCR.activity;

import android.app.Activity;
import android.graphics.Bitmap;
import android.os.Handler;

import com.szOCR.general.RecogResult;

/**
 * Created by Android Studio.
 * ProjectName: OcrLib
 * Author: haozi
 * Date: 2017/7/24
 * Time: 15:25
 */

public interface ScanView{

    /**
     * handler
     * */
    Handler getHandler();

    /**
     * 识别结构反馈
     * @param result
     * @param bmImage
     * */
    void returnRecogedData(RecogResult result, Bitmap bmImage);

    /**
     * 页面是否可用
     * @param isAvailable
     * */
    void setIsAvailable(boolean isAvailable);

    boolean isAvailable();

    void setAndshowPreviewSize();

    Activity getActivityContext();
}
