#include "StdAfx.h"
#include "Engine.h"
#include "RecogEngine.h"
#include "ImageBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
BOOL InitEngine() {
	if (RM_InitEngine()) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL FreeEngine() {
	RM_ReleaseEngine();
	return TRUE;
}

int Recognition(BYTE* pImage, int nWidth, int nHeight, int nBitCount,
		InitSet* iniSet, CARPLATE_DATA* carData, int charlen) {
	int ret = RM_Recognition(pImage, nWidth, nHeight, nBitCount, carData,
			charlen);
	return ret;
}

int anpr(BYTE* pBits, int nWidth, int nHeight, int wstep, int nBitCount,
		int flip, InitSet* iniSet, CARPLATE_DATA* rscarData) {
	if (nBitCount != 8 && nBitCount != 24)
		return 0;
	else {
		int i;
		int ret;
		// 	InitSet iniSet;
		// 	memset(&iniSet,0,sizeof(InitSet));
		//	CARPLATE_DATA carData;
		if (nWidth * nBitCount / 8 == wstep && flip == 1) {
			ret = RM_Recognition(pBits, nWidth, nHeight, nBitCount, rscarData);
		} else {
			int nBytePerPixel = nBitCount / 8;
			int nBytePerLine = nBytePerPixel * nWidth;
			BYTE* pImage = (BYTE*) malloc(nHeight * nBytePerLine);
			if (flip == 0) {
				for (i = 0; i < nHeight; i++)
					memcpy(pImage + i * nBytePerLine,
							pBits + (nHeight - i - 1) * wstep, nBytePerLine);
			} else {
				for (i = 0; i < nHeight; i++)
					memcpy(pImage + i * nBytePerLine, pBits + i * wstep,
							nBytePerLine);
			}
			ret = RM_Recognition(pImage, nWidth, nHeight, nBitCount, rscarData);
			free(pImage);
		}

		return ret;
	}
}
int anprdib(BYTE* pBMP, InitSet* iniSet, CARPLATE_DATA* rscarData,
		char* szCompanyName) {

	int nWidth, nHeight, wstep;
	BYTE* pDib;
	BYTE* pBits;
	int nBitCount;

	if (pBMP == NULL)
		return 0;
	else {
		pDib = pBMP + 14;
		nBitCount = GetBitCount(pDib);
		memset(rscarData, 0, sizeof(CARPLATE_DATA));

		if (nBitCount != 8 && nBitCount != 24) {
			return 0;
		} else {

			GetWidthHeight(pDib, &nWidth, &nHeight);
			pBits = (BYTE*) Get_lpBits(pDib);
			wstep = (nWidth * nBitCount + 31) / 32 * 4;
			if (pBits == NULL) {
				return 0;
			} else {
				int flip = 0;
				int i;
				int ret;
				// 	InitSet iniSet;
				// 	memset(&iniSet,0,sizeof(InitSet));

				if (nWidth * nBitCount / 8 == wstep && flip == 1) {
					ret = RM_Recognition(pBits, nWidth, nHeight, nBitCount,
							rscarData);
				} else {
					int nBytePerPixel = nBitCount / 8;
					int nBytePerLine = nBytePerPixel * nWidth;
					BYTE* pImage = (BYTE*) malloc(nHeight * nBytePerLine);
					if (flip == 0) {
						for (i = 0; i < nHeight; i++)
							memcpy(pImage + i * nBytePerLine,
									pBits + (nHeight - i - 1) * wstep,
									nBytePerLine);
					} else {
						for (i = 0; i < nHeight; i++)
							memcpy(pImage + i * nBytePerLine, pBits + i * wstep,
									nBytePerLine);
					}
					ret = RM_Recognition(pImage, nWidth, nHeight, nBitCount,
							rscarData);
					free(pImage);
				}

				return ret;
			}
		}
	}
}

