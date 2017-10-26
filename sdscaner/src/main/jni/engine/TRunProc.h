// PxyRun.h: interface for the CRunProc class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(_PXYRUN_H__)
#define _PXYRUN_H__


#include <math.h>

#ifndef IMAGESIZE
#define  	IMAGESIZE		200000//0
#endif

#define SORT_PIXELNUM   0
#define SORT_SIZE       1
#define SORT_LEFT       2
#define SORT_TOP        3
#define SORT_CENTER_X   4
#define SORT_CENTER_Y   5
#define SORT_LINEID		6
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
/*
��I�: CRunProc
�: Run������ ��I

��: KSD,Kim M.I.
���:2008/12/25
*/
template <class TRt>
class TRunProc
{

typedef CTypedPtrArray <CPtrArray, TRt*> TRtAry;

public:
	TRunProc();
	virtual ~TRunProc();
public:
	short*	m_pRunX;	//	X position of Run
	int*	m_pRunNo;	//	Index of Start Run in the Line
	int     m_h;
	int*    m_pDown;
	int*    m_pUp;
	int*    m_pLabel;

public:
	int				CreateRunTableFromBits(BYTE* lpBits,int w,int h,short *runX, int *runNo);
	int				CreateRunTableFromBits(BYTE* lpBits,int w,int h);
	int				CreateRunTableFromImg(BYTE* Img,int w,int h,short *runX, int *runNo);
	int				CreateRunTableFromImg(BYTE* Img,int w,int h, short* runX,int* runNo,CRect subrect);
	int				CreateRunTableFromImg(BYTE* Img,int w,int h);
	int				CreateRunTableFromImg(BYTE* Img,int w,int h, CRect subrect);
	static void		UpdateRunTable(short *runX, int *runNo, int h);
	static void		CreateVerticalRunTable(short *runX,int *runNo,int &w,int &h);
	static void		CreateVerticalRunTable(short *runX,int *runNo,int &w,int &h,
									  short* newRunX,int* newRunNo,int&w1,int&h1);

	static void		GetBitsFromRunXRunNo(BYTE* Bits,short* runX,int* runNo,CRect Rt);
	void			GetBitsFromRunXRunNo(BYTE* Bits,CRect Rt);
	static BYTE*	GetBinDibFromRunXRunNo(short* runX,int* runNo,CRect Rt);
	BYTE*			GetBinDibFromRunXRunNo(CRect Rt);
	void			GetImgFromRunXRunNo(BYTE* Img,CRect Rt);
	static void		GetImgFromRunXRunNo(BYTE* Img,short* runX,int* runNo,CRect Rt);
	
	void			GetImgFromRunRt(BYTE* Img,TRt *pRunRt,CRect Rt);
	BYTE*			GetImgFromRunRt(TRt *pRunRt,int &w,int &h);
	BYTE*			GetBinDibFromRunRt(TRt *pRunRt);
	BYTE*			GetImgFromRunRtAry(TRtAry& RunRtAry,CRect Rt);
	BYTE*			GetImgFromRunRtAry(TRtAry& RunRtAry,int& w,int& h);
	BYTE*			GetBinDibFromRunRtAry(TRtAry& RunRtAry);
	BYTE*			GetBinDibFromRunRtAry(TRtAry& RunRtAry,CRect Rt);

	int 			MakeConnectComponent(short *runX, int *runNo,int h, TRtAry& RunRtAry,int nUserDataSize=0);
	int				MakeConnectComponentFromBits(BYTE* pBits,int w,int h,TRtAry& RunRtAry,int nUserDataSize=0);
	int				MakeConnectComponentFromImg(BYTE* pImg, int w,int h, TRtAry& RunRtAry,int nUserDataSize=0);
	int				MakeConnectComponentFromImg(BYTE* pImg, int w,int h, TRtAry& RunRtAry, CRect subrect,int nUserDataSize=0);

	BYTE*			GetRemoveNoizeBits(BYTE* inBits,int w,int h,CSize ThSz,int ThAs);
	BYTE*			GetRemoveNoizeImg(BYTE* inImg,int w,int h,CSize ThSz,int ThAs);
	BYTE*			GetRemoveBigCCImg(BYTE* inImg,int w,int h,int ThW, int ThH, int ThPixelNum);
	
	CRect			GetRealLineRect(TRtAry& RunRtAry,int nLine);
	CRect			GetRealRect(TRtAry& RectAry);
	void			SortByOption(TRtAry& RunRtAry,int nStart,int nEnd,int Option);

	static void		CopyRunRtAry(TRtAry& DestAry,TRtAry& SourceAry);
	static void		RemoveAllRunRt(TRtAry& RunRtAry);
	static void		RemoveRunRt(TRtAry& RunRtAry,int index);
	
	void		OffsetRunRtAry(TRtAry& RunRtAry,CPoint ptOffset);


protected:
	void			CreateMemory_UpDownLabel(int *runNo, int h);
	void			DeleteMemory_UpDownLabel();
	void			MakeRunData(short *runX, int *runNo,int* pLabel,TRtAry& RunRtAry,int nId);
	void			MakeDownTable(short *runX, int *runNo,int h,int* pDown);
	void			MakeDownTable();
	void			MakeUpTable(short *runX, int *runNo,int h,int* pUp);
	void			MakeUpTable();
	void			RunFollowing(short *runX, int *runNo,int h,int* pDown,int* pUp,int* pLabel,TRtAry& RunRtAry,BOOL bMakeRunData=FALSE,int nUserDataSize=0);
	void			RunFollowing(TRtAry& RunRtAry,BOOL bMakeRunData=FALSE,int nUserDataSize=0);
	BOOL			NextRunNumber(int* pDown,int* pUp,int &i,int &p);
	BOOL			NextRunNumber(int &i,int &p);

	// Functions for Image rotation by runX,runNo Data
public:
	void			Rotate_RunXRunNo(short* runX,int* runNo,double fAng,CSize &size);
	BYTE*			Rotate_BinBits(BYTE* inBits,int& w,int& h,double fAng, BOOL bKeepSize=TRUE);
	BYTE*			Rotate_BinImg(BYTE* inImg,int& w,int& h,double fAng, BOOL bKeepSize=TRUE);
protected:
	void			rtConvert(short *runX,int *runNo, int h);
	void			Skew(short *runX,int *runNo, short *newRunX, int *run, CSize &size, double d1, double d2);
	void			Skew(short *runX,int *runNo, CSize& size, double d1, double d2);
	static  void	Transposing(short *runX, int *runNo, CSize &size);

	
public:
	// Angle Functions by runX,runNo Data
	BOOL			GetAngleFromBits(BYTE* lpBits, int w, int h, double& fAng);
	BOOL			GetAngleFromImg(BYTE* pImg,int w,int h,double& fAng);
	double			GetAngleFromRunRtAry(TRtAry& RunRtAry,int w,int h);
	double			GetAngleFromRunRtAry_1(TRtAry& RunRtAry,int w,int h);
	double			GetAngleByRtsInLines(TRtAry& RunRtAry,int w,int h,int nLen);

	BYTE*			AutoRotateCorrectBinBits(BYTE* inBits,int& w,int& h,double& fAngle, BOOL bKeepSize=TRUE);
	BYTE*           AutoRotateCorrectBinImg(BYTE* inImg,int& w,int& h,double& fAng, BOOL bKeepSize=TRUE);
	//For RnOCR
	BYTE*			GetRotatedDibFromRunXRunNo(short* runX,int* runNo,double fAng,CSize &Sz,BOOL bKeepSize=TRUE);

	// Functions for AveLineHeight
	int				DeleteNoizeRects(TRtAry& RectAry,CSize Sz);
	int				GetAverageLineHeight(TRtAry& rts,int w,int h,int &Lh);

	// Function for Get Preprocessing Information(fAngle,fZoomIn)
	void			GetPreProcessInfo(short* runX,int* runNo,int w,int h,double& fAngle,double& fZoomIn,BOOL& bAngle,BOOL& bZoomIn);

protected:
	void			GetLUAngle(CRect cRt1,CRect cRt2,double& ang1,double& ang2);
	int				GetBlkId(CRect cRt,CRect cSubAllRt[50],int nNum);

	// ImageCompress Functions by runX, runNo Data
public:
	void			ImageZoomOut2(BYTE* lpBits,int& w,int& h,short* runX,int* runNo,int rate);
	void			ImageZoomOut2(short* runX,int* runNo,short* newRunX,
									int* newRunNo,int& w,int& h,int rate);
public://rts merge functions
	void			MergeRts(TRtAry &ary, int w,int h, int charHeight,int lineSpace);

};

template <class TRt>
TRunProc<TRt>::TRunProc()
{
	m_pRunX     = NULL;
	m_pRunNo    = NULL;
	m_pDown		= NULL;
	m_pUp		= NULL;
	m_pLabel	= NULL;
}
template <class TRt>
TRunProc<TRt>::~TRunProc()
{
	if(m_pRunX  != NULL) { delete[] m_pRunX;  m_pRunX  = NULL; }
	if(m_pRunNo != NULL) { delete[] m_pRunNo; m_pRunNo = NULL; }

	if(m_pDown  != NULL) { delete []m_pDown;  m_pDown  = NULL; }
	if(m_pUp    != NULL) { delete []m_pUp;    m_pUp    = NULL; }
	if(m_pLabel != NULL) { delete []m_pLabel; m_pLabel = NULL; }
}
// ��:CreateRunTableFromBits
// �:lpBits�c�m_pRunX,m_pRunNo run������
template <class TRt>
int TRunProc<TRt>::CreateRunTableFromBits(BYTE* lpBits,int w,int h)
{
	if(m_pRunNo != NULL){ delete []m_pRunNo; m_pRunNo = NULL;}
	if(m_pRunX  != NULL){ delete []m_pRunX;  m_pRunX = NULL;}
	m_pRunNo = new int[h+2 * max(w, h)+1];
	m_pRunX = new short[IMAGESIZE];

	m_h = h;
	return	CreateRunTableFromBits(lpBits, w, h, m_pRunX, m_pRunNo);
}
// ��:CreateRunTableFromBits
// �:lpBits�c�runX,runNo run������
template <class TRt>
int TRunProc<TRt>::CreateRunTableFromBits(BYTE* lpBits,int w,int h,short *runX, int *runNo)
{
    int i,j,k,t,p,num,w_byte,RunNum=1;
    int *pCal;
	long add;
	int quot1,rem1,rem2;
    BOOL bl;
    BYTE By,By1,By2;
    BYTE *lpLine;

	int wBpp = 1;
	w_byte = ((((wBpp * w) + 31) / 32) * 4);//GetWidByte(w);

	quot1 = w/8;
	rem1 = w%8;
	if(rem1 != 0)  quot1++;

    runNo[0] = 1;

	pCal = new int[quot1];
	for(i=0; i<quot1; i++) pCal[i] = 8 * i;

    k = quot1 - 1;
    t = w;// - 1;

	for(i=0; i<h; i++)
	{
		add = (h - i - 1) * w_byte;
		//add = h * w_byte;
		lpLine = lpBits + add;
		num = 8; 
		bl = TRUE;

		for(j=0; j<quot1; j++) 
		{
			if(RunNum >= IMAGESIZE)
			{
					//AfxMessageBox("Search Error_RunNum >= IMAGESIZE...!");
					return 0;
			}
			By1 = (BYTE) * (lpLine + j);
			if(bl == FALSE && By1 == 0x00) continue;
			if(By1 == 0xFF)
			{
				if(bl == FALSE)
				{
					runX[RunNum] = pCal[j];
					RunNum++;
					bl = TRUE;
				}
				continue;
			}
			if(j == k && rem1 != 0)  num = rem1;

			By = 0x80;
			for(p=0; p<num; p++)
			{
				By2 = By1 & By;
				if(By2 == 0x00 && bl == TRUE)
				{
					runX[RunNum] = pCal[j] + p;
					RunNum++;
					bl = FALSE;
				}
				if(By2 != 0x00 && bl == FALSE)
				{
					runX[RunNum] = pCal[j] + p;
					RunNum++;
					bl = TRUE;
				}
				By = By >> 1;
			}
		}
		rem2 = RunNum%2;
		if(rem2 == 0)
		{
			if(runX[RunNum - 1] == t) RunNum--;
			else 
			{
				runX[RunNum] = t;
				RunNum++; 
			}
		}
		runNo[i + 1] = RunNum;
	}
    delete []pCal;
	return RunNum;
}
// ��:CreateRunTableFromImg
// �: Img�c�m_pRunX,m_pRunNo run������
template <class TRt>
int TRunProc<TRt>::CreateRunTableFromImg(BYTE* Img,int w,int h, CRect subrect)
{
	if(m_pRunNo != NULL){ delete []m_pRunNo; m_pRunNo = NULL;}
	if(m_pRunX  != NULL){ delete []m_pRunX;  m_pRunX = NULL;}
	m_pRunNo = new int[h+2 * max(w, h)+1];
	m_pRunX = new short[IMAGESIZE];
	m_h = h;
	return CreateRunTableFromImg(Img, w, h, m_pRunX, m_pRunNo, subrect);
}

// ��:CreateRunTableFromImg
// �: Img�c�m_pRunX,m_pRunNo run������
template <class TRt>
int TRunProc<TRt>::CreateRunTableFromImg(BYTE* Img,int w,int h)
{
	if(m_pRunNo != NULL){ delete []m_pRunNo; m_pRunNo = NULL;}
	if(m_pRunX  != NULL){ delete []m_pRunX;  m_pRunX = NULL;}
	m_pRunNo = new int[h+2 * max(w, h)+1];
	m_pRunX = new short[IMAGESIZE];
	m_h = h;
	return CreateRunTableFromImg(Img, w, h, m_pRunX, m_pRunNo);
}
// ��:CreateRunTableFromImg
// �: Img�c�runX,runNo run������
template <class TRt>
int TRunProc<TRt>::CreateRunTableFromImg(BYTE* Img,int w,int h,short *runX, int *runNo)
{
    int i,j,RunNum=1;
	int rem;
    BOOL b;
    BYTE By;

    runNo[0] = 1;
	for(i=0; i<h; i++)
	{
		b = TRUE;
		for(j=0; j<w; j++) 
		{
			By = Img[i*w+j];
			if(b == FALSE && By == 1) continue;
			if(b == TRUE  && By == 1 )
			{
				runX[RunNum] = j;
				RunNum++;
				b = FALSE;
			}
			else if(b == FALSE && By == 0)
			{
				runX[RunNum] = j;
				RunNum++;
				b = TRUE;
			}
		}
		rem =RunNum%2;
		if(rem == 0)
		{
			runX[RunNum] = w;
			RunNum++; 
		}
	runNo[i + 1] = RunNum;
	}
	return RunNum;
}
// ��:CreateRunTableFromImg
// �: Img����G~�c�runX,runNo run������
template <class TRt>
int TRunProc<TRt>::CreateRunTableFromImg(BYTE* Img,int w,int h, short* runX,int* runNo,CRect subrect)
{
    int i,j,RunNum=1;
	int rem;
    BOOL b;
    BYTE By;
	subrect.IntersectRect(subrect,CRect(0,0,w,h));
	for(i=0; i<=subrect.top; i++)    
		runNo[i] = 1;

	for(i=subrect.top; i<subrect.bottom; i++)
	{
		b = TRUE;
		for(j=subrect.left; j<subrect.right; j++) 
		{
			By = Img[i*w+j];
			if(b == FALSE && By == 1) continue;
			if(b == TRUE  && By == 1 )
			{
				runX[RunNum] = j;
				RunNum++;
				b = FALSE;
			}
			else if(b == FALSE && By == 0)
			{
				runX[RunNum] = j;
				RunNum++;
				b = TRUE;
			}
		}
		rem = RunNum % 2;
		if(rem == 0)
		{
			runX[RunNum] = (short)subrect.right;
			RunNum++; 
		}
		runNo[i + 1] = RunNum;
	}
	for(i=subrect.bottom; i<h+1; i++)    
		runNo[i] = RunNum;
	return RunNum;
}
// ��:UpdateRunTable
// �: runX -1� ���run(�� run��� ���� runX, runNo� ����.
template <class TRt>
void TRunProc<TRt>::UpdateRunTable(short *runX, int *runNo, int h)
{
	int i,j,st,num;
	
	st = num = 1;
	for(j=0; j<h; j++)
	{
		for(i=runNo[j]; i<runNo[j+1]; i+=2) 
		{
			if(runX[i] == -1) continue;
			runX[num] = runX[i];   num++;				
			runX[num] = runX[i+1]; num++;
		}
		runNo[j] = st; st = num;
	}
	runNo[j] = st;
}

template <class TRt>
void TRunProc<TRt>::CreateVerticalRunTable(short *runX,int *runNo,int &w,int &h)
{
	CSize size(w,h);
	Transposing(runX,runNo,size);
	w = size.cx;
	h = size.cy;
}

template <class TRt>
void TRunProc<TRt>::CreateVerticalRunTable(short *runX,int *runNo,int &w,int &h,
									  short* newRunX,int* newRunNo,int&w1,int&h1)
{
	memcpy(newRunX,runX,sizeof(short)*runNo[h]);
	memcpy(newRunNo,runNo,sizeof(int)*(h+1));
	CSize size(w,h);
	Transposing(newRunX,newRunNo,size);
	w1 = size.cx;
	h1 = size.cy;
}

// ��:CreateMemory_UpDownLabel
// �: m_pDown,m_pUp,m_pLabel ��� �V�.
template <class TRt>
void TRunProc<TRt>::CreateMemory_UpDownLabel(int *runNo, int h)
{
	int nSize = runNo[h] + 1;

	if(m_pDown != NULL) delete []m_pDown;
	m_pDown = new int[nSize];
	memset(m_pDown, 0, sizeof(int) * nSize);

	if(m_pUp != NULL) delete []m_pUp;
	m_pUp = new int[nSize];
	memset(m_pUp, 0, sizeof(int) * nSize);

	nSize = runNo[h];
	if(m_pLabel != NULL) delete []m_pLabel;
	m_pLabel = new int[nSize];
	memset(m_pLabel, 0, sizeof(int) * nSize);
}
// ��:DeleteMemory_UpDownLabel
// �: m_pDown,m_pUp,m_pLabel ����� ���.
template <class TRt>
void TRunProc<TRt>::DeleteMemory_UpDownLabel()
{
	if(m_pDown != NULL)  { delete []m_pDown;  m_pDown  = NULL; }
	if(m_pUp != NULL)    { delete []m_pUp;    m_pUp    = NULL; }
	if(m_pLabel != NULL) { delete []m_pLabel; m_pLabel = NULL; }
}
template <class TRt>
BYTE* TRunProc<TRt>::GetBinDibFromRunXRunNo(CRect Rt)
{
	if(m_pRunX==NULL || m_pRunNo==NULL) return NULL;
	return GetBinDibFromRunXRunNo(m_pRunX,m_pRunNo,Rt);
}
template <class TRt>
BYTE* TRunProc<TRt>::GetBinDibFromRunXRunNo(short* runX,int* runNo,CRect Rt)
{
	if(runX==NULL || runNo==NULL) return NULL;
	int w = Rt.Width();
	int h = Rt.Height();

	int WidByte = (w+31)/32*4;
	int	ImgSize = WidByte*h;
	int HeadSize=sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*2;
	int DibSize=HeadSize+ImgSize;
	BYTE *pDib=new BYTE[DibSize];

	BITMAPINFOHEADER* pBIH  = (BITMAPINFOHEADER*)pDib;
	pBIH->biSize			= sizeof(BITMAPINFOHEADER);
	pBIH->biWidth			= w; 
	pBIH->biHeight			= h; 
	pBIH->biPlanes			= 1; 
	pBIH->biBitCount		= 1; 
	pBIH->biCompression		= 0; 
	pBIH->biSizeImage		= ImgSize; 
	pBIH->biXPelsPerMeter	= 0; 
	pBIH->biYPelsPerMeter	= 0; 
	pBIH->biClrUsed			= 2; 
	pBIH->biClrImportant	= 0;

	BITMAPINFO* pInfoH  = (BITMAPINFO*)(LPSTR)pBIH;
	pInfoH->bmiColors[0].rgbRed      = 0;
	pInfoH->bmiColors[0].rgbGreen    = 0;
	pInfoH->bmiColors[0].rgbBlue     = 0;
	pInfoH->bmiColors[0].rgbReserved = 0;
	
	pInfoH->bmiColors[1].rgbRed      = 255;
	pInfoH->bmiColors[1].rgbGreen    = 255;
	pInfoH->bmiColors[1].rgbBlue     = 255;
	pInfoH->bmiColors[1].rgbReserved = 0;

	BYTE* pBits = pDib+HeadSize;
	GetBitsFromRunXRunNo(pBits,runX,runNo,Rt);
	return pDib;
}

// ��:GetBitsFromRunXRunNo
// �: m_pRunX,m_pRunNo~����G Rt � Bits� ��.
// ��:  �� �� Bits� �VF��.
template <class TRt>
void TRunProc<TRt>::GetBitsFromRunXRunNo(BYTE* Bits,CRect Rt)
{
	if(m_pRunX==NULL || m_pRunNo==NULL) return;

	GetBitsFromRunXRunNo(Bits,m_pRunX,m_pRunNo,Rt);
}
// ��:GetBitsFromRunXRunNo
// �: runX,runNo���Bits� ��.
// ��:  �� �� Bits� �VF��.
template <class TRt>
void TRunProc<TRt>::GetBitsFromRunXRunNo(BYTE* Bits,short* runX,int* runNo,CRect Rt)
{
	int i,j,L,R,un,k,i1,j1;
	int wd = Rt.Width();
	int hi = Rt.Height();
	int wb = ((((wd) + 31) / 32) * 4); 
	int s = wb * hi;
	BYTE* pLine = new BYTE[wd];
	int rem = wd % 8;
	BYTE reg;
	
	memset(Bits, 0, s);
	
	for(i=Rt.top; i<Rt.bottom; i++){ 
		memset(pLine, 0, wd);
		for(j=runNo[i]; j<runNo[i+1]; j+=2) 
		{
			L = max(Rt.left, runX[j]); 
			R = min(Rt.right, runX[j+1]);
			un = R - L;
			if(un > 0)
			{
				L -= Rt.left;
				R -= Rt.left;
				for(k=L; k<R; k++) pLine[k] = 1;
			}
		}
		i1 = i-Rt.top;
		i1 = hi-i1-1;
		j1=0;reg=0;
		for(k=0;k<wd;k++){
			if(pLine[k] == 0)	reg |= 1;
			if(((k+1) % 8) == 0){
				Bits[i1*wb+j1] = reg;
				j1++;
				reg = 0;
			}
			else{ reg<<=1;}
		}
		if(rem != 0){
			reg <<= 8 - rem-1;
			Bits[i1*wb+j1] = reg;
		}
	}
	delete[] pLine;pLine=NULL;
}
// ��:GetImgFromRunXRunNo
// �: m_pRunX,m_pRunNo ��Img� ��.
// ��:  �� �� Img� �VF��.
template <class TRt>
void TRunProc<TRt>::GetImgFromRunXRunNo(BYTE* Img,CRect Rt)
{
	GetImgFromRunXRunNo(Img, m_pRunX, m_pRunNo, Rt);
}
// ��:GetImgFromRunXRunNo
// �: runX,runNo���Img� ��.
// ��:  �� �� Img� �VF��.
template <class TRt>
void TRunProc<TRt>::GetImgFromRunXRunNo(BYTE* Img,short* runX,int* runNo,CRect Rt)
{
	int i,j,h,L,R,un,k;
	
	int wd = Rt.Width();
	int hi = Rt.Height();
	int s = wd * hi;
	memset(Img, 0, s);
	
	for(j=Rt.top; j<Rt.bottom; j++) 
		for(h=j-Rt.top, i=runNo[j]; i<runNo[j+1]; i+=2) 
		{
			L = max(Rt.left, runX[i]); 
			R = min(Rt.right,runX[i+1]);
			un = R - L;
			
			if(un > 0)
			{
				L -= Rt.left;
				R -= Rt.left;
				for(k=L; k<R; k++) Img[h * wd + k] = 1;
			}
		}
}

// ��:GetImgFromRunRt
// �: pRunRt�pRunData������G Rt � Img� ��.
// ��:  �� �� Img� �VF��.
template <class TRt>
void TRunProc<TRt>::GetImgFromRunRt(BYTE* Img, TRt *pRunRt, CRect Rt)
{
	int i,j,y,nm,x1,x2;
	
	int wd = Rt.Width();
	int hi = Rt.Height();
	CRect rect;
	if(!rect.IntersectRect(pRunRt->m_Rect,Rt)) return;
	DWORD *data = pRunRt->pRunData;
	nm=pRunRt->nRunNum;
	for(i=0;i<nm;i+=2)
	{
		x1=(int)LOWORD(data[i]) - Rt.left;
		x2=(int)LOWORD(data[i+1]) - Rt.left; 
		y =(int)HIWORD(data[i]) - Rt.top;
		if(y<0 ||y>=hi || x2<0 ||x1>=wd) continue;
		x1 = max(0,x1);	x2 = min(wd-1,x2);
		y = wd * y;
		for(j=x1; j<x2; j++) Img[y + j] = 1;
	}
}
template <class TRt>
BYTE* TRunProc<TRt>::GetBinDibFromRunRt(TRt *pRunRt)
{
	int w,h;
	BYTE* pImg = GetImgFromRunRt(pRunRt, w, h);

	int WidByte = (w+31)/32*4;
	int	ImgSize = WidByte*h;
	int HeadSize=sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*2;
	int DibSize=HeadSize+ImgSize;
	BYTE *pDib=new BYTE[DibSize];

	BITMAPINFOHEADER* pBIH  = (BITMAPINFOHEADER*)pDib;
	pBIH->biSize			= sizeof(BITMAPINFOHEADER);
	pBIH->biWidth			= w; 
	pBIH->biHeight			= h; 
	pBIH->biPlanes			= 1; 
	pBIH->biBitCount		= 1; 
	pBIH->biCompression		= 0; 
	pBIH->biSizeImage		= ImgSize; 
	pBIH->biXPelsPerMeter	= 0; 
	pBIH->biYPelsPerMeter	= 0; 
	pBIH->biClrUsed			= 2; 
	pBIH->biClrImportant	= 0;

	BITMAPINFO* pInfoH  = (BITMAPINFO*)(LPSTR)pBIH;
	pInfoH->bmiColors[0].rgbRed      = 0;
	pInfoH->bmiColors[0].rgbGreen    = 0;
	pInfoH->bmiColors[0].rgbBlue     = 0;
	pInfoH->bmiColors[0].rgbReserved = 0;
	
	pInfoH->bmiColors[1].rgbRed      = 255;
	pInfoH->bmiColors[1].rgbGreen    = 255;
	pInfoH->bmiColors[1].rgbBlue     = 255;
	pInfoH->bmiColors[1].rgbReserved = 0;

	BYTE* lpBits = pDib+HeadSize;

	BYTE reg = 0;
	int	 i,j,i1,j1;

	int rem = w % 8;

	i1=0;
	for(i=h-1;i>=0;i--){
		j1=0;
		for(j=0;j<w;j++){
			if(pImg[i*w+j] != 0)	reg |= 0;
			else					reg |= 1;		  
		    if(((j+1) % 8) == 0){
			  lpBits[i1*WidByte+j1] = reg;
			  j1++;
			  reg = 0;
			}
			else{ reg<<=1;}
		}
		if(rem != 0){
			reg <<= 8 - rem-1;
			lpBits[i1*WidByte+j1] = reg;
		}
		reg=0;
		i1++;
	}
	delete[] pImg;
	return pDib;
}

// ��:GetImgFromRunRt
// �: pRunRt�pRunData����G � Img� ��.
// ��:  ��� Img� �V� �D���� ����
template <class TRt>
BYTE* TRunProc<TRt>::GetImgFromRunRt(TRt *pRunRt,int &w,int &h)
{
	int i,j,s,y,nm,x1,x2;

	w = pRunRt->m_Rect.Width();
	h = pRunRt->m_Rect.Height();
	s = w * h;
	
	BYTE *Img = new BYTE[s];
	memset(Img,0,s);

	DWORD *data = pRunRt->pRunData;
	nm=pRunRt->nRunNum; 
	for(i=0;i<nm;i+=2)
	{
		x1=(int)LOWORD(data[i]) - pRunRt->m_Rect.left;
		x2=(int)LOWORD(data[i+1]) - pRunRt->m_Rect.left; 
		y =(int)HIWORD(data[i]) - pRunRt->m_Rect.top;
		y = w * y;
		for(j=x1; j<x2; j++) Img[y + j] = 1;
	}
	return Img;
}
template <class TRt>
BYTE* TRunProc<TRt>::GetBinDibFromRunRtAry(TRtAry& RunRtAry)
{
	CRect Rt = GetRealRect(RunRtAry);
	return GetBinDibFromRunRtAry(RunRtAry,Rt);
}
template <class TRt>
BYTE* TRunProc<TRt>::GetBinDibFromRunRtAry(TRtAry& RunRtAry,CRect Rt)
{
	int w = Rt.Width();
	int h = Rt.Height();

	BYTE* pImg = GetImgFromRunRtAry(RunRtAry, Rt);

	int WidByte = (w+31)/32*4;
	int	ImgSize = WidByte*h;
	int HeadSize=sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*2;
	int DibSize=HeadSize+ImgSize;
	BYTE *pDib=new BYTE[DibSize];

	BITMAPINFOHEADER* pBIH  = (BITMAPINFOHEADER*)pDib;
	pBIH->biSize			= sizeof(BITMAPINFOHEADER);
	pBIH->biWidth			= w; 
	pBIH->biHeight			= h; 
	pBIH->biPlanes			= 1; 
	pBIH->biBitCount		= 1; 
	pBIH->biCompression		= 0; 
	pBIH->biSizeImage		= ImgSize; 
	pBIH->biXPelsPerMeter	= 0; 
	pBIH->biYPelsPerMeter	= 0; 
	pBIH->biClrUsed			= 2; 
	pBIH->biClrImportant	= 0;

	BITMAPINFO* pInfoH  = (BITMAPINFO*)(LPSTR)pBIH;
	pInfoH->bmiColors[0].rgbRed      = 0;
	pInfoH->bmiColors[0].rgbGreen    = 0;
	pInfoH->bmiColors[0].rgbBlue     = 0;
	pInfoH->bmiColors[0].rgbReserved = 0;
	
	pInfoH->bmiColors[1].rgbRed      = 255;
	pInfoH->bmiColors[1].rgbGreen    = 255;
	pInfoH->bmiColors[1].rgbBlue     = 255;
	pInfoH->bmiColors[1].rgbReserved = 0;

	BYTE* lpBits = pDib+HeadSize;

	BYTE reg = 0;
	int	 i,j,i1,j1;

	int rem = w % 8;

	i1=0;
	for(i=h-1;i>=0;i--){
		j1=0;
		for(j=0;j<w;j++){
			if(pImg[i*w+j] != 0)	reg |= 0;
			else					reg |= 1;		  
		    if(((j+1) % 8) == 0){
			  lpBits[i1*WidByte+j1] = reg;
			  j1++;
			  reg = 0;
			}
			else{ reg<<=1;}
		}
		if(rem != 0){
			reg <<= 8 - rem-1;
			lpBits[i1*WidByte+j1] = reg;
		}
		reg=0;
		i1++;
	}
	delete[] pImg;
	return pDib;
}

// ��:GetImgFromRunRtAry
// �: TRtAry������GRt � Img� ��.
// ��:  ��� Img� �VF �����
template <class TRt>
BYTE* TRunProc<TRt>::GetImgFromRunRtAry(TRtAry& RunRtAry,CRect Rt)
{
	int i;
	int w = Rt.Width();
	int h = Rt.Height();
	int s = w * h;
	BYTE* Img = new BYTE[s];
	memset(Img, 0, s);
	int num = RunRtAry.GetSize();
	for(i=0; i<num; i++)
		GetImgFromRunRt(Img, RunRtAry[i], Rt);
	return Img;
}
// ��:GetImgFromRunRtAry
// �: TRtAry����G � Img� ��.
// ��:  ��� Img� �V� �D���� ����
template <class TRt>
BYTE* TRunProc<TRt>::GetImgFromRunRtAry(TRtAry& RunRtAry,int& w,int& h)
{
	int i;
	CRect Rt = GetRealRect(RunRtAry);
	w = Rt.Width();
	h = Rt.Height();
	int s = w * h;
	BYTE* Img = new BYTE[s];
	memset(Img, 0, s);
	int num = RunRtAry.GetSize();
	for(i=0; i<num; i++)
		GetImgFromRunRt(Img, RunRtAry[i], Rt);
	return Img;
}

// ��:GetRealRect
// �: TRtAry����G �����
template <class TRt>
CRect TRunProc<TRt>::GetRealRect(TRtAry& RunRtAry)
{
	int i;
	TRt* pRunRt;
	CRect rect;
	int l=10000,t=10000,r=0,b=0;
	int num = RunRtAry.GetSize();
	if(num==0){ rect.SetRect(0,0,0,0);return rect;}
	for(i=0;i<num;i++){
		pRunRt = RunRtAry.GetAt(i);
		l = min(l,pRunRt->m_Rect.left);t = min(t,pRunRt->m_Rect.top);
		r = max(r,pRunRt->m_Rect.right);b = max(b,pRunRt->m_Rect.bottom);
	}
	rect.SetRect(l,t,r+1,b+1);
	return rect;
}
template <class TRt>
CRect TRunProc<TRt>::GetRealLineRect(TRtAry& RunRtAry,int nLine)
{
	int i;
	TRt* pRunRt;
	CRect rect;
	int l=10000,t=10000,r=0,b=0,nK=0;
	int num = RunRtAry.GetSize();
	if(num==0){ rect.SetRect(0,0,0,0);return rect;}
	for(i=0;i<num;i++){
		pRunRt = RunRtAry.GetAt(i);
		if(pRunRt->nLineNo != nLine) continue;
		l = min(l,pRunRt->m_Rect.left);t = min(t,pRunRt->m_Rect.top);
		r = max(r,pRunRt->m_Rect.right);b = max(b,pRunRt->m_Rect.bottom);
		nK++;
	}
	if(nK>0)
		rect.SetRect(l,t,r+1,b+1);
	else
		rect.SetRect(0,0,0,0);
	return rect;
}
// ��:MakeDownTable
// �: m_pRunX, m_pRunNo � DownTable  m_pDown ���
template <class TRt>
void TRunProc<TRt>::MakeDownTable()
{
	MakeDownTable(m_pRunX, m_pRunNo,m_h,m_pDown);
}

// ��:MakeDownTable
// �: runX, runNo � DownTable  pDown ���
template <class TRt>
void TRunProc<TRt>::MakeDownTable(short *runX, int *runNo,int h,int* pDown)
{
	int i,j,m,n1,n2,n3,x,y,k,l,p;
    BOOL st;
    m=h-1;
	for(i=0;i<h;i++) 
	{
		n1=runNo[i]; 
		n2=k=runNo[i+1]; 
        if(i==m)
		{
            y=n2-1;
            for(l=n1;l<n2;l++) pDown[l]=y;
            continue;
        }
		n3=runNo[i+2]; 

        if(n2==n3)
		{
            y=n2-1;
            for(l=n1;l<n2;l++) pDown[l]=y;
            continue;
        }

		for(j=n1;j<n2;j+=2) 
		{
			x=runX[j]-1; st=FALSE;
			for(p=k;p<n3;p++) {
                if(runX[p]>x) { pDown[j]=p-1; k=p; st=TRUE; break; }
			}
            if(p==n3 && st==FALSE) { pDown[j]=p-1; k=p; }
		}

		k=n2;
		for(j=n1+1;j<n2;j+=2) 
		{
			x=runX[j]; st=FALSE;
			for(p=k;p<n3;p++) 
			{
                if(runX[p]>x) { pDown[j]=p-1; k=p; st=TRUE; break; }
			}
            if(p==n3 && st==FALSE) { pDown[j]=p-1; k=p; }
		}
	}
}
// ��:MakeUpTable
// �: m_pRunX, m_pRunNo � UpTable  m_pUp ���
template <class TRt>
void TRunProc<TRt>::MakeUpTable()
{
	MakeUpTable(m_pRunX, m_pRunNo, m_h, m_pUp);
}
// ��:MakeUpTable
// �: runX, runNo � UpTable  pUp ���
template <class TRt>
void TRunProc<TRt>::MakeUpTable(short *runX, int *runNo,int h,int* pUp)
{
	int i,j,n1,n2,n3,x,y,k,l,p;
    BOOL st;

	for(i=h;i>0;i--)
	{
		n1=runNo[i]; 
		n2=runNo[i-1]; 
        if(i==1) 
		{
            y=n2-1;
            for(l=n2;l<n1;l++) pUp[l]=y;
            continue;
        }
		n3=k=runNo[i-2]; 

        if(n2==n3) 
		{
            y=n2-1;
            for(l=n2;l<n1;l++) pUp[l]=y;
            continue;
        }

		for(j=n2;j<n1;j+=2) 
		{
			x=runX[j]-1; st=FALSE;
			for(p=k;p<n2;p++) 
			{
                if(runX[p]>x) { pUp[j]=p-1; k=p; st=TRUE; break; }
			}
            if(p==n2 && st==FALSE) { pUp[j]=p-1; k=p; }
		}

		k=n3;
		for(j=n2+1;j<n1;j+=2) 
		{
			x=runX[j]; st=FALSE;
			for(p=k;p<n2;p++) 
			{
                if(runX[p]>x) { pUp[j]=p-1; k=p; st=TRUE; break; }
			}
            if(p==n2 && st==FALSE) { pUp[j]=p-1; k=p; }
		}
	}
}
// ��:NextRunNumber
// �: m_pDown,m_pUp ��F ��run �k �run��� ��.
// ��I FALSE� ��Run �k � run ��� ��.
template <class TRt>
BOOL TRunProc<TRt>::NextRunNumber(int &i,int &p)
{
	return  NextRunNumber(m_pDown,m_pUp,i,p);
}
// ��:NextRunNumber
// �: pDown, pUp ��F ��run �k �run��� ��.
// ��I FALSE� ��Run �k � run ��� ��.
template <class TRt>
BOOL TRunProc<TRt>::NextRunNumber(int* pDown,int* pUp,int &i,int &p)
{
	int j,k,w;
	BOOL  st;

	w=i%2;
	if(w!=0)
	{
		j=pDown[i];
		w=j%2;
		if(w!=0) 
		{
			k=i-1;
			if(pDown[k]==j) i=k;
			else { i=j; p++; }
		}
		else
		{
			k=i+1;
			if(pDown[k]==j) i=k;
			else { i=j+1; p++; }
		}
	}
	else
	{
		j=pUp[i];
		w=j%2;
		if(w!=0) 
		{
			k=i+1;
			if(pUp[k]==j) i=k;
			else { i=j+1; p--; }
		}
		else
		{
			k=i-1;
			if(pUp[k]==j) i=k;
			else { i=j; p--; }
		}

	}
	w=i%2;
	st=(w)? TRUE: FALSE;
	return st;
}
// ��:MakeRunData
// �: runX, runNo, pLabel~���RunRtAry run��K���label�� ���
template <class TRt>
void TRunProc<TRt>::MakeRunData(short *runX, int *runNo,int *pLabel,TRtAry& RunRtAry,int nId)
{
	int j,k,t,p;
	int label,runEnd,runStart,runNum;
	
	TRt* pRunRt = RunRtAry[nId];
	runStart = pRunRt->nRunStart;	
	runEnd = pRunRt->nRunEnd;
	runNum = pRunRt->nRunNum; 
	label = pRunRt->nLabelNo;
	t = j = pRunRt->m_Rect.top;
	
	if(pRunRt->pRunData!=NULL){ delete[] pRunRt->pRunData;pRunRt->pRunData=NULL;}
	pRunRt->pRunData = new DWORD[runNum];
	if(pRunRt->pRunLabel!=NULL){ delete[] pRunRt->pRunLabel;pRunRt->pRunLabel=NULL;}
	pRunRt->pRunLabel = new short[runNum];
	p = 0;

	for(t++, k=runStart; k<=runEnd; k+=2)
	{
		if(runNo[t] <= k) { t++; j++; }
		if(pLabel[k] == label)
		{
			pRunRt->pRunData[p] = MAKELONG((WORD)runX[k],  (WORD)j); 
			pRunRt->pRunLabel[p] = label; 
			p++;
			pRunRt->pRunData[p] = MAKELONG((WORD)runX[k+1],(WORD)j); 
			pRunRt->pRunLabel[p] = label; 
			p++;
		}
	}
}
// ��:RunFollowing
// �: m_pRunX, m_pRunNo,m_pDown,m_pUp � �k��� ��F 
//       m_pLabel run��Labeling �� �� �k��� � �V� RunRtAry ���
template <class TRt>
void TRunProc<TRt>::RunFollowing(TRtAry& RunRtAry,BOOL bMakeRunData/*=FALSE*/,int nUserDataSize/*=0*/)
{
	if(m_pRunX==NULL || m_pRunNo==NULL || 
	   m_pDown==NULL || m_pUp==NULL || m_pLabel==NULL)
		return;
	RunFollowing(m_pRunX, m_pRunNo,m_h,m_pDown,m_pUp,m_pLabel,RunRtAry, bMakeRunData, nUserDataSize);
}
// ��:RunFollowing
// �: runX, runNo,pDown,pUp � �k��� ��F 
//       pLabel run��Labeling �� �� �k��� � �V� RunRtAry ���
//       bMakeRunData == FALSE� RunData� ��� ��.
template <class TRt>
void TRunProc<TRt>::RunFollowing(short *runX, int *runNo,int h,int* pDown,int* pUp,int* pLabel,TRtAry& RunRtAry,BOOL bMakeRunData/*=FALSE*/,int nUserDataSize/*=0*/)
{
	int i,j,k,m,g;
	int label,pixelNum,runEnd,runStart,runNum;
	int xmin,xmax,ymin,ymax;
	int maxrunlen;
	
	BOOL st;
	TRt *pRunRt;

	for(k=0, j=0; j<h; j++) for(i=runNo[j]; i<runNo[j+1]; i++) 
	{    
		if(pLabel[i] == 0) 
		{
			if(i % 2) { runStart = i; st = TRUE; k++; label = k;  } 
			else { label = pLabel[i-1]; i++; runStart = i; st = FALSE; }
			
			xmin = ymin = 10000;
			xmax = ymax = pixelNum = runEnd = runNum = maxrunlen = 0;
			
			do
			{
				if(i % 2)
				{ 
					m = runX[i + 1] - runX[i];
					pixelNum += m; 
					runEnd = max(runEnd, i);
				}
				runNum++;
				xmin = min(xmin, runX[i]);
				xmax = max(xmax, runX[i]);
				ymin = min(ymin, j);
				ymax = max(ymax, j);
				pLabel[i] = label;
				NextRunNumber(pDown,pUp,i, j);
			}while(i != runStart);
			
			if(st == TRUE) 
			{
				pRunRt = new TRt;
				pRunRt->m_Rect = CRect(xmin, ymin, xmax, ymax + 1);
				pRunRt->nRunStart = runStart;  pRunRt->nRunEnd = runEnd;
				pRunRt->nLabelNo = label;
				pRunRt->nPixelNum = pixelNum;
				pRunRt->nMaxLength = maxrunlen;
				pRunRt->nRunNum = runNum;
				RunRtAry.Add(pRunRt);
			}
			else
			{
				g = label - 1;
				RunRtAry[g]->nPixelNum += pixelNum;
				RunRtAry[g]->nMaxLength = max(RunRtAry[g]->nMaxLength,maxrunlen);
				RunRtAry[g]->nRunNum += runNum;
			}
		}
	}
	if(nUserDataSize>0)
	{
		int num = RunRtAry.GetSize();
		for(i=0; i<num; i++)
		{
			void* pUserData = malloc(nUserDataSize);
			memset(pUserData,0,nUserDataSize);
			RunRtAry.GetAt(i)->pUserData = pUserData;
		}
	}
	if(bMakeRunData)
	{
		int num = RunRtAry.GetSize();
		for(i=0; i<num; i++) MakeRunData(runX, runNo, pLabel,RunRtAry,i);
	}
}
// ��:CopyRunRtAry
// �: SourceAry��l DestAry����.
template <class TRt>
void TRunProc<TRt>::CopyRunRtAry(TRtAry& DestAry,TRtAry& SourceAry)
{
	int nSize=SourceAry.GetSize();
	for(int i=0;i<nSize;i++)
	{
		TRt*  p = new TRt;
		p->Copy(SourceAry.GetAt(i));
		DestAry.Add(p);
	}
}
// ��:RemoveAllRunRt
// �: RunRtAry��l � ���.
template <class TRt>
void TRunProc<TRt>::RemoveAllRunRt(TRtAry& RunRtAry)
{
	int i,num;
	num = RunRtAry.GetSize();
	if(num<1) return;
	for(i=0; i<num; i++) 
		delete (TRt*)RunRtAry.GetAt(i);
	RunRtAry.RemoveAll();
}

template <class TRt>
void TRunProc<TRt>::RemoveRunRt(TRtAry& RunRtAry,int index)
{
	delete RunRtAry.GetAt(index);
	RunRtAry.RemoveAt(index);
}
// ��:MakeConnectComponentFromBits
// �: pBits�c��k���V� �F RunRtAry ����
// ��I: �k�����
template <class TRt>
int TRunProc<TRt>::MakeConnectComponentFromBits(BYTE* pBits,int w,int h,TRtAry& RunRtAry,int nUserDataSize/*=0*/)
{
	RemoveAllRunRt(RunRtAry);
	int* runNo = new int[h+2];
	short* runX = new short[IMAGESIZE];
	CreateRunTableFromBits(pBits,w,h, runX,runNo);
	int retValue = MakeConnectComponent(runX, runNo, h, RunRtAry, nUserDataSize);
	delete [] runX;
	delete [] runNo;
	return retValue;
}

// ��:MakeConnectComponentFromImg
// �: pImg�c��k���V� �F RunRtAry ����
// ��I: �k�����
template <class TRt>
int TRunProc<TRt>::MakeConnectComponentFromImg(BYTE* pImg,int w,int h,TRtAry& RunRtAry,int nUserDataSize/*=0*/)
{
	CRect subrect(0,0,w,h);
	return MakeConnectComponentFromImg(pImg, w, h, RunRtAry, subrect, nUserDataSize);
}

// ��:MakeConnectComponentFromImg
// �: pImg����G subrect � �k���V� �F RunRtAry ����
// ��I: �k�����
template <class TRt>
int TRunProc<TRt>::MakeConnectComponentFromImg(BYTE* pImg, int w,int h, TRtAry& RunRtAry, CRect subrect, int nUserDataSize/*=0*/)
{
	RemoveAllRunRt(RunRtAry);
	int* runNo = new int[h+2];
	short* runX = new short[IMAGESIZE];
	CreateRunTableFromImg(pImg,w,h, runX,runNo,subrect);
	int num = MakeConnectComponent(runX, runNo, h, RunRtAry, nUserDataSize);
	delete[] runNo;
	delete[] runX;
	return num;
}

// ��:MakeConnectComponentFromImg
// �: runX, runNo~����k���V� �F RunRtAry ����
// ��I: �k�����
template <class TRt>
int TRunProc<TRt>::MakeConnectComponent(short *runX, int *runNo,int h,TRtAry& RunRtAry,int nUserDataSize/*=0*/)
{
	RemoveAllRunRt(RunRtAry);
	CreateMemory_UpDownLabel(runNo,h);
	MakeDownTable(runX, runNo, h, m_pDown);
	MakeUpTable(runX, runNo, h, m_pUp);
	RunFollowing(runX, runNo, h, m_pDown, m_pUp, m_pLabel, RunRtAry,TRUE, nUserDataSize);
	int num = RunRtAry.GetSize();
	return num;
}
template <class TRt>
void TRunProc<TRt>::OffsetRunRtAry(TRtAry& RunRtAry,CPoint ptOffset)
{
	int nSize = RunRtAry.GetSize();
	int i,nRunSize,j;
	WORD x,y;
	for(i=0;i<nSize;i++)
	{
		TRt* pRunRt = RunRtAry.GetAt(i);
		pRunRt->m_Rect.OffsetRect(ptOffset);
		nRunSize = pRunRt->nRunNum;
		for(j=0;j<nRunSize;j++)
		{
			y = HIWORD(pRunRt->pRunData[j]);
			x = LOWORD(pRunRt->pRunData[j]);
			x=WORD(x+ptOffset.x);
			y=WORD(y+ptOffset.y);
			pRunRt->pRunData[j] = MAKELONG(x,y);
		}
	}
}
//	Create By KSD 2008.9.26
//	Get Noize Removed Image using PxyRun data
//	it is Copyed from RnOCR
template <class TRt>
BYTE* TRunProc<TRt>::GetRemoveNoizeBits(BYTE* inBits,int w,int h,CSize ThSz,int ThAs)
{
	int i,j,nStart,Ps,num,wb;
	BOOL bl;

	TRtAry RunRtAry;
	CSize Sz = CSize(w,h);
	int* runNo = new int[h+2];
	short* runX = new short[IMAGESIZE];

	 CreateRunTableFromBits(inBits,w,h,runX,runNo);
	CreateMemory_UpDownLabel(runNo, h);
	MakeDownTable(runX, runNo, h, m_pDown);
	MakeUpTable(runX, runNo, h, m_pUp);
	RunFollowing(runX, runNo, h, m_pDown,m_pUp,m_pLabel, RunRtAry,TRUE);
	num = RunRtAry.GetSize();
	
	for(i=0;i<num;i++) {
		if((RunRtAry[i]->m_Rect.Width()<=ThSz.cx && RunRtAry[i]->m_Rect.Height()<=ThSz.cy) 
			|| RunRtAry[i]->nPixelNum < ThAs)
		{
			nStart=j=RunRtAry[i]->nRunStart;
			Ps=RunRtAry[i]->m_Rect.top;
			bl=TRUE;
			do {
				if(bl==TRUE)  runX[j]=-1;
				else          runX[j-1]=-1;
				bl=NextRunNumber(j,Ps);
			}while(j!=nStart);
		}
	}
	DeleteMemory_UpDownLabel();
	UpdateRunTable(runX,runNo, h);

	wb = ((((w) + 31) / 32) * 4); 
	BYTE* outBits = new BYTE[wb*h];
	GetBitsFromRunXRunNo(outBits,runX,runNo,CRect(0,0,w,h));

	delete[] runX;runX=NULL;
	delete[] runNo;runNo=NULL;

	RemoveAllRunRt(RunRtAry);
	
	return outBits;
}
template <class TRt>
BYTE* TRunProc<TRt>::GetRemoveNoizeImg(BYTE* inImg,int w,int h,CSize ThSz,int ThAs)
{
	int i,j,nStart,Ps,num;
	BOOL bl;

	TRtAry RunRtAry;
	CSize Sz = CSize(w,h);
	int* runNo = new int[h+2];
	short* runX = new short[IMAGESIZE];

	CreateRunTableFromImg(inImg,w,h,runX,runNo);
	CreateMemory_UpDownLabel(runNo, h);
	MakeDownTable(runX, runNo, h, m_pDown);
	MakeUpTable(runX, runNo, h, m_pUp);
	RunFollowing(runX, runNo, h, m_pDown,m_pUp,m_pLabel, RunRtAry,TRUE);
	num = RunRtAry.GetSize();
	
	for(i=0;i<num;i++) {
		if((RunRtAry[i]->m_Rect.Width()<=ThSz.cx && RunRtAry[i]->m_Rect.Height()<=ThSz.cy) 
			|| RunRtAry[i]->nPixelNum < ThAs)
		{
			nStart=j=RunRtAry[i]->nRunStart;
			Ps=RunRtAry[i]->m_Rect.top;
			bl=TRUE;
			do {
				if(bl==TRUE)  runX[j]=-1;
				else          runX[j-1]=-1;
				bl=NextRunNumber(j,Ps);
			}while(j!=nStart);
		}
	}
	DeleteMemory_UpDownLabel();
	UpdateRunTable(runX,runNo, h);

	BYTE* outImg = new BYTE[w*h];
	GetImgFromRunXRunNo(outImg,runX,runNo,CRect(0,0,w,h));

	delete[] runX;runX=NULL;
	delete[] runNo;runNo=NULL;

	RemoveAllRunRt(RunRtAry);
	
	return outImg;
}
template <class TRt>
BYTE* TRunProc<TRt>::GetRemoveBigCCImg(BYTE* inImg,int w,int h,int ThW, int ThH, int ThPixelNum)
{
	int i,j,nStart,Ps,num;
	BOOL bl;
	
	TRtAry RunRtAry;
	CSize Sz = CSize(w,h);
	int* runNo = new int[h+2];
	short* runX = new short[IMAGESIZE];
	
	int Rnum = CreateRunTableFromImg(inImg,w,h,runX,runNo);
	CreateMemory_UpDownLabel(runNo, h);
	MakeDownTable(runX, runNo, h, m_pDown);
	MakeUpTable(runX, runNo, h, m_pUp);
	RunFollowing(runX, runNo, h, m_pDown,m_pUp,m_pLabel, RunRtAry,TRUE);
	num = RunRtAry.GetSize();
	
	for(i=0;i<num;i++) {
		if(RunRtAry[i]->m_Rect.Width()>=ThW || RunRtAry[i]->m_Rect.Height()>=ThH 
			|| RunRtAry[i]->nPixelNum >= ThPixelNum)
		{
			nStart=j=RunRtAry[i]->nRunStart;
			Ps=RunRtAry[i]->m_Rect.top;
			bl=TRUE;
			do {
				if(bl==TRUE)  runX[j]=-1;
				else          runX[j-1]=-1;
				bl=NextRunNumber(j,Ps);
			}while(j!=nStart);
		}
	}
	DeleteMemory_UpDownLabel();
	UpdateRunTable(runX,runNo, h);
	
	BYTE* outImg = new BYTE[w*h];
	GetImgFromRunXRunNo(outImg,runX,runNo,CRect(0,0,w,h));
	
	delete[] runX;runX=NULL;
	delete[] runNo;runNo=NULL;
	
	RemoveAllRunRt(RunRtAry);
	
	return outImg;
}

// ��:SortByOption
// �: RunRtAry�  Option � Sorting�.
// Option��: SORT_PIXELNUM, SORT_SIZE, SORT_LEFT, SORT_TOP, SORT_CENTER_X, SORT_CENTER_Y, SORT_LINEID
template <class TRt>
void TRunProc<TRt>::SortByOption(TRtAry& RunRtAry,int nStart,int nEnd,int Option) 
{// Big->Small order sorting by Saturation of nPixelNum
	int i,j,num;
	TRt* swap,*pU,*pU1;
	num = RunRtAry.GetSize();
	nStart = max(0,nStart);
	nEnd = min(num, nEnd);
	switch(Option) {
	case SORT_PIXELNUM:
		for(i=nStart; i<nEnd-1; i++){
			for(j=i+1; j<nEnd; j++){
				pU = RunRtAry.GetAt(i);
				pU1 = RunRtAry.GetAt(j);
				if(pU->nPixelNum > pU1->nPixelNum)
				{
					swap = RunRtAry[i];	RunRtAry[i] = RunRtAry[j];RunRtAry[j] = swap;
				}
			}
		}
		break;
	case SORT_SIZE:
		for(i=nStart; i<nEnd-1; i++){
			for(j=i+1; j<nEnd; j++){
				pU = RunRtAry.GetAt(i);
				pU1 = RunRtAry.GetAt(j);
				if( pU->m_Rect.Width()*pU->m_Rect.Height() > pU1->m_Rect.Width()*pU1->m_Rect.Height())
				{
					swap = RunRtAry[i];	RunRtAry[i] = RunRtAry[j];RunRtAry[j] = swap;
				}
			}
		}
		break;
	case SORT_LEFT:
		for(i=nStart; i<nEnd-1; i++){
			for(j=i+1; j<nEnd; j++){
				pU = RunRtAry.GetAt(i);
				pU1 = RunRtAry.GetAt(j);
				if(pU->m_Rect.left > pU1->m_Rect.left)
				{
					swap = RunRtAry[i];	RunRtAry[i] = RunRtAry[j];RunRtAry[j] = swap;
				}
			}
		}
		break;
	case SORT_TOP:
		for(i=nStart; i<nEnd-1; i++){
			for(j=i+1; j<nEnd; j++){
				pU = RunRtAry.GetAt(i);
				pU1 = RunRtAry.GetAt(j);
				if(pU->m_Rect.top > pU1->m_Rect.top)
				{
					swap = RunRtAry[i];	RunRtAry[i] = RunRtAry[j];RunRtAry[j] = swap;
				}
			}
		}
		break;
	case SORT_CENTER_X:
		for(i=nStart; i<nEnd-1; i++){
			for(j=i+1; j<nEnd; j++){
				pU = RunRtAry.GetAt(i);
				pU1 = RunRtAry.GetAt(j);
				if((pU->m_Rect.CenterPoint().x > pU1->m_Rect.CenterPoint().x)
					|| (pU->m_Rect.CenterPoint().x == pU1->m_Rect.CenterPoint().x
					&& pU->m_Rect.CenterPoint().y > pU1->m_Rect.CenterPoint().y))
				{
					swap = RunRtAry[i];	RunRtAry[i] = RunRtAry[j];RunRtAry[j] = swap;
				}
			}
		}
		break;
	case SORT_CENTER_Y:
		for(i=nStart; i<nEnd-1; i++){
			for(j=i+1; j<nEnd; j++){
				pU = RunRtAry.GetAt(i);
				pU1 = RunRtAry.GetAt(j);
				if((pU->m_Rect.CenterPoint().y > pU1->m_Rect.CenterPoint().y)
					|| (pU->m_Rect.CenterPoint().y == pU1->m_Rect.CenterPoint().y
					&& pU->m_Rect.CenterPoint().x > pU1->m_Rect.CenterPoint().x))
				{
					swap = RunRtAry[i];	RunRtAry[i] = RunRtAry[j];RunRtAry[j] = swap;
				}
			}
		}
		break;
	case SORT_LINEID:
		for(i=nStart; i<nEnd-1; i++){
			for(j=i+1; j<nEnd; j++){
				pU = RunRtAry.GetAt(i);
				pU1 = RunRtAry.GetAt(j);
				if((pU->nLineNo > pU1->nLineNo)
					|| (pU->nLineNo == pU1->nLineNo
					&& pU->m_Rect.CenterPoint().x > pU1->m_Rect.CenterPoint().x))
				{
					swap = RunRtAry[i];	RunRtAry[i] = RunRtAry[j];RunRtAry[j] = swap;
				}
			}
		}
		break;
	default:
		for(i=nStart; i<nEnd-1; i++){
			for(j=i+1; j<nEnd; j++){
				pU = RunRtAry.GetAt(i);
				pU1 = RunRtAry.GetAt(j);
				if(pU->nPixelNum > pU1->nPixelNum)
				{
					swap = RunRtAry[i];	RunRtAry[i] = RunRtAry[j];RunRtAry[j] = swap;
				}
			}
		}
	}
}

// Functions for Image rotation by runX,runNo Data
template <class TRt>
void TRunProc<TRt>::rtConvert(short *runX,int *runNo, int h)
{
	int i,j,k,m,n,p,dt;
	for(j=0;j<=h;j++) {
		p=j+1;
		k=(runNo[p]-runNo[j])/2;
		for(i=0;i<k;i++) {
			m=runNo[j]+i;
			n=runNo[p]-i-1;
			dt=runX[m];
			runX[m]=1-runX[n];
			runX[n]=1-dt;
		}
	}
}
template <class TRt>
void TRunProc<TRt>::Skew(short *runX,int *runNo, short *newRunX, int *newRunNo,CSize &size, double d1, double d2)
{
	int i,j,k,p,h,z,t,x0,y0,x1,x2,num;
	double r, *pcs;
	num=1;

	int quot,rem;
	quot = size.cx / 2;
	rem = size.cx % 2;
	x0=quot;
	if(rem!=0)	p=x0+1;
	else p=x0;

	quot = size.cy / 2;
	rem = size.cy % 2;
	y0=quot;
	if(rem!=0)	k=y0+1;
	else k=y0;

    newRunNo[0]=1;
    for(j=0;j<size.cy;j++) 
    for(i=runNo[j];i<runNo[j+1];i++) 
		newRunX[i]=runX[i]-x0;

	if(d2<0) { rtConvert(newRunX,runNo,size.cy); d2=fabs(d2); }

    i=(int)floor(k*d1+p*d2);
	j=(int)floor(k*d1-p*d2);
	h=max(abs(i),abs(j))+2;

    z=p+2; pcs = new double[z];

	for(i=0;i<z;i++) pcs[i]=(double)i*d2;
	for(j=0;j<size.cy;j++) {

		r=(double)(j-y0)*d1;
		t=-100000;

		for(i=runNo[j];i<runNo[j+1];i+=2) {
			
            z=i+1; 
			if(newRunX[i]<0) x1=(int)(r-pcs[abs(newRunX[i])]);
			else x1=(int)(r+pcs[newRunX[i]]);
			
			if(newRunX[z]<0) x2=(int)(r-pcs[abs(newRunX[z])]);
  			else x2=(int)(r+pcs[newRunX[z]]);
			//if((x2-x1)==1) continue;

			if(x1<=t) { newRunX[num-1]=x2; t=x2; continue; }
			newRunX[num]=x1; num++;
			newRunX[num]=x2; num++;
			t=x2;
		}
		newRunNo[j+1]=num;
	}

    for(j=0;j<size.cy;j++)
    for(i=newRunNo[j];i<newRunNo[j+1];i++)
    newRunX[i]+=h;

    size.cx=2*h;
	delete []pcs; pcs=NULL;
}

template <class TRt>
void TRunProc<TRt>::Skew(short *runX,int *runNo, CSize& size, double d1, double d2)
{
	int i,j,k,p,h,z,st,t,x0,y0,x1,x2,num;
	double r, *pcs;
	st=num=1;
	int quot,rem;
	quot = size.cx/2;
	rem = size.cx%2;
	x0 = quot;
	if(rem!=0)	p=x0+1;
	else p=x0;

	quot = size.cy/2;
	rem = size.cy%2;
	y0=quot;
	if(rem!=0)	k=y0+1;
	else k=y0;

    for(j=0;j<size.cy;j++)
    for(i=runNo[j];i<runNo[j+1];i++)
		runX[i]-=x0;

	if(d2<0) { rtConvert(runX,runNo,size.cy); d2=fabs(d2); }

    i=(int)floor(k*d1+p*d2);
	j=(int)floor(k*d1-p*d2);
	h=max(abs(i),abs(j))+2;

    z=p+2; pcs = new double[z];
	for(i=0;i<z;i++) pcs[i]=(double)i*d2;

    for(j=0;j<size.cy;j++) {

		r=(double)(j-y0)*d1;
		t=-100000;

		for(i=runNo[j];i<runNo[j+1];i+=2) {
			
            z=i+1; 
			if(runX[i]<0) x1=(int)(r-pcs[abs(runX[i])]);
			else x1=(int)(r+pcs[runX[i]]);
			
			if(runX[z]<0) x2=(int)(r-pcs[abs(runX[z])]);
			else x2=(int)(r+pcs[runX[z]]);
			//if((x2-x1)==1) continue;

			if(x1<=t) { runX[num-1]=x2; t=x2; continue; }
			runX[num]=x1; num++;
			runX[num]=x2; num++;
			t=x2;
		}
		runNo[j]=st; st=num;
	}
	runNo[j]=st;

    for(j=0;j<size.cy;j++)
    for(i=runNo[j];i<runNo[j+1];i++)
    runX[i]+=h;

    size.cx=2*h;
	delete []pcs; pcs=NULL;
}
template <class TRt>
void TRunProc<TRt>::Transposing(short *runX,int *runNo,CSize &size)
{
	int i,j,k,p,q,n,h,t,cy,x1,x2,count;
	int *rn,*dd;
	short *rd;

    rn = new int[size.cy+2];
    dd = new int[size.cx];
    rd = new short[IMAGESIZE];

	rn[0]=1; count=1;
	cy=size.cy;

	for(j=0;j<=cy;j++) {
		if(j==0 || j==cy) {
			if(j==0) { t=j+1; x1=runNo[j]; x2=runNo[t]; }
			else { t=j-1; x1=runNo[t]; x2=runNo[j]; }
			for(i=x1;i<x2;i++) { rd[count]=runX[i]; count++; }
		}
		else {
            p=n=runNo[j]; q=runNo[j+1]; h=runNo[j-1]; 
            while(!(p==q || h==n)) {
                x1=runX[p]; x2=runX[h];
                if(x1<x2) { rd[count]=x1; p++; count++; }
                else if(x1>x2) { rd[count]=x2; h++; count++;}
                else { p++; h++; }
            }

            if((q-p)>0) for(i=p;i<q;i++) { rd[count]=runX[i]; count++; }
            if((n-h)>0) for(i=h;i<n;i++) { rd[count]=runX[i]; count++; }
		}
		rn[j+1]=count;
	}
    h=j;

	memset(runNo,0,4*(size.cx+1));
    for(j=0;j<=cy;j++) {
        for(i=rn[j];i<rn[j+1];i+=2) {
            for(k=rd[i];k<rd[i+1];k++) {
                runNo[k]++;
            }
        }
    }
    for(i=0;i<size.cx;i++)  dd[i]=0;
    for(i=size.cx;i>=0;i--) runNo[i+1]=runNo[i];
    for(runNo[0]=1,i=0;i<=size.cx;i++) runNo[i+1]+=runNo[i];

    for(j=0;j<=cy;j++) {
        for(i=rn[j];i<rn[j+1];i+=2) {
            for(k=rd[i];k<rd[i+1];k++) {
                runX[runNo[k]+dd[k]]=j; dd[k]++;
            }
        }
    }
	
    delete []rd; rd=NULL;
	delete []dd; dd=NULL; 
	delete []rn; rn=NULL;
	size.cy=size.cx+1;
	size.cx=h;
}
template <class TRt>
void TRunProc<TRt>::Rotate_RunXRunNo(short* runX,int* runNo,double fAng,CSize &size)
{
	double k1,k2,d1,d2,d3,d4,pd,rem;
	int i,quot;
	double fAngle = -fAng;
	CSize OrgSz = size;
	pd=fAngle/90.0;
	rem=fmod(fAngle,90.0);
	if(rem==0.0) {
		quot=(int)pd%4;
		switch(quot) {
			case 0:   k1=0.0;   k2=1.0;   break;
			case 1:   k1=1.0;   k2=0.0;   break;
			case 2:   k1=0.0;   k2=-1.0;  break;
			case 3:   k1=-1.0;  k2=0.0;   break;
			case -1:  k1=-1.0;  k2=0.0;   break;
			case -2:  k1=0.0;   k2=-1.0;  break;
			case -3:  k1=1.0;   k2=0.0;   break;
			default:  break;
		}
	}
	else {
	    pd=3.14159265359*fAngle/(double)180; 
		k1=sin(pd); k2=cos(pd);
	}

	short *tempRunX = new short[IMAGESIZE];
	int   *tempRunNo = new int[size.cy*20];

	if(k2<=-0.707 || k2>=0.707) { 
		d1=k1; d2=k2; d3=(-1)*k1/k2; d4=(double)1/k2; 
	    Skew(runX,runNo,tempRunX,tempRunNo,size,d1,d2);
	    Transposing(tempRunX,tempRunNo,size);
	    Skew(tempRunX,tempRunNo,size,d3,d4);
		Transposing(tempRunX,tempRunNo,size);
	}
	else  { 
		d1=k2; d2=(-1)*k1; d3=(-1)*k2/k1; d4=(double)1/k1; 
	    Skew(runX,runNo,tempRunX,tempRunNo,size,d1,d2);
	    Transposing(tempRunX,tempRunNo,size);
	    Skew(tempRunX,tempRunNo,size,d3,d4);
	}
	for(i=0;i<=size.cy;i++) runNo[i]=tempRunNo[i];
	for(i=0;i<tempRunNo[size.cy];i++) runX[i]=tempRunX[i];
	delete []tempRunNo; tempRunNo=NULL;
	delete []tempRunX; tempRunX=NULL;

}
template <class TRt>
BOOL TRunProc<TRt>::GetAngleFromBits(BYTE* lpBits, int w, int h, double& fAng)
{
	CSize Sz = CSize(w,h);
	TRtAry RunRtAry;
	int Rnum = CreateRunTableFromBits(lpBits,w,h);
	CreateMemory_UpDownLabel(m_pRunNo, h);
	MakeDownTable();
	MakeUpTable();
	RunFollowing(RunRtAry);
	DeleteMemory_UpDownLabel();
	fAng = GetAngleFromRunRtAry(RunRtAry,w,h);
	RemoveAllRunRt(RunRtAry);
	return TRUE;
}
template <class TRt>
BOOL TRunProc<TRt>::GetAngleFromImg(BYTE* pImg,int w,int h,double& fAng)
{
	CSize Sz = CSize(w,h);
	TRtAry RunRtAry;
	int Rnum = CreateRunTableFromImg(pImg,w,h);
	CreateMemory_UpDownLabel(m_pRunNo, h);
	MakeDownTable();
	MakeUpTable();
	RunFollowing(RunRtAry);
	DeleteMemory_UpDownLabel();
	fAng = GetAngleFromRunRtAry(RunRtAry,w,h);
	RemoveAllRunRt(RunRtAry);
	return TRUE;
}
template <class TRt>
void TRunProc<TRt>::MergeRts(TRtAry &ary, int w,int h, int charHeight,int lineSpace)
{
	int nNum=ary.GetSize();
	int nLimit1=lineSpace/2;
	int nLimit2=charHeight/10;
	int nLimit=(int)((charHeight*1.5f));

	int i,j,count,nOvlapW,nMinW,nOvlapH,nMinH;
	BOOL bAppend;CRect rect;
	for(i=0;i<nNum;i++)
	{
		ary[i]->bUse = TRUE;
		if(ary[i]->m_Rect.Width()>charHeight*2 || 
			ary[i]->m_Rect.Height()>charHeight*2)
			ary[i]->bUse = FALSE;
	}
	do {
		count=0;
		for(i=0;i<nNum;i++)
		{
			if(ary[i]->bUse==FALSE) continue;
			for(j=0;j<nNum;j++)
			{
				if(i==j || ary[j]->bUse==FALSE) continue;
				bAppend=FALSE;
				nOvlapW=min(ary[i]->m_Rect.right,ary[j]->m_Rect.right)
					-max(ary[i]->m_Rect.left,ary[j]->m_Rect.left);
				if(nOvlapW>0)
				{
					nMinW=min(ary[i]->m_Rect.Width(),ary[j]->m_Rect.Width());
					if(float(nOvlapW)/float(nMinW)>0.35f)
					{
						if(ary[i]->m_Rect.top-ary[j]->m_Rect.bottom<=nLimit1
							&& ary[i]->m_Rect.bottom>=ary[j]->m_Rect.bottom)
							bAppend=TRUE;
						else if(ary[j]->m_Rect.top-ary[i]->m_Rect.bottom<=nLimit1
							&& ary[j]->m_Rect.bottom>=ary[i]->m_Rect.bottom) 
							bAppend=TRUE;
					}
				}
				else if(bAppend==FALSE)
				{
					nOvlapH=min(ary[i]->m_Rect.bottom,ary[j]->m_Rect.bottom)
						-max(ary[i]->m_Rect.top,ary[j]->m_Rect.top);
					if(nOvlapH>0)
					{
						nMinH=min(ary[i]->m_Rect.Height(),ary[j]->m_Rect.Height());
						if(float(nOvlapH)/float(nMinH)>0.35f)
						{
							if(ary[i]->m_Rect.left-ary[j]->m_Rect.right<=nLimit2
								&& ary[i]->m_Rect.right>=ary[j]->m_Rect.right)
								bAppend=TRUE;
							else if(ary[j]->m_Rect.left-ary[i]->m_Rect.right<=nLimit2
								&& ary[j]->m_Rect.right>=ary[i]->m_Rect.right)
								bAppend=TRUE;
						}

					}
				}
				if(bAppend==TRUE)
				{
					rect.UnionRect(ary[i]->m_Rect,ary[j]->m_Rect);
					if(rect.Width()<nLimit && rect.Height()<nLimit)
					{
						ary[i]->AppendWithoutRunData(ary[j]);
						ary[j]->bUse=FALSE;
						count++;
					}
				}
			}
		} 
	}while(count!=0);
	for(i=0;i<nNum;i++)
	{
		if(ary[i]->bUse==FALSE)
		{
			RemoveRunRt(ary,i);
			i--;nNum--;
		}
	}

}

typedef struct tagCouple{
	CRect cRt1;
	CRect cRt2;
	BYTE	op;
	double	ang;
}Couple;

template <class TRt>
double TRunProc<TRt>::GetAngleFromRunRtAry(TRtAry& RunRtAry,int w,int h)
{
	double fAng=0.0;
	int i,j,k,hh,*pHighHis,nNum,nId,nMaxH,ww;
	TRt*	pRsn,*pRsn1,*pRsn2;
	CRect		cRt,cRt1,cRt2;

	pHighHis = new int[h+1];
	memset(pHighHis,0,h*sizeof(int));

	nNum = RunRtAry.GetSize();
	if(nNum >1000)
	{
		for(i=1000;i<nNum;i++)
		{		
			delete (TRt*)RunRtAry.GetAt(i);
			RunRtAry.RemoveAt(i);
			i--;nNum--;
		}
	}
	if(nNum<20)
	{
		delete []pHighHis;
		return 0.0;
	}
	nNum = RunRtAry.GetSize();

	//// ²ÚËË ÃÅº÷ÀË°ûµ½ °éÂ×±¨ ////

	for(i=0;i<nNum;i++){
		pRsn = (TRt*)RunRtAry.GetAt(i);
		pRsn->bUse = 1;
		cRt = pRsn->m_Rect;
		if(cRt.Height()<cRt.Width())continue;
		ww = cRt.Width();	hh = cRt.Height();
		if(hh<10 && ww<10)continue;
		pHighHis[hh]++;

	}
	nId = -1;nMaxH = 0;
	for(i=0;i<h;i++){
		if(pHighHis[i]>nMaxH){
			nMaxH = pHighHis[i];
			nId = i;
		}
	}
	if(nId<10 || nMaxH<10) 
	{
		delete []pHighHis;
		return 0.0;
	}
	if(nId == -1){
		for(i=0;i<nNum;i++)		
			delete (TRt*)RunRtAry.GetAt(i);
		RunRtAry.RemoveAll();
		delete[] pHighHis; pHighHis=NULL;
		return fAng;//FALSE;
	}

	int nTmin,nTmax;

	////// ²ÚËËÌ© ¾×´Ý,¾×ºÏ °éÂ×±¨ ///////
	nTmin = (int)((double)nId*0.5);
	nTmax = (int)((double)nId*1.5);

	for(i=0;i<nNum;i++){
		pRsn = (TRt*)RunRtAry.GetAt(i);
		cRt = pRsn->m_Rect;
		if(cRt.Height()<cRt.Width()){
			pRsn->bUse = 0;
			continue;
		}
		hh = cRt.Height();
		if(hh<nTmin || hh>nTmax){
			pRsn->bUse = 0;
			continue;
		}
	}

	for(i=0;i<nNum;i++){
		pRsn = (TRt*)RunRtAry.GetAt(i);
		if(pRsn->bUse == 1)continue;
		delete (TRt*)RunRtAry.GetAt(i);
		RunRtAry.RemoveAt(i);
		i--;nNum--;
	}

	delete[] pHighHis; pHighHis=NULL;


	//// ¹¦µá¿Í²ÚËËÌ® ¹¦µá¿Í±¶ºã °éÂ×±¨ //////
	int nSubBlockH,nBlockNum;

	nSubBlockH = nId*20;

	nBlockNum = (int)(h/nSubBlockH);
	if(nBlockNum == 0)nBlockNum=1;
	CRect cSubAllRt[50];

	for(i=0;i<nBlockNum;i++){
		cSubAllRt[i].left = 0;
		cSubAllRt[i].right = w;
		cSubAllRt[i].top = i*nSubBlockH;
		if(i == (nBlockNum-1))
			cSubAllRt[i].bottom = h;
		else 
			cSubAllRt[i].bottom = (i+1)*nSubBlockH;
	}


	////// °¢´ª ÃÅº÷ÀË°ûµ½ °éÂ×±¨ ///////

	int AngleHis[50][2][90],nBlkId1,nBlkId2;
	for(i=0;i<50;i++){
		for(j=0;j<2;j++){			
			for(k=0;k<90;k++){
				AngleHis[i][j][k] = 0;
			}
		}
	}

	for(i=0;i<nNum;i++){
		pRsn1 = (TRt*)RunRtAry.GetAt(i);
		cRt1 = pRsn1->m_Rect;
		nBlkId1 = GetBlkId(cRt1,cSubAllRt,nBlockNum);
		if(nBlkId1 == -1)continue;
		for(j=0;j<nNum;j++){
			if(i==j)continue;
			pRsn2 = (TRt*)RunRtAry.GetAt(j);
			cRt2 = pRsn2->m_Rect;
			nBlkId2 = GetBlkId(cRt2,cSubAllRt,nBlockNum);
			if(nBlkId2 == -1)continue;
			if(nBlkId1 != nBlkId2)continue;
			double ang1,ang2;
			GetLUAngle(cRt1,cRt2,ang1,ang2);			
			int a1 = (int)ang1,a2 = (int)ang2;
			if(abs(a1)<45)
				AngleHis[nBlkId1][0][45+a1]++;
			if(abs(a2)<45)
				AngleHis[nBlkId1][1][45+a2]++;

		}
	}
	//// °º¾¹ °¢´ª °éÂ×±¨ ////////////
	int n1,n2,n3,nMax;
	nMax = 0;
	for(i=0;i<nBlockNum;i++){
		for(j=0;j<2;j++){
			for(k=0;k<90;k++){
				if(AngleHis[i][j][k]>nMax){
					nMax = AngleHis[i][j][k];
					n1 = i;n2 = j;n3 = k;
				}
			}
		}
	}
	n3 = n3-45;

	///// °¢´ª n3Ë¾ ¾¡¾¥Â×²÷´ç ¶®Ë¦´ô 4°¢Âô ÊÐ±¨ /////
	int		nSubRtNum=0;
	double ang1;
	ang1 = n3;

	typedef struct tagCouple{
		CRect cRt1;
		CRect cRt2;
		BYTE	op;
		double	ang;
	}Couple;

	Couple *pCouple = new Couple[nNum*nNum];

	for(i=0;i<nNum;i++){
		pRsn1 = (TRt*)RunRtAry.GetAt(i);
		cRt1 = pRsn1->m_Rect;
		nBlkId1 = GetBlkId(cRt1,cSubAllRt,nBlockNum);
		if(nBlkId1 == -1)continue;
		for(j=0;j<nNum;j++){
			if(i==j)continue;
			pRsn2 = (TRt*)RunRtAry.GetAt(j);
			cRt2 = pRsn2->m_Rect;
			nBlkId2 = GetBlkId(cRt2,cSubAllRt,nBlockNum);
			if(nBlkId2 == -1)continue;
			if(nBlkId1 != nBlkId2)continue;
			double ang1,ang2;
			GetLUAngle(cRt1,cRt2,ang1,ang2);			
			int a1 = (int)ang1,a2 = (int)ang2;
			if(n2 == 0 &&  abs(a1)<45){
				if(n3 == a1){
					pCouple[nSubRtNum].cRt1 = cRt1;
					pCouple[nSubRtNum].cRt2 = cRt2;
					pCouple[nSubRtNum].ang = ang1;
					nSubRtNum++;				
				}
			}
			else if(n2 == 1 &&  abs(a1)<45){
				if(n3 == a2){
					pCouple[nSubRtNum].cRt1 = cRt1;
					pCouple[nSubRtNum].cRt2 = cRt2;
					pCouple[nSubRtNum].ang = ang2;
					nSubRtNum++;					
				}
			}
		}		
	}
	if(nSubRtNum == 0){
		fAng = ang1;

		for(i=0;i<nNum;i++)		
			delete (TRt*)RunRtAry.GetAt(i);
		RunRtAry.RemoveAll();
		delete[] pCouple; pCouple=NULL;
		if(fabs(fAng) > 5) fAng=0;
		return fAng;
	}


	double ag;
	int newId;
	int newAng[200];
	memset(newAng,0,200*sizeof(int));
	for(i=0;i<nSubRtNum;i++){
		if(pCouple[i].ang>n3-0.99 && pCouple[i].ang<n3+0.99){
			ag = pCouple[i].ang;
			newId = (int)(100.0*(ag-n3));			
			if(newId == 0)continue;
			newAng[100+newId]++;

		}			
	}
	//int max1,max2,max3;
	int maxValue=0;
	newId=0;
	for(i=0;i<200;i++){
		if(newAng[i]>maxValue){
			maxValue = newAng[i];
			newId = i;
		}
	}
	fAng = n3-(100-newId)*0.01;

	// Ãä¼­°¢´ª°¡ +-5´ª¶¦ ²¿Ëº·² °¢´ª¾¡¾¥Ëæº· ËËº¬Âïº¬ËË ËÎÊÌ²°´Ñ°Ö ¸õ³Þ.
	// ËËÆÁ²÷ ºã´´Ãä¼­±¨²þË¾ ¶®Ë¦ÃÍÊ¿ ÂÝ³£³Þ.
	if(fabs(fAng) > 25) fAng=0;

	delete[] pCouple; pCouple=NULL;
	return fAng;
}
template <class TRt>
double TRunProc<TRt>::GetAngleFromRunRtAry_1(TRtAry& RunRtAry,int w,int h)
{
	double fAng=0.0;
	int i,j,k,hh,*pHighHis,nNum,nId,nMaxH,ww;
	TRt* pRsn,*pRsn1,*pRsn2;
	CRect  cRt,cRt1,cRt2;

	pHighHis = new int[h+1];
	memset(pHighHis,0,h*sizeof(int));

	nNum = RunRtAry.GetSize();
	if(nNum >1000)
	{
		for(i=1000;i<nNum;i++)
		{  
			delete (TRt*)RunRtAry.GetAt(i);
			RunRtAry.RemoveAt(i);
			i--;nNum--;
		}
	}
	if(nNum<20)
	{
		delete []pHighHis;
		return 0.0;
	}
	nNum = RunRtAry.GetSize();

	//// 糙怂 门瑚浪胞到 伴伦报 ////

	for(i=0;i<nNum;i++){
		pRsn = (TRt*)RunRtAry.GetAt(i);
		pRsn->bUse = 1;
		cRt = pRsn->m_Rect;
		if(cRt.Height()<cRt.Width())continue;
		ww = cRt.Width(); hh = cRt.Height();
		if(hh<10 && ww<10)continue;
		pHighHis[hh]++;

	}
	nId = -1;nMaxH = 0;
	for(i=0;i<h;i++){
		if(pHighHis[i]>nMaxH){
			nMaxH = pHighHis[i];
			nId = i;
		}
	}
	if(nId<8 || nMaxH<10)//40) 
	{
		delete []pHighHis;
		return 0.0;
	}
	if(nId == -1){
		for(i=0;i<nNum;i++)  
			delete (TRt*)RunRtAry.GetAt(i);
		RunRtAry.RemoveAll();
		delete[] pHighHis; pHighHis=NULL;
		return fAng;//FALSE;
	}

	int nTmin,nTmax;

	////// 糙怂泰 咀摧,咀合 伴伦报 ///////
	nTmin = (int)((double)nId*0.5);
	nTmax = (int)((double)nId*1.5);

	for(i=0;i<nNum;i++){
		pRsn = (TRt*)RunRtAry.GetAt(i);
		cRt = pRsn->m_Rect;
		if(cRt.Height()<cRt.Width()){
			pRsn->bUse = 0;
			continue;
		}
		hh = cRt.Height();
		if(hh<nTmin || hh>nTmax){
			pRsn->bUse = 0;
			continue;
		}
	}

	for(i=0;i<nNum;i++){
		pRsn = (TRt*)RunRtAry.GetAt(i);
		if(pRsn->bUse == 1)continue;
		delete (TRt*)RunRtAry.GetAt(i);
		RunRtAry.RemoveAt(i);
		i--;nNum--;
	}

	delete[] pHighHis; pHighHis=NULL;


	//// 功滇客糙怂坍 功滇客倍恒 伴伦报 //////
	int nSubBlockH,nBlockNum;

	nSubBlockH = nId*20;

	nBlockNum = (int)(h/nSubBlockH);
	if(nBlockNum == 0)nBlockNum=1;
	CRect cSubAllRt[50];

	for(i=0;i<nBlockNum;i++){
		cSubAllRt[i].left = 0;
		cSubAllRt[i].right = w;
		cSubAllRt[i].top = i*nSubBlockH;
		if(i == (nBlockNum-1))
			cSubAllRt[i].bottom = h;
		else 
			cSubAllRt[i].bottom = (i+1)*nSubBlockH;
	}


	////// 阿椽 门瑚浪胞到 伴伦报 ///////

	int AngleHis[50][2][90],nBlkId1,nBlkId2;
	for(i=0;i<50;i++){
		for(j=0;j<2;j++){   
			for(k=0;k<90;k++){
				AngleHis[i][j][k] = 0;
			}
		}
	}

	for(i=0;i<nNum;i++){
		pRsn1 = (TRt*)RunRtAry.GetAt(i);
		cRt1 = pRsn1->m_Rect;
		nBlkId1 = GetBlkId(cRt1,cSubAllRt,nBlockNum);
		if(nBlkId1 == -1)continue;
		for(j=0;j<nNum;j++){
			if(i==j)continue;
			pRsn2 = (TRt*)RunRtAry.GetAt(j);
			cRt2 = pRsn2->m_Rect;
			nBlkId2 = GetBlkId(cRt2,cSubAllRt,nBlockNum);
			if(nBlkId2 == -1)continue;
			if(nBlkId1 != nBlkId2)continue;
			double ang1,ang2;
			GetLUAngle(cRt1,cRt2,ang1,ang2);   
			int a1 = (int)ang1,a2 = (int)ang2;
			if(abs(a1)<45)
				AngleHis[nBlkId1][0][45+a1]++;
			if(abs(a2)<45)
				AngleHis[nBlkId1][1][45+a2]++;

		}
	}
	//// 昂竟 阿椽 伴伦报 ////////////
	int n1,n2,n3,nMax;
	nMax = 0;
	for(i=0;i<nBlockNum;i++){
		for(j=0;j<2;j++){
			for(k=0;k<90;k++){
				if(AngleHis[i][j][k]>nMax){
					nMax = AngleHis[i][j][k];
					n1 = i;n2 = j;n3 = k;
				}
			}
		}
	}
	n3 = n3-45;

	///// 阿椽 n3司 尽茎伦谗寸 懂甩呆 4阿卖 市报 /////
	int  nSubRtNum=0;
	double ang1;
	ang1 = n3;

	typedef struct tagCouple{
		CRect cRt1;
		CRect cRt2;
		BYTE op;
		double ang;
	}Couple;

	Couple *pCouple = new Couple[nNum*nNum];

	for(i=0;i<nNum;i++){
		pRsn1 = (TRt*)RunRtAry.GetAt(i);
		cRt1 = pRsn1->m_Rect;
		nBlkId1 = GetBlkId(cRt1,cSubAllRt,nBlockNum);
		if(nBlkId1 == -1)continue;
		for(j=0;j<nNum;j++){
			if(i==j)continue;
			pRsn2 = (TRt*)RunRtAry.GetAt(j);
			cRt2 = pRsn2->m_Rect;
			nBlkId2 = GetBlkId(cRt2,cSubAllRt,nBlockNum);
			if(nBlkId2 == -1)continue;
			if(nBlkId1 != nBlkId2)continue;
			double ang1,ang2;
			GetLUAngle(cRt1,cRt2,ang1,ang2);   
			int a1 = (int)ang1,a2 = (int)ang2;
			if(n2 == 0 &&  abs(a1)<45){
				if(n3 == a1){
					pCouple[nSubRtNum].cRt1 = cRt1;
					pCouple[nSubRtNum].cRt2 = cRt2;
					pCouple[nSubRtNum].ang = ang1;
					nSubRtNum++;    
				}
			}
			else if(n2 == 1 &&  abs(a1)<45){
				if(n3 == a2){
					pCouple[nSubRtNum].cRt1 = cRt1;
					pCouple[nSubRtNum].cRt2 = cRt2;
					pCouple[nSubRtNum].ang = ang2;
					nSubRtNum++;     
				}
			}
		}  
	}
	if(nSubRtNum == 0){
		fAng = ang1;

		for(i=0;i<nNum;i++)  
			delete (TRt*)RunRtAry.GetAt(i);
		RunRtAry.RemoveAll();
		delete[] pCouple; pCouple=NULL;
		if(fabs(fAng) > 5) fAng=0;
		return fAng;
	}


	double ag;
	int newId;
	int newAng[200];
	memset(newAng,0,200*sizeof(int));
	for(i=0;i<nSubRtNum;i++){
		if(pCouple[i].ang>n3-0.99 && pCouple[i].ang<n3+0.99){
			ag = pCouple[i].ang;
			newId = (int)(100.0*(ag-n3));   
			if(newId == 0)continue;
			newAng[100+newId]++;

		}   
	}
	//int max1,max2,max3;
	int maxValue=0;
	newId=0;
	for(i=0;i<200;i++){
		if(newAng[i]>maxValue){
			maxValue = newAng[i];
			newId = i;
		}
	}
	fAng = n3-(100-newId)*0.01;

	// 娩辑阿椽啊 +-5椽鼎 部撕凡 阿椽尽茎随悍 怂含嘛含怂 宋侍舶囱爸 铬侈.
	// 怂屏谗 恒创娩辑报猖司 懂甩猛士 螺常侈.
	if(fabs(fAng) > 25) fAng=0;

	delete[] pCouple; pCouple=NULL;
	return fAng;
}
template <class TRt>
double	TRunProc<TRt>::GetAngleByRtsInLines(TRtAry& RunRtAry,int w,int h,int nLen)
{
	double fAng=0.0;
	int i,j,k,nNum;
	TRt	*pRsn1,*pRsn2;
	CRect		cRt,cRt1,cRt2;

	nNum = RunRtAry.GetSize();
	for(i=0;i<nNum;i++)
	{	
		CRect r(RunRtAry.GetAt(i)->m_Rect);
		if(r.Width()>nLen || r.Height()>20 || r.Height()<r.Width())
		{
			delete (TRt*)RunRtAry.GetAt(i);
			RunRtAry.RemoveAt(i);
			i--;nNum--;
		}
	}
	nNum = RunRtAry.GetSize();


	
	int nSubBlockH,nBlockNum;

	nSubBlockH = h/2;

	nBlockNum = (int)(h/nSubBlockH);
	if(nBlockNum == 0)nBlockNum=1;
	CRect cSubAllRt[50];

	for(i=0;i<nBlockNum;i++){
		cSubAllRt[i].left = 0;
		cSubAllRt[i].right = w;
		cSubAllRt[i].top = i*nSubBlockH;
		if(i == (nBlockNum-1))
			cSubAllRt[i].bottom = h;
		else 
			cSubAllRt[i].bottom = (i+1)*nSubBlockH;
	}


	

	int AngleHis[50][2][90],nBlkId1,nBlkId2;
	for(i=0;i<50;i++){
		for(j=0;j<2;j++){			
			for(k=0;k<90;k++){
				AngleHis[i][j][k] = 0;
			}
		}
	}

	for(i=0;i<nNum;i++){
		pRsn1 = (TRt*)RunRtAry.GetAt(i);
		cRt1 = pRsn1->m_Rect;
		nBlkId1 = GetBlkId(cRt1,cSubAllRt,nBlockNum);
		if(nBlkId1 == -1)continue;
		for(j=0;j<nNum;j++){
			if(i==j)continue;
			pRsn2 = (TRt*)RunRtAry.GetAt(j);
			cRt2 = pRsn2->m_Rect;
			nBlkId2 = GetBlkId(cRt2,cSubAllRt,nBlockNum);
			if(nBlkId2 == -1)continue;
			if(nBlkId1 != nBlkId2)continue;
			double ang1,ang2;
			GetLUAngle(cRt1,cRt2,ang1,ang2);			
			int a1 = (int)ang1,a2 = (int)ang2;
			if(abs(a1)<45)
				AngleHis[nBlkId1][0][45+a1]++;
			if(abs(a2)<45)
				AngleHis[nBlkId1][1][45+a2]++;

		}
	}
	
	int n1,n2,n3,nMax;
	nMax = 0;
	for(i=0;i<nBlockNum;i++){
		for(j=0;j<2;j++){
			for(k=0;k<90;k++){
				if(AngleHis[i][j][k]>nMax){
					nMax = AngleHis[i][j][k];
					n1 = i;n2 = j;n3 = k;
				}
			}
		}
	}
	n3 = n3-45;
	
	
	int		nSubRtNum=0;
	double ang1;
	ang1 = n3;

	typedef struct tagCouple{
		CRect cRt1;
		CRect cRt2;
		BYTE	op;
		double	ang;
	}Couple;
	
	Couple *pCouple = new Couple[nNum*nNum];
	
	for(i=0;i<nNum;i++){
		pRsn1 = (TRt*)RunRtAry.GetAt(i);
		cRt1 = pRsn1->m_Rect;
		nBlkId1 = GetBlkId(cRt1,cSubAllRt,nBlockNum);
		if(nBlkId1 == -1)continue;
		for(j=0;j<nNum;j++){
			if(i==j)continue;
			pRsn2 = (TRt*)RunRtAry.GetAt(j);
			cRt2 = pRsn2->m_Rect;
			nBlkId2 = GetBlkId(cRt2,cSubAllRt,nBlockNum);
			if(nBlkId2 == -1)continue;
			if(nBlkId1 != nBlkId2)continue;
			double ang1,ang2;
			GetLUAngle(cRt1,cRt2,ang1,ang2);			
			int a1 = (int)ang1,a2 = (int)ang2;
			if(n2 == 0 &&  abs(a1)<45){
				if(n3 == a1){
					pCouple[nSubRtNum].cRt1 = cRt1;
					pCouple[nSubRtNum].cRt2 = cRt2;
					pCouple[nSubRtNum].ang = ang1;
					nSubRtNum++;				
				}
			}
			else if(n2 == 1 &&  abs(a1)<45){
				if(n3 == a2){
					pCouple[nSubRtNum].cRt1 = cRt1;
					pCouple[nSubRtNum].cRt2 = cRt2;
					pCouple[nSubRtNum].ang = ang2;
					nSubRtNum++;					
				}
			}
		}		
	}
	if(nSubRtNum == 0){
		fAng = ang1;
		
		for(i=0;i<nNum;i++)		
			delete (TRt*)RunRtAry.GetAt(i);
		RunRtAry.RemoveAll();
		delete[] pCouple; pCouple=NULL;
		if(fabs(fAng) > 5) fAng=0;
		return fAng;
	}

	
	double avAng = 0.0,maxAng = -180,minAng=180,ag;
	int nAvNum=0,newId;
	int newAng[200];
	memset(newAng,0,200*sizeof(int));
	for(i=0;i<nSubRtNum;i++){
		if(pCouple[i].ang>n3-0.99 && pCouple[i].ang<n3+0.99){
			ag = pCouple[i].ang;
			newId = (int)(100.0*(ag-n3));			
			if(newId == 0)continue;
			newAng[100+newId]++;
		
		}			
	}
	//int max1,max2,max3;
	int maxValue=0;
	newId=0;
	for(i=0;i<200;i++){
		if(newAng[i]>maxValue){
			maxValue = newAng[i];
			newId = i;
		}
	}
	fAng = n3-(100-newId)*0.01;

	// ���� +-5�� �~� ���� �� ��� ��.
	// L����� �l����
	if(fabs(fAng) > 25) fAng=0;

	delete[] pCouple; pCouple=NULL;
	return fAng;
}

template <class TRt>
void TRunProc<TRt>::GetLUAngle(CRect cRt1,CRect cRt2,double& ang1,double& ang2)
{
	CPoint tPtL,tPtR;
	CPoint bPtL,bPtR;
	int	tw,th,bw,bh;

	double a1,a2;
	
	
	tPtL = CPoint((cRt1.left + cRt1.right)/2,cRt1.top);
	tPtR = CPoint((cRt2.left + cRt2.right)/2,cRt2.top);

	bPtL = CPoint((cRt1.left + cRt1.right)/2,cRt1.bottom);
	bPtR = CPoint((cRt2.left + cRt2.right)/2,cRt2.bottom);

	th = tPtR.y - tPtL.y; tw = tPtR.x - tPtL.x;
	bh = bPtR.y - bPtL.y; bw = bPtR.x - bPtL.x;

	a1 = atan2((double)th,tw);
	a2 = atan2((double)bh,(double)bw);

	a1 *= 57.2957;
	a2 *= 57.2957;

	if(tw == 0)a1 = 90.0;
	if(bw == 0)a2 = 90.0;

	ang1 = a1;ang2 = a2;
	if(ang1>90)ang1 = -1*(180-ang1);
	else if(ang1<-90)ang1 = 180+ang1;
	
	if(ang2>90)ang2 = -1*(180-ang2);
	else if(ang2<-90)ang2 = 180+ang2;
}
template <class TRt>
int TRunProc<TRt>::GetBlkId(CRect cRt,CRect cSubAllRt[50],int nNum)
{
	int i,nId = -1;
	CRect Rt;
	for(i=0;i<nNum;i++){
		Rt = cSubAllRt[i];
		if(cRt.left>=Rt.left && cRt.right<=Rt.right && 
			cRt.top>=Rt.top && cRt.bottom<=Rt.bottom){
			nId = i;break;
		}
	}
	return nId;
}
template <class TRt>
BYTE* TRunProc<TRt>::AutoRotateCorrectBinBits(BYTE* inBits,int& w,int& h,double& fAng, BOOL bKeepSize/*=TRUE*/)
{
	int wb;
	BYTE* outBits = NULL;
	TRtAry RunRtAry;
	CSize Sz = CSize(w,h);

	int Rnum = CreateRunTableFromBits(inBits,w,h);
	CreateMemory_UpDownLabel(m_pRunNo, h);
	MakeDownTable();
	MakeUpTable();
	RunFollowing(RunRtAry);
	DeleteMemory_UpDownLabel();
	fAng = GetAngleFromRunRtAry(RunRtAry,w,h);
	RemoveAllRunRt(RunRtAry);

	if(fabs(fAng)<0.2){
		wb = ((((w) + 31) / 32) * 4); 
		outBits = new BYTE[wb*h];
		memcpy(outBits,inBits,wb*h);
		return outBits;
	}
	
	CSize OrgSz = Sz;
	Rotate_RunXRunNo(m_pRunX,m_pRunNo,-fAng,Sz);
	CRect Rt;
	if(bKeepSize==TRUE){
		Rt.left = (Sz.cx-OrgSz.cx)/2;
		Rt.right = Rt.left + OrgSz.cx;
		Rt.top = (Sz.cy-OrgSz.cy)/2;
		Rt.bottom = Rt.top+OrgSz.cy;
		w = OrgSz.cx; h = OrgSz.cy;
	}
	else{
		Rt.left = 0;
		Rt.right = Sz.cx;
		Rt.top = 0;
		Rt.bottom = Sz.cy;
		w = Sz.cx; h = Sz.cy;
	}
	wb = ((((w) + 31) / 32) * 4); 
	outBits = new BYTE[wb*h];
	GetBitsFromRunXRunNo(outBits,Rt);
	return outBits;
}
template <class TRt>
BYTE* TRunProc<TRt>::AutoRotateCorrectBinImg(BYTE* inImg,int& w,int& h,double& fAng, BOOL bKeepSize/*=TRUE*/)
{
	BYTE* outImg = NULL;
	TRtAry RunRtAry;
	CSize Sz = CSize(w,h);

	int Rnum = CreateRunTableFromImg(inImg,w,h);
	CreateMemory_UpDownLabel(m_pRunNo, h);
	MakeDownTable();
	MakeUpTable();
	RunFollowing(RunRtAry);
	DeleteMemory_UpDownLabel();
	fAng = GetAngleFromRunRtAry(RunRtAry,w,h);
	RemoveAllRunRt(RunRtAry);

	if(fabs(fAng)<0.2){
		outImg = new BYTE[w*h];
		memcpy(outImg,inImg,w*h);
		return outImg;
	}
	
	CSize OrgSz = Sz;
	Rotate_RunXRunNo(m_pRunX,m_pRunNo,-fAng,Sz);
	CRect Rt;
	if(bKeepSize==TRUE){
		Rt.left = (Sz.cx-OrgSz.cx)/2;
		Rt.right = Rt.left + OrgSz.cx;
		Rt.top = (Sz.cy-OrgSz.cy)/2;
		Rt.bottom = Rt.top+OrgSz.cy;
		w = OrgSz.cx; h = OrgSz.cy;
	}
	else{
		Rt.left = 0;
		Rt.right = Sz.cx;
		Rt.top = 0;
		Rt.bottom = Sz.cy;
		w = Sz.cx; h = Sz.cy;
	}
	outImg = new BYTE[w*h];
	GetImgFromRunXRunNo(outImg,Rt);
	return outImg;
}
template <class TRt>
BYTE* TRunProc<TRt>::Rotate_BinBits(BYTE* inBits,int& w,int& h,double fAng, BOOL bKeepSize/*=TRUE*/)
{
	int wb;
	BYTE* outBits = NULL;
	if(fabs(fAng)<0.2){
		wb = ((((w) + 31) / 32) * 4); 
		outBits = new BYTE[wb*h];
		memcpy(outBits,inBits,wb*h);
		return outBits;
	}

	CSize Sz = CSize(w,h);
	CreateRunTableFromBits(inBits,w,h);
	CSize OrgSz = Sz;
	Rotate_RunXRunNo(m_pRunX,m_pRunNo,fAng,Sz);
	CRect Rt;
	if(bKeepSize==TRUE){
		Rt.left = (Sz.cx-OrgSz.cx)/2;
		Rt.right = Rt.left + OrgSz.cx;
		Rt.top = (Sz.cy-OrgSz.cy)/2;
		Rt.bottom = Rt.top+OrgSz.cy;
		w = OrgSz.cx; h = OrgSz.cy;
	}
	else{
		Rt.left = 0;
		Rt.right = Sz.cx;
		Rt.top = 0;
		Rt.bottom = Sz.cy;
		w = Sz.cx; h = Sz.cy;
	}
	wb = ((((w) + 31) / 32) * 4); 
	outBits = new BYTE[wb*h];
	GetBitsFromRunXRunNo(outBits,Rt);
	return outBits;
}
template <class TRt>
BYTE* TRunProc<TRt>::Rotate_BinImg(BYTE* inImg,int& w,int& h,double fAng, BOOL bKeepSize/*=TRUE*/)
{
	BYTE* outImg = NULL;
	if(fabs(fAng)<0.2){
		outImg = new BYTE[w*h];
		memcpy(outImg,inImg,w*h);
		return outImg;
	}

	TRtAry RunRtAry;
	CSize Sz = CSize(w,h);
	CreateRunTableFromImg(inImg,w,h);
	CSize OrgSz = Sz;
	Rotate_RunXRunNo(m_pRunX,m_pRunNo,fAng,Sz);
	CRect Rt;
	if(bKeepSize==TRUE){
		Rt.left = (Sz.cx-OrgSz.cx)/2;
		Rt.right = Rt.left + OrgSz.cx;
		Rt.top = (Sz.cy-OrgSz.cy)/2;
		Rt.bottom = Rt.top+OrgSz.cy;
		w = OrgSz.cx; h = OrgSz.cy;
	}
	else{
		Rt.left = 0;
		Rt.right = Sz.cx;
		Rt.top = 0;
		Rt.bottom = Sz.cy;
		w = Sz.cx; h = Sz.cy;
	}
	outImg = new BYTE[w*h];
	GetImgFromRunXRunNo(outImg,Rt);
	return outImg;
}
template <class TRt>
BYTE* TRunProc<TRt>::GetRotatedDibFromRunXRunNo(short* runX,int* runNo,double fAng,CSize &Sz,BOOL bKeepSize/*=TRUE*/)
{
	int w,h;
	CSize OrgSz = Sz;
	Rotate_RunXRunNo(runX,runNo,fAng,Sz);
	CRect Rt;
	if(bKeepSize==TRUE){
		Rt.left = (Sz.cx-OrgSz.cx)/2;
		Rt.right = Rt.left + OrgSz.cx;
		Rt.top = (Sz.cy-OrgSz.cy)/2;
		Rt.bottom = Rt.top+OrgSz.cy;
		w = OrgSz.cx; h = OrgSz.cy;
	}
	else{
		Rt.left = 0;
		Rt.right = Sz.cx;
		Rt.top = 0;
		Rt.bottom = Sz.cy;
		w = Sz.cx; h = Sz.cy;
	}
	Sz.cx = w; Sz.cy = h;
	BYTE* outDib = GetBinDibFromRunXRunNo(runX,runNo,Rt);
	return outDib;
}
/////////////////////////////////////////
// RnOCR� �l��
// �o� ����_�����A��
template <class TRt>
void TRunProc<TRt>::GetPreProcessInfo(short* runX,int* runNo,int w,int h,double& fAngle,double& fZoomIn,BOOL& bAngle,BOOL& bZoomIn)
{
	int AveLineH,lineSpace;
	TRtAry RunRtAry;
	CreateMemory_UpDownLabel(runNo,h);
	MakeDownTable(runX, runNo, h, m_pDown);
	MakeUpTable(runX, runNo, h, m_pUp);
	RunFollowing(runX, runNo, h, m_pDown, m_pUp, m_pLabel,RunRtAry,FALSE);
	DeleteMemory_UpDownLabel();
	DeleteNoizeRects(RunRtAry,CSize(5,5));


	AveLineH = GetAverageLineHeight(RunRtAry,w,h,lineSpace);
	fZoomIn = 50.0/(double)AveLineH;
	if(fZoomIn>2.5) fZoomIn = 2.5;
	if(fZoomIn<1.3) bZoomIn = FALSE;
	else			bZoomIn = TRUE;
	if(w*fZoomIn>5000 || h*fZoomIn>5000)
		bZoomIn=FALSE;
	//MergeRts(RunRtAry,w,h,AveLineH,lineSpace);
	fAngle = GetAngleFromRunRtAry(RunRtAry,w,h);
	if(fabs(fAngle)<0.1)	bAngle = FALSE;
	else					bAngle = TRUE;	
	RemoveAllRunRt(RunRtAry);
}
//Delete Too Small Rects, Rects of Out side of SubRect,Rects attached Sides
//These are regarded as Noize 
template <class TRt>
int TRunProc<TRt>::DeleteNoizeRects(TRtAry& RectAry,CSize Sz)
{
	BOOL b;
	int wd,hi;
	
	TRt* pU;
	int i,num = RectAry.GetSize();
	
	for(i=num-1; i>=0; i--){
		b = TRUE;
		pU = RectAry.GetAt(i);
		wd = pU->m_Rect.Width(); hi = pU->m_Rect.Height();
		if(wd <Sz.cx && hi <Sz.cy ){
			b = FALSE;//Small rects 
		}
		else if(pU->nPixelNum < 8){
			b = FALSE;//Too Small Rects
		}
// 		else if((hi < 5 && wd/hi > 20)){
// 			b = FALSE;//Long Horizontal Line Rects
// 		}
		if(b == FALSE || pU->bUse == FALSE){
			delete (TRt*)RectAry.GetAt(i);
			RectAry.RemoveAt(i);
		}
	}
	num = RectAry.GetSize();
	return num;
}
template <class TRt>
int TRunProc<TRt>::GetAverageLineHeight(TRtAry& rts,int w,int h,int &Lh)
{
	int Th = 0,kk;
	Lh = 0;
	int i,j,k,p,bl,num,ww[9],limit;
	int th[500],lh[500],*SubPP; 
	int nDivNum=4;
	memset(th, 0, sizeof(int) * 500);
	memset(lh, 0, sizeof(int) * 500);
	
	
	limit = 10;

	if(w>2500) nDivNum=8;
	else if(w>1400) nDivNum=4;
	else if(w>500) nDivNum=2;
	else
		nDivNum=1;
	k = w / nDivNum;
	
	for(i=0;i<(nDivNum+1);i++) ww[i] = k * i;
	CRect rt;
	num = rts.GetSize();
	if(num<1) return 50;

	SubPP = new int[(h + 1)*8]; 
	memset(SubPP, 0, sizeof(int) * 8 * (h + 1));
	for(i=0; i<nDivNum; i++) 
		for(p=i+1, j=0; j<num; j++) 
		{
			if(rts[j]->m_Rect.Height() <= limit) continue;
			rt=rts[j]->m_Rect;
			if(rts[j]->m_Rect.left >= ww[i] && rts[j]->m_Rect.right <= ww[p]) 
			{ 
				for(k=rts[j]->m_Rect.top; k<rts[j]->m_Rect.bottom; k++) SubPP[k*8+i] = 1; 
			}
		}
		
		for(i=0; i<nDivNum; i++)
		{
			for(j=0; j<h; j++)
				if(SubPP[j*8+i] == 1)
					break;
			bl = 1;
			j = max(1, j);
			for(k=j; j<h; j++) 
			{
				if(!bl) if((!SubPP[j*8+i] && SubPP[(j+1)*8+i])) 
				{
					p = j - k; 
					if(p < 500)	lh[p]++; k = j; bl = 1; continue; 
				}
				if(bl) if(j==h-1 || (SubPP[(j-1)*8+i] && !SubPP[j*8+i])) 
				{
					p = j - k; 
					if(p < 500)	th[p]++; k = j - 1; bl = 0; 
				}
			}
		}	

					
			for(kk=0, i=0; i<500; i++) if(th[i] >= kk) { kk = th[i]; Th = i;} 
			for(kk=0, i=0; i<500; i++) if(lh[i] >= kk) { kk = lh[i]; Lh = i;} 
			delete []SubPP;
			
			if(Th==0 || Th==499)
			{
				Th=0;
				for(i=0;i<num;i++)
					Th=max(Th,rts[i]->m_Rect.Height());
			}
			if(Lh==0)
				Lh  = 10;
			return Th;
}
template <class TRt>
void TRunProc<TRt>::ImageZoomOut2(BYTE* lpBits,int& w,int& h,short* runX,int* runNo,int rate)
{
	CreateRunTableFromBits(lpBits,w,h);
	ImageZoomOut2(m_pRunX,m_pRunNo,runX,runNo,w,h,rate);
	if (m_pRunNo)
	{
		delete [] m_pRunNo; m_pRunNo = NULL;
		delete [] m_pRunX; m_pRunX = NULL;
	}
}
template <class TRt>
void TRunProc<TRt>::ImageZoomOut2(short* runX,int* runNo,short* newRunX,int* newRunNo,int& w,int& h,int rate)
{
	int i,j,k,p,n,x,wd,hi,ST,num,*hp;
	BOOL tf;
	int T16[2][2][2][2]={0,0,0,1,0,1,1,1,0,1,1,1,1,1,1,1}; 
	
	wd=w/rate;
	hi=h/rate;
	x=wd-1;
	
	switch(rate) 
	{
	case 2: ST=0; break;
	case 4: ST=1; break;
	case 8: ST=3; break;
	default: return;
	}
	
	BYTE (*pc)[2] = new BYTE[w][2];
	
	p=max(wd,hi);
	hp=new int[p];
	for(i=0;i<p;i++) hp[i] = ST+rate*i;
	
	num=1; newRunNo[0]=1; tf=TRUE;
	for(k=0;k<hi;k++) {
		for(j=0;j<2;j++) for(i=0;i<w;i++) pc[i][j]=0;
		for(j=0;j<2;j++) {
			i=hp[k]+j;
			for(p=runNo[i];p<runNo[i+1];p+=2)
				for(n=runX[p];n<runX[p+1];n++)
					pc[n][j]=1;
		}
		
		for(i=0;i<wd;i++) {
			j=hp[i];  p=j+1;
			p=T16[pc[j][0]][pc[j][1]][pc[p][0]][pc[p][1]];
			if(p==1 && tf==TRUE)  { n=i; tf=FALSE; }
			if(p==0 && tf==FALSE) { 
				newRunX[num]=n; num++;
				newRunX[num]=i; num++;
				tf=TRUE; 
			}
		}
		if(tf==FALSE) {
			if(n!=x) { newRunX[num]=n; num++; newRunX[num]=x; num++; }
			tf=TRUE; 
		}
		newRunNo[k+1]=num;
	}

	w = wd; h = hi;
	delete []hp; hp=NULL;
	delete []pc; pc=NULL;
}


class  CRunRt{
public:
	CRect		m_Rect;		//
	int		nRunStart;
	int		nRunEnd;
	int		nLabelNo;
	int		nPixelNum;//(Black)
	int		nMaxLength;		//Max length of run. by KBG  
	
	DWORD*	pRunData;
	short*	pRunLabel;
	int		nRunNum;
	
	BOOL	bUse;
	int		nLineNo;
	
	void*   pUserData;
	int		nAddNum;
	CRunRt() {
		nRunStart=nRunEnd=nLabelNo=nPixelNum=nRunNum=nLineNo=nMaxLength=0;
		m_Rect = CRect(0,0,0,0);
		pRunData = NULL;
		pRunLabel = NULL;
		pUserData = NULL;
		bUse = TRUE;
		nAddNum = 1;
	};
	CRunRt(int nUserDataSize) {
		nRunStart=nRunEnd=nLabelNo=nPixelNum=nRunNum=nLineNo=nMaxLength=0;
		m_Rect = CRect(0,0,0,0);
		pRunData = NULL;
		pRunLabel = NULL;
		pUserData = malloc(nUserDataSize);
		memset(pUserData,0,nUserDataSize);
		bUse = TRUE;
		nAddNum = 1;
	};
	virtual ~CRunRt() {
		if(pRunData !=NULL) delete[] pRunData; pRunData=NULL;
		if(pRunLabel!=NULL) delete[] pRunLabel;pRunLabel=NULL;
		if(pUserData!=NULL) free(pUserData); pUserData=NULL;
	};
	void InitRunData(){
		if(pRunData !=NULL) delete[] pRunData; pRunData=NULL;
		if(pRunLabel!=NULL) delete[] pRunLabel;pRunLabel=NULL;
	};
	void InitUserData(int nUserDataSize){
		if(pUserData!=NULL) free(pUserData);
		pUserData = malloc(nUserDataSize);
		memset(pUserData,0,nUserDataSize);
	};
	virtual void Append(CRunRt *pU) 
	{
		nAddNum += pU->nAddNum;
		int n1,n2;
		m_Rect.UnionRect(m_Rect, pU->m_Rect); 
		nPixelNum += pU->nPixelNum;
		nMaxLength = max(nMaxLength, pU->nMaxLength);
		DWORD* new_runData,*p1;
		short* new_runLabel,*p2;
		n1 = nRunNum;
		n2 = pU->nRunNum;
		nRunNum = n1+n2;
		
		new_runData = new DWORD[nRunNum];
		new_runLabel = new short[nRunNum];
		
		memcpy(new_runData,pRunData,sizeof(DWORD)*n1);
		memcpy(new_runLabel,pRunLabel,sizeof(short)*n1);
		p1 = new_runData+n1; p2 = new_runLabel+n1;
		memcpy(p1,pU->pRunData,sizeof(DWORD)*n2);
		memcpy(p2,pU->pRunLabel,sizeof(short)*n2);
		delete []pRunData;pRunData=NULL;
		pRunData = new_runData;
		delete []pRunLabel;pRunLabel=NULL;
		pRunLabel = new_runLabel;
	};
	virtual void AppendWithoutRunData(CRunRt *pU) 
	{
		nAddNum += pU->nAddNum;
		m_Rect.UnionRect(m_Rect, pU->m_Rect); 
		nPixelNum += pU->nPixelNum;
		nMaxLength = max(pU->nMaxLength,nMaxLength);
	};
	virtual void Copy(CRunRt* pU)
	{
		nAddNum = pU->nAddNum;
		m_Rect= pU->m_Rect; 
		nPixelNum = pU->nPixelNum;
		nMaxLength = pU->nMaxLength;
		DWORD* new_runData;
		short* new_runLabel;
		
		nRunNum =  pU->nRunNum;;
		new_runData = new DWORD[nRunNum];
		new_runLabel = new short[nRunNum];
		
		memcpy(new_runData, pU->pRunData,sizeof(DWORD)*nRunNum);
		memcpy(new_runLabel, pU->pRunLabel,sizeof(short)*nRunNum);
		if(pRunData)delete []pRunData;pRunData=NULL;
		pRunData = new_runData;
		if(pRunLabel)delete []pRunLabel;pRunLabel=NULL;
		pRunLabel = new_runLabel;
	};
};

typedef TRunProc<CRunRt> CRunProc;

typedef CTypedPtrArray <CPtrArray, CRunRt*> CRunRtAry;

#endif // !defined(_PXYRUN_H__)
