package com.recogEngine;


import android.graphics.Bitmap;
import android.graphics.Rect;

import com.szOCR.general.RecogResult;

public class RecogEngine {

    public native int initEngine();
    public native int endEngine();
    public native int doRecogBitmap(Bitmap bitmap, int rot, int[] result);
    public native int doRecogGrayImg(byte[] data, int width, int height, int rot, int[] result);

    public RecogEngine() {}

    public RecogResult RecogPhoneNumber_bitmap(Bitmap bitmap, int rot, Rect cropRect) {
        RecogResult rst = null;

        int[] intData = new int[100];
        intData[0] = cropRect.left;
        intData[1] = cropRect.top;
        intData[2] = cropRect.right;
        intData[3] = cropRect.bottom;

        int ret = doRecogBitmap(bitmap, rot, intData);
        if (ret > 0) {
            rst = new RecogResult();
            int i, k = 0;
            int nPhoneNum = intData[k++];
            int num = intData[k++];
            if (num > 11) num = 11;
            for (i = 0; i < num; ++i) {
                rst.m_szNumber[i] = (char) intData[k++];
                rst.m_diffPos[i] = 0;
            }
            rst.m_szNumber[i] = 0;
            rst.m_nRecogDis = (double) intData[k++];
            rst.m_nResultCount = 1;

            if (nPhoneNum == 2) {
                num = intData[k++];
                if (num > 11) num = 11;
                for (i = 0; i < num; ++i) {
                    rst.m_szNumber1[i] = (char) intData[k++];
                    if (rst.m_szNumber[i] != rst.m_szNumber1[i])
                        rst.m_diffPos[i] = 1;
                }
                rst.m_szNumber1[i] = 0;
                rst.m_nResultCount = 2;
            }

        }


        return rst;
    }

    public RecogResult RecogPhoneNumber_data(byte[] data, int width, int height, int rot, Rect cropRect) {
        RecogResult rst = new RecogResult();

        int[] intData = new int[100];
        intData[0] = cropRect.left;
        intData[1] = cropRect.top;
        intData[2] = cropRect.right;
        intData[3] = cropRect.bottom;

        int ret = doRecogGrayImg(data, width, height, rot, intData);
        if (ret > 0) {
            int i, k = 0;
            int nPhoneNum = intData[k++];
            int num = intData[k++];
            if (num > 11) num = 11;
            for (i = 0; i < num; ++i) {
                rst.m_szNumber[i] = (char) intData[k++];
                rst.m_diffPos[i] = 0;
            }
            rst.m_szNumber[i] = 0;
            rst.m_nRecogDis = (double) intData[k++];
            rst.m_nResultCount = 1;

            if (nPhoneNum == 2) {
                num = intData[k++];
                if (num > 11) num = 11;
                for (i = 0; i < num; ++i) {
                    rst.m_szNumber1[i] = (char) intData[k++];
                    if (rst.m_szNumber[i] != rst.m_szNumber1[i])
                        rst.m_diffPos[i] = 1;
                }
                rst.m_szNumber1[i] = 0;
                rst.m_nResultCount = 2;
            }

        } else {
            rst.m_nResultCount = 0;
        }

        return rst;
    }

    static {
        System.loadLibrary("testEngine");
    }
}
