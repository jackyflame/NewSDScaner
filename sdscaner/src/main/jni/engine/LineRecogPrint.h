// LineRecogPrint.h: interface for the CLineRecogPrint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINERECOGPRINT_H__FB3CA690_0B98_409A_AB92_4F0F1797F895__INCLUDED_)
#define AFX_LINERECOGPRINT_H__FB3CA690_0B98_409A_AB92_4F0F1797F895__INCLUDED_

#include "TRunProc.h"
class CLineRecogPrint {
public:
	CLineRecogPrint();
	virtual ~CLineRecogPrint();

public:

	int m_CharSize;
	CRect m_LineRt;

	int RemoveNoneUseRects(CRunRtAry& RunRtAry);
	int RemoveNoiseRect(CRunRtAry& RunRtAry, int minH, int maxH, int minW);
	void ForcedSegment(CRunRtAry& RunRtAry, int minH);
public:

private:

	void Del_Rect(CRunRtAry& RunRtAry, int id, int mode);
	void GetImgYXFromRunRt_Ext(CRunRt* pU, BYTE** Img, int nWd, int nHi);
	int GetStrokeWidth(CRunRt* pU, BYTE** Img, int nWd, int nHi);
	float GetStrokeWidth(CRunRt* pU);

	void Smooth(float *Boon, int pit, int w_d);
	void FirstMerge_Horizontal(CRunRtAry& RunRtAry);
	void FirstMerge_Vertical(CRunRtAry& RunRtAry);

	int ForcedCut(CRunRtAry& RunRtAry, int nId);
	int ForcedEngCut(CRunRtAry& RunRtAry, int nId);
	int Is_Valley1(BYTE* Img, int nWd, int nHi, int nPos[]);
	int Is_Valley(BYTE** Img, int nWd, int nHi, int nPos[], int mode);
	int GetCutPosition(int nWd, int nHi, float *BoonPo1, float *BoonPo2,
			int nPos[], int mode);
	int ReFineDeterminePos(float* BoonPo1, int St, int Ed);

	float GetMinValue(float *dif, int n, int& AryNo);
};

#endif // !defined(AFX_LINERECOGPRINT_H__FB3CA690_0B98_409A_AB92_4F0F1797F895__INCLUDED_)
