#include <jni.h>

#include <android/log.h>
#include <android/bitmap.h>
#include "engine/StdAfx.h"
#include "engine/RecogEngine.h"
#include "engine/Engine.h"
#include "engine/ImageBase.h"

#define  LOG_TAG    "libtestEngine"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#ifndef eprintf
#define eprintf(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)

#define RGB565_R(p) ((((p) & 0xF800) >> 11) << 3)
#define RGB565_G(p) ((((p) & 0x7E0 ) >> 5)  << 2)
#define RGB565_B(p) ( ((p) & 0x1F  )        << 3)
#define MAKE_RGB565(r,g,b) ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

#define RGBA_A(p) (((p) & 0xFF000000) >> 24)
#define RGBA_R(p) (((p) & 0x00FF0000) >> 16)
#define RGBA_G(p) (((p) & 0x0000FF00) >>  8)
#define RGBA_B(p)  ((p) & 0x000000FF)
#define MAKE_RGBA(r,g,b,a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

#define ROTATE_0	0
#define ROTATE_90	90
#define ROTATE_180	180
#define ROTATE_270	270

int PhoneNumber_Recognition(BYTE* pImage, int nWidth, int nHeight, int nBitCount, int* rstPhoneNumber) ;
BYTE* MakeGrayImgFromBitmapBits(void* pixels,int& w,int& h,int wstep,int rot);
BYTE* makeRotatedImg(BYTE* data,int& width,int& height,int rot);
BYTE* makeRotatedImg_Crop(BYTE* data,int& width,int& height,int rot,CRect cropRect,int& ww,int& hh);

extern "C" {
	JNIEXPORT jint JNICALL Java_com_recogEngine_RecogEngine_initEngine(JNIEnv* env, jobject thiz);
	JNIEXPORT jint JNICALL Java_com_recogEngine_RecogEngine_endEngine(JNIEnv* env, jobject thiz);
	JNIEXPORT jint JNICALL Java_com_recogEngine_RecogEngine_doRecogBitmap(JNIEnv* env, jobject thiz, jobject bitmap,jint rot,jintArray result);
	JNIEXPORT jint JNICALL Java_com_recogEngine_RecogEngine_doRecogGrayImg(JNIEnv* env, jobject thiz, jbyteArray ImgData,jint w, jint h,jint rot,jintArray result);
};
jint Java_com_recogEngine_RecogEngine_initEngine(JNIEnv* env, jobject thiz)
{
	LOGI("step_intEngine");
	InitEngine();
}
jint Java_com_recogEngine_RecogEngine_endEngine(JNIEnv* env, jobject thiz)
{
	FreeEngine();
}
jint Java_com_recogEngine_RecogEngine_doRecogBitmap(JNIEnv* env, jobject thiz, jobject bitmap,jint rot,jintArray result)
{
	int* presult = (int*)env->GetIntArrayElements(result, NULL);

	LOGI("step0");

	AndroidBitmapInfo  info;
	void*              pixels;
	int ret;
	if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
		LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
		return 0;
	}

	if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) LOGE("Bitmap format is RGBA_8888 !");
	else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) LOGE("Bitmap format is RGB_565 !");
	else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_4444) LOGE("Bitmap format is RGBA_4444 !");
	else if (info.format == ANDROID_BITMAP_FORMAT_A_8) LOGE("Bitmap format is A_8 !");
	else LOGE("Bitmap format is Format none !");

	if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
		LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
	}

	/* Now fill the values with a nice little plasma */
	int i,j,w,h,wstep;
	CRect cropRect = CRect(presult[0],presult[1],presult[2],presult[3]);
	w = info.width; h = info.height;wstep = info.stride;
	BYTE* pGray = MakeGrayImgFromBitmapBits(pixels,w,h,wstep,rot);
	LOGI("(w,h) = (%5d,%5d)",w,h);
	AndroidBitmap_unlockPixels(env, bitmap);

	//ret = PhoneNumber_Recognition(pGray,w,h,8, presult) ;

	BYTE* cropImg = CropImg(pGray,w,h,cropRect);
	int ww = cropRect.Width();
	int hh = cropRect.Height();
	ret = PhoneNumber_Recognition(cropImg,ww,hh,8, presult) ;
	delete[] pGray;
	delete[] cropImg;

	LOGI("recog processing completed!");
	env->ReleaseIntArrayElements(result, (jint*)presult, 0);
	return ret;
}
jint JNICALL Java_com_recogEngine_RecogEngine_doRecogGrayImg(JNIEnv* env, jobject thiz, jbyteArray ImgData,jint w, jint h,jint rot,jintArray result)
{
	BYTE* pdata = (BYTE*)env->GetByteArrayElements(ImgData, NULL);
	int* presult = (int*)env->GetIntArrayElements(result, NULL);

	CRect cropRect = CRect(presult[0],presult[1],presult[2],presult[3]);
	//BYTE* pGray = makeRotatedImg(pdata,w,h,rot);
	LOGI("(w,h,rot) = (%5d,%5d,%d) ",w,h,rot);
	int ww,hh;
	BYTE* cropImg = makeRotatedImg_Crop(pdata,w,h,rot,cropRect,ww,hh);
	LOGI("(ww,hh) = (%5d,%5d) ",ww,hh);
	int ret = PhoneNumber_Recognition(cropImg,ww,hh,8, presult) ;
	delete[] cropImg;

	env->ReleaseByteArrayElements(ImgData, (jbyte*)pdata, 0);
	env->ReleaseIntArrayElements(result, (jint*)presult, 0);
	return ret;
}
int PhoneNumber_Recognition(BYTE* pImage, int nWidth, int nHeight, int nBitCount, int* rstPhoneNumber)
{
	int i,k = 0,num;
	CARPLATE_DATA carData;
	memset(&carData,0,sizeof(CARPLATE_DATA));
	memset(rstPhoneNumber,0,sizeof(int)*50);
	int ret =  RM_Recognition(pImage,nWidth,nHeight,nBitCount,&carData,11);
	if(ret == 0){
		//LOGI("(ret=0");
		return ret;
	}
	if(carData.pPlate[0].szLicense1[0] == 0)	carData.nPlate =1;
	else										carData.nPlate =2;
	rstPhoneNumber[k++] = carData.nPlate;

	num = carData.pPlate[0].nLetNum;
	rstPhoneNumber[k++] = num;
	for(i=0;i<num;++i){
		rstPhoneNumber[k++] = carData.pPlate[0].szLicense[i];
	}
	rstPhoneNumber[k++] = carData.pPlate[0].nTrust;
	if(carData.nPlate == 2){
		LOGI("(step2");
		if(mystrcmp(carData.pPlate[0].szLicense,carData.pPlate[0].szLicense1) != 0){
			num = carData.pPlate[0].nLetNum;
			rstPhoneNumber[k++] = num;
			for(i=0;i<num;++i){
				rstPhoneNumber[k++] = carData.pPlate[0].szLicense1[i];
			}
			rstPhoneNumber[k++] = carData.pPlate[0].nTrust;
		}
		else{
			rstPhoneNumber[0]=1;
		}
	}
	rstPhoneNumber[k++] = (int)carData.nProcTime;
	rstPhoneNumber[k++] = 0;

	char rstTemp[100];
	for(i=0;i<num;++i){
		rstTemp[i] = carData.pPlate[0].szLicense[i];
	}
	rstTemp[i] = 0;
	LOGI("ret=%d, result =%s",ret,rstTemp);
	return ret;
}

BYTE* MakeGrayImgFromBitmapBits(void* pixels,int& w,int& h,int wstep,int rot)
{
	int x,y;
	int* pImgRGBA = (int*)pixels;
	BYTE* rgbaPixel,r,g,b;
	BYTE* rotatedData = new BYTE[w*h];
	if(rot == ROTATE_0){
		for(y=0;y<h;y++)for(x=0;x<w;x++)
		{
			rgbaPixel = (BYTE*) &pImgRGBA[y*w+x];
			b = rgbaPixel[1];
			g = rgbaPixel[2];
			r = rgbaPixel[3];
			rotatedData[y*w+x] = RGB2GRAY(r,g,b);
		}
	}
	else if(rot == ROTATE_90)//CDefines.FR_90)
	{
		for (y = 0; y < h; y++)	for (x = 0; x < w; x++)
		{
			rgbaPixel = (BYTE*) &pImgRGBA[y*w+x];
			b = rgbaPixel[1];
			g = rgbaPixel[2];
			r = rgbaPixel[3];
			rotatedData[x * h + h - y - 1] = RGB2GRAY(r,g,b);
		}
		int tmp = w;w = h; h = tmp;
	}
	else if (rot == ROTATE_180)//CDefines.FR_180)
	{
		for (y = 0; y < h; y++)for (x = 0; x < w; x++)
		{
			rgbaPixel = (BYTE*) &pImgRGBA[y*w+x];
			b = rgbaPixel[1];
			g = rgbaPixel[2];
			r = rgbaPixel[3];
			rotatedData[(h-y-1) * w + (w - x-1)] = RGB2GRAY(r,g,b);
		}
	}
	else if (rot == ROTATE_270)//CDefines.FR_270)
	{
		for ( y = 0; y < h; y++)for (x = 0; x < w; x++)
		{
			rgbaPixel = (BYTE*) &pImgRGBA[y*w+x];
			b = rgbaPixel[1];
			g = rgbaPixel[2];
			r = rgbaPixel[3];
			rotatedData[(w-x-1) * w + y] = RGB2GRAY(r,g,b);
		}
		int tmp = w;w = h; h = tmp;
	}

	return rotatedData;
}
BYTE* makeRotatedImg(BYTE* data,int& width,int& height,int rot)
{
	if(rot == ROTATE_0) return data;
	int x,y;
	BYTE* rotatedData = new BYTE[width*height];
	if(rot == ROTATE_90)//CDefines.FR_90)
	{
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				rotatedData[x * height + height - y - 1] = data[x + y * width];
			}
		}

		int tmp = width; // Here we are swapping, that's the difference to #11
		width = height;
		height = tmp;
	}
	else if (rot == ROTATE_180)//CDefines.FR_180)
	{
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				rotatedData[(height-y-1) * width + (width - x-1)] = data[x + y * width];
			}
		}
	}
	else if (rot == ROTATE_270)//CDefines.FR_270)
	{
		for ( y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				rotatedData[(width-x-1) * width + y] = data[x + y * width];
			}
		}
		int tmp = width; // Here we are swapping, that's the difference to #11
		width = height;
		height = tmp;
	}
	return rotatedData;
}
BYTE* makeRotatedImg_Crop(BYTE* data,int& width,int& height,int rot,CRect cropRect,int& ww,int& hh)
{
	BYTE* cropRotImg = NULL;
	if(rot == 0){
		cropRotImg = CropImg(data,width,height,cropRect);
		ww = cropRect.Width();
		hh = cropRect.Height();
		return cropRotImg;
	}

	int x,y,x1,y1;
	int cropwidth = cropRect.Width();
	int cropheight= cropRect.Height();
	int grey,inputOffset,outputOffset,temp;

	cropRotImg = new BYTE[cropwidth * cropheight];

	if(rot == 90){
		for ( y = 0; y < cropheight; y++) {
	    	y1 = cropRect.top + y;
		    for ( x = 0; x < cropwidth; x++) {
		    	x1 = cropRect.left + x;
		    	cropRotImg[x*cropheight + cropheight - y - 1] = data[y1*width+x1];
		    }
		}
		temp = cropwidth;
		cropwidth = cropheight; cropheight = temp;
		ww = cropwidth;
		hh = cropheight;
	}
	return cropRotImg;
}
