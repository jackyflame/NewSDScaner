// RecogEngine.h: interface for the CRecogEngine class.
//
//////////////////////////////////////////////////////////////////////

//recognition width range

#pragma  once

#include "ImageBase.h"

#define MAX_PLATE_WIDTH			400/*250*/
#define MIN_PLATE_WIDTH				70//60
//recognition height range
#define MAX_PLATE_HEIGHT			70
#define MIN_PLATE_HEIGHT			20//12
//Letter range
#define MAX_LETTER_HEIGHT			90
#define MIN_LETTER_HEIGHT			15

#define LIMIT_ROTATE_ANGLE		 10

#define MAX_STROKE_NUM			300
#define MAX_LETTER_NUM				100
#define MAX_PLATE_NUM				10
#define MAX_SEG_NUM					50

#define MAX_CDN				10

#define MAX_HIST			220

#define IMAGE_WIDTH			1232

#define SEG_FINE			1
#define SEG_BAD				2
#define SEG_LONG			8
#define SEG_FRAME			16

#define NOTIMAGE			-9
#define NOTFININD			-8
#define NOTPLATE			-7

#define ALL_MODE 127
#define ENG_MODE	1 
#define DIGIT_MODE	2
#define HANZI_MODE	4
#define JUNZI_MODE	8
#define HOUZI_MODE	16
#define BIN_MODE	50
#define WZI_MODE	31
#define GANG_MODE	32
#define D0ZI_MODE	33
#define WJ2_MODE	34
#define JINGCHA_MODE	64
#define XIANG_MODE	66
#define YELLOWLASTCHA_MODE	96
#define DASHI_MODE   128

#include "Engine.h"

typedef struct tagSTROKE {
	int nPointNum;
	RECT rtRegion;
	int nFirst;
	int nCol;
//	int		bUse;
} STROKE, *LPSTROKE;

typedef struct tagLETTER {
	STROKE Stroke;
	BOOL bIType;
	int nDigit;
	int nBkCol;
	int Letter_index;
} LETTER, *LPLETTER;

typedef struct tagPLATE {
	BYTE bBkColor;
	BYTE bColor;
	int nLetterNum;
	int nLetterAvgW;
	int nLetterAvgH;
	LETTER pLetter[MAX_SEG_NUM];
//	RECT  blob[MAX_SEG_NUM];

	RECT rtOrg;
	RECT rtExpand;

	BYTE bCarColor;
	BYTE bCarColDp;
	float rot_ang;
	POINT area_center;
} PLATE, *LPPLATE;

typedef struct tagSEGMENT {
	RECT rtRegion;
	int nStyle;
} SEGMENT, *LPSEGMENT;

typedef struct tagSEGLETTER {
	RECT rtRegion;
	int pnInd[2][2];
	int pfDist[2][2];
	int nRecogMode;
	int nStyle;
} SEGLETTER, *LPSEGLETTER;
#define MAXPATHNUM 15
#define MAXDATA 15
typedef struct tagPATHSTRUCT {
	int path[MAXPATHNUM];
	int minuscount;
	int score;
	int recogscore;
	int rtscore;
	int mode;
} PATHSTRUCT;
typedef struct tagRECTSTRUCT {
	PATHSTRUCT pathdata[MAXPATHNUM][MAXDATA];
	int pathdatanum[MAXPATHNUM];
	int recogDis[2];
	RECT rect;
} RECTSTRUCT;
typedef struct tagHUBOSTRUCT {
	int i, j, k;
	int st, ed;
	int avH;
	int mode;
	int score;
	int recogscore;
	int rectScore;
	TCHAR sz[15];
	RECT rt;
	RECT rtZi[15];
	int dis[15];
//     HUBOSTRUCT(){
//         sz[0] = 0;
// 		st=0;ed=0;
// 		avH = 1;
//         score = recogscore = rectScore = 1000.0f;
//     }
} HUBOSTRUCT;

typedef struct _Point2D32f {
	float x;
	float y;
} Point2D32f;

typedef struct _Point2D {
	int x;
	int y;
} Point2D;

typedef struct _SizeInfo {
	int width;
	int height;
} SizeInfo;

BOOL RM_InitEngine();
BOOL InitSetting();
void RM_ReleaseEngine();
int RecogMain(BYTE *pbImage, int nWidth, int nHeight, int nBitCount,
		LICENSE* pLP, int* nPlate, int mode);
BOOL AllocMem(BYTE* pbImage, int* nWidth, int* nHeight, int nBitCount);
BOOL initMem(BYTE* pbImage, int* nWidth, int* nHeight, int nBitCount);
void FreeMem();

void RGBToGrayNoMask(BYTE *pbRGB, BYTE *pbGray, int nWidth, int nHeight);
void BinaryByEdge(BYTE *pbGray, BYTE *pbEdge, BYTE *pbBin, int nWidth,
		int nHeight);
void BinaryByHist(BYTE *pbGray, BYTE *pbBin, int nWidth, int nHeight,
		int nPercent, BYTE nCol);
int DetectStorke(BYTE *pbIn, BYTE *pbOut, int *pnStroke, STROKE pStroke[],
		RECT rtRegion, int nWidth, int nHeight, int nMaxNum, BYTE nBk,
		int nSmallNum, int nSmallW, int nSmallH, BOOL bSmallRemove);
//int  DetectStorkebycch(BYTE *pbIn, BYTE *pbOut, int *pnStroke, STROKE pStroke[], RECT* rtRegions,int rtNum, int nWidth, int nHeight, int nMaxNum, BYTE nBk, int nSmallNum, int nSmallW, int nSmallH, BOOL bSmallRemove);

TCHAR ConvIndToString(int idx);

// 	void RemoveNoise(BYTE *pbImage, int nWidth, int nHeight);
BYTE* CropMinRegion(BYTE *pbImage, int nWidth, int nHeight, RECT rtRegion,
		int *nWidth2, int *nHeight2, int skewmode = 0);

//	void SetColorizeStroke(BYTE* pbImage, int nWidth, STROKE pStroke, int* pnStroke, BYTE nCol);
BOOL GetStroke(RECT rt);
BOOL IsLetterCand(STROKE* pStroke, BOOL* bIType);
void SortStroke(STROKE *pStr, int* nStNum);

void Histogram(BYTE* src, int width, int height, int* hist);
void MaxMin(int* src, int len, int* max, int* min, int* total);
void Resize(BYTE *pSrc, BYTE *pDst, int srcWidth, int srcHeight, int dstWidth,
		int dstHeight, int BitCount);

void SetImageROI(BYTE *pSrc, BYTE *pDst, int nSrcWidth, int nSrcHeight,
		int nDstWidth, int nDstHeight, RECT scan_area, int nBitCount);

void pre_MakeIntImage(BYTE * pbGray, int *pnSum, int nWidth, int nHeight);
void pre_DetectEdgeSpec2(BYTE * pbGray, int * pnSum, int *pnEdge, int nWidth,
		int nHeight);
void SubBinary2(BYTE * pbGray, int * pnSum, int* lpOut, int nWidth, int nHeight,
		int x0, int x1, int y0, int y1, int rw, int rh);
void pre_MakeMonochrome3(int* pnEdge, BYTE *pbEdge, int nWidth, int nHeight,
		RECT rtReg, int mode);
int RM_Recognition(BYTE* pImage, int nWidth, int nHeight, int nBitCount,
		CARPLATE_DATA* carData, int charlen = 5);

BOOL Dynamic_Programming_SAMZHU(SEGLETTER *pSegLetter, int segNum, LICENSE* pLP,
		BYTE* pImg, int nWidth, int nHeight, int nmode);
int GetRecogLetter(SEGLETTER *pSegLetter, int segNum, int avCharH, BYTE* pImg,
		int nWidth, int nHeight, int *path, HUBOSTRUCT* pt, RECTSTRUCT* run,
		int mode, int PathNum);
int CropRecogRegion(BYTE* pImg, int nWidth, int nHeight, RECT*rt, int nMode,
		int Ind[], int Dist[]);
int CropRecogRegion_1(BYTE* pImg, int nWidth, int nHeight, RECT*rt, int nMode,
		int Ind[], int Dist[]);

int RecogLetterSAMZHU(BYTE *lpImg, int nWidth, int nHeight, int nMode,
		int* lpCandiNum, int *lpDis, BOOL bOut);

int ConvertRealString(TCHAR *sz);
