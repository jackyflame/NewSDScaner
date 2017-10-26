// LineRecogPrint.cpp: implementation of the CLineRecogPrint class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"//

#include "LineRecogPrint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
CLineRecogPrint::CLineRecogPrint() {
	m_CharSize = 0;

}

CLineRecogPrint::~CLineRecogPrint() {

}

void CLineRecogPrint::ForcedSegment(CRunRtAry& RunRtAry, int minH) {

	int i, n, wd, hi, count;
//	BYTE *pImg;
	BOOL bCheck;
	CSize Sz;
	int nCharNum;
	CRunRt*pU;

	count = 0;
	m_CharSize = minH;
	nCharNum = RunRtAry.GetSize();
	for (i = 0; i < nCharNum; i++) {
		pU = RunRtAry.GetAt(i);
		wd = pU->m_Rect.Width();
		hi = pU->m_Rect.Height();
		if (wd * 5 < hi * 4 || hi < minH) {
			pU->bUse = FALSE;
		}
	}
	RemoveNoneUseRects(RunRtAry);
	nCharNum = RunRtAry.GetSize();
	for (i = 0; i < nCharNum; i++) {
		bCheck = FALSE;
		pU = RunRtAry.GetAt(i);
		wd = pU->m_Rect.Width();
		hi = pU->m_Rect.Height();
		if (wd * 5 > hi * 4 /*&& hi*10>m_CharSize*8*/) {
			bCheck = TRUE;

		}

		if (bCheck) {
			n = 0;
			n = ForcedEngCut(RunRtAry, i);
			if (n != 0) {
				Del_Rect(RunRtAry, i, 0);
				//i+=n;
				nCharNum += n;
			}

			if (n == 0) {
				n = ForcedCut(RunRtAry, i);
				if (n != 0) {
					Del_Rect(RunRtAry, i, 0);
					//i+=n;
					nCharNum += n;
				}
			}
		}
	}
}
int CLineRecogPrint::ForcedEngCut(CRunRtAry& RunRtAry, int nId) {
	CRunProc cRunProc;
	CRunRt* pU = RunRtAry[nId], *pU1;
	int w, h, p;
	BYTE* Img = cRunProc.GetImgFromRunRt(RunRtAry[nId], w, h);
	float nThick = GetStrokeWidth(RunRtAry[nId]);
	int* hist = new int[w + 1];
	int* hist1 = new int[w + 1];
	memset(hist, 0, (w + 1) * sizeof(int));
	memset(hist1, 0, (w + 1) * sizeof(int));
	int i, j, k;
	for (i = 0; i < w; i++) {
		for (j = 0; j < h; j++)
			hist[i] += Img[j * w + i];
	}
	for (i = 2; i < w - 3; i++) {
		hist1[i] = hist[i - 2] + hist[i - 1] + hist[i] + hist[i + 1]
				+ hist[i + 2];

	}
	int nCutNum = 0, nPos[300];
	nPos[0] = RunRtAry[nId]->m_Rect.left;
	bool bdowning = false;
	for (i = 5; i < w - 5; i++) {
		if (hist1[i - 1] > hist1[i])
			bdowning = true;
		if (bdowning && hist1[i] > hist1[i - 1]) {
			if (hist[i] < nThick * 2)
				nPos[++nCutNum] = i - 1 + RunRtAry[nId]->m_Rect.left;
			bdowning = false;
		}
	}
	nPos[nCutNum + 1] = RunRtAry[nId]->m_Rect.right;
	delete[] hist;
	delete[] hist1;
	delete[] Img;
	if (!nCutNum)
		return 0;
//	int u;
	/*for(i=1;i<=nCutNum;i++)
	 {

	 for(u=0;u<3;u++)
	 {
	 k=0;
	 for(j=0;j<h-h/5;j++)
	 {
	 if(Img[j*w+nPos[i]+u-RunRtAry[nId]->m_Rect.left]==1)
	 {
	 k=1;
	 break;
	 }
	 }
	 if(k==0) break;
	 if(u==0) continue;
	 for(j=0;j<h-h/5;j++)
	 {
	 if(Img[j*w+nPos[i]-u-RunRtAry[nId]->m_Rect.left]==1)
	 {
	 k=1;
	 break;
	 }
	 }
	 if(k==0) {u=-1*u;break;}
	 }
	 if(u==3)
	 {
	 for(j=i;j<=nCutNum;j++)
	 {
	 nPos[j]=nPos[j+1];
	 }
	 nCutNum--;i--;
	 }
	 else
	 nPos[i]+=u;
	 }
	 if(!nCutNum) return 0;
	 */
	int L, R, T, B;
	int nRunCount = 0;
	int nInsertIdx = nId + 1;
	for (i = 0; i <= nCutNum; i++) {
		T = 10000;
		B = 0;
		k = i + 1;
		nRunCount = 0;
		for (j = 0; j < pU->nRunNum; j += 2) {
			L = max(nPos[i], (int)(LOWORD(pU->pRunData[j])));
			R = min(nPos[k], (int)(LOWORD(pU->pRunData[j + 1])));
			if (R - L <= 0)
				continue;
			p = (int) (HIWORD(pU->pRunData[j]));
			T = min(T, p);
			B = max(B, p);
			nRunCount++;
		}
		pU1 = new CRunRt;
		pU1->pRunData = new DWORD[nRunCount * 2];
		pU1->pRunLabel = new short[nRunCount * 2];
		nRunCount = 0;
		for (j = 0; j < pU->nRunNum; j += 2) {
			L = max(nPos[i], (int)(LOWORD(pU->pRunData[j])));
			R = min(nPos[k], (int)(LOWORD(pU->pRunData[j + 1])));
			if (R - L <= 0)
				continue;
			p = (int) (HIWORD(pU->pRunData[j]));
			pU1->pRunData[nRunCount] = MAKELONG(L,p);
			pU1->pRunLabel[nRunCount++] = pU->pRunLabel[j];
			pU1->pRunData[nRunCount] = MAKELONG(R,p);
			pU1->pRunLabel[nRunCount++] = pU->pRunLabel[j + 1];
		}
		pU1->nRunNum = nRunCount;
		pU1->m_Rect = CRect(nPos[i], T, nPos[k], B + 1);
		//pU1->nReserved1 = 1;
		RunRtAry.InsertAt(nInsertIdx++, pU1);
	}
	return nCutNum;
}
int CLineRecogPrint::ForcedCut(CRunRtAry& RunRtAry, int nId) {
	int i, j, k, w, h, p, nPos[100], nCutNum;
	CRunProc cRtProc;
	CRunRt *pU, *pU1;
	pU = RunRtAry.GetAt(nId);
	w = pU->m_Rect.Width() + 2;
	h = pU->m_Rect.Height() + 2;
	if (w < h * 0.5f)
		return 0;
	BYTE* Img = new BYTE[w * h];
	BYTE* ImgThin = new BYTE[w * h];
	BYTE** TempImg = (BYTE **) new BYTE[w * sizeof(BYTE*)];
	BYTE** TempThin = (BYTE **) new BYTE[w * sizeof(BYTE*)];
	for (i = 0; i < w; i++)
		TempImg[i] = &Img[i * h];
	for (i = 0; i < w; i++)
		TempThin[i] = &ImgThin[i * h];
	memset(Img, 0, w * h);
	memset(ImgThin, 0, w * h);
	//Á¡´¸¶®ÂÙÃùºÏ¹¤ Ãú»þÂ×°Ö Á¡´¸¶®ÂÙÃùºÏ¹¤Ë¾ 0Ëºµá º¼¼³ÂÙ Ãùº¬ÊÐ±¨
	GetImgYXFromRunRt_Ext(pU, TempImg, w, h);
	memcpy(ImgThin, Img, w * h);

// 	BYTE* tt = new BYTE[w*h];
// 	for(i=0;i<w;i++)for(k=0;k<h;++k) tt[k*w+i] = TempThin[i][k];
// 	char FName[MAX_PATH];sprintf(FName,"C:\\Temp_%d.bmp",nId);
// 	CImageIO::SaveImgToFile(FName,tt,w,h,1);

//divide with connect component

	CRunProc runProc;
	int ww, hh;
	BYTE* pImg = runProc.GetImgFromRunRt(pU, ww, hh);
	//CImageBase::SaveImgFile(_T("d:\\temp\\char.bmp"),pImg,CSize(ww,hh),1);
	CRunRtAry runAry;
	runProc.MakeConnectComponentFromImg(pImg, ww, hh, runAry);
	FirstMerge_Horizontal(runAry);
	int ncc = runAry.GetSize();
	delete[] pImg;
	if (ncc == 2) {
		runProc.SortByOption(runAry, 0, 2, SORT_CENTER_X);
		nPos[0] = 0;
		nPos[1] = (runAry[1]->m_Rect.left + runAry[0]->m_Rect.right) / 2;
		nPos[2] = w - 1;
		nCutNum = 1;
		if (nPos[1] < 1)
			nCutNum = 0;
		runProc.RemoveAllRunRt(runAry);
	} else {
		runProc.RemoveAllRunRt(runAry);
		GetStrokeWidth(pU, TempImg, w, h);
		{
			nCutNum = Is_Valley(TempImg, w, h, nPos, 1);
			if (nCutNum == 0 /*|| m_nLanguage==ENGLISH*/) {
				int nPos1[10];
				int nn;
				nn = Is_Valley(TempImg, w, h, nPos1, 0);
				if (nn) {
					for (i = 1; i <= nn; i++)
						nPos[nCutNum + i] = nPos1[i];
					nCutNum += nn;
					nPos[nCutNum + 1] = w;
					for (i = 0; i < nCutNum; i++)
						for (j = i + 1; j <= nCutNum; j++) {
							if (nPos[i] > nPos[j]) {
								nn = nPos[i];
								nPos[i] = nPos[j];
								nPos[j] = nn;
							}
						}
					for (i = 0; i < nCutNum; i++)
						if (nPos[i + 1] - nPos[i] < 5) {
							memcpy(&nPos[i + 1], &nPos[i + 2],
									sizeof(int) * (nCutNum - i));
							nCutNum--;
							i--;
						}
				}
			}
		}
	}
	int splitLoc[100];
	memset(splitLoc, 0, sizeof(int) * 100);
	for (i = 1; i <= nCutNum; i++) {
		splitLoc[i] = 1;
		for (k = max(0,nPos[i] - 2); k < min(nPos[i] + 3,w - 1); k++) {
			for (j = 0; j < h; j++) {
				if (TempImg[k][j] == 1)
					break;

			}
			if (j == h) {
				splitLoc[i] = 0;
				break;
			}
		}
	}
	delete Img;
	delete ImgThin;
	delete TempImg;
	delete TempThin;

//	int num = StrokeAry.GetSize();
//	for(i=0;i<num;i++) delete [] (CTwoPoint*)StrokeAry.GetAt(i);
//	StrokeAry.RemoveAll();

	if (!nCutNum)
		return nCutNum;

	int L, R, T, B;
	for (p = nCutNum + 1, i = 0; i <= p; i++) {
		if (i == p)
			nPos[i] = pU->m_Rect.right;
		else
			nPos[i] += pU->m_Rect.left;
	}
	int nRunCount = 0;
	int LL, RR;
	int nErrCount = 0;
	int nInsertIdx = nId + 1;
	for (i = 0; i <= nCutNum; i++) {
		T = 10000;
		B = 0;
		k = i + 1;
		LL = 10000;
		RR = 0;
		nRunCount = 0;
		for (j = 0; j < pU->nRunNum; j += 2) {
			L = max(nPos[i], (int)(LOWORD(pU->pRunData[j])));
			R = min(nPos[k], (int)(LOWORD(pU->pRunData[j + 1])));
			if (R - L <= 0)
				continue;
			p = (int) (HIWORD(pU->pRunData[j]));
			T = min(T, p);
			B = max(B, p);
			LL = min(LL, L);
			RR = max(RR, R);
			nRunCount++;
		}
		if (nRunCount == 0 || LL >= RR || T >= B) {
			//nCutNum --;
			nErrCount++;
			continue;
		}
		pU1 = new CRunRt;
		pU1->pRunData = new DWORD[nRunCount * 2];
		pU1->pRunLabel = new short[nRunCount * 2];
		nRunCount = 0;
		for (j = 0; j < pU->nRunNum; j += 2) {
			L = max(nPos[i], (int)(LOWORD(pU->pRunData[j])));
			R = min(nPos[k], (int)(LOWORD(pU->pRunData[j + 1])));
			if (R - L <= 0)
				continue;
			p = (int) (HIWORD(pU->pRunData[j]));
			pU1->pRunData[nRunCount] = MAKELONG(L,p);
			pU1->pRunLabel[nRunCount++] = pU->pRunLabel[j];
			pU1->pRunData[nRunCount] = MAKELONG(R,p);
			pU1->pRunLabel[nRunCount++] = pU->pRunLabel[j + 1];
		}
		pU1->nRunNum = nRunCount;
		pU1->m_Rect = CRect(LL, T, RR, B + 1);
		//pU1->nReserved1 = splitLoc[i]*2+splitLoc[k];
		RunRtAry.InsertAt(nInsertIdx++, pU1);
	}
	return nCutNum - nErrCount;
}

int CLineRecogPrint::Is_Valley1(BYTE* Img, int nWd, int nHi, int nPos[]) {
	int i, j, k, h, nCutNum = 0;
	float BoonPo1[300];
	if (nWd > 200 || nWd < 20)
		return 0;
	memset(BoonPo1, 0, sizeof(float) * nWd);
	for (i = 0; i < nWd; i++) {
		for (k = -1, j = 0; j < nHi; j++)
			if (Img[j * nWd + i] != 0) {
				k = j;
				break;
			}
		for (h = -1, j = nHi - 1; j >= 0; j--)
			if (Img[j * nWd + i] != 0) {
				h = j;
				break;
			}
		if (k != -1 && h != -1 && h > k)
			BoonPo1[i] = (float) (h - k);
	}
	int St = nWd / 3;
	int Ed = nWd * 2 / 3 + 3;
	float nMin = (float) nHi;
	int nMinId = St;
	for (i = St; i < Ed; i++) {
		if (BoonPo1[i] < nMin) {
			nMin = BoonPo1[i];
			nMinId = i;
		}
	}
	if (nMin < nHi / 2) {
		nPos[0] = 0;
		nPos[1] = nMinId;
		nPos[2] = nWd;
		nCutNum = 1;
	}

	return nCutNum;
}
int CLineRecogPrint::Is_Valley(BYTE** Img, int nWd, int nHi, int nPos[],
		int mode) {
	int i, j, k, h, nPitch, nCutNum;
	float *BoonPo1 = new float[nWd];
	float *BoonPo2 = new float[nWd];
	float *BoonPo3 = new float[nWd];
	//float BoonPo3[200];

	memset(BoonPo2, 0, sizeof(float) * nWd);
	memset(BoonPo3, 0, sizeof(float) * nWd);

	for (i = 0; i < nWd; i++) {
		for (k = 0, j = 0; j < nHi; j++)
			k += Img[i][j];
		BoonPo3[i] = (float) k;
	}

	if (!mode) {
		memcpy(BoonPo2, BoonPo3, sizeof(float) * nWd);
		// 		for(i=0; i<nWd; i++) 
		// 		{
		// 			for(k=0, j=0; j<nHi; j++)   k += Img[i][j];
		// 			BoonPo2[i] = (float)k;
		// 		}
	} else {
		for (i = 0; i < nWd; i++) {
			for (k = -1, j = 0; j < nHi; j++)
				if (Img[i][j] != 0) {
					k = j;
					break;
				}
			for (h = -1, j = nHi - 1; j >= 0; j--)
				if (Img[i][j] != 0) {
					h = j;
					break;
				}
			if (k != -1 && h != -1 && h > k)
				BoonPo2[i] = (float) (h - k);
		}
	}

	nPitch = max(2,nHi / 4); //4;
	for (i = 0; i < 2; i++)
		Smooth(BoonPo2, nPitch, nWd);
	for (i = 0; i < nWd; i++)
		BoonPo1[i] = BoonPo2[i];
	Smooth(BoonPo2, max(2, nHi * 2 / 3), nWd);
	nCutNum = GetCutPosition(nWd, nHi, BoonPo1, BoonPo2, nPos, mode);

	//ReFineDeterminePos , Inserted By KSD 2013_02_10
	int St, Ed, newPos;
	for (i = 1; i <= nCutNum; ++i) {
		St = max((nPos[i]+nPos[i-1])/2, nPos[i]-3);
		Ed = min((nPos[i+1]+nPos[i])/2, nPos[i]+3);
		newPos = ReFineDeterminePos(BoonPo3, St, Ed);
		if (newPos >= 0)
			nPos[i] = newPos + 1;
	}
	delete[] BoonPo3;
	delete[] BoonPo2;
	delete[] BoonPo1;

	return nCutNum;
}
int CLineRecogPrint::ReFineDeterminePos(float* BoonPo1, int St, int Ed) {
	int i, minId, num = Ed - St;
	if (num <= 1)
		return -1;
	float* BoonPo = new float[num];
	for (i = 0; i < num; ++i)
		BoonPo[i] = BoonPo1[St + i];
	GetMinValue(BoonPo, num, minId);
	minId = St + minId;
	delete BoonPo;
	return minId;
}
float CLineRecogPrint::GetMinValue(float *dif, int n, int& AryNo) {
	AryNo = 0;
	if (n < 1)
		return 0;
	if (n == 1)
		return dif[0];
	float min = dif[0];
	int i, k = 0;
	for (i = 1; i < n; ++i) {
		if (dif[i] < min) {
			min = dif[i];
			k = i;
		}
	}
	AryNo = k;
	return min;
}
void CLineRecogPrint::Smooth(float *Boon, int pit, int w_d) {
	int i, n, x, q, k;
	float HH;

	q = pit / 2;
	float* BoonPo = new float[w_d];

	for (k = 0; k < w_d; k++) {
		n = 0;
		HH = 0.0f;
		for (i = -q; i < q; i++) {
			x = k - i;
			if (x < 0)
				break;
			if (x >= w_d)
				continue;
			HH = HH + Boon[x];
			n++;
		}
		BoonPo[k] = HH / (float) n;
	}
	for (i = 0; i < w_d; i++)
		Boon[i] = BoonPo[i];
	delete[] BoonPo;
}
/*
 //ÂÜºãËË¶§:GetImgFromRunRt
 //±¨²þ: pRunRtÌ© pRunDataµá ¹¢À¾ Ë÷¼±µßÊßËæ ´ÝÂÙ ÂÙÃùºÏ Ãú»þ´ô Img¶¦ ÊÐ²÷³Þ.
 //½Ø°Ö: ËË ÂÜºã³­Ëæº· Img¶¦ (w+2,h+2)¿Í±¨µá Ãú¸óÂ×ÊÞ ´®µØ¼Ó³Þ.
 //		ËËÆÁ ±ÙÃûÂ×²÷ Ãùº¬Ë¼ ÃÔºåËË Ê­³£µ¹ µÛºåËºµá ´ô Ãùº¬ËË³Þ.
 */
void CLineRecogPrint::GetImgYXFromRunRt_Ext(CRunRt* pU, BYTE** Img, int nWd,
		int nHi) {
	int i, j, y, nm, x1, x2;

	DWORD *data = pU->pRunData;
	nm = pU->nRunNum;
	for (i = 0; i < nm; i += 2) {
		x1 = (int) LOWORD(data[i]) - pU->m_Rect.left + 1;
		x2 = (int) LOWORD(data[i+1]) - pU->m_Rect.left + 1;
		y = (int) HIWORD(data[i]) - pU->m_Rect.top + 1;
		for (j = x1; j < x2; j++)
			Img[j][y] = 1;
	}
}
int CLineRecogPrint::GetStrokeWidth(CRunRt* pU, BYTE** Img, int nWd, int nHi) {
	int i, j, k, p, w, h, x, y;

	w = nWd - 1;
	h = nHi - 1;

	int num = 0, count = 0;
	int ki[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
	int kj[] = { 0, 1, 1, 1, 0, -1, -1, -1 };

	p = 1;

	do {
		p++;

		for (j = 1; j < h; j++)
			for (i = 1; i < w; i++) {
				if (Img[i][j] == 1) {
					for (k = 0; k < 8; k++) {
						x = i + ki[k];
						y = j + kj[k];

						if (Img[x][y] == 1)
							continue;
						if (Img[x][y] < p) {
							Img[i][j] = p;
							num++;
							break;
						}
					}
				}
			}
		count++;
	} while ((float) (pU->nPixelNum - num) / (float) pU->nPixelNum > 0.05f);

	for (j = 0; j < h; j++)
		for (i = 0; i < w; i++)
			if (Img[i][j] != 0)
				Img[i][j] = 1;

	int nLimit, wd[30];

	nLimit = count;
	count *= 2;
	count *= 3;
	if (count > 30)
		count = 30;
	memset(&wd, 0, sizeof(int) * 30);

	for (i = 0; i < pU->nRunNum; i += 2) {
		p = (int) LOWORD(pU->pRunData[i+1]) - (int) LOWORD(pU->pRunData[i]);
		if (p >= count)
			continue;
		wd[p]++;
	}

	for (k = 0, i = nLimit; i < count; i++) {
		if (wd[i] > k) {
			k = wd[i];
			p = i;
		}
	}

	return p;
}
float CLineRecogPrint::GetStrokeWidth(CRunRt* pU) {

	CRunProc proc;
	int w, h;
	BYTE* pImg = proc.GetImgFromRunRt(pU, w, h);
	int n1 = 0, n2 = 0;
	int i, j;
	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++) {
			if (pImg[i * w + j])
				n1++;
		}
	for (i = 1; i < h; i++)
		for (j = 1; j < w; j++) {
			if (pImg[(i - 1) * w + (j - 1)] && pImg[(i - 1) * w + j]
					&& pImg[i * w + (j - 1)] && pImg[i * w + j])
				n2++;
		}
	float width = (float) n1 / (float) (n1 - n2);
	delete[] pImg;
	return width;
}

int CLineRecogPrint::GetCutPosition(int nWd, int nHi, float *BoonPo1,
		float *BoonPo2, int nPos[], int mode) {
	int i, j, L, R, nTemp, nCutNum;
	double fDis, fSel;
	BOOL b1, b2;

	b2 = TRUE;
	nCutNum = 0;
	nPos[nCutNum] = 0;

	fSel = mode == 1 ? 1.0 : nHi / 4.0;

	for (i = 0; i < nWd; i++) {
		if (!i) {
			if (BoonPo1[i] < BoonPo2[i])
				b1 = TRUE;
			else
				b1 = FALSE;
			continue;
		}
		if (b1 && BoonPo1[i] > BoonPo2[i])
			b1 = FALSE;
		if (!b1) {
			if (b2 && BoonPo1[i] < BoonPo2[i]) {
				L = i;
				b2 = FALSE;
			}
			if (!b2 && BoonPo1[i] > BoonPo2[i]) {
				R = i;
				b2 = TRUE;
				for (j = L + 1; j < R; j++) {
					if (BoonPo1[j] <= BoonPo1[j + 1]) {
						fDis = fabs(BoonPo1[j] - BoonPo2[j]);
						if (fDis > fSel/* && BoonPo1[j] < nHi / 2*/) {
							nCutNum++;
							nPos[nCutNum] = j;
							nTemp = R;
						}
						break;
					}
				}
				if (nCutNum >= 8)
					break;
				b1 = TRUE;
			}
		}
	}
	nPos[nCutNum + 1] = nWd;

	if (nCutNum) {
		for (i = nTemp; i < nWd; i++)
			if (fabs(BoonPo1[i] - BoonPo2[i]) > 2.0)
				break;
		if (i == nWd) {
			memcpy(&nPos[nCutNum], &nPos[nCutNum + 1], sizeof(int));
			nCutNum--;
		}
	}
	return nCutNum;
}

void CLineRecogPrint::Del_Rect(CRunRtAry& RunRtAry, int id, int mode) {
	if (!mode) {
		delete (CRunRt*) RunRtAry.GetAt(id);
		RunRtAry.RemoveAt(id);
	} else {
		int nCharNum = RunRtAry.GetSize();
		for (int i = 0; i < nCharNum; i++)
			delete (CRunRt*) RunRtAry.GetAt(i);
		RunRtAry.RemoveAll();
	}
}

void CLineRecogPrint::FirstMerge_Horizontal(CRunRtAry& RunRtAry) {
	int p, n, m, mn, mx, w1, h1, w2, h2, un, count, nSum, mn1, mx1, un1;
	float lm1, lm2, lm3, lm4, rate;
	CRect Ru;
	CRunRt* pU, *pU1;
	int nCharNum = RunRtAry.GetSize();

	lm1 = (float) m_CharSize * 0.33f;
	lm2 = (float) m_CharSize * 0.85f;
	lm3 = (float) m_CharSize * 1.2f;
	lm4 = (float) m_CharSize * 0.8f;

	for (n = 0; n < nCharNum; n++) {
		pU = RunRtAry.GetAt(n);
		//pU->nAddNum = 0;
	}
	do {
		count = 0;
		for (n = 0; n < nCharNum; n++) {
			pU = RunRtAry.GetAt(n);
			if (pU->bUse == FALSE)
				continue;
			w1 = pU->m_Rect.Width();
			h1 = pU->m_Rect.Height();
			if (w1 < m_CharSize * 0.2f || w1 > m_CharSize * 0.8f)
				continue;
			m = (lm1 > (float) w1 && lm2 < (float) h1) ? 1 : 0;
			for (p = n + 1; p < nCharNum; p++) {
				pU1 = RunRtAry.GetAt(p);
				if (n == p || pU1->bUse == FALSE)
					continue;
				w2 = pU1->m_Rect.Width();
				h2 = pU1->m_Rect.Height();
				if (w2 < m_CharSize * 0.2f || w2 > m_CharSize * 0.8f)
					continue;
				mn = max(pU->m_Rect.left, pU1->m_Rect.left);
				mx = min(pU->m_Rect.right, pU1->m_Rect.right);
				un = mx - mn;
				mn1 = max(pU->m_Rect.top, pU1->m_Rect.top);
				mx1 = min(pU->m_Rect.bottom, pU1->m_Rect.bottom);
				un1 = mx1 - mn1;
				if (un > 0) {
					mn = min(w1, w2);
					rate = (float) un / (float) mn;

					if (rate > 0.8f && un1 > -m_CharSize * 0.3333f) {
						Ru.UnionRect(pU->m_Rect, pU1->m_Rect);
						if (Ru.Width() > m_CharSize * 0.8f)
							continue;
						//pU->m_Rect = Ru;
						pU->Append(pU1);
						w1 = Ru.Width();
						h1 = Ru.Height();
						//pU->nAddNum++;
						pU1->bUse = FALSE;
						count++;
						continue;

					}

				} else {
					nSum = max(pU->m_Rect.right, pU1->m_Rect.right)
							- min(pU->m_Rect.left, pU1->m_Rect.left);
					if ((float) nSum > lm3)
						break;
				}
			}
		}
	} while (count != 0);
	RemoveNoneUseRects(RunRtAry);
}

void CLineRecogPrint::FirstMerge_Vertical(CRunRtAry& RunRtAry) {
	int p, h, w, mn, mx, un, count;
	int nCharNum = RunRtAry.GetSize();
	CRunRt* pU, *pU1;

	do {
		count = 0;
		for (h = 0; h < nCharNum; h++) {
			pU = RunRtAry.GetAt(h);
			if (pU->bUse == FALSE)
				continue;
			w = pU->m_Rect.Height();

			for (p = 0; p < nCharNum; p++) {
				pU1 = RunRtAry.GetAt(p);
				if (h == p || pU1->bUse == FALSE)
					continue;
				mn = max(pU->m_Rect.top, pU1->m_Rect.top);
				mx = min(pU->m_Rect.bottom, pU1->m_Rect.bottom);
				un = mx - mn;

				if (un > 0) {
					mn = min(w, pU1->m_Rect.Height());
					if ((float) un / (float) mn >= 0.1f) {
						//pU->m_Rect.UnionRect(pU->m_Rect, pU1->m_Rect);
						pU->Append(pU1);
						w = pU->m_Rect.Height();
						//pU->nAddNum++; 
						pU1->bUse = FALSE;
						count++;
					}
				}
			}
		}
	} while (count != 0);
	RemoveNoneUseRects(RunRtAry);
}
int CLineRecogPrint::RemoveNoneUseRects(CRunRtAry& RunRtAry) {
	CRunRt* pU;
	int i, nCharNum = RunRtAry.GetSize();
	for (i = 0; i < nCharNum; ++i) {
		pU = RunRtAry.GetAt(i);
		if (pU->bUse == FALSE) {
			delete (CRunRt*) RunRtAry.GetAt(i);
			RunRtAry.RemoveAt(i);
			i--;
			nCharNum--;
		}
	}
	return nCharNum;
}

int CLineRecogPrint::RemoveNoiseRect(CRunRtAry& RunRtAry, int minH, int maxH,
		int minW) {
	CRunRt* pU;
	int i, w, h, nCharNum = RunRtAry.GetSize();
	for (i = 0; i < nCharNum; ++i) {
		pU = RunRtAry.GetAt(i);
		if (pU->nPixelNum < 10)
			pU->bUse = FALSE;
		w = pU->m_Rect.Width();
		h = pU->m_Rect.Height();
		if (w < minW)
			pU->bUse = FALSE;
		if (h < minH)
			pU->bUse = FALSE;
		if (h > maxH)
			pU->bUse = FALSE;
	}
	nCharNum = RemoveNoneUseRects(RunRtAry);
	return nCharNum;
}
