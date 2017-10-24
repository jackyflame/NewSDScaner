// RecogCore.h: interface for the CRecogCore class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

//#include "typedefine.h"

#define		RC_FALSE		0
#define		RC_OK			1

#define		ACCEPT			1
#define		REJECT			0

#define		MAX_CAND	10//max number of candidate
typedef struct tagCAND {
	WORD Code[MAX_CAND]; //Candidate codes
	int Index[MAX_CAND]; //Candidate Indexes
	float Dis[MAX_CAND];
	int nCandNum; //number of Candiates
} CAND;

#define GH2			40//50//32//50//80//54//55//66//50//66//54//50
#define GW2			32//50//32//50//80//54//55//66//50//63//54//50 
#define PDIM		800
#define GDIM2		160//128//200//200//128//200	//구배특징량차원수
#define CMAXNum		10

//extern float	m_Bec[PDIM];
extern int m_iBec[PDIM];
extern CAND m_Hubo;

void GetFeatureGradH2Gray(BYTE* pImg, int w, int h);
void initRecogCore();

// !defined(AFX_RECOGCORE_H__7DDD4323_860F_447F_B77B_9F5605B0FE5E__INCLUDED_)
