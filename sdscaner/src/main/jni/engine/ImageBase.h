// ImageBase.h: interface for the CImageBase class.
//
//////////////////////////////////////////////////////////////////////

#define	    HIGH_LEVEL		0
#define	    LOW_LEVEL		1//255
BYTE* Get_lpBits(BYTE *pDib);
void GetWidthHeight(BYTE *pDib, int* w, int* h);
int GetBitCount(BYTE *pDib);

BYTE* CropImg(BYTE* pImg, int w, int h, RECT* r);
BYTE* ZoomImgSimple(BYTE* pImg, int *w, int *h);
void ZoomInImg(BYTE *pImg, int w, int h, int new_w, int new_h);
#define    SMODE_NON                         0
#define    SMODE_SMALL_DIST                  1
#define    SMODE_GLOBAL_OTSU                 2
#define    SMODE_SMALL_DIST_GLOBAL_OTSU      3
//////////////////////////////////////////////////////////////////////////
BYTE* Binarization_Camera(BYTE *pImg, int w, int h);
BYTE* Binarization_HJI(BYTE *pImg, int w, int h);
BYTE* Binarization_By_Th(BYTE *pImg, int w, int h, int th);

BYTE* Binarization_DynamicThreshold1(BYTE* pImg, int w, int h, int nGridX,
		int nGridY, int special_mode);
BYTE* Binarization_DynamicThreshold2(BYTE* pImg, int w, int h, int winX,
		int winY, int nGridX, int nGridY, int special_mode);

BYTE* Binarization_Windows(BYTE* pImg, int w, int h, int nWinSize);

int GetThreshold_Otsu1(BYTE *pImg, int w, int h);
int GetThreshold_Otsu2(BYTE *pImg, int w, int h, RECT subrect);
int GetThreshold_Otsu3(BYTE *pImg, int w, int h, int* dist);
int GetThreshold_Otsu4(BYTE *pImg, int w, int h, int* dist, RECT subrect);
int GetThreshold_Otsu_From_Histogram(int* Hist, int* dist);

BYTE* MeanFilter(BYTE* pImg, int w, int h, int nWinSize);
void MeanFilter(BYTE* pImg, int w, int h);
void GetSortValueOrder(int* fValue, int* Ord, int n, int Direct);

// !defined(AFX_IMAGEBASE_H__35D175DD_5EEF_4C08_9E41_E1AF109ED272__INCLUDED_)
