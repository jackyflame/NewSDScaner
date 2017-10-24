// RecogCore.cpp: implementation of the CRecogCore class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RecogCore.h"

//#include "Normalize.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#include "basicfunc.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
float m_ExpW1[][5] = { { 0.0014810f, 0.0092149f, 0.0169488f, 0.0092149f,
		0.0014810f }, { 0.0092149f, 0.0573374f, 0.1054599f, 0.0573374f,
		0.0092149f }, { 0.0169488f, 0.1054599f, 0.1939708f, 0.1054599f,
		0.0169488f }, { 0.0092149f, 0.0573374f, 0.1054599f, 0.0573374f,
		0.0092149f }, { 0.0014810f, 0.0092149f, 0.0169488f, 0.0092149f,
		0.0014810f } };

float m_tan[8] = { 0.0984914f, 0.3033467f, 0.5345111f, 0.8206789f, 1.2185037f,
		1.8708688f, 3.2965586f, 10.1531706f };
int m_itan[8] = { 98, 303, 534, 820, 1218, 1870, 3296, 10153 };

float m_Bec[PDIM];
int m_iBec[PDIM];
CAND m_Hubo;
BYTE m_F2[(GH2 + 2) * (GW2 + 2)];
//#pragma DATA_ALIGN(m_iFS, 8);
int m_iFS[(GH2 + 2) * (GW2 + 2)];
BYTE meanFS[(GH2 + 2) * (GW2 + 2)];

BYTE m_directId[(GH2) * (GW2)];
short m_GRAD[(GH2) * (GW2)];
int m_Be[9 * 7 * 32], m_Be1[9 * 7 * 16], m_BB[13 * 11 * 8];

int m_iExpW1[5 * 5];
int m_offi[GW2 + 2];
int m_offj[GH2 + 2];
int m_Wi[GW2 + 2];
int m_Hj[GH2 + 2];
//#define /*restrict*/ 
void initRecogCore() {
	int i, j;
	for (i = 0; i < 5; i++)
		for (j = 0; j < 5; j++) {
			m_iExpW1[i * 5 + j] = int(m_ExpW1[i][j] * 1000 + 0.5f);
		}

	//Create Gause Filter
// 	int i,j;
// 	float alpa = 1.218754f;
// 	float d=15;
// 	float pr2 = (d*d)/(4*alpa);
// 	float PI =  3.14159265f;
// 	float thita = PI/32.0f; 
// 
// 	d=2;
// 	pr2 = (d*d)/(4*alpa);
// 	for(i=0;i<5;i++)for(j=0;j<5;j++){
// 		m_ExpW1[i][j]=(float)((exp( (-1)*(i-2)*(i-2)/(2*pr2) )*exp( (-1)*(j-2)*(j-2)/(2*pr2) ))/(2*3.141592*pr2));
// 	}
// 	/////////////////////////////////	
// 
// 	for(i=0;i<8;i++){
// 		m_tan[i]=(float)tan(thita);
// 		thita += PI/16.0f;
// 	}
}

void MeanFilterH2Gray(BYTE* /*restrict*/F) {
	register int i, j;
	short Buff = 0;
	int W = GW2 + 2;
	BYTE */*restrict*/FS = meanFS;

//	_nassert((int) F % 8 == 0);
//	_nassert((int) FS % 8 == 0);

	memcpy(FS, F, (GH2 + 2) * (GW2 + 2));
	for (i = 1; i <= GH2; i++) {
//		#pragma MUST_ITERATE(GW2, GW2, 4)
//		#pragma UNROLL(4)
		for (j = 1; j <= GW2; j++) {
			Buff = FS[(i - 1) * W + j - 1] + FS[(i - 1) * W + j]
					+ FS[(i - 1) * W + j + 1] + FS[i * W + j - 1]
					+ FS[i * W + j] + FS[i * W + j + 1]
					+ FS[(i + 1) * W + j - 1] + FS[(i + 1) * W + j]
					+ FS[(i + 1) * W + j + 1];
			F[i * W + j] = Buff / 9;
		}
	}
}

void GetGradientBectorH2Gray(const BYTE* /*restrict*/F) {
	register int i, j, k, l, m;
	int* /*restrict*/FS = m_iFS;
	//int (*FS)[GW2+2] = m_iFS;
	memset(FS, 0, sizeof(int) * (GH2 + 2) * (GW2 + 2));
	int dir = 0;
	int Max = F[0], Ave = 0;
	int num = 0;
	short GradX = 0, GradY = 0, Grad;
	int tanVal;
	int* /*restrict*/mtan = m_itan;
	int* /*restrict*/Be = m_Be;
	int* /*restrict*/Be1 = m_Be1;
	int* /*restrict*/BB = m_BB;
	BYTE* /*restrict*/directId = m_directId;
	short* /*restrict*/GRAD = m_GRAD;

//	_nassert((int) FS % 8 == 0);
//	_nassert((int) GRAD % 8 == 0);
//	_nassert((int) directId % 8 == 0);
//	_nassert((int) Be % 8 == 0);
//	_nassert((int) Be1 % 8 == 0);
//	_nassert((int) BB % 8 == 0);
//	_nassert((int) m_itan % 8 == 0);

	register int W = GW2 + 2;
	for (i = 0; i < GH2 + 2; i++) {
//		#pragma MUST_ITERATE(GW2+2, GW2+2, 2)
//		#pragma UNROLL(2)
		//#pragma loop(hint_parallel(2))
		//#pragma loop(ivdep)
		for (j = 0; j < GW2 + 2; j++) {
			//if(F[i][j]==0)continue;
			num++;
			Ave += F[i * W + j];
			if (Max < F[i * W + j])
				Max = F[i * W + j];
		}
	}
	if (num == 0)
		num = 1;
	Ave = (int) ((float) Ave / num + 0.5f);
	if (Max - Ave != 0)
		for (i = 0; i < GH2 + 2; i++)
			for (j = 0; j < GW2 + 2; j++)
				FS[i * W + j] = ((F[i * W + j] - Ave) * 256) / (Max - Ave);

#ifdef IQMATH_USE
	_iq10 iIn;
	_iq10 iRet;
#endif
	register int pos;
	register int pos1;
	for (i = 1; i <= GH2; i++) {
//		#pragma MUST_ITERATE(GW2, GW2, 4)
//		#pragma UNROLL(4)
		for (j = 1; j <= GW2; j++) {
			GradX = 0, GradY = 0;
			GradX = FS[(i - 1) * W + j + 1] + FS[i * W + j + 1]
					+ FS[(i + 1) * W + j + 1] - FS[(i - 1) * W + j - 1]
					- FS[i * W + j - 1] - FS[(i + 1) * W + j - 1];
			GradY = FS[(i - 1) * W + j - 1] + FS[(i - 1) * W + j]
					+ FS[(i - 1) * W + j + 1] - FS[(i + 1) * W + j - 1]
					- FS[(i + 1) * W + j] - FS[(i + 1) * W + j + 1];
			pos = (i - 1) * GW2 + j - 1;
			if (GradX == 0) {
				if (GradY > 0) {
					directId[pos] = 8;
					GRAD[pos] = GradY;
					continue;
				}
				if (GradY < 0) {
					directId[pos] = 24;
					GRAD[pos] = -GradY;
					continue;
				}
				if (GradY == 0) {
					directId[pos] = 0;
					GRAD[pos] = 0;
					continue;
				}
			}
			if (GradY == 0) {
				if (GradX > 0) {
					directId[pos] = 0;
					GRAD[pos] = GradX;
					continue;
				}
				if (GradX < 0) {
					directId[pos] = 16;
					GRAD[pos] = -GradX;
					continue;
				}
			}
			//Grad = (abs(GradX) + abs(GradY));
			Grad = (int) (sqrt((float) (GradX * GradX + GradY * GradY)) + 0.5f);
			tanVal = abs(GradY * 1000 / GradX);
			//#pragma UNROLL(2)
			for (k = 0; k < 8; k++)
				if (tanVal < mtan[k])
					break;

			if (GradY > 0) {
				if (GradX > 0) {
					directId[pos] = k;
					GRAD[pos] = Grad;
					continue;
				}
				if (GradX < 0) {
					directId[pos] = 16 - k;
					GRAD[pos] = Grad;
					continue;
				}
			}
			if (GradY < 0) {
				if (GradX < 0) {
					directId[pos] = 16 + k;
					GRAD[pos] = Grad;
					continue;
				}
				if (GradX > 0) {
					if (k == 0)
						directId[pos] = 0;
					else
						directId[pos] = 32 - k;
					GRAD[pos] = Grad;
					continue;
				}
			}
		}
	}
////////////////////////////////////////////////////

	memset(Be, 0, sizeof(int) * 9 * 7 * 32);
	memset(Be1, 0, sizeof(int) * 9 * 7 * 16);
	memset(BB, 0, sizeof(int) * 13 * 11 * 8);

	for (i = 0; i < 9; i++)
		for (j = 0; j < 7; j++) {
			pos = (i * 7 + j) * 32;
			for (k = 0; k < 8; k++) {
//		#pragma MUST_ITERATE(8, 8, 8)
//		#pragma UNROLL(8)
				for (l = 0; l < 8; l++) {
					Be[pos + directId[(i * 4 + k) * GW2 + j * 4 + l]] += GRAD[(i
							* 4 + k) * GW2 + j * 4 + l];
				}
			}
			for (k = 2; k < 6; k++) {
//		#pragma MUST_ITERATE(4, 4, 4)
//		#pragma UNROLL(4)
				for (l = 2; l < 6; l++) {
					Be[pos + directId[(i * 4 + k) * GW2 + j * 4 + l]] += GRAD[(i
							* 4 + k) * GW2 + j * 4 + l];
				}
			}
// 		for (k=4;k<12;k++) for (l=4;l<12;l++){
// 			pBe[directId[i*8+k][j*8+l]] += GRAD[i*8+k][j*8+l]; 
// 		}
// 		for (k=6;k<10;k++) for (l=6;l<10;l++){
// 			pBe[directId[i*8+k][j*8+l]] += GRAD[i*8+k][j*8+l]; 
// 		}
		}
	for (i = 0; i < 9; i++)
		for (j = 0; j < 7; j++) {
			pos1 = (i * 7 + j) * 16;
			pos = (i * 7 + j) * 32;
//		#pragma MUST_ITERATE(14, 14, 2)
//		#pragma UNROLL(2)
			for (k = 0; k < 14; k++) {
				Be1[pos1 + k] = Be[pos + k * 2] + Be[pos + 2 * k + 1] * 4
						+ Be[pos + 2 * k + 2] * 6 + Be[pos + 2 * k + 3] * 4
						+ Be[pos + 2 * k + 4];
			}
			Be1[pos1 + 14] = Be[pos + 28] + Be[pos + 29] * 4 + Be[pos + 30] * 6
					+ Be[pos + 31] * 4 + Be[pos];
			Be1[pos1 + 15] = Be[pos + 30] + Be[pos + 31] * 4 + Be[pos] * 6
					+ Be[pos + 1] * 4 + Be[pos + 2];
		}
	for (i = 0; i < 9; i++)
		for (j = 0; j < 7; j++) {
			pos = ((i + 2) * 11 + j + 2) * 8;
			pos1 = (i * 7 + j) * 16;
//		#pragma MUST_ITERATE(7, 7, 7)
//		#pragma UNROLL(7)

			for (k = 0; k < 7; k++) {
				BB[pos + k] = Be1[pos1 + k * 2] + Be1[pos1 + 2 * k + 1] * 2
						+ Be1[pos1 + 2 * k + 2];
			}
			BB[pos + 7] = Be1[pos1 + 14] + Be1[pos1 + 15] * 2 + Be1[pos1];
		}

	int* /*restrict*/ibec = m_iBec;
	float* /*restrict*/bec = m_Bec;
	memset(ibec, 0, sizeof(int) * GDIM2);
	//memset(bec,0,sizeof(float)*GDIM2);
	register int vid = 0;
	int fMax = 0;
	int* /*restrict*/Expw1 = m_iExpW1;
	for (i = 0; i < 5; i++)
		for (j = 0; j < 4; j++) {
			for (m = 0; m < 8; m++) {
				for (k = 0; k < 5; k++) {
					for (l = 0; l < 5; l++) {
						ibec[vid] += (BB[((i * 2 + k) * 11 + j * 2 + l) * 8 + m]
								* Expw1[k * 5 + l]);
					}
				}
				//ibec[vid]/=1000;
				if (fMax < ibec[vid])
					fMax = ibec[vid];
				vid++;
			}
		}
	if (fMax == 0)
		return;

#ifdef IQMATH_USE	
	_iq2 iX, iY;
	_iq2 iDiv,iMpy,iTh;
	iY = _FtoIQ20(0.4);
	iTh = _IQ2(fMax);

#endif
//#pragma MUST_ITERATE(GDIM2, GDIM2, 4)
//#pragma UNROLL(4)
	for (i = 0; i < GDIM2; i++) {
		//ibec[i]/=fMax;
#ifdef IQMATH_USE		
		//bec[i] = (float)ibec[i]/fMax;
		//iDiv = _FtoIQ(bec[i]);
		iRet = (_IQ20div(_IQ2(ibectemp[i]), iTh));
		iRet=_IQ20sqrt(iRet);
		//bec[i] = _IQtoF(iRet);

		//bec[i]= powf(bec[i],0.4f);

		//iRet = _FtoIQ20(bec[i]);
		iMpy = _IQ0mpy(iRet, _IQ0(1000));
		ibec[i] = _IQ20int(iMpy);
#else
		bec[i] = (float) ibec[i] / fMax;
		bec[i] = (float) pow(bec[i], 0.4f);
		ibec[i] = int(bec[i] * 1000);
#endif
	}

}

void LinearNormalizeH2Gray(const BYTE* /*restrict*/pImg, int w1, int h1,
		BYTE* /*restrict*/F) {

	register int i, j;
	int i0, j0;
	int w = GW2 + 2, h = GH2 + 2;
	int x, y, xx, yy;
	int val00, val01, val10, val11;
	int iscale = ((w1 - 1) << 11) / (w - 1);
	int jscale = ((h1 - 1) << 11) / (h - 1);
// 	float iscale=(float)(w1-1)/(float)(w-1);
// 	float jscale=(float)(h1-1)/(float)(h-1);
	int* /*restrict*/offi = m_offi;
	int* /*restrict*/offj = m_offj;
	int* /*restrict*/Wi = m_Wi;
	int* /*restrict*/Hj = m_Hj;

//	_nassert((int) F % 8 == 0);
//	_nassert((int) offi % 8 == 0);
//	_nassert((int) offj % 8 == 0);
//	_nassert((int) Wi % 8 == 0);
//	_nassert((int) Hj % 8 == 0);

	memset(F, 0, (GW2 + 2) * (GH2 + 2));
//#pragma MUST_ITERATE(GW2+2, GW2+2, 2)
//#pragma UNROLL(2)
	for (i = 0; i < GW2 + 2; i++) {
		i0 = iscale * i; //+0.5f;
		Wi[i] = i0 >> 11;
		offi[i] = (i0 - (Wi[i] << 11));
	}
//#pragma MUST_ITERATE(GH2+2, GH2+2, 2)
//#pragma UNROLL(2)
	for (i = 0; i < GH2 + 2; i++) {
		j0 = jscale * i; //+0.5f;
		Hj[i] = j0 >> 11;
		offj[i] = (j0 - (Hj[i] << 11));
	}
//#pragma MUST_ITERATE(GH2+2, GH2+2, 2)
//#pragma UNROLL(2)
	for (j = 0; j < h; j++) {
		y = Hj[j];
		if (y >= h1 - 1)
			yy = y;
		else
			yy = y + 1;
//#pragma MUST_ITERATE(GW2+2, GW2+2, 2)
//#pragma UNROLL(2)
		for (i = 0; i < w; i++) {
			x = Wi[i];
			if (x >= w1 - 1)
				xx = x;
			else
				xx = x + 1;
			val00 = (int) pImg[y * w1 + x];
			val01 = (int) pImg[y * w1 + xx];
			val10 = (int) pImg[yy * w1 + x];
			val11 = (int) pImg[yy * w1 + xx];

			int newval = (int) (((val00 * (2048 - offj[j]) * (2048 - offi[i])
					+ val01 * (2048 - offj[j]) * offi[i]
					+ val10 * offj[j] * (2048 - offi[i])
					+ val11 * offj[j] * offi[i])));
			newval = (newval >> 22);
			if (newval >= 255)
				newval = 255;
			if (newval <= 0)
				newval = 0;
			F[j * w + i] = (BYTE) newval;
		}
	}
}
void GetFeatureGradH2Gray(BYTE* /*restrict*/pImg, int w, int h) {
	int i;

	LinearNormalizeH2Gray(pImg, w, h, m_F2);
	if (max(w,h) > 20) {
		MeanFilterH2Gray(m_F2);
	}
	GetGradientBectorH2Gray(m_F2);

}
