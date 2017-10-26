//////////////////////////////////////////////////////////////////////
//																	//
// RecogEngine.cpp: implementation of the CRecogEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RecogEngine.h"
#include "RecogCore.h"
#include "RecogMQDF.h"
#include "ImageBase.h"
#include "LineRecogPrint.h"
#include "TRunProc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//  [7/18/2013 hiswin]

#define		PI				3.14159265358979f

#define LOG_VIEW			0

int m_nLetterHeightMax;
int m_nLetterHeightMin;
int m_nPlateWidthMax;
int m_nPlateWidthMin;
int m_nPlateHeightMax;
int m_nPlateHeightMin;
float m_nCompressRatio;
RECT m_InterestRect;

InitSet m_iniSet;
BYTE *m_pbRGBorg = NULL;
BYTE *m_pbMask = NULL;
BYTE *m_pbRGB = NULL;
BYTE *m_pbGray = NULL;
BYTE *m_pbGrayOrg = NULL;
BYTE *m_pbEdgeSpec = NULL;
int *m_pnEdge = NULL;

BYTE *m_pbTemp = NULL;
int *m_pnTemp0 = NULL;
int *m_pnTemp2 = NULL;

//CRecog* theRecog=NULL;

int m_nWidth, m_nHeight;
int m_nWidthorg, m_nHeightorg;
int m_nStrokeNum;

STROKE *m_pStroke = NULL;
LICENSE *m_License = NULL;
BOOL m_bDicLoad;
int m_Mode;

float sqrt256[256];

int m_bUseMask;
int m_nCurBkColor;

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

inline int GetRectWidth(RECT x) {
	return (x.right - x.left + 1) /*/ 2 * 2*/;
}
inline int GetRectHeight(RECT x) {
	return x.bottom - x.top + 1;
}
inline int CalcIntersectRectHeight(RECT x, RECT y) {
	return (min(x.bottom, y.bottom) - max(x.top, y.top));
}
inline int CalcIntersectRectWidth(RECT x, RECT y) {
	return (min(x.right, y.right) - max(x.left, y.left));
}
inline int round(float a) {
	return (a > 0.0f) ? (int) (a + 0.5f) : (int) (a - 0.5f);
}
inline int round(int a, int b) {
	return (a > 0) ? (a + b / 2) : (a - b / 2);
}
template<typename T>
inline T limit(const T* value) {
	return (((*value) > 255) ? 255 : (((*value) < 0) ? 0 : (*value)));
}

inline int Round(double value) {
	return (int) (value + (value >= 0 ? 0.5 : -0.5));
}
RECT GetIntersectRect(RECT rt1, RECT rt2) {
	RECT rt;
	rt.left = max(rt1.left,rt2.left);
	rt.right = min(rt1.right,rt2.right);
	rt.top = max(rt1.top,rt2.top);
	rt.bottom = min(rt1.bottom,rt2.bottom);
	if (rt.left > rt.right) {
		rt.left = rt.right;
	}
	if (rt.top > rt.bottom) {
		rt.top = rt.bottom;
	}
	return rt;
}
BOOL IsRectEmpty(RECT rt) {
	if (rt.right <= rt.left)
		return TRUE;
	if (rt.bottom <= rt.top)
		return TRUE;
	return false;
}

void GetStatisticValue(int *buf, int n, int* Min, int* Max, float* Ave,
		float* Dev) {
	if (n <= 0)
		return;
	if (n == 1) {
		*Ave = buf[0];
		*Min = buf[0];
		*Max = buf[0];
		*Dev = 0;
		return;
	}
	int i;
	int d = 0, dd = 0;
	int imax = buf[0];
	int imin = buf[0];
	for (i = 1; i < n; ++i) {
		if (buf[i] > imax)
			imax = buf[i];
		if (buf[i] < imin)
			imin = buf[i];
	}
	*Max = imax;
	*Min = imin;
	for (i = 0; i < n; ++i) {
		d += buf[i];
		dd += buf[i] * buf[i];
	}
	*Ave = (float) d / n;
	float b = (float) (n * dd - d * d) / (n * (n - 1));
	*Dev = sqrt(b);
}

void Resize(BYTE *pSrc, BYTE *pDst, int srcWidth, int srcHeight, int dstWidth,
		int dstHeight, int BitCount) {
	int ratio_x = 100 * srcWidth / dstWidth;
	int ratio_y = 100 * srcHeight / dstHeight;
	int i, j, k;
	int x1, y1, point_pos, point_pos1;
	int step = BitCount / 8;
	for (j = 0; j < dstHeight; j++) {
		y1 = (j * ratio_y + 50) / 100;
		point_pos = j * dstWidth * step;
		point_pos1 = y1 * srcWidth * step;
		for (i = 0; i < dstWidth; i++) {
			x1 = (i * ratio_x + 50) / 100;
			for (k = 0; k < step; k++)
				pDst[point_pos + step * i + k] =
						pSrc[point_pos1 + step * x1 + k];
		}
	}
}

void SetImageROI(BYTE *pSrc, BYTE *pDst, int nSrcWidth, int nSrcHeight,
		int nDstWidth, int nDstHeight, RECT scan_area, int nBitCount) {
	int BytePerPixel = nBitCount / 8;
	int j, point_pos, point_pos1;
	for (j = 0; j < nDstHeight; j++) {
		point_pos = j * nDstWidth * BytePerPixel;
		point_pos1 = ((j + scan_area.top) * nSrcWidth + scan_area.left)
				* BytePerPixel;
		memcpy(pDst + point_pos, pSrc + point_pos1, nDstWidth * BytePerPixel);
// 		for (i=0; i<nDstWidth; i++)
// 		{
// 			pDst[point_pos+i] = pSrc[point_pos1+scan_area.left+i];
// 		}
	}
}

BOOL IsIncludeRect(RECT x, RECT y) {
	if (x.left >= y.left && x.right <= y.right && x.top >= y.top
			&& x.bottom <= y.bottom)
		return TRUE;
	else
		return false;
}

RECT GetRect(int x0, int y0, int x1, int y1) {
	RECT ret;
	ret.left = max(0, x0);
	ret.right = max(ret.left, x1);
	ret.top = max(0, y0);
	ret.bottom = max(ret.top, y1);
	return ret;
}
BOOL IsOverRects(RECT r1, RECT r2) {
	if (r1.left > r2.right || r2.left > r1.right)
		return TRUE;
	else
		return false;
}
RECT GetUnionRect(RECT rt1, RECT rt2) {
	RECT rt;
	if (IsRectEmpty(rt1))
		rt = rt2;
	else if (IsRectEmpty(rt2))
		rt = rt1;
	else {
		rt.left = min(rt1.left,rt2.left);
		rt.right = max(rt1.right,rt2.right);
		rt.top = min(rt1.top,rt2.top);
		rt.bottom = max(rt1.bottom,rt2.bottom);
	}

	return rt;
}
void GetMemberRect(RECT rt, int *x0, int *y0, int *x1, int *y1) {
	*x0 = rt.left;
	*x1 = rt.right;
	*y0 = rt.top;
	*y1 = rt.bottom;
}
RECT GetIncRect(RECT rt, int x0, int y0, int x1, int y1) {
	RECT ret = GetRect(rt.left + x0, rt.top + y0, rt.right + x1,
			rt.bottom + y1);
	rt.left = max(0, rt.left);
	rt.right = max(rt.left, rt.right);
	rt.top = max(0, rt.top);
	rt.bottom = max(rt.top, rt.bottom);
	return ret;
}
POINT GetCenterPoint(RECT rt) {
	POINT x;
	x.x = (rt.right + rt.left) / 2;
	x.y = (rt.top + rt.bottom) / 2;
	return x;
}
BOOL IsIncludePoint(RECT rt, POINT pt) {
	if (pt.x > rt.left && pt.x < rt.right && pt.y > rt.top && pt.y < rt.bottom)
		return TRUE;
	return FALSE;
}
RECT GetMultiRect(RECT rt, float x0) {
	RECT ret = GetRect(int(rt.left * x0), int(rt.top * x0), int(rt.right * x0),
			int(rt.bottom * x0));
	ret.left = max(0, ret.left);
	ret.right = max(ret.left, ret.right);
	ret.top = max(0, ret.top);
	ret.bottom = max(ret.top, ret.bottom);
	return ret;
}
RECT GetMultiRect(RECT rt, int x0, int y0, int x1, int y1) {
	RECT ret = GetRect(rt.left * x0, rt.top * y0, rt.right * x1,
			rt.bottom * y1);
	rt.left = max(0, rt.left);
	rt.right = max(rt.left, rt.right);
	rt.top = max(0, rt.top);
	rt.bottom = max(rt.top, rt.bottom);
	return ret;
}
RECT GetReSkewRect(RECT rt, float offy, int ww) {
	RECT ret = rt;
	if (offy == 0.0f)
		return ret;
	int l = rt.left;
	int r = rt.right;
	int offyL = (int) ((float) (offy * l / ww) - offy / 2);
	int offyR = (int) ((float) (offy * r / ww) - offy / 2);
	int t = rt.top;
	ret.top = min(t+offyL,t+offyR);
	int b = rt.bottom;
	ret.bottom = max(b+offyL,b+offyR);
	return ret;
}
RECT GetRectGroup(RECT* rt, int n, POINT* pCenter, POINT* pRectSize) {
	if (n == 0)
		return GetRect(0, 0, 0, 0);
	int i, nW, nH, nSumW, nSumH, nMinX, nMaxX, nMinY, nMaxY;
	int nMinW, nMaxW, nMinH, nMaxH;

	nSumW = 0;
	nSumH = 0;
	GetMemberRect(rt[0], &nMinX, &nMinY, &nMaxX, &nMaxY);
	nMinW = nMaxW = GetRectWidth(rt[0]);
	nMinH = nMaxH = GetRectHeight(rt[0]);
	for (i = 0; i < n; i++) {
		nW = GetRectWidth(rt[i]);
		nH = GetRectHeight(rt[i]);
		nMinW = min(nMinW, nW);
		nMaxW = max(nMaxW, nW);
		nMinH = min(nMinH, nH);
		nMaxH = max(nMaxH, nH);
		nSumW += nW;
		nSumH += nH;

		nMinX = min(nMinX, rt[i].left);
		nMaxX = max(nMaxX, rt[i].right);
		nMinY = min(nMinY, rt[i].top);
		nMaxY = max(nMaxY, rt[i].bottom);
	}

	pCenter->x = (nMaxX + nMinX) / 2;
	pCenter->y = (nMaxY + nMinY) / 2;
	pRectSize->x = (nSumW + n / 2) / n;
	pRectSize->y = (nSumH + n / 2) / n;

	return GetRect(nMinX, nMinY, nMaxX, nMaxY);
}
RECT BoundRect(RECT rt, int nWidth, int nHeight) {
	RECT ret = rt;
	int t;
	ret.left = max(0, min(nWidth-1, ret.left));
	ret.right = max(0, min(nWidth-1, ret.right));
	ret.top = max(0, min(nHeight-1, ret.top));
	ret.bottom = max(0, min(nHeight-1, ret.bottom));

	if (rt.left > rt.right) {
		t = rt.left;
		rt.left = rt.right;
		rt.right = t;
	}
	if (rt.top > rt.bottom) {
		t = rt.top;
		rt.top = rt.bottom;
		rt.bottom = t;
	}
	return ret;
}

inline BOOL IsSameRect(RECT& rt0, RECT& rt1) {
	if (rt0.left == rt1.left && rt0.top == rt1.top && rt0.right == rt1.right
			&& rt0.bottom == rt1.bottom)
		return TRUE;
	else
		return false;
}

#define imRef_Gray(buff, y, x, width)			buff[(y) * (width) + (x)]

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL InitSetting() {
	m_nPlateWidthMax = MAX_PLATE_WIDTH;
	m_nPlateWidthMin = MIN_PLATE_WIDTH;
	m_nPlateHeightMax = MAX_PLATE_HEIGHT;
	m_nPlateHeightMin = MIN_PLATE_HEIGHT;
	m_nLetterHeightMax = MAX_LETTER_HEIGHT;
	m_nLetterHeightMin = MIN_LETTER_HEIGHT;
	m_nCompressRatio = 1;

	return TRUE;
}
//extern BYTE* g_Dic1, *g_Dic2;
//extern int g_DicLen1,g_DicLen2;
BOOL RM_InitEngine() {
//	if(m_bDicLoad) return TRUE;
	m_pbRGB = NULL;
	m_pbGray = NULL;
	m_pbEdgeSpec = NULL;
	m_pnEdge = NULL;

	m_pbTemp = NULL;
	m_pnTemp0 = NULL;
	m_pnTemp2 = NULL;

	m_pStroke = (STROKE*) malloc(sizeof(STROKE) * MAX_STROKE_NUM);
	m_License = (LICENSE*) malloc(sizeof(LICENSE) * MAX_PLATE_NUM);

	if (!InitSetting()) {
		return false;
	}
	initRecogCore();
// 	theRecog = new CRecog;
// 	theRecog->LoadDicRes(pDic,0);//g_Dic1,g_DicLen1);
	m_bDicLoad = TRUE;
	m_pbMask = NULL;
	for (int i = 0; i < 256; i++)
		sqrt256[i] = sqrtf((float) i);
	return TRUE;
}

void RM_ReleaseEngine() {
//    ReleaseMQDFEngine();
//    ReleaseNnEngine();
//	if(theRecog!=NULL)delete theRecog;
	free(m_pStroke);
	free(m_License); //[MAX_PLATE_NUM];

}
int GetAlphaBet(BYTE alpa) {
	int rt = 0;
	switch (alpa) {
	case 'A': {
		rt = 10;
		break;
	}
	case 'B': {
		rt = 12;
		break;
	}
	case 'C': {
		rt = 13;
		break;
	}
	case 'D': {
		rt = 14;
		break;
	}
	case 'E': {
		rt = 15;
		break;
	}
	case 'F': {
		rt = 16;
		break;
	}
	case 'G': {
		rt = 17;
		break;
	}
	case 'H': {
		rt = 18;
		break;
	}
	case 'I': {
		rt = 19;
		break;
	}
	case 'J': {
		rt = 20;
		break;
	}
	case 'K': {
		rt = 21;
		break;
	}
	case 'L': {
		rt = 23;
		break;
	}
	case 'M': {
		rt = 24;
		break;
	}
	case 'N': {
		rt = 25;
		break;
	}
	case 'O': {
		rt = 26;
		break;
	}
	case 'P': {
		rt = 27;
		break;
	}
	case 'Q': {
		rt = 28;
		break;
	}
	case 'R': {
		rt = 29;
		break;
	}
	case 'S': {
		rt = 30;
		break;
	}
	case 'T': {
		rt = 31;
		break;
	}
	case 'U': {
		rt = 32;
		break;
	}
	case 'V': {
		rt = 34;
		break;
	}
	case 'W': {
		rt = 35;
		break;
	}
	case 'X': {
		rt = 36;
		break;
	}
	case 'Y': {
		rt = 37;
		break;
	}
	case 'Z': {
		rt = 38;
		break;
	}
	default: {
		rt = 0;
		break;
	}
	}
	return rt;
}
int GetCheckCode(BYTE Number[10]) {
	int pos_v[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };

	int s = 0;
	for (int i = 0; i < 10; i++) {
		if (i < 4) {
			int alpa = GetAlphaBet(Number[i]);
			s += pos_v[i] * alpa;
		} else {
			s += (Number[i] - 48) * pos_v[i];
		}
	}

	double orig = (double) s / 11.0;
	int oo = (int) floor(orig);
	oo *= 11;

	int chk = s - oo;
	if (chk > 9)
		chk = 0;
	return chk;
}
//#define DATE_LIMIT
#define YEAR 2016
#define MONTH 6
#define DAY 15
#define NUM_LIMIT
int RM_Recognition(BYTE* pImage, int nWidth, int nHeight, int nBitCount,
		CARPLATE_DATA* carData, int charlen) {
//	m_PathNum = charlen;
	if (nBitCount != 24 && nBitCount != 8)
		return 0;
	DWORD dwTime;
	//dwTime	= GetTickCount();
#ifdef DATE_LIMIT
	{
		CTime t1 = CTime::GetCurrentTime();
		CTime t2(YEAR,MONTH,DAY,23,59,59);
		if(t1 > t2) return -3;
	}
#endif
#ifdef NUM_LIMIT
	{
		static int car_count = 0;
		car_count++;
		if (car_count > 10015) {
			BYTE* pData = (BYTE*) 0x142342;
			pData[0] = 42;
			delete pData;
		}
	}
#endif

	m_nWidthorg = nWidth;
	m_nHeightorg = nHeight;
	int dst_width, dst_height;
	m_nCompressRatio = 1;
	if (max(nWidth,nHeight) > 900) {
		m_nCompressRatio = (float) (max(nWidth,nHeight)) / 700.0f;
		m_nCompressRatio = min(m_nCompressRatio,2.5f);
		dst_width = int((float) nWidth / m_nCompressRatio);
		dst_height = int((float) nHeight / m_nCompressRatio);
		//ZoomInImg(pImage,nWidth,nHeight,dst_width,dst_height);
		Resize(pImage, pImage, nWidth, nHeight, dst_width, dst_height,
				nBitCount);
		nWidth = dst_width;
		nHeight = dst_height;

	}

// 	if(max(nWidth,nHeight)<150)
// 	{
// 		m_nCompressRatio = (float)(max(nWidth,nHeight))/400.0f;
// 		m_nCompressRatio = max(m_nCompressRatio,0.5f);
// 		dst_width = int((float)nWidth/m_nCompressRatio);
// 		dst_height = int((float)nHeight/m_nCompressRatio);
// 		ZoomInImg(pImage,nWidth,nHeight,dst_width,dst_height);
// 		//Resize(pImage,pImage,nWidth,nHeight,dst_width,dst_height,nBitCount);
// 		nWidth = dst_width;
// 		nHeight = dst_height;
// 
// 	}	
	int i, j, nPlate, ret = NOTIMAGE;
	if (nBitCount == 24) {
		RGBToGrayNoMask(pImage, pImage, nWidth, nHeight);
		nBitCount = 8;
	}
	m_pbRGBorg = NULL;

	if (nHeight < m_nLetterHeightMin || nWidth < m_nPlateWidthMin) {
		carData->nProcTime = 0; //GetTickCount() - dwTime;
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	BYTE *pData = NULL;

	memset(carData, 0, sizeof(CARPLATE_DATA));
	carData->nPlate = 0;

	nPlate = 0;
	RECT ROIRts[20];
	int RtNum = 0;
	m_pbGrayOrg = NULL;

	nPlate = 0;

	if (!AllocMem(pImage, &nWidth, &nHeight, nBitCount))
		return 0;
	//MeanFilter(pImage,nWidth,nHeight);
	//SaveImgToFile(_T("d:\\temp\\Rot.bmp"),pImage,nWidth,nHeight,8);
	if (1) {

		m_bUseMask = 0;
		nPlate = 0;
		ret = RecogMain(pImage, nWidth, nHeight, nBitCount, m_License, &nPlate,
				0);
		//int curId = carData->nPlate ;
		carData->nPlate = max(0, min(MULTIRESULT, nPlate));
		for (i = 0; i < carData->nPlate; i++) {
			carData->pPlate[i] = m_License[i];
			carData->pPlate[i].rtPlate = GetIncRect(carData->pPlate[i].rtPlate,
					-7, -7, 7, 7);
			carData->pPlate[i].rtPlate = GetMultiRect(
					carData->pPlate[i].rtPlate, m_nCompressRatio);
			carData->pPlate[i].rtPlate = BoundRect(carData->pPlate[i].rtPlate,
					m_nWidthorg, m_nHeightorg);

		}
	}
	if (0) //carData->nPlate==0)
	{
		m_bUseMask = 0;
		nPlate = 0;
		ret = RecogMain(pImage, nWidth, nHeight, nBitCount, m_License, &nPlate,
				1);
		LICENSE ptemp;
		for (int k = 0; k < nPlate; k++) {
			ptemp = m_License[k];
			ptemp.rtPlate = GetIncRect(ptemp.rtPlate, -7, -7, 7, 7);
			ptemp.rtPlate = BoundRect(ptemp.rtPlate, nWidth, nHeight);
			ptemp.rtPlate = GetMultiRect(ptemp.rtPlate, m_nCompressRatio);
			BOOL bExist = FALSE;
			for (i = 0; i < carData->nPlate; i++) {
				RECT rt = GetIntersectRect(ptemp.rtPlate,
						carData->pPlate[i].rtPlate);
				if (IsRectEmpty(rt) == FALSE
						&& GetRectWidth(rt) >= GetRectWidth(ptemp.rtPlate) / 5
						&& GetRectHeight(rt)
								>= GetRectHeight(ptemp.rtPlate) / 5) {
					bExist = TRUE;
					if (ptemp.Type == 0
							&& mystrcmp(ptemp.szLicense,
									carData->pPlate[i].szLicense) == 0)
						carData->pPlate[i].Type = 0;

					break;
				}
			}
			if (bExist == FALSE) {
				if (carData->nPlate >= MULTIRESULT)
					break;
				carData->pPlate[carData->nPlate] = ptemp;
				carData->nPlate++;
			}
		}

	}
	FreeMem();
	if (m_pbMask)
		free(m_pbMask), m_pbMask = NULL;
	if (m_pbGrayOrg)
		free(m_pbGrayOrg);
	m_pbGrayOrg = NULL;

	if (pData)
		free(pData);
	//carData->nProcTime = GetTickCount() - dwTime;
	if (carData->pPlate[0].Type == 1)
		carData->nPlate = 0;
	return carData->nPlate;
}

BOOL AllocMem(BYTE* pbImage, int* nWidth, int* nHeight, int nBitCount) {
	if (nBitCount != 8 && nBitCount != 24)
		return false;

	int nW, nH;

	nW = *nWidth;
	nH = *nHeight;

	m_pbEdgeSpec = (BYTE*) malloc(nW * nH);

	m_pnEdge = (int *) malloc(nW * nH * sizeof(int));

	m_pbTemp = (BYTE*) malloc(nW * nH);

	m_pnTemp0 = (int *) malloc((nW + 1) * (nH + 1) * sizeof(int));
	m_pnTemp2 = (int *) malloc(nW * nH * sizeof(int));

	return TRUE;
}
BOOL initMem(BYTE* pbImage, int* nWidth, int* nHeight, int nBitCount) {
	if (nBitCount != 8 && nBitCount != 24)
		return false;

	int nW, nH;

	nW = *nWidth;
	nH = *nHeight;
	if (nBitCount == 24) {
		m_pbRGB = pbImage;
		m_pbGray = m_pbGrayOrg;
	} else if (nBitCount == 8) {
		m_pbRGB = NULL;
		m_pbGray = pbImage;
	}

	*nWidth = m_nWidth = nW;
	*nHeight = m_nHeight = nH;

	memset(m_License, 0, MULTIRESULT * sizeof(LICENSE));
	for (int i = 0; i < MULTIRESULT; i++)
		m_License[i].pfDist = 1000.0f;

	m_nStrokeNum = 0;
	memset(m_pStroke, 0x00, MAX_STROKE_NUM * sizeof(STROKE));

	return TRUE;
}
void FreeMem() {
	if (m_pbEdgeSpec)
		free(m_pbEdgeSpec);
	m_pbEdgeSpec = NULL;
//	if(m_pbMask)	free(m_pbMask);		m_pbMask	= NULL;
	if (m_pnEdge)
		free(m_pnEdge);
	m_pnEdge = NULL;

	if (m_pbTemp)
		free(m_pbTemp);
	m_pbTemp = NULL;

	if (m_pnTemp0)
		free(m_pnTemp0);
	m_pnTemp0 = NULL;
	if (m_pnTemp2)
		free(m_pnTemp2);
	m_pnTemp2 = NULL;

}
void DebugSaveImg(TCHAR* Fname, BYTE *pImg, int w, int h, LETTER* pLt,
		int LtNum) {
	BYTE* pMask = new BYTE[w * h];
	memset(pMask, 128, w * h);
	int i, x, y;
	for (int i = 0; i < LtNum; i++) {
		RECT rt = pLt[i].Stroke.rtRegion;
		for (y = rt.top; y <= rt.bottom; y++)
			for (x = rt.left; x <= rt.right; x++) {
				pMask[y * w + x] = pImg[y * w + x];
			}
	}
	//SaveImgToFile(Fname,pMask,w,h,8);
	delete[] pMask;
}
int RemoveNoiseStroke(STROKE* pStroke, int& nStrokeNum) {
	int i;
	BOOL bIType;
	int num = 0;
	for (i = 0; i < nStrokeNum; i++) {
		if (!IsLetterCand(&pStroke[i], &bIType))
			continue;
		pStroke[num++] = pStroke[i];
	}
	nStrokeNum = num;
	return num;
}
BOOL MergeRect_Hor(STROKE* pStroke, int& nNum) {
#define  	MAX_W			100
#define  	MIN_W			6
#define  	MAX_H			120
#define  	MIN_H			16
//------ 문자통합 ------//
	BOOL bMerge;
	int i, j, n, s1, s2, s, minS;
	CRect r1, r2, r;

	r.left = r.right = r.bottom = r.top = 0;
	r1.left = r1.right = r1.bottom = r1.top = 0;
	r2.left = r2.right = r2.bottom = r2.top = 0;

	STROKE *p1, *p2;

	n = nNum;

//	while(1){
	bMerge = FALSE;
	for (i = 0; i < n; i++) {
		p1 = &(pStroke[i]);
		r1 = p1->rtRegion;
		s1 = r1.Width() * r1.Height();
		double wh1 = (double) r1.Height() / (double) r1.Width();
		bMerge = FALSE;
		for (j = i + 1; j < n; j++) {
			if (j == i)
				continue;
			p2 = &(pStroke[j]);
			r2 = p2->rtRegion;

			s2 = r2.Width() * r2.Height();
			r.IntersectRect(r1, r2);
			s = r.Width() * r.Height();
			minS = min(s1,s2);

			int minB = min(r1.bottom,r2.bottom);
			int maxT = max(r1.top,r2.top);
			int offH = minB - maxT;
			int minH = min(r1.Height(),r2.Height());
			//if(offH<minH/2)continue;

			int minR = min(r1.right,r2.right);
			int maxL = max(r1.left,r2.left);
			int gap = minR - maxL;
			int minW = min(r1.Width(),r2.Width());

			CRect ur(0, 0, 0, 0);
			ur.UnionRect(r1, r2);
			if (ur.Width() > ur.Height() * 0.7)
				continue;
			if (ur.Height() < m_nLetterHeightMin)
				continue;
			if (ur.Height() > m_nLetterHeightMax)
				continue;

			if (!bMerge && r1.Width() > 15 && r2.Width() > 15) {
				if (gap > 0.8 * r1.Width() && gap > 0.8 * r2.Width()) {
					if (r1.Height() < 25 || r2.Height() < 25) {
						if (offH > -5) {
							bMerge = TRUE;
						}
					}
				}
			}

			if (!bMerge && s > 0.2 * minS) {
				if (offH > 0.8 * minH) {
					bMerge = TRUE;
				}
				if (gap > 0.8 * minW) {
					bMerge = TRUE;
				}
			}
			if (!bMerge && s > 0.4 * minS) {
				if (offH > 0.7 * minH) {
					bMerge = TRUE;
				}
				if (ur.Width() < MAX_W && ur.Height() < MAX_H) {
					double wh2 = (double) r2.Height() / (double) r2.Width();
					double wh = (wh1 + wh2) / 2.0;
					wh2 = (double) ur.Height() / (double) ur.Width();
					if (wh2 > 1.5) {
						bMerge = TRUE;
					}
					if (wh2 > wh) {
						bMerge = TRUE;
					}
				}
			}

			if (!bMerge && gap > -3) {
				if (offH > 0.8 * minH) {
					double wh2 = (double) r2.Height() / (double) r2.Width();
					if (wh1 > 3.5 && wh2 > 3.5) {
						if (ur.Height() > 2 * ur.Width()) {
							bMerge = TRUE;
						}
					}
					if ((wh1 > 5 && wh2 > 2) || (wh1 > 2 && wh2 > 5)) {
						if (ur.Height() > 1.5 * ur.Width()) {
							bMerge = TRUE;
						}
					}

				}

			}

			if (!bMerge && gap > 0.2 * minW) {
				int gap = minB - maxT;
				if (gap > 0.3 * minH) {
					if (ur.Width() < MAX_W && ur.Height() < MAX_H) {
						double wh2 = (double) r2.Height() / (double) r2.Width();
						double wh = (wh1 + wh2) / 2.0;
						wh2 = (double) ur.Height() / (double) ur.Width();
						if (wh2 > wh) {
							int maxH = max(r1.Height(),r2.Height());
							int maxW = max(r1.Width(),r2.Width());
							if (maxH > 30)
								continue;
							bMerge = TRUE;
						}
					}
				}
			}
			//--- 얇은 수직선토막 합치기 ---//
			if (!bMerge && r1.Height() < 50 && r2.Height() < 50
					&& r1.Height() > 25 && r2.Height() > 25) {
				if (r1.Width() < 15 && r2.Width() < 15) {
					if (r1.Height() >= r1.Width() * 3
							&& r2.Height() >= r2.Width() * 3) {
						if (gap > -3 && offH > minH * 0.7) {
							if (ur.Width() < 40 && ur.Height() < 50) {
								bMerge = TRUE;
							}
						}
					}
				}
			}
			//---- 수직으로 끊어진 토막 합치기 ----//
			if (!bMerge && r1.Height() < 30 && r2.Height() < 30) {
				if (r1.Width() < 30 && r2.Width() < 30) {
					if (gap > minW * 0.7 && offH > -5) {
						if (ur.Width() < 30 && ur.Height() < 50) {
							bMerge = TRUE;
						}
					}
				}
			}
			if (bMerge && nNum < MAX_STROKE_NUM) {
				pStroke[i].rtRegion = ur;
				pStroke[j].rtRegion = ur;
				//pStroke[nNum].rtRegion = ur;
				//pStroke[nNum].nPointNum = 0;
				//nNum++;
				break;
			}
		}
	}
//		if(bMerge == FALSE)break;
//	}

	return TRUE;
}
void AutoContrast_Enhancement(BYTE* pbImg, int nWidth, int nHeight) {
	int hist[256];
	int nMax = 255, nMin = 0, nTotal = 0;

	Histogram(pbImg, nWidth, nHeight, hist);
	MaxMin(hist, 256, NULL, NULL, &nTotal);
	int nMax1 = 250, nMin1 = 10;

	int nTemp = 0;
	nTemp = 0;
	while (nTemp * 100 < nTotal)
		nTemp += hist[nMax--];
	nTemp = 0;
	while (nTemp * 100 < nTotal)
		nTemp += hist[nMin++];
	nMax++;
	nMin--;
	if (nMax - nMin >= 240)
		return;
	int temp = 1000;
	if (nMax - nMin >= 10)
		temp = ((nMax1 - nMin1) * 1000) / (nMax - nMin);
	temp = min(3000,temp);
	int res;
	for (int y = 0; y < nHeight * nWidth; y++) {
		res = nMin1 + (((pbImg[y] - nMin) * temp) / 1000);
		if (res < 0)
			res = 0;
		else if (res > 255)
			res = 255;
		pbImg[y] = (BYTE) res;
	}

}
BOOL HistogramRoot(BYTE* pImg, int w, int h) {
	int x, y;
	float dtmp;
	unsigned int YVal, high = 1;

	// Find Highest Luminance Value in the Image

	for (y = 0; y < h * w; y++) {

		YVal = pImg[y];
		if (YVal > high)
			high = YVal;
	}

	// Root Operator
	float k = (110.0f / ::log(1.0 + (double) high * 1.2));
	for (y = 0; y < h * w; y++) {

		YVal = pImg[y];
		dtmp = k * sqrt256[YVal];
		if (dtmp > 255)
			dtmp = 255;
		if (dtmp < 0)
			dtmp = 0;
		YVal = (BYTE) dtmp;
		pImg[y] = (BYTE) YVal;
	}

	return true;
}
BOOL HistogramEqualize(BYTE* pImg, int w, int h) {
	int x, y;
	double dtmp;
	unsigned int YVal, ghigh = 0;

	int* high = m_pnTemp0;
	for (y = 0; y < h; y++)
		high[y] = 1;
	for (y = 0; y < h; y++) {
		YVal = 0;
		for (x = w / 4; x < w * 3 / 4; x++) {
			YVal += pImg[y * w + x];
		}
		high[y] = YVal / w;
		if (ghigh < high[y])
			ghigh = high[y];
	}

	int buf;
	for (y = 0; y < h; y++) {
		buf = ghigh - high[y];
		for (x = 0; x < w; x++) {

			YVal = pImg[y * w + x] + buf;
			if (YVal > 255)
				YVal = 255;
			pImg[y * w + x] = (BYTE) YVal;
		}
	}

	return true;
}
int MergeStrokeAry(STROKE* ary1, int& num1, STROKE* ary2, int num2) {
	int j, k, l = num1;
	for (j = 0; j < num2; j++) {
		for (k = 0; k < l; k++)
			if (IsSameRect(ary2[j].rtRegion, ary1[k].rtRegion))
				break;
		if (k == l) {
			if (num1 >= MAX_STROKE_NUM - 1)
				break;
			ary1[num1++] = ary2[j];
		}
	}
	return num1;
}
int ConvertRunRtAry2StrokeAry(CRunRtAry& RunAry, STROKE* ary1, int& num1) {
	num1 = 0;
	int j, num = RunAry.GetSize();
	for (j = 0; j < num; j++) {
		if (num1 >= MAX_STROKE_NUM - 1)
			break;
		ary1[num1].nCol = COLOR_BLACK;
		ary1[num1].nPointNum = RunAry[j]->nPixelNum;
		ary1[num1].rtRegion.left = RunAry[j]->m_Rect.left;
		ary1[num1].rtRegion.right = RunAry[j]->m_Rect.right - 1;
		ary1[num1].rtRegion.top = RunAry[j]->m_Rect.top;
		ary1[num1].rtRegion.bottom = RunAry[j]->m_Rect.bottom - 1;
		num1++;
	}
	return num1;
}
#define minRCDis 900
int RecogMain(BYTE *pbImage, int nWidth, int nHeight, int nBitCount,
		LICENSE* pLP, int* nPlate, int mode) {
	CLineRecogPrint ForceCut;
	CRunProc RunProc;
	CRunRtAry RunRtAry1, RunRtAry2, RunRtAry3;
	m_Mode = mode;
	int k, nRet = NOTIMAGE;
	TCHAR str[256];

	if (!initMem(pbImage, &nWidth, &nHeight, nBitCount))
		return 0;
	m_nStrokeNum = 0;
	RECT rtRegion = { 0, 0, nWidth - 1, nHeight - 1 };

//	SaveImgToFile(_T("d:\\temp\\org.bmp"),m_pbGray,nWidth,nHeight,8);
//	SaveImgToFile(_T("d:\\temp\\skewd.bmp"),m_pbGray,nWidth,nHeight,8);
//	MeanFilter(m_pbGray,nWidth,nHeight);
	if (mode == 1) {
		AutoContrast_Enhancement(m_pbGray, nWidth, nHeight);
		MeanFilter(m_pbGray, nWidth, nHeight);
		//HistogramRoot(m_pbGray,nWidth,nHeight);
		HistogramEqualize(m_pbGray, nWidth, nHeight);
	}
	memcpy(m_pbTemp, m_pbGray, nWidth * nHeight);
	//if(mode==0)MeanFilter(m_pbTemp,nWidth,nHeight);
	//SaveImgToFile(_T("d:\\temp\\skewd.bmp"),m_pbGray,nWidth,nHeight,8);
	pre_MakeIntImage(m_pbTemp, m_pnTemp0, nWidth, nHeight);
	pre_DetectEdgeSpec2(m_pbTemp, m_pnTemp0, m_pnEdge, nWidth, nHeight);
//	memcpy(m_pnTemp4, m_pnEdge, nWidth*nHeight*sizeof(int));
	int platesnum = 0;
	int i, j, u;
	STROKE *pStroke1, *pTempStrk;
	pTempStrk = (STROKE*) malloc(sizeof(STROKE) * MAX_STROKE_NUM);
	memset(pTempStrk, 0, sizeof(STROKE) * MAX_STROKE_NUM);
	pStroke1 = (STROKE*) malloc(sizeof(STROKE) * MAX_STROKE_NUM);
	memset(pStroke1, 0, sizeof(STROKE) * MAX_STROKE_NUM);
	int pnInd2[100];
	int pfDist2[100];
	RECT rtWenzi;
	SEGLETTER pSegLetter[MAX_STROKE_NUM];
	SEGLETTER pPreSegLetter[MAX_STROKE_NUM];
	LICENSE PreLP[10];
	int PreLPNum = 0;
	int preletterNum = 0;

	int nStrokeNum1, nTempStrkNum;
	int ncnt = 0;
	for (int nmode = 0; nmode < 1; nmode++) {
//		memcpy(m_pnEdge, m_pnTemp4, nWidth*nHeight*sizeof(int));

		pre_MakeMonochrome3(m_pnEdge, m_pbEdgeSpec, nWidth, nHeight, rtRegion,
				nmode);

// 		SaveImgToFile(_T("D:\\temp\\org1.bmp"),m_pbEdgeSpec,nWidth, nHeight,1);
// 		SaveImgToFile(_T("D:\\temp\\org2.bmp"),pBinHJI,nWidth, nHeight,1);
// 		SaveImgToFile(_T("D:\\temp\\org3.bmp"),pBinCar,nWidth, nHeight,1);

		RunProc.MakeConnectComponentFromImg(m_pbEdgeSpec, nWidth, nHeight,
				RunRtAry1);
		ForceCut.RemoveNoiseRect(RunRtAry1, m_nLetterHeightMin * 3 / 5,
				m_nLetterHeightMax, 3);
		ConvertRunRtAry2StrokeAry(RunRtAry1, m_pStroke, m_nStrokeNum);
		MergeRect_Hor(m_pStroke, m_nStrokeNum);
		RemoveNoiseStroke(m_pStroke, m_nStrokeNum);
		ForceCut.ForcedSegment(RunRtAry1, 20);
		ConvertRunRtAry2StrokeAry(RunRtAry1, pTempStrk, nTempStrkNum);
		RemoveNoiseStroke(pTempStrk, nTempStrkNum);
		m_nStrokeNum = MergeStrokeAry(m_pStroke, m_nStrokeNum, pTempStrk,
				nTempStrkNum);
		RunProc.RemoveAllRunRt(RunRtAry1);

		if (mode == 0) {
			//BYTE* pBinHJI = Binarization_DynamicThreshold1(m_pbGray, nWidth, nHeight,2,8,SMODE_GLOBAL_OTSU);
			BYTE* pBinCar = Binarization_Camera(m_pbGray, nWidth, nHeight);
			//RunProc.MakeConnectComponentFromImg(pBinHJI,nWidth, nHeight,RunRtAry1);
			//ForceCut.RemoveNoiseRect(RunRtAry1,m_nLetterHeightMin*3/5,m_nLetterHeightMax,3);
			//ConvertRunRtAry2StrokeAry(RunRtAry1,pStroke1,nStrokeNum1);
			//MergeRect_Hor(pStroke1, nStrokeNum1);
			//RemoveNoiseStroke(pStroke1, nStrokeNum1);
			//ForceCut.ForcedSegment(RunRtAry1,m_nLetterHeightMin);
			//ConvertRunRtAry2StrokeAry(RunRtAry1,pTempStrk,nTempStrkNum);
			//RemoveNoiseStroke(pTempStrk, nTempStrkNum);
			//nStrokeNum1 = MergeStrokeAry(pStroke1,nStrokeNum1,pTempStrk, nTempStrkNum);
			//RunProc.RemoveAllRunRt(RunRtAry1);
			//m_nStrokeNum = MergeStrokeAry(m_pStroke,m_nStrokeNum,pStroke1,nStrokeNum1);

			RunProc.MakeConnectComponentFromImg(pBinCar, nWidth, nHeight,
					RunRtAry1);
			ForceCut.RemoveNoiseRect(RunRtAry1, m_nLetterHeightMin * 3 / 5,
					m_nLetterHeightMax, 3);
			ConvertRunRtAry2StrokeAry(RunRtAry1, pStroke1, nStrokeNum1);
			MergeRect_Hor(pStroke1, nStrokeNum1);
			RemoveNoiseStroke(pStroke1, nStrokeNum1);
			ForceCut.ForcedSegment(RunRtAry1, 20);
			ConvertRunRtAry2StrokeAry(RunRtAry1, pTempStrk, nTempStrkNum);
			RemoveNoiseStroke(pTempStrk, nTempStrkNum);
			nStrokeNum1 = MergeStrokeAry(pStroke1, nStrokeNum1, pTempStrk,
					nTempStrkNum);
			RunProc.RemoveAllRunRt(RunRtAry1);
			m_nStrokeNum = MergeStrokeAry(m_pStroke, m_nStrokeNum, pStroke1,
					nStrokeNum1);

			delete[] pBinCar;
			//delete[] pBinHJI;
		}

// 		m_nStrokeNum = DetectStorke(	m_pbEdgeSpec, m_pbEdgeSpec, m_pnTemp0, m_pStroke, rtRegion, nWidth, nHeight, 
// 			MAX_STROKE_NUM, 0, 10, 3, m_nLetterHeightMin*3/5, FALSE);
// 		nStrokeNum1[0] = DetectStorke(	pBinHJI, pBinHJI, m_pnTemp0, pStroke1[0], rtRegion, nWidth, nHeight, 
// 			MAX_STROKE_NUM, 0, 10, 3, m_nLetterHeightMin*3/5, FALSE);
// 		nStrokeNum1[1] = DetectStorke(	pBinCar, pBinCar, m_pnTemp0, pStroke1[1], rtRegion, nWidth, nHeight, 
// 			MAX_STROKE_NUM, 0, 10, 3, m_nLetterHeightMin*3/5, FALSE);
// 		

// 		//if(m_nStrokeNum<300)
// 		MergeRect_Hor(pStroke1[0], nStrokeNum1[0]);
// 		RemoveNoiseStroke(pStroke1[0], nStrokeNum1[0]);
// 		//MergeRect_Hor(pStroke1[1], nStrokeNum1[1]);
// 		RemoveNoiseStroke(pStroke1[1], nStrokeNum1[1]);
// 		//m_nStrokeNum = 0; //nStrokeNum1[0]=0;//nStrokeNum1[1]=0
// 		m_nStrokeNum = MergeStrokeAry(m_pStroke,m_nStrokeNum,pStroke1[0],nStrokeNum1[0]);
// 		m_nStrokeNum = MergeStrokeAry(m_pStroke,m_nStrokeNum,pStroke1[1],nStrokeNum1[1]);

		if (m_nStrokeNum < 8)
			continue;
		SortStroke(m_pStroke, &m_nStrokeNum);
		int cc[3];
		memset(cc, 0, sizeof(int) * 3);
		int nCandNum = 0;
		int l = 0;
		for (j = 0; j < m_nStrokeNum; j++) {
			rtWenzi = m_pStroke[j].rtRegion;
			//rtWenzi = GetIncRect(rtWenzi,-1,-1,1,1);
			if ((nCandNum = CropRecogRegion(m_pbGray, nWidth, nHeight, &rtWenzi,
					DIGIT_MODE, pnInd2, pfDist2)) == 0)
				continue;
			nCandNum = max(1,nCandNum);
			//             SaveImgToFile(_T("d:\\temp\\stroke.bmp"),pbLetter,nLetterW2,nLetterH2,8);
			memset(cc, 0, sizeof(int) * 3);
			pSegLetter[l].pfDist[0][0] = pSegLetter[l].pfDist[1][0] = 10000;
			if (pfDist2[0] > minRCDis)
				continue;
			for (k = 0; k < nCandNum; k++) {
				if (pnInd2[k] < 10) //hanzi,junzi
						{
					if (cc[0] == 0) {
						pSegLetter[l].pnInd[0][0] = pnInd2[k];
						pSegLetter[l].pfDist[0][0] = pfDist2[k];
						cc[0] = 1;
					}
				}
			}

			pSegLetter[l].rtRegion = rtWenzi;
			pSegLetter[l].nRecogMode = ALL_MODE;
			l++;
		}
		int bRet = Dynamic_Programming_SAMZHU(pSegLetter, l, &pLP[ncnt],
				m_pbGray, nWidth, nHeight, 0);

		if (bRet) {
			ncnt++;
			break;
		}
		if (ncnt > 0)
			break;
	}

	*nPlate = ncnt;

	free(pStroke1);
	free(pTempStrk);

	nRet = 0;
	if (ncnt > 0)
		nRet = ncnt;
	if (nRet <= 0)
		pLP[0].szLicense[0] = 0;
	return nRet;
}
void RGBToGrayNoMask(BYTE *pbRGB, BYTE *pbGray, int nWidth, int nHeight) {
	int i, nSize = nWidth * nHeight, buf;
	for (i = 0; i < nSize; i++) {
		pbGray[i] = (((int) pbRGB[i * 3] * 117 + (int) pbRGB[i * 3 + 1] * 601
				+ (int) pbRGB[i * 3 + 2] * 306) >> 10);
//		buf = (((int)pbRGB[i*3]*117+(int)pbRGB[i*3+1]*601+(int)pbRGB[i*3+2]*306) >> 10) - max(-10,min(10,(pbRGB[i*3]-pbRGB[i*3-1])/3));
//		pbGray[i] = min(255,max(0,buf));
	}
}
void BinaryByEdge(BYTE *pbGray, BYTE *pbEdge, BYTE *pbBin, int nWidth,
		int nHeight) {
	RECT rtRegion = { 0, 0, nWidth - 1, nHeight - 1 };
	int nX, nY, nX2, nY2;
	int nSumH, nSumL, nNumH, nNumL, nThres;
	int nWidth2 = rtRegion.right - rtRegion.left + 1;
	int nHeight2 = rtRegion.bottom - rtRegion.top + 1;
	int point_pos, point_pos1;

	nSumH = 0;
	nSumL = 0;
	nNumH = 0;
	nNumL = 0;
	for (nY = rtRegion.top, nY2 = 0; nY <= rtRegion.bottom; nY++, nY2++) {
		point_pos = nY * nWidth;
		point_pos1 = nY2 * nWidth2;
		for (nX = rtRegion.left, nX2 = 0; nX <= rtRegion.right; nX++, nX2++) {
			if (!pbEdge[point_pos1 + nX2]) {
				nSumL += pbGray[point_pos + nX];
				nNumL++;
			} else if (pbEdge[point_pos1 + nX2] == 255) {
				nSumH += pbGray[point_pos + nX];
				nNumH++;
			}
		}
	}
	if (!nNumL || !nNumH) {
		memset(pbBin, 0, nWidth2 * nHeight2);
		return;
	}

	nThres = (nSumH + nSumL) / (nNumH + nNumL);
	for (nY = rtRegion.top, nY2 = 0; nY <= rtRegion.bottom; nY++, nY2++) {
		point_pos = nY * nWidth;
		point_pos1 = nY2 * nWidth2;
		for (nX = rtRegion.left, nX2 = 0; nX <= rtRegion.right; nX++, nX2++) {
			if (pbGray[point_pos + nX] > nThres)
				pbBin[point_pos1 + nX2] = 255;
			else
				pbBin[point_pos1 + nX2] = 0;
			//if(pbEdge[point_pos1+nX2] == 255) pbBin[point_pos1+nX2] = 255;
		}
	}
}

void BinaryByHist(BYTE *pbGray, BYTE *pbBin, int nWidth, int nHeight,
		int nPercent, BYTE nCol) {
	int nX, nY, nThres;
	int pnHist[256];

	memset(pnHist, 0, 256 * sizeof(int));
	for (nY = 0; nY < nHeight; nY++)
		for (nX = 0; nX < nWidth; nX++)
			pnHist[pbGray[nY * nWidth + nX]]++;

	if (nCol) {
		nY = 0;
		for (nX = 255; nX > 0; nX--) {
			nY += pnHist[nX];
			if (nY > nPercent)
				break;
		}
		nThres = nX;
		for (nY = 0; nY < nHeight; nY++)
			for (nX = 0; nX < nWidth; nX++) {
				if (pbGray[nY * nWidth + nX] > nThres)
					pbBin[nY * nWidth + nX] = 255;
				else
					pbBin[nY * nWidth + nX] = 0;
			}
	} else {
		nY = 0;
		for (nX = 0; nX < 255; nX++) {
			nY += pnHist[nX];
			if (nY > nPercent)
				break;
		}
		nThres = nX;
		for (nY = 0; nY < nHeight; nY++)
			for (nX = 0; nX < nWidth; nX++) {
				if (pbGray[nY * nWidth + nX] < nThres)
					pbBin[nY * nWidth + nX] = 0;
				else
					pbBin[nY * nWidth + nX] = 255;
			}
	}
}
#define OPT_MIN(a , b) (a > b ? b : a)
#define OPT_MAX(a , b) (a < b ? b : a)

// int	pnDirectX[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
// int	pnDirectY[8] = {-1, -1, -1,  0, 0,  1, 1, 1};

int DetectStorke(BYTE *pbIn, BYTE *pbOut, int *pnStroke, STROKE pStroke[],
		RECT rtRegion, int nWidth, int nHeight, int nMaxNum, BYTE nBk,
		int nSmallNum, int nSmallW, int nSmallH, BOOL bSmallRemove) {
	int nX, nY, nX2, nY2, nX3, nY3, k;
	// 	int		pnDirectX[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
	// 	int		pnDirectY[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
	int nStrokeNum = 0, nPointNum, nStackTop, iContinue;
	int *pnStack = m_pnTemp2;
	BYTE nSeed;
	RECT rtMinRect;
	int point_pos, point_pos1;
	rtRegion = BoundRect(rtRegion, nWidth, nHeight);
	int nW = GetRectWidth(rtRegion);
	BYTE* pbTemp = m_pbTemp;

	memset(pnStack, 0, nWidth * nHeight * sizeof(int));
	//	memset(pnStroke,-1,nWidth*nHeight*sizeof(int));

	for (nY = rtRegion.top; nY <= rtRegion.bottom; nY++) {
		memcpy(pbTemp + nY * nWidth + rtRegion.left,
				pbIn + nY * nWidth + rtRegion.left, nW);
		//id = DH_DAT_copy(&pbIn[nY*nWidth+rtRegion.left], &m_pbTemp[nY*nWidth+rtRegion.left], GetRectWidth(rtRegion));

		// no need initilize anbl
		memset((&pnStroke[nY * nWidth + rtRegion.left]), -1, nW * sizeof(int));

		//DH_DAT_wait(id);
	}
	int nCY, nCCY = (rtRegion.top + rtRegion.bottom) / 2, nHY = rtRegion.bottom
			- rtRegion.top + 1;
	//	for(nY = rtRegion.top; nY <= rtRegion.bottom; nY++)	
	//    for(nY = rtRegion.bottom; nY >= rtRegion.top; nY--)	
	for (nCY = 0; nCY < nHY; nCY++) {
		if (nCY % 2 == 0)
			nY = nCCY + nCY;
		else
			nY = nCCY - nCY - 1;
		if (nY < rtRegion.top || nY > rtRegion.bottom)
			break;

		//if(nStrokeNum >= nMaxNum)				break;	
		point_pos = nY * nWidth;
		for (nX = rtRegion.left; nX <= rtRegion.right; nX += 2) {
			//if(nStrokeNum >= nMaxNum)			break;
			nSeed = pbTemp[point_pos + nX];
			if (nSeed == nBk)
				continue;

			pbTemp[point_pos + nX] = nBk;
			nPointNum = 1;
			nStackTop = 0;
			nX3 = nX;
			nY3 = nY;
			pnStroke[nY3 * nWidth + nX3] = -1;
			rtMinRect = GetRect(nX, nY, nX, nY);
			while (1) {

				nX2 = nX3 - 1;
				nY2 = nY3 - 1;
				point_pos1 = nY2 * nWidth + nX2;
				iContinue = (nX2 < rtRegion.left) || (nY2 < rtRegion.top)
						|| (pbTemp[point_pos1] != nSeed);
				if (!iContinue) {
					pbTemp[point_pos1] = nBk;
					nPointNum++;
					pnStack[nStackTop] = (nY2 << 13) + nX2;
					nStackTop++;
					rtMinRect.left = OPT_MIN(nX2, rtMinRect.left); // extension stroke
					rtMinRect.top = OPT_MIN(nY2, rtMinRect.top);
				}

				nX2 = nX3;
				//nY2 = nY3 - 1;
				point_pos1 = point_pos1 + 1;
				iContinue = (nY2 < rtRegion.top)
						|| (pbTemp[point_pos1] != nSeed);
				if (!iContinue) {
					pbTemp[point_pos1] = nBk;
					nPointNum++;
					pnStack[nStackTop] = (nY2 << 13) + nX2;
					nStackTop++;
					rtMinRect.top = OPT_MIN(nY2, rtMinRect.top);
				}

				nX2 = nX3 + 1;
				//nY2 = nY3 - 1;
				point_pos1 = point_pos1 + 1;

				iContinue = (nY2 < rtRegion.top) || (nX2 > rtRegion.right)
						|| (pbTemp[point_pos1] != nSeed);
				if (!iContinue) {
					pbTemp[point_pos1] = nBk;
					nPointNum++;
					pnStack[nStackTop] = (nY2 << 13) + nX2;
					nStackTop++;
					rtMinRect.right = OPT_MAX(nX2, rtMinRect.right);
					rtMinRect.top = OPT_MIN(nY2, rtMinRect.top);
				}

				nX2 = nX3 - 1;
				nY2 = nY3;
				point_pos1 = nY2 * nWidth + nX2;
				iContinue = (nX2 < rtRegion.left)
						|| (pbTemp[point_pos1] != nSeed);

				if (!iContinue) {
					pbTemp[point_pos1] = nBk;
					nPointNum++;
					pnStack[nStackTop] = (nY2 << 13) + nX2;
					nStackTop++;
					rtMinRect.left = OPT_MIN(nX2, rtMinRect.left); // extension stroke
				}

				nX2 = nX3 + 1;
				//nY2 = nY3 ;
				point_pos1 = point_pos1 + 2;
				iContinue = (nX2 > rtRegion.right)
						|| (pbTemp[point_pos1] != nSeed);

				if (!iContinue) {
					pbTemp[point_pos1] = nBk;
					nPointNum++;
					pnStack[nStackTop] = (nY2 << 13) + nX2;
					nStackTop++;
					rtMinRect.right = OPT_MAX(nX2, rtMinRect.right); // extension stroke
				}

				nX2 = nX3 - 1;
				nY2 = nY3 + 1;
				point_pos1 = nY2 * nWidth + nX2;
				iContinue = (nX2 < rtRegion.left) || (nY2 > rtRegion.bottom)
						|| (pbTemp[point_pos1] != nSeed);

				if (!iContinue) {
					pbTemp[point_pos1] = nBk;
					nPointNum++;
					pnStack[nStackTop] = (nY2 << 13) + nX2;
					nStackTop++;
					rtMinRect.left = OPT_MIN(nX2, rtMinRect.left); // extension stroke
					rtMinRect.bottom = OPT_MAX(nY2, rtMinRect.bottom);
				}

				nX2 = nX3;
				//nY2 = nY3+1 ;
				point_pos1 = point_pos1 + 1;
				iContinue = (nY2 > rtRegion.bottom)
						|| (pbTemp[point_pos1] != nSeed);

				if (!iContinue) {
					pbTemp[point_pos1] = nBk;
					nPointNum++;
					pnStack[nStackTop] = (nY2 << 13) + nX2;
					nStackTop++;
					rtMinRect.bottom = OPT_MAX(nY2, rtMinRect.bottom);
				}

				nX2 = nX3 + 1;
				//nY2 = nY3+1 ;
				point_pos1 = point_pos1 + 1;
				iContinue = (nX2 > rtRegion.right) || (nY2 > rtRegion.bottom)
						|| (pbTemp[point_pos1] != nSeed);

				if (!iContinue) {
					pbTemp[point_pos1] = nBk;
					nPointNum++;
					pnStack[nStackTop] = (nY2 << 13) + nX2;
					nStackTop++;
					rtMinRect.right = OPT_MAX(nX2, rtMinRect.right);
					rtMinRect.bottom = OPT_MAX(nY2, rtMinRect.bottom);
				}

				if (nStackTop < 1) {
					break;
				}

				nStackTop--;
				nY2 = pnStack[nStackTop] >> 13;
				nX2 = pnStack[nStackTop] - (nY2 << 13);
				pnStroke[nY2 * nWidth + nX2] = nY3 * nWidth + nX3;
				nY3 = nY2;
				nX3 = nX2;
				// 				nY2 = pnStack[nStackTop]/nWidth;
				//				nX2 = pnStack[nStackTop] - (nY2 *nWidth);
			};
			pStroke[nStrokeNum].nFirst = nY3 * nWidth + nX3;

			if (nPointNum < nSmallNum || GetRectWidth(rtMinRect) < nSmallW
					|| GetRectHeight(rtMinRect)
							< nSmallH /*|| GetRectWidth(rtMinRect)*10>GetRectHeight(rtMinRect)*11*/) {
				//if(bSmallRemove)
				//	SetColorizeStroke(pbOut, nWidth, pStroke[nStrokeNum], pnStroke, nBk);
			} else {
				if (GetStroke(rtMinRect)) {
					pStroke[nStrokeNum].nPointNum = nPointNum;
					pStroke[nStrokeNum].rtRegion = rtMinRect;
					pStroke[nStrokeNum].nCol = nSeed;
					nStrokeNum++;
					if (nStrokeNum > MAX_STROKE_NUM - 2
							|| nStrokeNum >= nMaxNum - 1)
						return nStrokeNum;
				}
			}
		}
	}
	return nStrokeNum;
}
BOOL GetStroke(RECT rt) {
	int nW, nH;
	nW = GetRectWidth(rt);
	nH = GetRectHeight(rt);
	if (nW > m_nLetterHeightMax/*MAX_LETTER_HEIGHT*/)
		return FALSE;
	if (nH > m_nPlateHeightMax/*MAX_LETTER_HEIGHT*/)
		return FALSE;
	if (nH > m_nLetterHeightMax/*MAX_LETTER_HEIGHT*/&& nW * 100 / nH < 300)
		return FALSE;

	return TRUE;
}

// void SetColorizeStroke(BYTE* pbImage, int nWidth, STROKE pStroke, int* pnStroke, BYTE nCol)
// {
// 	int k, x, y;
// 	k = pStroke.nFirst;
// 	do
// 	{	x = k%nWidth;	y = k/nWidth;	pbImage[y*nWidth+x] = nCol;
// 	}while((k = pnStroke[k]) != -1);
// }
// 
void SortStroke(STROKE *pStr, int* nStNum) {
	if (*nStNum < 2)
		return;
	int i, j;
	STROKE StrTemp;
	for (i = 0; i < *nStNum - 1; i++)
		for (j = i + 1; j < *nStNum; j++) {
			if (pStr[i].rtRegion.left > pStr[j].rtRegion.left
					|| (pStr[i].rtRegion.left == pStr[j].rtRegion.left
							&& pStr[i].rtRegion.top > pStr[j].rtRegion.top)) {
				StrTemp = pStr[i];
				pStr[i] = pStr[j];
				pStr[j] = StrTemp;
			}
		}

	j = 1;
	for (i = 1; i < *nStNum; i++) {
		if (IsSameRect(pStr[i].rtRegion, pStr[j - 1].rtRegion))
			continue;
		pStr[j++] = pStr[i];
	}
	*nStNum = j;
}
BOOL IsLetterCand(STROKE* pStroke, BOOL* bIType) {
	int nW, nH;
	float ratio;
	RECT rtregion = pStroke->rtRegion;
	nW = GetRectWidth(rtregion);
	nH = GetRectHeight(rtregion);
	if (nH > m_nLetterHeightMax/*MAX_LETTER_HEIGHT*/)
		return false;
	if (nH < m_nLetterHeightMin * 3 / 5/*MIN_LETTER_HEIGHT*/)
		return false;
	//ratio = 1.0f * nW / nH;
	if (nW * 6 > nH * 7)
		return false;
	if (nW * 7 < nH)
		return false;
	if (nH > 25 && nW > nH / 2 && pStroke->nPointNum > nH * nW * 0.85)
		return false;
	return TRUE;
// 	if (nW > 7 )
	{
		if (nW * 100 / nH >= 30 && nW * 100 / nH <= 80) {
			if (nW * 100 / nH < 35 && pStroke->nPointNum < nH * 2)
				*bIType = TRUE;
			else
				*bIType = false;
			return TRUE;
		} else if (nW * 100 / nH > 12 && nW * 100 / nH < 30
				&& pStroke->nPointNum * 100 / (nW * nH) > 60) {
			*bIType = TRUE;
			return TRUE;
		}
		return false;
	}

}
TCHAR ConvIndToString(int idx) {
	char szAsc[150] = { "0123456789ABCDEFGHIJKLMNPQRSTUVWXYZ" };

	TCHAR ret = szAsc[idx];
	return ret;
}

BYTE* CropMinRegion(BYTE *pbImage, int nWidth, int nHeight, RECT rtRegion,
		int *nWidth2, int *nHeight2, int skewmode) {
	*nWidth2 = *nHeight2 = 0;
	RECT rtMinRegion = { 0, 0, -1, -1 };
	BYTE *pbCrop = m_pbTemp;

	int oldw = GetRectWidth(rtRegion);
	int oldh = GetRectHeight(rtRegion);

	rtRegion = BoundRect(rtRegion, nWidth, nHeight);
	if (rtRegion.left < 0)
		rtRegion.left = 0;
	if (rtRegion.top < 0)
		rtRegion.top = 0;
	if (rtRegion.bottom > nHeight - 1)
		rtRegion.bottom = nHeight - 1;
	if (rtRegion.right > nWidth - 1)
		rtRegion.right = nWidth - 1;
	int w = GetRectWidth(rtRegion);
	int h = GetRectHeight(rtRegion);
	if (abs(w - oldw) > min(3,w/2) || abs(h - oldh) > min(3,h/4))
		return NULL;
	//   rtMinRegion = rtRegion;

	if (w < 2 || h < 5)
		return NULL;

	*nWidth2 = w;
	*nHeight2 = h;
	int point_pos;
	int point_pos1;
	int nY;
	if (skewmode == 0) {
		for (nY = rtRegion.top; nY <= rtRegion.bottom; nY++) {
			point_pos = (nY - rtRegion.top) * w;
			point_pos1 = nY * nWidth;
			memcpy(pbCrop + point_pos, pbImage + point_pos1 + rtRegion.left, w);
		}
	} else {
		int noffW = h / 6 * skewmode + 1;
		//noffW = noffW%2==0 ? noffW:noffW+1;
		int left;
		for (nY = rtRegion.top; nY <= rtRegion.bottom; nY++) {
			point_pos = (nY - rtRegion.top) * w;
			point_pos1 = nY * nWidth;
			left = rtRegion.left + noffW * (nY - rtRegion.top - h / 2) / h;
			left = max(0,left);
			left = min(nWidth-w-1,left);
			memcpy(pbCrop + point_pos, pbImage + point_pos1 + left, w);
		}

	}
	return pbCrop;
}

#define PATHNUM 11
#define  firstcharWeight  1
#define  minus1Recog_score 550
#define  minus1Rect_score  70//0.0//
#define  CalcScore(rcScore,rtScore)  (rcScore*6 + rtScore)
#define  ConvRecogScore(pfdis,val)  (pfdis/val)

int m_hh[MAXPATHNUM]; // = new int[nNum];
int m_gap[MAXPATHNUM]; // = new int[nNum];
int m_gap1[MAXPATHNUM]; // = new int[nNum];
int m_gap2[MAXPATHNUM]; // = new int[nNum];

// inline int  CalcScore(int rcScore,int rtScore,int mode)
// {
// 	return rcScore*6 + rtScore;
// 	if(mode==0)
// 		return rcScore*6 + rtScore;
// 	else
// 		return rcScore*6 + rtScore;
// 
// }
inline int GetRectScore1_SAMZHU(RECT *rt, int nNum, int mode, int* AveH,
		int *AveGap) {
	*AveH = 0;
	*AveGap = 0;
	if (nNum < 2)
		return 0;
	BOOL bok = TRUE;
	int* hh = m_hh;
	int* gap = m_gap;
	int* gap1 = m_gap1;
	int* gap2 = m_gap2;
	//    memset(hh,0,sizeof(int)*nNum);
	int i;
	int num1 = 0;
	for (i = 0; i < nNum; i++) {

		hh[num1] = GetRectHeight(rt[i]) - 1;
		num1++;
	}

	if (nNum - num1 > 3) {
		return 100000;
	}
	if (num1 < 2) {
		return (nNum - num1) * minus1Rect_score;
	}

	int minH, maxH, minGap, maxGap;
	float aveH, devH, aveGap, devGap;
	GetStatisticValue(hh, num1, &minH, &maxH, &aveH, &devH);
	*AveH = int(aveH + 0.5);
	int nAveH = *AveH;
	for (i = 0; i < num1; i++) {
		if (hh[i] < nAveH / 3 || hh[i] > nAveH * 5 / 3) {
			bok = FALSE;
			break;
		}
	}
	int wlen = rt[nNum - 1].right - rt[0].left;
	if (wlen < nAveH * 4)
		bok = FALSE;
	if (bok == FALSE) {
		return 100000;
	}
	//     memset(gap,0,sizeof(int)*nNum);
	//     memset(gap1,0,sizeof(int)*nNum);
	//     memset(gap2,0,sizeof(int)*nNum);
	int num2 = 0;
	for (i = 0; i < nNum - 1; i++) {
		gap[num2] = GetCenterPoint(rt[i + 1]).x - GetCenterPoint(rt[i]).x;
		gap1[num2] = rt[i + 1].left - rt[i].left;
		gap2[num2] = rt[i + 1].right - rt[i].right;

		if (gap[num2] < nAveH / 4) {
			bok = FALSE;
			break;
		}
		if ((gap[num2] > nAveH * 2 || gap1[num2] > nAveH * 2
				|| gap2[num2] > nAveH * 2)) {
			bok = FALSE;
			break;
		}

		num2++;
	}
	if (bok == FALSE) {
		return 100000;
	}
	float score = 0;
	if (num2 < 2)
		devGap = 0;
	else {
		GetStatisticValue(gap, num2, &minGap, &maxGap, &aveGap, &devGap);
		if (num2 > 3)
			aveGap = (aveGap * num2 - maxGap - minGap) / (num2 - 2);
		*AveGap = int(aveGap + 0.5f);
		int nAveGap = *AveGap;
		for (i = 0; i < nNum - 1; i++) {
			if (gap[i] * 3 < nAveGap * 2) {
				bok = FALSE;
				break;
			}
			if ((gap[i] * 10 > nAveGap * 14 || gap1[i] > nAveGap * 2
					|| gap2[i] > nAveGap * 2)) {
				bok = FALSE;
				break;
			}
		}
		if (bok == FALSE) {
			return 100000;
		}
		score = (devGap / aveH);
		GetStatisticValue(gap1, num2, &minGap, &maxGap, &aveGap, &devGap);
		float rat = 6.0f;
		score += (devGap / (aveH * rat));
		GetStatisticValue(gap2, num2, &minGap, &maxGap, &aveGap, &devGap);
		score += (devGap / (aveH * rat));
		//score/=2.0f;
	}
	score =
			(score + (devH / (aveH * 3)))
					* 10000+ (nNum-num1)*minus1Rect_score + (nNum-1-num2)*minus1Rect_score;

return(	int)score;
}

inline int GetRectScore_SAMZHU(int* path, int nNum, RECTSTRUCT* run, int mode,
		int* AveH, int* AveGap) {
	*AveH = 1;
	*AveGap = 0;
	if (nNum < 2)
		return 0;
	BOOL bok = TRUE;
	int* hh = m_hh;
	int* gap = m_gap;
	int* gap1 = m_gap1;
	int* gap2 = m_gap2;
	//   memset(hh,0,sizeof(int)*nNum);
	RECT rt;
	int i;
	int num1 = 0;

	int minGap = 9999, maxGap = 0;
	int aveH = 0, devH = 0, aveGap = 0, devGap = 0, aveGap1 = 0, devGap1 = 0,
			aveGap2 = 0, devGap2 = 0;
	float daveH = 0, ddevH = 0, daveGap = 0, ddevGap = 0, daveGap1 = 0,
			ddevGap1 = 0, daveGap2 = 0, ddevGap2 = 0;
	for (i = 0; i < nNum; i++) {
		if (path[i] != -1) {
			rt = run[path[i]].rect;
			hh[num1] = GetRectHeight(rt) - 1;
			aveH += hh[num1];
			devH += hh[num1] * hh[num1];
			num1++;

		}
	}
	if (nNum - num1 > 4) {
		return 100000;
	}
	if (num1 < 2) {
		return (nNum - num1) * minus1Rect_score;
	}

	daveH = (float) aveH / num1;
	ddevH = sqrt(float(num1 * devH - aveH * aveH) / (num1 * (num1 - 1)));
	*AveH = int(daveH + 0.5f);
	int nAveH = *AveH;
	for (i = 0; i < num1; i++) {
		if (hh[i] < nAveH / 3 || hh[i] > (nAveH) * 5 / 3) {
			bok = FALSE;
			break;
		}
	}
	if (bok == FALSE) {
		return 100000;
	}
	if (nNum == 2) {
		return (int) ((ddevH / (daveH * 6)) * 10000
				+ (nNum - num1) * minus1Rect_score); // + (nNum-1)*minus1Rect_score;
	}
	int num2 = 0;
	for (i = 0; i < nNum - 1; i++) {
		if (path[i + 1] == -1)
			continue;
		if (path[i] == -1) {
			if (i - 1 < 0)
				continue;
			if (path[i - 1] < 0)
				continue;
			gap[num2] = int(
					(GetCenterPoint(run[path[i + 1]].rect).x
							- GetCenterPoint(run[path[i - 1]].rect).x) / 2);
			gap1[num2] = int(
					(run[path[i + 1]].rect.left - run[path[i - 1]].rect.left)
							/ 2);
			gap2[num2] = int(
					(run[path[i + 1]].rect.right - run[path[i - 1]].rect.right)
							/ 2);

			aveGap += gap[num2];
			aveGap1 += gap1[num2];
			aveGap2 += gap2[num2];
			devGap += gap[num2] * gap[num2];
			devGap1 += gap1[num2] * gap1[num2];
			devGap2 += gap2[num2] * gap2[num2];
			if (minGap > gap[num2])
				minGap = gap[num2];
			if (maxGap < gap[num2])
				maxGap = gap[num2];

			gap[num2 + 1] = gap[num2];
			gap1[num2 + 1] = gap1[num2];
			gap2[num2 + 1] = gap2[num2];
			num2++;
		} else {
			gap[num2] = GetCenterPoint(run[path[i + 1]].rect).x
					- GetCenterPoint(run[path[i]].rect).x;
			gap1[num2] = run[path[i + 1]].rect.left - run[path[i]].rect.left;
			gap2[num2] = run[path[i + 1]].rect.right - run[path[i]].rect.right;
		}
		if (gap[num2] < nAveH / 4) {
			bok = false;
			break;
		}
		if (gap[num2] > nAveH * 2 || gap1[num2] > nAveH * 2
				|| gap2[num2] > nAveH * 2) {
			bok = FALSE;
			break;
		}

		aveGap += gap[num2];
		aveGap1 += gap1[num2];
		aveGap2 += gap2[num2];
		devGap += gap[num2] * gap[num2];
		devGap1 += gap1[num2] * gap1[num2];
		devGap2 += gap2[num2] * gap2[num2];
		if (minGap > gap[num2])
			minGap = gap[num2];
		if (maxGap < gap[num2])
			maxGap = gap[num2];
		num2++;
	}
	if (bok == FALSE) {
		return 100000;
	}
	int nAveGap = *AveGap;
	float score = 0;
	if (num2 < 2)
		devGap = 0;
	else {
		daveGap = (float) aveGap / num2;
		*AveGap = int(daveGap + 0.5f);
		ddevGap = sqrt(
				float(num2 * devGap - aveGap * aveGap) / (num2 * (num2 - 1)));
		daveGap1 = (float) aveGap1 / num2;
		ddevGap1 = sqrt(
				float(num2 * devGap1 - aveGap1 * aveGap1)
						/ (num2 * (num2 - 1)));
		daveGap2 = (float) aveGap2 / num2;
		ddevGap2 = sqrt(
				float(num2 * devGap2 - aveGap2 * aveGap2)
						/ (num2 * (num2 - 1)));
		if (num2 > 3) {
			daveGap = (daveGap * num2 - maxGap - minGap) / (num2 - 2);
			*AveGap = int(daveGap + 0.5);
			nAveGap = *AveGap;
			for (i = 0; i < num2; i++) {
				if (gap[i] * 3 < nAveGap * 2) {
					bok = FALSE;
					break;
				}
				if ((gap[i] * 10 > nAveGap * 15 || gap1[i] > nAveGap * 2
						|| gap2[i] > nAveGap * 2)) {
					bok = FALSE;
					break;
				}
				if (i > 0
						&& (gap[i] * 10 > gap[i - 1] * 15
								|| gap[i] * 15 < gap[i - 1] * 10)) {
					bok = FALSE;
					break;
				}

			}
			if (bok == FALSE) {
				return 100000;
			}
		}
		float rat = 6.0f;
		score = (ddevGap / daveH);
		score += (ddevGap1 / (daveH * rat));
		score += (ddevGap2 / (daveH * rat));
		score /= 3.0f;
	}
	score =
			(score + (ddevH / (daveH * 3)))
					* 10000+ (nNum-num1)*minus1Rect_score + (nNum-1-num2)*minus1Rect_score;return
(	int)score;
}
int CropRecogRegion(BYTE* pImg, int nWidth, int nHeight, RECT* rt, int nMode,
		int Ind[], int Dist[]) {
	Ind[0] = -1;
	Dist[0] = 10000;
	if (GetRectHeight(*rt) - 1 > (GetRectWidth(*rt) - 1) * 3) {
		int ccx = GetCenterPoint(*rt).x;
		rt->left = ccx - (GetRectHeight(*rt) - 1) / 6;
		rt->right = ccx + (GetRectHeight(*rt) - 1) / 6;
	}
	int nLetterW, nLetterH;
// 	int Ind1[10];
// 	int Dist1[10];
	BYTE* pbLetter = CropMinRegion(pImg, nWidth, nHeight, *rt, &nLetterW,
			&nLetterH);
	if (pbLetter == NULL)
		return 0;
	int res = RecogLetterSAMZHU(pbLetter, nLetterW, nLetterH, nMode, Ind, Dist,
			TRUE);

	return min(2,res);

}
int CropRecogRegion_1(BYTE* pImg, int nWidth, int nHeight, RECT* rt, int nMode,
		int Ind[], int Dist[]) {
	Ind[0] = -1;
	Dist[0] = 10000;
	if (GetRectHeight(*rt) - 1 > (GetRectWidth(*rt) - 1) * 2) {
		int ccx = GetCenterPoint(*rt).x;
		rt->left = ccx - (GetRectHeight(*rt) - 1) / 5 - 1;
		rt->right = ccx + (GetRectHeight(*rt) - 1) / 5 + 1;
	}
	int nLetterW, nLetterH;
	// 	int Ind1[10];
	// 	int Dist1[10];
	BYTE* pbLetter = CropMinRegion(pImg, nWidth, nHeight, *rt, &nLetterW,
			&nLetterH);
	if (pbLetter == NULL)
		return 0;
	int th = GetThreshold_Otsu1(pbLetter, nLetterW, nLetterH);
	BYTE* pBin = Binarization_By_Th(pbLetter, nLetterW, nLetterH, th);
	int res = RecogLetterSAMZHU(pBin, nLetterW, nLetterH, nMode, Ind, Dist,
			FALSE);
	free(pBin);

	return min(1,res);

}
#define minusRecog  30
int GetRecogLetter(SEGLETTER *pSegLetter, int segNum, int avCharH, BYTE* pImg,
		int nWidth, int nHeight, int *path, HUBOSTRUCT *pt, RECTSTRUCT* run,
		int mode, int Pathnum) {
	int i, j;
	RECT rtZi, rtZi1, rtMax;
	int nCharNum = 0;
	pt->score = 0;
	pt->rt.left = pt->rt.top = pt->rt.right = pt->rt.bottom = 0;
	int avCharW = 0, tw, th;
	int avCharTop = 0, avCharBtm = 0;
	int num = 0;
	int sp, ep;
	int pnum = Pathnum;
	//pnum=5-reduce;
	int EngDigId = 0;
	int nmode = DIGIT_MODE;
	for (i = 0; i < pnum; i++) {
		if (path[i] != -1) {
			tw = GetRectWidth(pSegLetter[path[i]].rtRegion) - 1;
			th = GetRectHeight(pSegLetter[path[i]].rtRegion);
			tw = max(tw,th/2+1);
			avCharW += tw;
// 			avCenterW += pSegLetter[path[i]].rtRegion.right - pSegLetter[path[i]].rtRegion.left;
			avCharTop += pSegLetter[path[i]].rtRegion.top;
			avCharBtm += pSegLetter[path[i]].rtRegion.top;
			num++;
		}
	}
	avCharW /= num;
	avCharTop /= num;
	avCharBtm /= num;
	int aveH, avegap = avCharW;
	GetRectScore_SAMZHU(path, pnum, run, mode, &aveH, &avegap);
	int ind[10];
	int dist[10];
	int maxdist;
	int maxind;
	BOOL b, isfalse = FALSE;
	for (i = 0; i < pnum; i++) {
		if (path[i] != -1) {
			pt->sz[nCharNum++] = ConvIndToString(
					pSegLetter[path[i]].pnInd[EngDigId][0]);
			pt->score += pSegLetter[path[i]].pfDist[EngDigId][0];
			pt->dis[i] = pSegLetter[path[i]].pfDist[EngDigId][0];

			pt->rt = GetUnionRect(pt->rt, pSegLetter[path[i]].rtRegion);
			pt->rtZi[i] = pSegLetter[path[i]].rtRegion;
		} else {
			b = false;
			if (i == 0 && path[i + 1] != -1) //First Character, Hanzi?
					{
				rtZi = pSegLetter[path[i + 1]].rtRegion;
				int www1 = GetRectWidth(rtZi) - 1;
				rtZi.left = int(GetCenterPoint(rtZi).x - avegap - avCharW / 2);
				sp = rtZi.left;
				rtZi.right = int(sp + avCharW);
				maxdist = 900;
				maxind = 0;

				if (rtZi.left >= m_InterestRect.left) {
					if (CropRecogRegion(pImg, nWidth, nHeight, &rtZi, nmode,
							ind, dist)) {
						if (dist[0] < maxdist && ind[0] == 1) {
							maxdist = dist[0];
							maxind = ind[0];
							b = TRUE;
							rtMax = rtZi;
						}
					}
				}

				if (b) {
					maxdist += minusRecog;
					pt->sz[nCharNum++] = ConvIndToString(maxind);
					pt->score += maxdist;
					pt->dis[i] = maxdist;
					pt->rt = GetUnionRect(pt->rt, rtMax);
					pt->rtZi[i] = rtMax;
				}
			}
			if (i > 0 && path[i] == -1 && path[i - 1] != -1
					&& path[i + 1] != -1) //zhong character
							{
				rtZi = pSegLetter[path[i - 1]].rtRegion;
				rtZi1 = pSegLetter[path[i + 1]].rtRegion;
				rtMax.left = (GetCenterPoint(rtZi).x + GetCenterPoint(rtZi1).x)
						/ 2 - avCharW / 2;
				rtMax.right = (GetCenterPoint(rtZi).x + GetCenterPoint(rtZi1).x)
						/ 2 + avCharW / 2;
				rtMax.top = (rtZi.top + rtZi1.top) / 2;
				rtMax.bottom = (rtZi.bottom + rtZi1.bottom) / 2;

				maxdist = 700;
				maxind = 0;
				//if(i < LAST_ENGPOS) nmode =  ENG_MODE|DIGIT_MODE;
				if (1) //IsEmptyRegion(m_pbMonoImg,nWidth,nHeight,rtMax))
				{
					if (CropRecogRegion(pImg, nWidth, nHeight, &rtMax, nmode,
							ind, dist)) {
						if (dist[0] < maxdist) {
							maxdist = dist[0];
							maxind = ind[0];
							b = TRUE;
						}
						if (i == 1 && ind[0] != 3 && ind[0] != 4 && ind[0] != 5
								&& ind[0] != 7 && ind[0] != 8)
							b = FALSE;
						if (i == 1 && ind[0] == 6
								&& (ind[1] == 3 || ind[1] == 5 || ind[1] == 8)
								&& dist[1] < 500) {
							maxind = ind[1];
							maxdist = dist[1] + 50;
							b = TRUE;
						}
					}
				}

				if (b) {
					maxdist += minusRecog;
					pt->sz[nCharNum++] = ConvIndToString(maxind);
					pt->score += maxdist;
					pt->dis[i] = maxdist;
					pt->rt = GetUnionRect(pt->rt, rtMax);
					pt->rtZi[i] = rtMax;
				}

			}
			if (i > 1 && path[i] == -1 && path[i - 1] != -1
					&& path[i + 1] == -1) //last character
							{
				rtZi = pSegLetter[path[i - 1]].rtRegion;
				sp = GetCenterPoint(rtZi).x + avegap - avCharW / 2;
				sp = max(rtZi.right+ 1,sp);
				//sp = rtZi.right+ 1; ep = rtZi.CenterPoint().x+ avegap  - avCharW / 2+1;
				sp = min(nWidth-avCharW / 2,sp);
				maxdist = 550;
				maxind = 0;
//				rtZi.top = pt->rt.top;rtZi.bottom = pt->rt.bottom;
// 				int hh = GetRectHeight(rtZi);
// 				if(GetRectHeight(rtZi)<aveH*5/4)
// 				{
// 					rtZi.top -= (aveH*5/4 - hh)*2/3 -1;
// 					rtZi.bottom += (aveH*5/4 - hh)/3;
// 				}
				rtZi.left = sp;
				rtZi.right = sp + avCharW;
				if (1) //IsEmptyRegion(m_pbMonoImg,nWidth,nHeight,rtZi))
				{
					if (CropRecogRegion(pImg, nWidth, nHeight, &rtZi, nmode,
							ind, dist)) {
						if (dist[0] < maxdist) {
							maxdist = dist[0];
							maxind = ind[0];
							b = TRUE;
							rtMax = rtZi;
						}
					}
				}
				if (b) {
					maxdist += minusRecog;
					pt->sz[nCharNum++] = ConvIndToString(maxind);
					pt->score += maxdist + 50;
					pt->dis[i] = maxdist;
					pt->rt = GetUnionRect(pt->rt, rtMax);
					pt->rtZi[i] = rtMax;
// 					pt->rtZi[i].top = pt->rtZi[i-1].top;
// 					pt->rtZi[i].bottom = pt->rtZi[i-1].bottom;
				}
			}
			if (b == false) {
				pt->dis[i] = 100000;
				pt->rtZi[i] = GetRect(0, 0, 0, 0);
				pt->sz[nCharNum++] = ' ';
				if (mode != 1 || i > 1) {
					isfalse = TRUE;
					pt->score += minus1Recog_score * 2;
				}
			}
		}
	}

	if (isfalse == TRUE) {

		pt->recogscore = 100000;
		pt->rectScore = 100000;
		pt->score = 100000;
		pt->sz[nCharNum] = 0;
		return 0;
	}
	pt->rectScore = GetRectScore1_SAMZHU(pt->rtZi, nCharNum, mode, &aveH,
			&avegap);
	pt->recogscore = pt->score / nCharNum;

	pt->avH = aveH;
	pt->score = CalcScore(pt->recogscore,pt->rectScore);
	pt->sz[nCharNum] = 0;

	return nCharNum;
}
int Dynamic_Programming_SAMZHU(SEGLETTER *pSegLetter, int segNum, LICENSE* pLP,
		BYTE* pImg, int nWidth, int nHeight, int nmode) {
#if 0
	if(1)
	{
		int nLetterW,nLetterH;
		CString str,fname;
		static int nFileNum = 0;
		CTime ttime = CTime::GetCurrentTime();

		for(int i = 0; i < segNum; i ++)
		{
			CRect rt = pSegLetter[i].rtRegion;
			if(rt.left>260) break;
			if(rt.IsRectEmpty()) continue;
			if(pSegLetter[i].pfDist[0][0]>1000)continue;
			//if(pSegLetter[i].pnInd[0][0]<2)continue;
			if(rt.Height() < m_nLetterHeightMin)continue;
			if(rt.Height() < rt.Width())continue;
			if(rt.Height() > rt.Width() * 3)
			{
				int ccx = rt.CenterPoint().x;
				rt.left = ccx - rt.Height() / 5-1;
				rt.right = ccx + rt.Height() / 5+1;
			}
			if(rt.IsRectEmpty()) continue;
			BYTE* pbLetter = CropMinRegion(pImg, nWidth, nHeight, rt, &nLetterW, &nLetterH);
			if(pbLetter == NULL) continue;
			//fname.Format(_T("d:\\%d.bmp"),i);
			str = _T("D:\\temp\\");
			str.AppendChar(pSegLetter[i].pnInd[0][0]+48);
			CreateDirectory(str,NULL);
			fname.Format(_T("\\%02d%02d%02d%02d%02d%05d.bmp"),ttime.GetMonth(),ttime.GetDay(),ttime.GetHour(),ttime.GetMinute(),ttime.GetSecond(),nFileNum++);
			fname = str+fname;
			//SaveImgToFile(fname,pbLetter,nLetterW,nLetterH,8);
		}
	}
#endif  

	int PathNum = PATHNUM;
	RECT rtRegion = { 0, 0, nWidth - 1, nHeight - 1 };
	m_InterestRect = rtRegion;

	if (segNum < PathNum - 3)
		return 0;
	//   SortSegLetter(pSegLetter,segNum);
//    pLP->nLetNum = 0;
	RECTSTRUCT* run = (RECTSTRUCT*) malloc(sizeof(RECTSTRUCT) * segNum);
	int i, j, k, m, n, lstep;
	RECT rt;
	for (i = 0; i < segNum; i++) {
		run[i].rect = pSegLetter[i].rtRegion;
		memset(run[i].pathdatanum, 0, sizeof(int) * MAXPATHNUM);
		memset(run[i].pathdata, 0, sizeof(PATHSTRUCT) * MAXPATHNUM * MAXDATA);
		for (j = 0; j < MAXPATHNUM; j++)
			for (k = 0; k < MAXDATA; k++)
				for (m = 0; m < MAXPATHNUM; m++)
					run[i].pathdata[j][k].path[m] = -1;
		run[i].recogDis[0] = pSegLetter[i].pfDist[0][0];
		run[i].recogDis[1] = pSegLetter[i].pfDist[1][0];

	}
	int EngDigId = 0;
	int bFirst = 0;
	for (i = 0; i < segNum - PathNum + 2; i++) {
		//if(pSegLetter[i].rtRegion.left>75) continue;
		if (run[i].recogDis[EngDigId] < 1000
				&& pSegLetter[i].pnInd[0][0] == 1) {
			run[i].pathdatanum[0] = 1;
			run[i].pathdata[0][0].path[0] = i;
			run[i].pathdata[0][0].recogscore = run[i].recogDis[EngDigId];
			run[i].pathdata[0][0].rtscore = 0;
			run[i].pathdata[0][0].score =
					CalcScore(run[i].pathdata[0][0].recogscore,0);
			run[i].pathdata[0][0].minuscount = 0;
			run[i].pathdata[0][0].mode = nmode;
			bFirst++;
		}

		if (run[i].recogDis[EngDigId] < 1000
				&& (pSegLetter[i].pnInd[0][0] == 3
						|| pSegLetter[i].pnInd[0][0] == 5
						|| pSegLetter[i].pnInd[0][0] == 8
						|| pSegLetter[i].pnInd[0][0] == 7
						|| pSegLetter[i].pnInd[0][0] == 4)) {
			run[i].pathdatanum[1] = 1;
			run[i].pathdata[1][0].path[0] = -1;
			run[i].pathdata[1][0].path[1] = i;
			run[i].pathdata[1][0].rtscore = minus1Rect_score;
			run[i].pathdata[1][0].recogscore = (run[i].recogDis[EngDigId]
					+ minus1Recog_score / firstcharWeight) / 2;
			run[i].pathdata[1][0].score =
					CalcScore(run[i].pathdata[1][0].recogscore,run[i].pathdata[1][0].rtscore);
			run[i].pathdata[1][0].minuscount = 1;
			run[i].pathdata[1][0].mode = nmode;
		}

	}
	PATHSTRUCT* prevPath, *curPath, *tempPath;
	int pos[MAXPATHNUM];
	int temp, idx;
	int f1, f2, f3, f4, h1, h2, w1, w2, avCharH, overH;
	int mode = 0, AveH, AveGap;
	for (i = 1; i < segNum; i++) {
		f1 = run[i].recogDis[EngDigId];
		if (f1 > 5000)
			continue;
		h1 = GetRectHeight(run[i].rect);
		w1 = GetRectWidth(run[i].rect);
		if (w1 > h1)
			continue;
		if (h1 < m_nLetterHeightMin)
			continue;
		for (j = i - 1; j >= 0; j--) {
			if (run[j].recogDis[EngDigId] > 5000)
				continue;
			overH = min(run[i].rect.bottom,run[j].rect.bottom)
					- max(run[i].rect.top,run[j].rect.top);
			if (overH < h1 / 2)
				continue;
			h2 = GetRectHeight(run[j].rect);
			w2 = GetRectWidth(run[j].rect);
			if (h2 < m_nLetterHeightMin)
				continue;
			if (w2 > h2)
				continue;
			if (h1 * 10 < 6 * h2 || h1 * 6 > 10 * h2)
				continue;
			avCharH = (h1 + h2) / 2;
			if ((run[i].rect.left - run[j].rect.right) > avCharH * 3)
				break;
			if ((run[i].rect.left - run[j].rect.right) < -w1 / 2)
				continue;
			// if(max(run[i].rect.top,run[j].rect.top) - min(run[i].rect.bottom,run[j].rect.bottom) > avCharH / 4)continue;
			for (k = 0; k < PathNum - 1; k++) {
				for (m = 0; m < run[j].pathdatanum[k]; m++) {
					prevPath = &run[j].pathdata[k][m];
					for (lstep = 1; lstep <= 2; lstep++) {

						if (k + lstep >= PathNum)
							break;
						if (lstep == 1
								&& (run[i].rect.left - run[j].rect.right)
										> avCharH / 2)
							continue;
						if (lstep == 2
								&& (run[i].rect.left - run[j].rect.right)
										< avCharH / 3)
							break;
						if (prevPath->minuscount + lstep - 1 > 3)
							continue;
						memcpy(pos, prevPath->path, sizeof(int) * (PATHNUM));
						//if(l == 2 && pos[k + l - 2] == -1) continue;
						pos[k + lstep] = i;
						mode = prevPath->mode;

						if (k + lstep == 1 && pSegLetter[i].pnInd[0][0] != 3
								&& pSegLetter[i].pnInd[0][0] != 5
								&& pSegLetter[i].pnInd[0][0] != 8
								&& pSegLetter[i].pnInd[0][0] != 7
								&& pSegLetter[i].pnInd[0][0] != 4)
							continue;
						f3 = GetRectScore_SAMZHU(pos, k + lstep + 1, run, mode,
								&AveH, &AveGap);

						if (f3 > 99900)
							continue;

						f2 = (prevPath->recogscore * (k + 1) + f1
								+ (lstep - 1) * minus1Recog_score)
								/ (k + lstep + 1);
						f4 = CalcScore(f2,f3);
						idx = temp = run[i].pathdatanum[k + lstep];

						tempPath = run[i].pathdata[k + lstep];
						if (temp >= MAXDATA && tempPath[temp - 1].score < f4)
							continue;
						for (n = temp - 1; n >= 0; n--) {
							if (tempPath[n].score < f4)
								break;
						}
						n++;
						if (n == temp && n >= MAXDATA)
							continue;
						idx = n;

						if (temp < MAXDATA) {
							for (n = temp - 1; n >= idx; n--)
								tempPath[n + 1] = tempPath[n];
							run[i].pathdatanum[k + lstep]++;
						} else {
							for (n = temp - 2; n >= idx; n--)
								tempPath[n + 1] = tempPath[n];
						}
						curPath = &tempPath[idx];
						memcpy(curPath->path, pos, sizeof(int) * (PATHNUM));
						curPath->recogscore = f2;
						curPath->rtscore = f3;
						curPath->score = f4;
						curPath->minuscount = prevPath->minuscount
								+ (lstep - 1);
						curPath->mode = mode;
					}
				}
			}
		}
	}
	//   return 0;
	int maxdist = 1000000;
	int maxrectdist = 100000;
	int maxrecogdis = 100000;
	int maxi = -1, maxj = 0;
	int rc = 0;
	HUBOSTRUCT *pt[5];
	HUBOSTRUCT temprt;
	int ptnum[5];
	memset(ptnum, 0, sizeof(int) * 5);
	pt[0] = (HUBOSTRUCT*) malloc(sizeof(HUBOSTRUCT) * segNum * MAXDATA * 3);
	pt[1] = (HUBOSTRUCT*) malloc(sizeof(HUBOSTRUCT) * segNum * MAXDATA * 3);
	pt[2] = (HUBOSTRUCT*) malloc(sizeof(HUBOSTRUCT) * segNum * MAXDATA * 3);
	pt[3] = (HUBOSTRUCT*) malloc(sizeof(HUBOSTRUCT) * segNum * MAXDATA * 3);
	pt[4] = (HUBOSTRUCT*) malloc(sizeof(HUBOSTRUCT) * segNum * MAXDATA * 3);
	int MINUSPATH = 3;
	int MINPATH = PathNum - MINUSPATH;
	int firId = 0;
	int pretwoTemp = 0, twoTemp = 0, oneTemp = 0;
	if (1) {
		for (i = segNum - 1; i >= MINPATH; i--) {
			for (j = PathNum - 1; j >= MINPATH; j--) {
				for (k = 0; k < run[i].pathdatanum[j]; k++) {
					m = run[i].pathdata[j][k].minuscount + PathNum - 1 - j;
					if (m < MINUSPATH && PathNum - 1 - j < 2) {
						pt[m][ptnum[m]].i = i;
						pt[m][ptnum[m]].j = j;
						pt[m][ptnum[m]].k = k;
						//if(run[i].pathdata[j][k].path[0]==-1) continue;
						//if(nmode==1 && run[i].pathdata[j][k].path[3]==-1) continue;
						//if(nmode==2 && run[i].pathdata[j][k].path[4]==-1) continue;
						pt[m][ptnum[m]].st = run[0].rect.left;
						pt[m][ptnum[m]].ed = run[i].rect.right;
						pt[m][ptnum[m]].recogscore =
								(run[i].pathdata[j][k].recogscore * (j + 1)
										+ (PathNum - 1 - j)
												* (minus1Recog_score))
										/ PathNum;
						pt[m][ptnum[m]].rectScore =
								run[i].pathdata[j][k].rtscore
										+ (PathNum - 1 - j) * minus1Rect_score;
						pt[m][ptnum[m]].score =
								CalcScore( pt[m][ptnum[m]].recogscore,pt[m][ptnum[m]].rectScore);
						pt[m][ptnum[m]].mode = run[i].pathdata[j][k].mode;
						ptnum[m]++;
					}
				}
			}

		}
		for (i = 0; i < 5; i++) {
			for (j = 0; j < ptnum[i]; j++)
				for (k = j + 1; k < ptnum[i]; k++) {
					if (pt[i][j].score > pt[i][k].score) {
						temprt = pt[i][j];
						pt[i][j] = pt[i][k];
						pt[i][k] = temprt;
					}
				}
		}
		for (i = 4; i >= 0; i--) {
			temp = 0;
			twoTemp = 0;
			oneTemp = 0;
			for (j = 0; j < ptnum[i]; j++) {
				//if(j > 0 && abs(pt[i][j].ed - pt[i][0].ed)<2 && pt[i][j].mode == pt[i][0].mode) continue;
				prevPath = &(run[pt[i][j].i].pathdata[pt[i][j].j][pt[i][j].k]);
				{

					int num = GetRecogLetter(pSegLetter, segNum, avCharH, pImg,
							nWidth, nHeight, prevPath->path, &pt[i][j], run,
							prevPath->mode, PathNum);
					oneTemp++;
					if (oneTemp > 20)
						break;
					if (num == 0)
						continue;
					if (j > 0)
						pt[i][temp] = pt[i][j];
					temp++;
					if (i == 0 && temp > 10)
						break;
					if (i >= 1 && temp > 5)
						break;
				}
			}
			ptnum[i] = temp;
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < ptnum[i]; j++) {
				prevPath = &(run[pt[i][j].i].pathdata[pt[i][j].j][pt[i][j].k]);
				if (pt[i][j].score > CalcScore(700,1600))
					continue;
				if ((prevPath->path[0] != -1 || prevPath->path[1] != -1)
						&& pt[i][j].score < maxdist) {
					maxdist = pt[i][j].score;
					maxrectdist = pt[i][j].rectScore;
					maxrecogdis = pt[i][j].recogscore;
					maxi = i;
					maxj = j;
				}
			}
		}

		if (maxi != -1) {
			if (mystrlen(pt[maxi][maxj].sz) > 0
					&& (maxdist < CalcScore(500,1000) && maxrectdist < 1500
							&& maxrecogdis < 760)) {
				mystrcpy(pLP->szLicense, pt[maxi][maxj].sz);
				pLP->rtPlate = pt[maxi][maxj].rt;
				pLP->nLetNum = mystrlen(pLP->szLicense);
				pLP->pfDist = maxdist;
				int trust = int((12000 - maxdist) / 100 + 20);
				pLP->nTrust = max(0,min(99,trust));
				rc = 0;
				goto enddynamic;
				//free(pt[0]);free(pt[1]);free(pt[2]);free(pt[3]);
				//free(run);
				//return 1;
			}
		}
	}

	enddynamic: if (maxi != -1) {
		int len = mystrlen(pt[maxi][maxj].sz);
		if (len > 0
				&& (maxdist < CalcScore(500,1000) && maxrectdist < 1200
						&& maxrecogdis < 400)) {
			rc = 1;
			int maxdis = 0;
			int maxId = 1;
			int maxdis1 = 0;
			int maxId1 = 1;
			int maxdis2 = 0;
			int maxId2 = 1;
			int aveH = pt[maxi][maxj].avH, avegap;
			int aveW = 0;
			//GetRectScore1_SAMZHU(pt[maxi][maxj].rtZi,lstrlen(pt[maxi][maxj].sz),0,&aveH,&avegap);
			for (i = 2; i < PathNum - 1; i++) {
				if (pt[maxi][maxj].dis[i] > maxdis) {
					maxdis = pt[maxi][maxj].dis[i];
					maxId = i;
				}

				CRect rt = pt[maxi][maxj].rtZi[i];
				int hh = rt.Height();
				aveW += rt.Width();
				hh = abs(hh - aveH);
				if (hh > maxdis1) {
					maxdis1 = hh;
					maxId1 = i;
				}

				int cenx = (GetCenterPoint(pt[maxi][maxj].rtZi[i - 1]).x
						+ GetCenterPoint(pt[maxi][maxj].rtZi[i + 1]).x) / 2;
				int realcenx = GetCenterPoint(pt[maxi][maxj].rtZi[i]).x;
				hh = abs(cenx - realcenx);
				if (hh > maxdis2) {
					maxdis2 = hh;
					maxId2 = i;
				}

			}
			aveW /= (PathNum - 3);
			if (maxdis > 500)
				rc = 0;
// 				pt[maxi][maxj].sz[5]=0;
			int ind[100];
			int dist[100];
			for (i = 2; i < len - 1; i++) {
				if (i != maxId1 && i != maxId2 && i != maxId)
					continue;
				CRect rt = pt[maxi][maxj].rtZi[i];
				int hh = rt.Height();
				if (pt[maxi][maxj].dis[i] < 250 && i != maxId1 && i != maxId2)
					continue;
				if (abs(hh - aveH) < aveH / 5 && i != maxId && i != maxId2)
					continue;
				rt.top = (pt[maxi][maxj].rtZi[i - 1].top
						+ pt[maxi][maxj].rtZi[i + 1].top) / 2;
				rt.bottom = (pt[maxi][maxj].rtZi[i - 1].bottom
						+ pt[maxi][maxj].rtZi[i + 1].bottom) / 2;
				if (i == maxId2) {
					rt.left = (GetCenterPoint(pt[maxi][maxj].rtZi[i - 1]).x
							+ GetCenterPoint(pt[maxi][maxj].rtZi[i + 1]).x) / 2
							- aveW / 2;
					rt.right = (GetCenterPoint(pt[maxi][maxj].rtZi[i - 1]).x
							+ GetCenterPoint(pt[maxi][maxj].rtZi[i + 1]).x) / 2
							+ aveW / 2;
				}

				//int dist[100],ind[100];
				CropRecogRegion(pImg, nWidth, nHeight, &rt, DIGIT_MODE, ind,
						dist);
				if (dist[0] < pt[maxi][maxj].dis[i]) {
					pt[maxi][maxj].sz[i] = ConvIndToString(ind[0]);
					pt[maxi][maxj].rtZi[i] = rt;
				}
			}

			if (mystrcmp(pLP->szLicense, pt[maxi][maxj].sz) != 0) {
				mystrcpy(pLP->szLicense1, pLP->szLicense);
				mystrcpy(pLP->szLicense, pt[maxi][maxj].sz);
			} else {
				pLP->szLicense1[0] = 0;
				if (maxrectdist > 900 || maxrecogdis > 350
						|| pt[maxi][maxj].dis[maxId] > 400) {
					CRect rt = pt[maxi][maxj].rtZi[maxId];
					CropRecogRegion(pImg, nWidth, nHeight, &rt, DIGIT_MODE, ind,
							dist);
					pt[maxi][maxj].sz[maxId] = ConvIndToString(ind[1]);
					mystrcpy(pLP->szLicense1, pt[maxi][maxj].sz);
				}

			}

			pLP->rtPlate = pt[maxi][maxj].rt;
			pLP->nLetNum = mystrlen(pLP->szLicense);
			pLP->pfDist = maxdist;
			int trust = int((12000 - maxdist) / 100 + 20);
			pLP->nTrust = max(0,min(99,trust));
			pLP->aveH = pt[maxi][maxj].avH;
			pLP->Type = 0;
			//rc=1;
		}
	}

	free(pt[0]);
	free(pt[1]);
	free(pt[2]);
	free(pt[3]);
	free(pt[4]);
	free(run);

	// pLP->nLetNum = ConvertRealString(pLP->szLicense);
	return rc;
	// return (pLP->nLetNum > 0 &&( maxdist < 1.5 && maxrectdist<0.2 && maxrecogdis< 0.5));

}
void Histogram(BYTE* src, int width, int height, int* hist) {
	int point_pos;
	memset(hist, 0, sizeof(int) * 256);
	for (int y = 0; y < height; y++) {
		point_pos = y * width;
		for (int x = 0; x < width; x++) {
			hist[src[x + point_pos]]++;
		}
	}
	return;
}

void MaxMin(int* src, int len, int* max, int* min, int* total) {
	int max1 = 0, min1 = 255, total1 = 0;
	for (int i = 0; i < len; i++) {
		if (src[i] == 0)
			continue;
		total1 += src[i];
		if (src[i] > max1)
			max1 = src[i];
		if (src[i] < min1)
			min1 = src[i];
	}
	if (max != NULL)
		*max = max1;
	if (min != NULL)
		*min = min1;
	if (total != NULL)
		*total = total1;
	return;
}

void pre_MakeIntImage(BYTE * pbGray, int *pnSum, int nWidth, int nHeight) {
	int nW = nWidth + 1, nH = nHeight + 1;
	int x, y;
	int partialsum;

	memset(pnSum, 0, nW * sizeof(int));
	BYTE* pbGraybuff;
	int* pnSumBuff, *pnSumBuff1;
	for (y = 1; y < nH; y++) {
		pnSum[y * nW] = 0;
		partialsum = 0;
		pbGraybuff = &pbGray[(y - 1) * nWidth];
		pnSumBuff = &pnSum[(y - 1) * nW];
		pnSumBuff1 = &pnSum[y * nW];
		for (x = 1; x < nW; x++) {
			partialsum += (int) pbGraybuff[(x - 1)];
			pnSumBuff1[x] = pnSumBuff[x] + partialsum;
		}
	}
	/*	for (y=nH-2; y<nH; y++)
	 {
	 pnSum[y*nW] = 0;
	 partialsum = 0;
	 pbGraybuff = &pbGray[(y-1)*nWidth];
	 pnSumBuff = &pnSum[(y-1)*nW];
	 for (x=1; x<nW; x++)
	 {
	 partialsum += (int)pbGraybuff[(x-1)];
	 pnSum[y*nW+x] = pnSumBuff[x] + partialsum;
	 }
	 }
	 */
}
void pre_DetectEdgeSpec2(BYTE * pbGray, int * pnSum, int *pnEdge, int nWidth,
		int nHeight) {
	int nW = nWidth + 1;
	int rw = 3, rh = 3;
	int x, y;
	int m3;
	int sx0, sy0, sx1, sy1;
	int w3, w1;

	unsigned int iStartTime = 0, iEndTime = 0;

	//iStartTime = CLK_gethtime() / CLK_countspms();

	m3 = (rw * 2 + 1) * (rh * 2 + 1) - 5;

	if (m_bUseMask == 1 && m_pbMask != NULL) {
		BYTE* pbMask = m_pbMask;
		for (y = rh; y < nHeight - rh; y++) {

			for (x = (rw + 1); x < nWidth - rw; x++) {
				if (pbMask[y * nWidth + (x)] == 0) {
					pnEdge[y * nWidth + (x)] = 0;
					continue;
				}
				sx0 = x - rw;
				sy0 = y - rh;
				sx1 = x + rw;
				sy1 = y + rh;

				w1 = (pbGray[(y - 1) * nWidth + x]
						+ pbGray[y * nWidth + (x - 1)] + pbGray[y * nWidth + x]
						+ pbGray[y * nWidth + (x + 1)]
						+ pbGray[(y + 1) * nWidth + x]);
				w3 = (pnSum[(sy1 + 1) * nW + (sx1 + 1)]
						- pnSum[sy0 * nW + (sx1 + 1)]
						- pnSum[(sy1 + 1) * nW + sx0] + pnSum[sy0 * nW + sx0]
						- w1) / 44;

				pnEdge[y * nWidth + (x)] = (w3) - (w1 / 5);

			}
		}
	} else {
		BYTE* _pbGray;
		int* _pnSum, *_pnSum1;
		int *_pnEdge;
		for (y = rh; y < nHeight - rh; y++) {
			sy0 = y - rh;
			sy1 = y + rh;
			_pbGray = &pbGray[y * nWidth];
			_pnSum = &pnSum[(sy1 + 1) * nW];
			_pnSum1 = &pnSum[sy0 * nW];
			_pnEdge = &pnEdge[y * nWidth];

			for (x = (rw + 1); x < nWidth - rw; x++) {
				sx0 = x - rw;
				sx1 = x + rw;

				w1 = (_pbGray[-nWidth + x] + _pbGray[(x - 1)] + _pbGray[x]
						+ _pbGray[x + 1] + _pbGray[nWidth + x]);

				w3 = (_pnSum[(sx1 + 1)] - _pnSum1[(sx1 + 1)] - _pnSum[sx0]
						+ _pnSum1[sx0] - w1) / 44;

				_pnEdge[(x)] = (w3) - (w1 / 5);

			}
		}
	}

	//iEndTime = CLK_gethtime() / CLK_countspms();
	//printf("=======================0 is %d\n", iEndTime - iStartTime);

	SubBinary2(pbGray, pnSum, pnEdge, nWidth, nHeight, 0, nWidth - 1, 0, rh - 1,
			rw, rh);
	SubBinary2(pbGray, pnSum, pnEdge, nWidth, nHeight, 0, nWidth - 1,
			nHeight - rh, nHeight - 1, rw, rh);
	SubBinary2(pbGray, pnSum, pnEdge, nWidth, nHeight, 0, rw, rh, nHeight - rh,
			rw, rh);
	SubBinary2(pbGray, pnSum, pnEdge, nWidth, nHeight, nWidth - rw, nWidth - 1,
			rh, nHeight - rh, rw, rh);
}
void SubBinary2(BYTE * pbGray, int * pnSum, int* lpOut, int nWidth, int nHeight,
		int x0, int x1, int y0, int y1, int rw, int rh) {
	int nW = nWidth + 1;
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++) {
			int sx0, sy0, sx1, sy1;
			int w3, n3;
			int w1 = 0, n1 = 0;
			sx0 = x - rw;
			if (sx0 < 0)
				sx0 = 0;
			sy0 = y - rh;
			if (sy0 < 0)
				sy0 = 0;
			sx1 = x + rw;
			if (sx1 >= nWidth)
				sx1 = nWidth - 1;
			sy1 = y + rh;
			if (sy1 >= nHeight)
				sy1 = nHeight - 1;
			n3 = (sx1 - sx0 + 1) * (sy1 - sy0 + 1);
			w3 = pnSum[(sy1 + 1) * nW + (sx1 + 1)] - pnSum[sy0 * nW + (sx1 + 1)]
					- pnSum[(sy1 + 1) * nW + sx0] + pnSum[sy0 * nW + sx0];
			if (x - 1 >= 0)
				n3--, w3 -= pbGray[y * nWidth + (x - 1)], n1++, w1 += pbGray[y
						* nWidth + (x - 1)];
			if (x + 1 < nWidth)
				n3--, w3 -= pbGray[y * nWidth + (x + 1)], n1++, w1 += pbGray[y
						* nWidth + (x + 1)];
			if (y - 1 >= 0)
				n3--, w3 -= pbGray[(y - 1) * nWidth + x], n1++, w1 += pbGray[(y
						- 1) * nWidth + x];
			if (y + 1 < nHeight)
				n3--, w3 -= pbGray[(y + 1) * nWidth + x], n1++, w1 += pbGray[(y
						+ 1) * nWidth + x];
			n3--, w3 -= pbGray[y * nWidth + x], n1++, w1 += pbGray[y * nWidth
					+ x];
			w1 /= n1;
			w3 /= n3;
			lpOut[y * nWidth + x] = w3 - w1;
		}
	}
}
void pre_MakeMonochrome3(int* pnEdge, BYTE *pbEdge, int nWidth, int nHeight,
		RECT rtReg, int mode) {
	int x, y;
	int* _pnEdge;
	BYTE* _pbEdge;
	int pnThres = 3;
	if (mode == 1) {
		for (y = rtReg.top; y <= rtReg.bottom; y++) {
			_pnEdge = &pnEdge[y * nWidth];
			_pbEdge = &pbEdge[y * nWidth];
			for (x = rtReg.left; x <= rtReg.right; x++) {

				if (_pnEdge[x] < -pnThres)
					_pbEdge[x] = 1;
				else
					_pbEdge[x] = 0;
			}
		}
	} else {
		for (y = rtReg.top; y <= rtReg.bottom; y++) {
			_pnEdge = &pnEdge[y * nWidth];
			_pbEdge = &pbEdge[y * nWidth];
			for (x = rtReg.left; x <= rtReg.right; x++) {
				if (_pnEdge[x] > pnThres)
					_pbEdge[x] = 1;
				else
					_pbEdge[x] = 0;
			}
		}
	}

	/*	for(y = rtReg.bottom-1;  y <= rtReg.bottom; y++)
	 {
	 for(x = rtReg.left; x <= rtReg.right;  x++)
	 {
	 if(		pnEdge[y*nWidth+x] >  pnThres)	pbEdge[y*nWidth+x] = 0;
	 else if(pnEdge[y*nWidth+x] < -pnThres)	pbEdge[y*nWidth+x] = 255;
	 else									pbEdge[y*nWidth+x] = 128;
	 }
	 }
	 */
}
#define NORMALIZEDIS(a) (sqrtf(max(a,0))*1000 / 55.0f)
int m_idx[100];
int RecogLetterSAMZHU(BYTE *lpImg, int nWidth, int nHeight, int nMode,
		int* lpCandiNum, int *lpDis, BOOL bOut) {
	if (lpImg == NULL)
		return 0;

	int i;
	lpCandiNum[0] = 0;
	lpDis[0] = 100000;
	if (!nWidth || !nHeight || !lpImg)
		return 0;
	if (nWidth * 100 / nHeight < 20) {
		lpCandiNum[0] = 1;
		if ((nMode & DIGIT_MODE)) {
			lpDis[0] = 700;
			return 1;
		} else {
			lpDis[0] = 1000;
			return 0;
		}
	}
	int si, ei;
// 	int* idx = m_idx;
//     memset(idx,0,sizeof(int)*100);

	//   si = 0; ei = 9;

//     if(nMode == DIGIT_MODE)				{ si=0;  ei=10-1; }
// 	if(nMode == ENG_MODE)				{ si=10;  ei=20-1; }
//	for(i=si;i<=ei;i++)idx[i] = 1;

	RecogMQDFCharImg(lpImg, nWidth, nHeight);
	//RecogMQDFCharImg(lpImg,nWidth,nHeight,idx);

	if (nMode == ALL_MODE) {
		int CandNum = min(3,m_Hubo.nCandNum);
		memcpy(lpCandiNum, m_Hubo.Index, sizeof(int) * CandNum);
		// memcpy(lpDis,m_Hubo.Dis,sizeof(float)*CandNum);
		for (i = 0; i < CandNum; i++) {
			lpDis[i] = (int) NORMALIZEDIS(m_Hubo.Dis[i]);
			if (m_Hubo.Index[i] == 1)
				lpDis[i] += 50;
		}
		//if(lpCandiNum[0]==1 && nWidth*5>nHeight*3) lpDis[0] = 10000;
		return CandNum;
	}
	int CandNum = min(2,m_Hubo.nCandNum);

	for (i = 0; i < CandNum; i++) {
		lpCandiNum[i] = m_Hubo.Index[i];
		lpDis[i] = (int) NORMALIZEDIS(m_Hubo.Dis[i]);
		if (m_Hubo.Index[i] == 1)
			lpDis[i] += 50;
		//if(lpCandiNum[0]==1 && nWidth*5>nHeight*3) lpDis[0] = 10000;

	}
	return CandNum;
	// return 0;
}
double GetSkewAngle_CCH(BYTE* pImg, int w, int h) {

	int i, j, k, step;
	int* sh = (int*) malloc(sizeof(int) * (h + 10));
	int* sh1 = (int*) malloc(sizeof(int) * (h + 10));
	memset(sh, 0, sizeof(int) * (h + 10));
	memset(sh1, 0, sizeof(int) * (h + 10));
	BYTE* pGrImg = (BYTE*) malloc(w * h);
	memset(pGrImg, 0, w * h);
	for (i = 1; i < h / 2; i++)
		for (j = 0; j < w; j++) {
			if (pImg[i * w + j] > pImg[(i - 1) * w + j])
				pGrImg[(i - 1) * w + j] = 1;

		}

	int MaxOffY = w / 4;
	int *yoff = (int*) malloc(sizeof(int) * w * (MaxOffY * 2 + 1));
	memset(yoff, 0, sizeof(int) * w * (MaxOffY * 2 + 1));
	int *sha = (int*) malloc(sizeof(int) * h * (MaxOffY * 2 + 1));
	memset(sha, 0, sizeof(int) * h * (MaxOffY * 2 + 1));
	int yy;
	int* yofftemp = yoff;
	for (i = 0; i < w; i++)
		for (j = -MaxOffY; j <= MaxOffY; j++) {
			*yofftemp = (j * i) / (w - 1);
			yofftemp++;
		}
	int *pDetectLine;
	for (i = 0; i < w; i++) {
		pDetectLine = &yoff[i * (MaxOffY * 2 + 1)];
		for (j = 0; j < h; j++) {
			if (pGrImg[j * w + i] == 0)
				continue;
			for (k = -MaxOffY; k <= MaxOffY; k++) {
				//j + k + g_DetectLine[i][k+MAXHEIGHTDIFF][0];
				yy = j - pDetectLine[k + MaxOffY];
				if (yy >= 0 && yy < h)
					sha[(k + MaxOffY) * h + yy] += 1;
			}
		}
	}
	int y;
	float fscore, maxfscore = 0;
	int* ord = (int*) malloc(sizeof(int) * h);
	int* shTemp;
	int offY = 0;
	for (step = -MaxOffY; step <= MaxOffY; step++) {
		shTemp = &sha[(step + MaxOffY) * h];
		GetSortValueOrder(shTemp, ord, h, 1);
		fscore = 0;
		for (i = 0; i < 4; i++) {
			fscore += shTemp[ord[i]];
		}
		if (maxfscore < fscore) {
			offY = step;
			maxfscore = fscore;
		}
	}
	free(yoff);
	free(ord);
	free(sha);
	free(pGrImg);
	free(sh);
	free(sh1);
	//delete pImg;
	double fAng = atan(offY / double(w)) * 180 / 3.141592;
	return fAng;
}
BYTE* Rotate_GrayImg(BYTE *pImg, int &w, int &h, double fAng,
		BYTE GrayBackGround) {
	BYTE *pNewImg;
	int w0 = w;
	int h0 = h;
	int i;
	if (!pImg)
		return NULL;

	if (fabs(fAng) < 0.05) {
		BYTE* pNewImg = new BYTE[w0 * h0];
		memcpy(pNewImg, pImg, w0 * h0);
		return pNewImg;
	}

	double ang = fAng * acos(0.0f) / 90.0f; //convert angle to radians and invert (positive angle performs clockwise rotation)
	float cos_angle = (float) cos(ang); //these two are needed later (to rotate)
	float sin_angle = (float) sin(ang);

	double pX[4] = { -0.5f, w0 - 0.5f, -0.5f, w0 - 0.5f };
	double pY[4] = { -0.5f, -0.5f, h0 - 0.5f, h0 - 0.5f };

	double newpX[4] = { 0 };
	double newpY[4] = { 0 };

	for (i = 0; i < 4; i++) {
		newpX[i] = pX[i];
		newpY[i] = pY[i];
	}
	//(read new dimensions from location of corners)
	float minx = (float) min(min(newpX[0],newpX[1]),min(newpX[2],newpX[3]));
	float miny = (float) min(min(newpY[0],newpY[1]),min(newpY[2],newpY[3]));
	float maxx = (float) max(max(newpX[0],newpX[1]),max(newpX[2],newpX[3]));
	float maxy = (float) max(max(newpY[0],newpY[1]),max(newpY[2],newpY[3]));

	int newWidth = (int) floor(maxx - minx + 0.5f);
	int newHeight = (int) floor(maxy - miny + 0.5f);

	w = newWidth;
	h = newHeight;

	float ssx = ((maxx + minx) - ((float) newWidth - 1)) / 2.0f; //start for x
	float ssy = ((maxy + miny) - ((float) newHeight - 1)) / 2.0f; //start for y

	float newxcenteroffset = 0.5f * newWidth;
	float newycenteroffset = 0.5f * newHeight;
	ssx -= 0.5f * w0;
	ssy -= 0.5f * h0;

	pNewImg = new BYTE[w * h];
	memset(pNewImg, 0, w * h);

	float x, y; //destination location (float, with proper offset)
	float origx, origy;
	int nearx, neary; //origin location
	int destx, desty; //destination location
	double t1, t2, a, b, c, d;

	y = ssy;

	for (desty = 0; desty < h; desty++) {
		x = ssx;
		for (destx = 0; destx < w; destx++) {

			origx = (cos_angle * x + sin_angle * y);
			origy = (cos_angle * y - sin_angle * x);

			origx += newxcenteroffset;
			origy += newycenteroffset;

			nearx = (int) origx;
			if (nearx < 0)
				nearx--;
			neary = (int) origy;
			if (neary < 0)
				neary--;

			t1 = origx - nearx;
			t2 = origy - neary;

			d = t1 * t2;
			b = t1 - d;
			c = t2 - d;
			a = 1 - t1 - c;

			if (nearx > 0 && nearx <= w0 - 1 && neary > 0 && neary <= h0 - 1) {
				BYTE a1, b1, c1, d1;
				if (nearx == w0 - 1) {
					if (neary == h0 - 1)
						a1 = b1 = c1 = d1 = pImg[nearx + neary * w0];
					else {
						a1 = pImg[nearx + neary * w0];
						b1 = pImg[nearx + neary * w0];
						c1 = pImg[nearx + (neary + 1) * w0];
						d1 = pImg[nearx + (neary + 1) * w0];
					}
				} else if (neary == h0 - 1) {
					if (nearx == w0 - 1)
						a1 = b1 = c1 = d1 = pImg[nearx + neary * w0];
					else {
						a1 = pImg[nearx + neary * w0];
						b1 = pImg[nearx + 1 + neary * w0];
						c1 = pImg[nearx + neary * w0];
						d1 = pImg[nearx + 1 + neary * w0];
					}
				} else {
					a1 = pImg[nearx + neary * w0];
					b1 = pImg[nearx + 1 + neary * w0];
					c1 = pImg[nearx + (neary + 1) * w0];
					d1 = pImg[nearx + 1 + (neary + 1) * w0];
				}
				int tt = (int) (a * a1 + b * b1 + c * c1 + d * d1 + 0.5);
				pNewImg[desty * w + destx] = max(0,min(255,tt));
			} else
				pNewImg[desty * w + destx] = GrayBackGround;
			x++;
		}
		y++;
	}

	return pNewImg;
}
BYTE* RotateImg(double fAng, BYTE* pImg, int w, int h) {

	int i, j, y, x, d;
	int xs, ys, m, n;
	BYTE *pNewImg;
	double c, s;

	fAng *= 0.01745;
	xs = w / 2;
	ys = h / 2;
	c = cos(fAng);
	s = sin(fAng);

	pNewImg = new BYTE[w * h];
	memset(pNewImg, 0, w * h);

	for (i = -ys; i < ys; i++) {
		for (j = -xs; j < xs; j++) {
			y = (int) (j * s + i * c);
			x = (int) (j * c - i * s);
			if (y > 0)
				m = y;
			else
				m = y - 1;
			if (x > 0)
				n = x;
			else
				n = x - 1;

			if (m >= -ys && m < ys && n >= -xs && n < xs)
				d = pImg[(m + ys) * w + n + xs];
			else
				d = 128;
			if (d < 0)
				d = 0;
			pNewImg[(i + ys) * w + j + xs] = d;
		}
	}

	return pNewImg;
}
