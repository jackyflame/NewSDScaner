// ImageBase.cpp: implementation of the CImageBase class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ImageBase.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

//#define RGB2GRAY(r,g,b) (((b)*114 + (g)*587 + (r)*299)/1000)
#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)
bool IsIntersectRect(RECT x, RECT y) {
	if (min(x.bottom, y.bottom) - max(x.top, y.top) > 0
			&& min(x.right, y.right) - max(x.left, y.left) > 0)
		return true;
	return false;
}
BYTE* CropImg(BYTE* pImg, int w, int h, RECT* r) {
	int i, j;
	int w1;
	int h1;
	BYTE* pCropImg;

	if (pImg == NULL)
		return NULL;

	r->left = max(0,r->left);
	r->top = max(0,r->top);
	r->right = min(w,r->right);
	r->bottom = min(h,r->bottom);
	w1 = r->right - r->left;
	h1 = r->bottom - r->top;
	if (w1 == 0 || h1 == 0)
		return NULL;
	pCropImg = (BYTE*) malloc(w1 * h1);
	for (i = r->top; i < r->bottom; ++i)
		for (j = r->left; j < r->right; ++j) {
			pCropImg[(i - r->top) * w1 + j - r->left] = pImg[i * w + j];
		}
	return pCropImg;
}
BYTE* ZoomImgSimple(BYTE* pImg, int *w, int *h) {
	int diff = 0, diff1 = 0, diff2 = 0;
	int i, j;
	int newH = *h / 2, newW = *w / 2;
	BYTE* pNewImg = (BYTE*) malloc(newH * newW);
	int ii = 0, jj = 0;
	for (i = 0; i < newH; i++) {
		ii = i * 2;
		for (j = 0; j < newW; j++) {
			jj = j * 2;
			pNewImg[i * newW + j] = pImg[ii * (*w) + jj];
		}
	}
	*w = newW;
	*h = newH;
	return pNewImg;
}
void ZoomInImg(BYTE *pImg, int w, int h, int new_w, int new_h) {
	if (pImg == NULL)
		return;

	int i, j;
	int i_x, i_y;
	int f_x, f_y, d_x, d_y;
	int val00, val01, val10, val11;

	int xscale = 1000 * w / new_w;
	int yscale = 1000 * h / new_h;

	BYTE *pOrgPlusImg = new BYTE[(w + 2) * (h + 2)];
	memset(pOrgPlusImg, 0, (w + 2) * (h + 2));
	for (i = 1; i <= h; i++) {
		memcpy(pOrgPlusImg + i * (w + 2) + 1, pImg + (i - 1) * w, w);
	}

	for (i = 0; i < new_h; i++) {
		f_y = yscale * i + yscale / 2 + 500;
		i_y = f_y / 1000;
		d_y = f_y - i_y * 1000;
		for (j = 0; j < new_w; j++) {
			f_x = xscale * j + xscale / 2 + 500;
			i_x = f_x / 1000;
			d_x = f_x - i_x * 1000;

			val00 = pOrgPlusImg[i_y * (w + 2) + i_x];
			val01 = pOrgPlusImg[i_y * (w + 2) + (i_x + 1)];
			val10 = pOrgPlusImg[(i_y + 1) * (w + 2) + i_x];
			val11 = pOrgPlusImg[(i_y + 1) * (w + 2) + (i_x + 1)];

			int newval = (int) ((val00 * (1000 - d_y) * (1000 - d_x)
					+ val01 * (1000 - d_y) * d_x + val10 * d_y * (1000 - d_x)
					+ val11 * d_y * d_x));
			newval /= 1000000;
			if (newval > 255)
				newval = 255;
			pImg[i * new_w + j] = (BYTE) newval;
		}
	}
	delete[] pOrgPlusImg;
	return;
}
void GetWidthHeight(BYTE *pDib, int* w, int* h) {
	LPBITMAPINFOHEADER lpBIH;
	if (pDib == NULL)
		return;
	lpBIH = (LPBITMAPINFOHEADER) pDib;
	*w = (int) lpBIH->biWidth;
	*h = (int) lpBIH->biHeight;
}
int GetBitCount(BYTE *pDib) {
	LPBITMAPINFOHEADER lpBIH = (LPBITMAPINFOHEADER) pDib;
	return (int) lpBIH->biBitCount;
}
BYTE* Get_lpBits(BYTE *pDib) {
	LPBITMAPINFOHEADER lpBIH = (LPBITMAPINFOHEADER) pDib;
	int QuadSize;
	BYTE* lpBits;

	if (lpBIH->biBitCount == 1)
		QuadSize = sizeof(RGBQUAD) * 2;
	else if (lpBIH->biBitCount == 2)
		QuadSize = sizeof(RGBQUAD) * 4;
	else if (lpBIH->biBitCount == 4)
		QuadSize = sizeof(RGBQUAD) * 16;
	else if (lpBIH->biBitCount == 8)
		QuadSize = sizeof(RGBQUAD) * 256;
	else
		QuadSize = 0; //16,24,32
	lpBits = (BYTE*) (pDib + sizeof(BITMAPINFOHEADER) + QuadSize);
	return lpBits;
}

BYTE* Binarization_By_Th(BYTE *pImg, int w, int h, int th) {
	int i, j;
	BYTE *pBinImg = (BYTE*) malloc(w * h);
	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
			if (pImg[i * w + j] <= th)
				pBinImg[i * w + j] = LOW_LEVEL;
			else
				pBinImg[i * w + j] = HIGH_LEVEL;
	return pBinImg;
}
int GetThreshold_Otsu1(BYTE *pImg, int w, int h) {
	int dist;
	RECT sr;
	sr.left = 0;
	sr.top = 0;
	sr.right = w;
	sr.bottom = h;
	return GetThreshold_Otsu4(pImg, w, h, &dist, sr);
}
int GetThreshold_Otsu2(BYTE *pImg, int w, int h, RECT subrect) {
	int dist;

	return GetThreshold_Otsu4(pImg, w, h, &dist, subrect);
}
int GetThreshold_Otsu3(BYTE *pImg, int w, int h, int* dist) {
	RECT sr;
	sr.left = 0;
	sr.top = 0;
	sr.right = w;
	sr.bottom = h;
	return GetThreshold_Otsu4(pImg, w, h, dist, sr);
}

int GetThreshold_Otsu4(BYTE *pImg, int w, int h, int* dist, RECT subrect) {
	int xs, xe, ys, ye;
	int i, j, k, n[256];

	if (pImg == NULL)
		return -1;
	if (subrect.left < 0)
		subrect.left = 0;
	if (subrect.top < 0)
		subrect.top = 0;
	if (subrect.right >= w)
		subrect.right = w - 1;
	if (subrect.bottom >= h)
		subrect.bottom = h - 1;

	xs = subrect.left;
	ys = subrect.top;
	xe = subrect.right;
	ye = subrect.bottom;

	//1. Original histogram
	for (i = 0; i < 256; i++)
		n[i] = 0;
	for (i = ys; i <= ye; i++)
		for (j = xs; j <= xe; j++) {
			k = (int) pImg[i * w + j];
			n[k]++; //gray histogram
		}

	return GetThreshold_Otsu_From_Histogram(n, dist);
}

int GetThreshold_Otsu_From_Histogram(int* Hist, int* dist) {
	int i, tmin, tmax, t;
	int s;
	float m0, d0, WW, m, beta, p0;
	int s1 = 0;
	int nIgnoe;

	t = 0;

	for (i = 0; i < 256; i++)
		t += Hist[i];

	nIgnoe = (int) sqrt((float) t);

	t = 0;
	tmin = 0;
	for (i = 0; i < 256; i++) {
		t += Hist[i];
		if (t > nIgnoe) {
			tmin = i;
			break;
		} // min gray level
	}
	t = 0;
	tmax = 255;
	for (i = 255; i > 0; i--) {
		t += Hist[i];
		if (t > nIgnoe) {
			tmax = i;
			break;
		} // max gray level
	}
	if ((tmax - tmin) <= 0)
		return tmin; // no object

	//2. Binarization
	// total mean value of image
	s = 0;
	t = 0;
	for (i = tmin; i <= tmax; i++) {
		t += i * Hist[i];
		s += Hist[i];
	}

	for (i = 0; i < 256; ++i) {
		if (Hist[i] > 0)
			s1++;
	}
	if (s1 <= 2)
		return (t / s);
	m0 = (float) t / s; //total mean gray level value

	// Optimal threshold value determination
	WW = m = beta = d0 = 0;
	t = tmin;
	for (i = tmin; i < tmax; i++) {
		if (Hist[i] == 0)
			continue;
		p0 = (float) Hist[i] / (float) s;
		WW = WW + p0;
		m = m + i * p0;
		if (WW >= 0.999999)
			break; //2000.12.7
		d0 = (m0 * WW - m) * (m0 * WW - m) / (WW * (1 - WW));
		//To avoid difference between DEBUG and RELEASE
		if (beta < d0) {
			beta = d0;
			t = i;
		}
	}
	*dist = (int) beta;
	return t;
}

BYTE* Binarization_DynamicThreshold1(BYTE* pImg, int w, int h, int nGridX,
		int nGridY, int special_mode/*=SMODE_NON*/) {
	int winX = 2 * w / nGridX;
	int winY = 2 * h / nGridY;
	return Binarization_DynamicThreshold2(pImg, w, h, winX, winY, nGridX,
			nGridY, special_mode);
}
BYTE* Binarization_DynamicThreshold2(BYTE* pImg, int w, int h, int winX,
		int winY, int nGridX, int nGridY, int special_mode/*=SMODE_NON*/) {
	int i, j;
	BYTE *pBinImg;
	int globalTh;
	int *Dist;
	int *Ths;
	int SumDist = 0;
	int y_step;
	int x_step;

	if (pImg == NULL)
		return NULL;
	if (winX <= 0 || winY <= 0)
		return NULL;
	if (winX > w || winY > h)
		return NULL;
	if (nGridX <= 0 || nGridY <= 0)
		return NULL;

	pBinImg = (BYTE*) malloc(w * h);
	if (pBinImg == NULL)
		return NULL;

	Ths = (int*) malloc(sizeof(int) * nGridX * nGridY);
	Dist = (int*) malloc(sizeof(int) * nGridX * nGridY);

	y_step = h / (nGridY);
	x_step = w / (nGridX);

	if (special_mode == SMODE_GLOBAL_OTSU
			|| special_mode == SMODE_SMALL_DIST_GLOBAL_OTSU)
		globalTh = GetThreshold_Otsu1(pImg, w, h);

	for (i = 0; i < nGridY; i++)
		for (j = 0; j < nGridX; j++) {
			RECT rect;
			int y = y_step * i;
			int x = x_step * j;
			rect.top = y - winY / 2;
			rect.bottom = y + winY / 2;
			rect.left = x - winX / 2;
			rect.right = x + winX / 2;
			if (rect.top < 0) {
				rect.bottom -= rect.top;
				rect.top = 0;
			}
			if (rect.left < 0) {
				rect.right -= rect.left;
				rect.left = 0;
			}
			if (rect.bottom >= h) {
				rect.top -= (rect.bottom - h + 1);
				rect.bottom = h - 1;
			}
			if (rect.right >= w) {
				rect.left -= (rect.right - w + 1);
				rect.right = w - 1;
			}
			if (rect.left < h) {
				rect.right += (h - rect.left);
				rect.left = h;
			}
			if (rect.right > w - h) {
				rect.left -= (rect.right - (w - h));
				rect.right = w - h;
			}
			Ths[i * nGridX + j] = GetThreshold_Otsu4(pImg, w, h,
					&(Dist[i * nGridX + j]), rect);
			SumDist += Dist[i * nGridX + j];
			if (special_mode == SMODE_GLOBAL_OTSU
					|| special_mode == SMODE_SMALL_DIST_GLOBAL_OTSU)
				if (Ths[i * nGridX + j] > globalTh)
					Ths[i * nGridX + j] = (int) (globalTh
							+ (Ths[i * nGridX + j] - globalTh) * 0.3);

		}
	if (special_mode == SMODE_SMALL_DIST
			|| special_mode == SMODE_SMALL_DIST_GLOBAL_OTSU) {
		SumDist /= nGridY * nGridX;
		for (i = 0; i < nGridY; i++)
			for (j = 0; j < nGridX; j++)
				if (Dist[i * nGridX + j] < SumDist / 10)
					Ths[i * nGridX + j] = -1;
	}

	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++) {
			int Th = 0;
			int ii = i / y_step;
			int jj = j / x_step;
			int di, dj;
			if (ii >= nGridY - 1) {
				ii = nGridY - 2;
				di = y_step;
			} else
				di = i - ii * y_step;

			if (jj >= nGridX - 1) {
				jj = nGridX - 2;
				dj = x_step;
			} else
				dj = j - jj * x_step;

			Th = (Ths[ii * nGridX + jj] * (y_step - di) * (x_step - dj)
					+ Ths[(ii + 1) * nGridX + jj] * di * (x_step - dj)
					+ Ths[ii * nGridX + jj + 1] * (y_step - di) * dj
					+ Ths[(ii + 1) * nGridX + jj + 1] * di * dj) / y_step
					/ x_step;

			if (pImg[i * w + j] <= Th)
				pBinImg[i * w + j] = LOW_LEVEL;
			else
				pBinImg[i * w + j] = HIGH_LEVEL;
		}
	free(Ths);
	free(Dist);
	return pBinImg;
}
BYTE* Binarization_HJI(BYTE *pImg, int w, int h) {
	int i, j;
	int ii, jj;
	int meanV;
	int hh[5][5] = { { -1, -1, -1, -1, -1 }, { -1, -2, -2, -2, -1 }, { -1, -2,
			35, -2, -1 }, { -1, -2, -2, -2, -1 }, { -1, -1, -1, -1, -1 }, };

	if (!pImg)
		return NULL;

	int N = w * h;

	BYTE* pImgT1 = new BYTE[N];
	memset(pImgT1, 0, N);

	//Binarization  of image by BST method
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			meanV = 0;
			if (i < 2 || i > h - 3 || j < 2 || j > w - 3) {
				meanV = 255;
			} else {
				for (ii = -2; ii < 3; ii++) {
					for (jj = -2; jj < 3; jj++) {
						meanV += pImg[(i + ii) * w + j + jj]
								* hh[ii + 2][jj + 2];
					}
				}
			}
			pImgT1[i * w + j] = meanV > 128 ? HIGH_LEVEL : LOW_LEVEL;
		}
	}
	return pImgT1;
}
BYTE* Binarization_Camera(BYTE *pImg, int w, int h) {
	BYTE* pMeanImg = MeanFilter(pImg, w, h, 50);
	if (pMeanImg == NULL)
		return NULL;
	int i, j;
	for (i = 0; i < h * w; i++) {
		if (pImg[i] > pMeanImg[i])
			pMeanImg[i] = 255;
		else
			pMeanImg[i] = 255 + pImg[i] - pMeanImg[i];
	}

	int th = GetThreshold_Otsu1(pMeanImg, w, h);
	BYTE *pBinImg = Binarization_By_Th(pMeanImg, w, h, th);
	delete[] pMeanImg;
	return pBinImg;
}
BYTE* MeanFilter(BYTE* pImg, int w, int h, int nWinSize) {
	if (pImg == NULL || w < 1 || h < 1)
		return NULL;
	if (nWinSize >= w - 1 || nWinSize >= h - 1)
		nWinSize = min(w,h) - 1;
	int i, j;
	int nSum = 0;
	int divid = (2 * nWinSize + 1) * (2 * nWinSize + 1);
	int* pTempImg = new int[w * h];
	BYTE* pOutImg = new BYTE[w * h];
	int p1, p2;
	for (i = 0; i < h; i++) {
		int iw = i * w;
		nSum = pImg[iw];
		for (j = 1; j <= nWinSize; j++)
			nSum += pImg[iw + j] + pImg[iw + j];
		pTempImg[iw] = nSum;
		for (j = 1; j < w; j++) {
			p1 = j - nWinSize - 1;
			if (p1 < 0)
				p1 = -p1;
			p2 = j + nWinSize;
			if (p2 >= w)
				p2 = w + w - p2 - 2;
			nSum -= pImg[iw + p1];
			nSum += pImg[iw + p2];
			pTempImg[iw + j] = nSum;
		}
	}
	for (j = 0; j < w; j++) {
		nSum = pTempImg[j];
		for (i = 1; i <= nWinSize; i++)
			nSum += pTempImg[i * w + j] + pTempImg[i * w + j];
		pOutImg[j] = BYTE(nSum / divid);
		for (i = 1; i < h; i++) {
			p1 = i - nWinSize - 1;
			if (p1 < 0)
				p1 = -p1;
			p2 = i + nWinSize;
			if (p2 >= h)
				p2 = h + h - p2 - 2;
			nSum -= pTempImg[p1 * w + j];
			nSum += pTempImg[p2 * w + j];
			pOutImg[i * w + j] = BYTE(nSum / divid);
		}
	}
	delete[] pTempImg;
	return pOutImg;
}

void MeanFilter(BYTE* pImg, int w, int h) {
	int i, j;
	int* NewImg = (int*) malloc(sizeof(int) * w * h);
	int p, pix = w * h;
	for (i = 1; i < pix - 1; i++) {
		NewImg[i] = pImg[i - 1] + pImg[i] + pImg[i + 1];
	}
	for (i = 1; i < h - 1; i++) {
		for (j = 1; j < w - 1; j++) {
			p = i * w + j;
			pImg[p] = (BYTE) ((NewImg[p - w] + NewImg[p] + NewImg[p + w]) / 9);
		}
	}
	free(NewImg);
}
void GetSortValueOrder(int* fValue, int* Ord, int n, int Direct/*=0*/) {
	int i, j, tm;
	int d;
	for (i = 0; i < n; i++)
		Ord[i] = i; //distance  number
	if (Direct == 0) {
		for (i = 0; i < n; i++) {
			d = fValue[Ord[i]];
			for (j = i + 1; j < n; j++) {
				if (d > fValue[Ord[j]]) {
					tm = Ord[j];
					Ord[j] = Ord[i];
					Ord[i] = tm;
					d = fValue[Ord[i]];
				}
			}
		}
	} else {
		for (i = 0; i < n; i++) {
			d = fValue[Ord[i]];
			for (j = i + 1; j < n; j++) {
				if (d < fValue[Ord[j]]) {
					tm = Ord[j];
					Ord[j] = Ord[i];
					Ord[i] = tm;
					d = fValue[Ord[i]];
				}
			}
		}
	}
}
void GetHistogram(BYTE *pImg, int w, int h, RECT subRt, int Hist[256],
		int* tmin, int* tmax) {
	int x0 = subRt.left, x1 = subRt.right, y0 = subRt.top, y1 = subRt.bottom;
	int wid = x1 - x0, high = y1 - y0;
	int i, j, k = 0;
	int Th = wid; //*2;

	memset(Hist, 0, sizeof(int) * 256);
	for (i = y0; i < y1; i++)
		for (j = x0; j < x1; j++)
			Hist[pImg[i * w + j]]++;
	k = 0;
	for (i = 0; i < 256; i++) {
		k += Hist[i];
		if (k > (Th)) {
			*tmin = i;
			break;
		} // min gray level
	}
	k = 0;
	for (i = 255; i >= 0; i--) {
		k += Hist[i];
		if (k > (Th)) {
			*tmax = i;
			break;
		} // max gray level
	}
}
BYTE* Binarization_Windows(BYTE* pImg, int w, int h, int nWinSize) {
	if (!pImg || w < 1 || h < 1)
		return NULL;
	int nWindowSize = nWinSize;
	int i = 0, j = 0, i1, j1;
	int nSum = 0, nTemp = 0;
	int divid = (2 * nWindowSize + 1) * (2 * nWindowSize + 1) - 1;

	int ww = w + 2 * nWindowSize + 1;
	int hh = h + 2 * nWindowSize;
	BYTE* temp = new BYTE[ww * hh];
	memset(temp, 255, ww * hh);
	for (i = 0; i < h; i++)
		memcpy(temp + (i + nWindowSize) * ww + nWindowSize + 1, pImg + i * w,
				w);

	BYTE* pbImage = new BYTE[w * h];

	for (i = nWindowSize; i < hh - nWindowSize; i++) {
		nSum = 0;
		for (i1 = i - nWindowSize; i1 <= i + nWindowSize; i1++)
			for (j1 = 0; j1 < 2 * nWindowSize + 1; j1++) {
				nSum += temp[i1 * ww + j1];
			}
		for (j = nWindowSize + 1; j < ww - nWindowSize; j++) {
			for (i1 = i - nWindowSize; i1 <= i + nWindowSize; i1++) {
				nSum -= temp[i1 * ww + j - nWindowSize - 1];
				nSum += temp[i1 * ww + j + nWindowSize];
			}
			nTemp = (i - nWindowSize) * w + (j - nWindowSize - 1);
			if (pImg[nTemp] + 10 < (nSum - pImg[nTemp]) / divid)
				pbImage[nTemp] = LOW_LEVEL;
			else
				pbImage[nTemp] = HIGH_LEVEL;
		}
	}
	delete[] temp;
	return pbImage;
}
