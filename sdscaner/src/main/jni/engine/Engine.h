#pragma once

typedef struct tagInitSet {
	RECT RoiRect;
	int nLetterHeightMin;
	int nLetterHeightMax;
	int skewAng;
	int mode; //0: picture, 1:video
} InitSet;

#define MULTIRESULT			1

typedef struct tagLICENSE {
	int nLetNum;
	TCHAR szLicense[20];
	TCHAR szLicense1[20];
	float pfDist;
	int nTrust;
	int aveH;
	BYTE Type;
	BYTE bBkColor;
	RECT rtPlate; // Get plate region
	RECT blob[20];

	BYTE bCarColor;
	BYTE bCarColDp;
} LICENSE, *LPLICENSE;

typedef struct tagCARPLATE {
	int nPlate;
	LICENSE pPlate[MULTIRESULT];
	DWORD nProcTime; // Get Recognition Speed(ms)
} CARPLATE_DATA;

#define COLOR_BLACK			0
#define COLOR_BLUE			30
#define COLOR_YELLOW		50
#define COLOR_WHITE			255
#define COLOR_RED			10
#define COLOR_GREEN			70

#define CAR_COLOR_WHITE			0
#define CAR_COLOR_SILVER		1
#define CAR_COLOR_BLACK			2
#define CAR_COLOR_RED			3
#define CAR_COLOR_BLUE			4
#define CAR_COLOR_GOLDEN		5
#define CAR_COLOR_GREEN			6
#define CAR_COLOR_BROWN			7
#define CAR_COLOR_PINK			8

#define CAR_COLOR_DEEP			0
#define CAR_COLOR_SHALLOW		1

BOOL InitEngine();
BOOL FreeEngine();
int Recognition(BYTE* pImage, int nWidth, int nHeight, int nBitCount,
		InitSet* iniSet, CARPLATE_DATA* carData, int charlen);
int anpr(BYTE* pBits, int nWidth, int nHeight, int wstep, int nBitCount,
		int flip, InitSet* iniSet, CARPLATE_DATA* rscarData);
int anprdib(BYTE* pBMP, InitSet* iniSet, CARPLATE_DATA* rscarData,
		char* szCompanyName);
