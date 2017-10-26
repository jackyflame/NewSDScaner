// RecogMQDF.cpp: implementation of the CRecogMQDF class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RecogMQDF.h"
#include "RecogCore.h"
#include "MQDF.h"
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
int iDiff[100][160];
int m_nDim = 160; //GDIM2 or PDIM
int m_nSeDim = 0;
int m_nPCADim = 160;
int m_nMQDFDim = 12;
int m_nCMFDim = 12;
int m_KohonenReduce = 0;

int m_nMQDFCNum = 10;
//WORD*	m_CodeMQDF = NULL;

int m_maxCand = 10;

//BYTE*	m_DicBuff = NULL;
//Diction*m_Dic = NULL;

//float	m_thita = 1.0f/0.0025898884f;//0.00258208f;//
//float*	m_SubPCAValue = NULL;

//float*	m_SubPCAMain[CMAXNum];
#define NHUBO		10
float m_pDis[CMAXNum];
int m_Ord[CMAXNum];

void GetSortingAZ(float *buf, int *ord, int n, int SortNum) {
	int i, j, m;
	float d;
	if (SortNum > n)
		SortNum = n;
	for (i = 0; i < SortNum; i++) {
		for (j = i + 1; j < n; j++) {
			if (buf[i] > buf[j]) {
				d = buf[i];
				buf[i] = buf[j];
				buf[j] = d;
				m = ord[i];
				ord[i] = ord[j];
				ord[j] = m;
			}
		}
	}
}
int RecogStep1() {
	int i, j, Id;
	int DisOne;
	int cc[10];
	const int sdim = m_nPCADim;
	int t;

	int nCandNum;
	float* pDis = m_pDis;
	int* Ord = m_Ord;
	int* iBec = m_iBec;
	memset(pDis, 0, sizeof(float) * m_nMQDFCNum);
	//sdim = m_nPCADim;

#ifdef IQMATH_USE
	_iq0 iqDiv;
#endif

	for (i = 0; i < m_nMQDFCNum; i++) {
		Ord[i] = i;

		DisOne = 0;
		int * pMean = iDiction[i];
		int * pDiff = iDiff[i];

#pragma MUST_ITERATE(160, 160, 4)
#pragma UNROLL(4)
		for (j = 0; j < sdim; j++) {
			t = pMean[j] - iBec[j];
			pDiff[j] = t;
			DisOne += (t * t);
		}

#ifdef IQMATH_USE
		iqDiv = _IQ0div(_FtoIQ0(DisOne), _FtoIQ0(1000000));
		pDis[i] = _IQ0toF(iqDiv);
#else		
		pDis[i] = (float) DisOne / 1000000;
#endif

	}
	memset(&m_Hubo, 0, sizeof(CAND));
	nCandNum = m_nMQDFCNum;
	GetSortingAZ(pDis, Ord, nCandNum, nCandNum);

	memset(cc, 0, sizeof(int) * 10);
	j = 0;
	bool bAdd = false;
	for (i = 0; i < nCandNum; ++i) {
		Id = Ord[i];
		if (pDis[i] > 12)
			break;
		bAdd = false;
		if (Id >= 0 && Id <= 9) {
			if (cc[0] < 5) {
				bAdd = true;
				cc[0]++;
			}
		} else if (Id >= 10 && Id <= 35) {
			if (cc[1] < 12) {
				bAdd = true;
				cc[1]++;
			}
		} else {
			bAdd = true;
		}
		if (bAdd) {
			// m_Hubo.Code[j]=m_CodeMQDF[Id];
			m_Hubo.Index[j] = Id;
			m_Hubo.Dis[j] = pDis[i];
			j++;
		}
	}
	m_Hubo.nCandNum = j; //nCandNum;
	return m_Hubo.nCandNum;
}

inline void ExchangeHubo(CAND* Hubo, int i, int j) {
	WORD w;
	int Id; //,Font;
	float Dis; //,ri;
	w = Hubo->Code[i];
	Hubo->Code[i] = Hubo->Code[j];
	Hubo->Code[j] = w;
	Id = Hubo->Index[i];
	Hubo->Index[i] = Hubo->Index[j];
	Hubo->Index[j] = Id;
	Dis = Hubo->Dis[i];
	Hubo->Dis[i] = Hubo->Dis[j];
	Hubo->Dis[j] = Dis;
	//	Dis=Hubo->FineDis[i];Hubo->FineDis[i]=Hubo->FineDis[j];	Hubo->FineDis[j]=Dis;
	// 	ri=Hubo->Conf[i];	Hubo->Conf[i]=Hubo->Conf[j];Hubo->Conf[j]=ri;
	//	Font=Hubo->Font[i];	Hubo->Font[i]=Hubo->Font[j];	Hubo->Font[j]=Font;
}
int RecogMQDF() { //MQDF
	int nOrder = m_maxCand;
	int i, j, k, Id;

	int* fPCAMatix;
	float* PCAVal;
	int* PCAMean;
	int* iBec = m_iBec;
	int HuboNum = m_Hubo.nCandNum;
	//float* pDis = new float[HuboNum];
	float* pDis = m_pDis;
	int scala;
	float fscalar, thita = m_thita;
	float val = 0, DisOne;

#ifdef IQMATH_USE
	_iq20 iqfscalar;
	_iq iqsqrfscalar;
	_iq iqmpy;
	_iq iqDisone;
	_iq0 iqCon = _IQ0(10000000);
#endif
	//m_nMQDFDim = 100;
	for (i = 0; i < HuboNum; i++) {
		Id = m_Hubo.Index[i];
		if (m_KohonenReduce == 0)
			fPCAMatix = m_iSubPCAMain[Id];
		PCAVal = (float*) (m_thitaVal[Id]);
		//PCAMean = (iDiction[Id]);
		DisOne = 0;
		int* pDiff = iDiff[Id];
		for (j = 0; j < 12/*MARDIM*/; j++) {
			scala = 0;
			for (k = 0; k < 160; k++)
				scala += pDiff[k] * fPCAMatix[k];

#ifdef IQMATH_USE
			iqfscalar = _IQ0div(_FtoIQ0(scala), iqCon);
			iqsqrfscalar = _IQ20mpy(iqfscalar, iqfscalar);
			iqmpy = _IQ20mpy(iqsqrfscalar, _FtoIQ20(PCAVal[j]));
			iqDisone = _FtoIQ20(DisOne);
			iqDisone += iqmpy;
			DisOne = _IQ20toF(iqDisone);
#else		
			fscalar = (float) scala / 10000000;
			DisOne += (fscalar * fscalar) * PCAVal[j];
#endif
			fPCAMatix += m_nPCADim;
		}
		pDis[i] = (float) ((m_Hubo.Dis[i] - DisOne) * thita + m_logVal[Id]);

	}

	float d, d1;
	for (i = 0; i < HuboNum; i++) {
		d = pDis[i];
		for (j = i + 1; j < HuboNum; j++)
			if (d > pDis[j]) {
				ExchangeHubo(&m_Hubo, i, j);
				d = pDis[j];
				d1 = pDis[i];
				pDis[i] = pDis[j];
				pDis[j] = d1;
			}
	}
	for (i = 0; i < HuboNum; i++)
		m_Hubo.Dis[i] = pDis[i];
//	delete pDis;

	return m_Hubo.Index[0];
}


int RecogMQDFCharImg(BYTE* pImg, int w, int h) {
	int retId;
	memset(&m_Hubo, 0, sizeof(CAND));

	GetFeatureGradH2Gray(pImg, w, h);
	retId = RecogStep1();
	retId = RecogMQDF(); //	MQDF, 1th fine classification
	// 	retId = RecogFineCMF();		//	CMF,  2th fine classification
	//SetConfidence();
	return retId;
}
