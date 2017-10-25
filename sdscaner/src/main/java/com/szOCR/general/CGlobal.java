package com.szOCR.general;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.graphics.Rect;
import android.os.Environment;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.recogEngine.RecogEngine;

import java.io.File;
import java.io.FileOutputStream;

public class CGlobal {

    public static RecogEngine myEngine = null;
    public static RecogResult g_RecogResult = new RecogResult();

    public static Point g_screenSize;
    public static Rect g_rectPreview = null;
    public static Rect g_rectCrop = null;
    public static Bitmap g_bitmapPhoneNumber = null;

    public static boolean g_bAutoFocused = false;

    public static final String PHONENUMBER_BITMAP = "phonenumber_bitmap";

    public static String MakePhoneNumberTypeString(char[] szNumber) {
        String full = String.valueOf(szNumber);
        String typeStr = full.substring(0, 3) + "-" + full.substring(3, 7) + "-" + full.substring(7, 11);
        return typeStr;
    }

    public static Rect GetRotateRect(Rect orgRect, int rot) {
        Rect rotRect = new Rect();
        if (rot == 0) rotRect.set(orgRect);
        else if (rot == 90) {
            rotRect.left = orgRect.top;
            rotRect.top = orgRect.left;
            rotRect.right = orgRect.bottom;
            rotRect.bottom = orgRect.right;
        }
        return rotRect;
    }

    public static Rect getOrgCropRect(int width, int height, int rot, Rect rotRect) {
        Rect orgRect = new Rect();
        if (rot == 0) {
            orgRect.set(rotRect);
        } else if (rot == 90) {
            orgRect.left = rotRect.top;
            orgRect.top = height - rotRect.right - 1;
            orgRect.right = rotRect.bottom;
            orgRect.bottom = height - rotRect.left - 1;
        }
        return orgRect;
    }

    public static Bitmap makeCropedGrayBitmap(byte[] data, int width, int height, int rot, Rect cropRect) {
        int cropwidth = cropRect.width();
        int cropheight = cropRect.height();
        int[] pixels = new int[cropwidth * cropheight];
        int grey, inputOffset, outputOffset, temp;
        byte[] yuv = data;

        if (rot == 0) {
            inputOffset = cropRect.top * width;
            for (int y = 0; y < cropheight; y++) {
                outputOffset = y * cropwidth;
                for (int x = 0; x < cropwidth; x++) {
                    grey = yuv[inputOffset + x + cropRect.left] & 0xff;
                    pixels[outputOffset + x] = 0xFF000000 | (grey * 0x00010101);
                }
                inputOffset += width;
            }
        } else if (rot == 90) {
            int x, y, x1, y1;
            for (y = 0; y < cropheight; y++) {
                y1 = cropRect.top + y;
                for (x = 0; x < cropwidth; x++) {
                    x1 = cropRect.left + x;
                    grey = yuv[y1 * width + x1] & 0xff;
                    pixels[x * cropheight + cropheight - y - 1] = 0xFF000000 | (grey * 0x00010101);
                }

            }
            temp = cropwidth;
            cropwidth = cropheight;
            cropheight = temp;
        }
        Bitmap bitmap = Bitmap.createBitmap(cropwidth, cropheight, Bitmap.Config.ARGB_8888);
        bitmap.setPixels(pixels, 0, cropwidth, 0, 0, cropwidth, cropheight);

        pixels = null;
        yuv = null;

        return bitmap;
    }


    public static int getByteLength(char[] str, int maxLen) {
        int i, len = 0;
        for (i = 0; i < maxLen; ++i) {
            if (str[i] == 0) break;
        }
        len = i;
        return len;
    }

    public static String getIMEI(Context mContext) {
        TelephonyManager telephonyManager = (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
        String strIMEI = telephonyManager.getDeviceId();

        if (strIMEI == null || strIMEI.equals(""))
            strIMEI = "0";

        return strIMEI;
    }

    public static String getIMSI(Context mContext) {
        TelephonyManager telephonyManager = (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
        String strIMSI = telephonyManager.getSubscriberId();

        if (strIMSI == null || strIMSI.equals(""))
            strIMSI = "0";

        return strIMSI;
    }

    public static String SaveRecogImage(String szFileName, Bitmap bmImage) {
        File dir = new File(Environment.getExternalStorageDirectory().toString(), "/aPhoneNumberImage/images/");

        if (!dir.exists()) {
            if (!dir.mkdirs()) {
                Log.d("App", "failed to create directory");
                return "";
            }
        }

        File file = new File(dir.getAbsolutePath(), szFileName);

        try {
            FileOutputStream fOut = new FileOutputStream(file);
            bmImage.compress(Bitmap.CompressFormat.JPEG, 90, fOut);
            fOut.flush();
            fOut.close();
        } catch (Exception ex) {
            ex.printStackTrace();
            return "";
        }

        return file.getAbsolutePath();
    }


}
