#include "StdAfx.h"

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
template<class TYPE> void CopyElements(TYPE* pDest, const TYPE* pSrc,
		int nCount) {
	ASSERT(nCount == 0 || AfxIsValidAddress(pDest, nCount * sizeof(TYPE)));
	ASSERT(nCount == 0 || AfxIsValidAddress(pSrc, nCount * sizeof(TYPE)));

	// default is element-copy using assignment
	while (nCount--)
		*pDest++ = *pSrc++;
}

template<class TYPE> void DestructElements(TYPE* pElements, int nCount) {
	ASSERT(nCount == 0 || AfxIsValidAddress(pElements, nCount * sizeof(TYPE)));

	// call the destructor(s)
	for (; nCount--; pElements++)
		pElements->~TYPE();
}

template<class TYPE> void ConstructElements(TYPE* pElements, int nCount) {
	ASSERT(nCount == 0 || AfxIsValidAddress(pElements, nCount * sizeof(TYPE)));

	// first do bit-wise zero initialization
	memset((void*) pElements, 0, nCount * sizeof(TYPE));

	// then call the constructor(s)
	for (; nCount--; pElements++)
		::new ((void*) pElements) TYPE;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

CPoint::CPoint() {
	x = 0;
	y = 0;
}

CPoint::CPoint(int initX, int initY) {
	x = initX;
	y = initY;
}

CPoint::CPoint(POINT initPt) {
	x = initPt.x;
	y = initPt.y;
}

CPoint::CPoint(SIZE initSize) {
	x = initSize.cx;
	y = initSize.cy;
}

CPoint::CPoint(DWORD dwPoint) {
	x = LOWORD(dwPoint);
	y = HIWORD(dwPoint);
}

void CPoint::Offset(int xOffset, int yOffset) {
	x = x + xOffset;
	y = y + yOffset;
}

void CPoint::Offset(POINT point) {
	x = x + point.x;
	y = y + point.y;
}

void CPoint::Offset(SIZE size) {
	x = x + size.cx;
	y = y + size.cy;
}

BOOL CPoint::operator==(POINT point) const {
	if ((x == point.x) && (y == point.y)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL CPoint::operator!=(POINT point) const {
	if ((x != point.x) || (y != point.y)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void CPoint::operator+=(SIZE size) {
	x = x + size.cx;
	y = y + size.cy;
}

void CPoint::operator-=(SIZE size) {
	x = x - size.cx;
	y = y - size.cy;
}

void CPoint::operator+=(POINT point) {
	x = x + point.x;
	y = y + point.y;
}

void CPoint::operator-=(POINT point) {
	x = x - point.x;
	y = y - point.y;
}

CPoint CPoint::operator+(SIZE size) const {
	CPoint rPoint;
	rPoint.x = x + size.cx;
	rPoint.y = y + size.cy;
	return rPoint;
}

CPoint CPoint::operator-(SIZE size) const {
	CPoint rPoint;
	rPoint.x = x - size.cx;
	rPoint.y = y - size.cy;
	return rPoint;
}

CPoint CPoint::operator-() const {
	CPoint rPoint;
	rPoint.x = -x;
	rPoint.y = -y;
	return rPoint;
}

CPoint CPoint::operator+(POINT point) const {
	CPoint rPoint;
	rPoint.x = x + point.x;
	rPoint.y = y + point.y;
	return rPoint;
}

SIZE CPoint::operator-(POINT point) const {
	SIZE rSize;
	rSize.cx = x - point.x;
	rSize.cy = y - point.y;
	return rSize;

}

CRect CPoint::operator+(const RECT* lpRect) const {
	CRect rRect;
	rRect.bottom = y + lpRect->bottom;
	rRect.top = y + lpRect->top;
	rRect.left = x + lpRect->left;
	rRect.right = x + lpRect->right;
	return rRect;
}

CRect CPoint::operator-(const RECT* lpRect) const {
	CRect rRect;
	rRect.bottom = lpRect->bottom - y;
	rRect.top = lpRect->top - y;
	rRect.left = lpRect->left - x;
	rRect.right = lpRect->right - x;
	return rRect;
}

////////////////////////////////////////////////////////////////////
// class CSize

// CSize.cpp: implementation of the CSize class.
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSize::CSize() {
	cx = 0;
	cy = 0;
}

CSize::CSize(int initCX, int initCY) {
	cx = initCX;
	cy = initCY;
}

CSize::CSize(SIZE initSize) {
	cx = initSize.cx;
	cy = initSize.cy;
}

CSize::CSize(POINT initPt) {
	cx = initPt.x;
	cy = initPt.y;
}

CSize::CSize(DWORD dwSize) {
	cx = LOWORD(dwSize);
	cy = HIWORD(dwSize);
}

BOOL CSize::operator ==(SIZE size) const {
	if ((cx == size.cx) && (cy == size.cy))
		return TRUE;
	return FALSE;
}

BOOL CSize::operator !=(SIZE size) const {
	if ((cx != size.cx) || (cy != size.cy))
		return TRUE;
	return FALSE;
}

void CSize::operator +=(SIZE size) {
	cx = cx + size.cx;
	cy = cy + size.cy;
}

void CSize::operator -=(SIZE size) {
	cx = cx - size.cx;
	cy = cy - size.cy;
}

CSize CSize::operator+(SIZE size) const {
	CSize rSize;
	rSize.cx = cx + size.cx;
	rSize.cy = cy + size.cy;
	return rSize;
}

CSize CSize::operator-(SIZE size) const {
	CSize rSize;
	rSize.cx = cx - size.cx;
	rSize.cy = cy - size.cy;
	return rSize;
}

CSize CSize::operator -() const {
	CSize rSize;
	rSize.cx = -cx;
	rSize.cy = -cy;
	return rSize;
}

CPoint CSize::operator+(POINT point) const {
	CPoint rPoint;
	rPoint.x = cx + point.x;
	rPoint.y = cy + point.y;
	return rPoint;
}

CPoint CSize::operator-(POINT point) const {
	CPoint rPoint;
	rPoint.x = cx - point.x;
	rPoint.y = cy - point.y;
	return rPoint;
}

CRect CSize::operator+(const RECT* lpRect) const {
	CRect rRect;
	rRect.top = lpRect->top + cy;
	rRect.left = lpRect->left + cx;
	rRect.right = lpRect->right + cx;
	rRect.bottom = lpRect->bottom + cy;
	return rRect;
}

CRect CSize::operator-(const RECT* lpRect) const {
	CRect rRect;
	rRect.top = lpRect->top - cy;
	rRect.left = lpRect->left - cx;
	rRect.right = lpRect->right - cx;
	rRect.bottom = lpRect->bottom - cy;
	return rRect;
}

////////////////////////////////////////////////////////////////////
// class CRect
// CRect.cpp: implementation of the CRect class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRect::CRect() {
	top = 0;
	bottom = 0;
	left = 0;
	right = 0;
}

CRect::CRect(int l, int t, int r, int b) {
	top = t;
	bottom = b;
	left = l;
	right = r;
}

CRect::CRect(const RECT& srcRect) {
	top = srcRect.top;
	bottom = srcRect.bottom;
	left = srcRect.left;
	right = srcRect.right;
}

CRect::CRect(LPRECT lpSrcRect) {
	top = lpSrcRect->top;
	bottom = lpSrcRect->bottom;
	left = lpSrcRect->left;
	right = lpSrcRect->right;
}

CRect::CRect(POINT point, SIZE size) {
	top = point.x;
	left = point.y;
	right = left + size.cx;
	bottom = top + size.cy;
}

CRect::CRect(POINT topLeft, POINT bottomRight) {
	top = topLeft.y;
	left = topLeft.x;
	right = bottomRight.x;
	bottom = bottomRight.y;
}

int CRect::Width() const {
	return right - left;
}

int CRect::Height() const {
	return bottom - top;
}

CSize CRect::Size() const {
	CSize rsize;
	rsize.cx = right - left;
	rsize.cy = bottom - top;
	return rsize;
}

CPoint CRect::TopLeft() {
	CPoint r;
	r.x = left;
	r.y = top;
	return r;
}

CPoint CRect::BottomRight() {
	CPoint r;
	r.x = right;
	r.y = bottom;
	return r;
}

const CPoint CRect::TopLeft() const {
	CPoint r;
	r.x = left;
	r.y = top;
	return r;
}

const CPoint CRect::BottomRight() const {
	CPoint r;
	r.x = right;
	r.y = bottom;
	return r;
}

CPoint CRect::CenterPoint() const {
	CPoint rpoint;
	rpoint.y = (bottom + top) / 2;
	rpoint.x = (right + left) / 2;
	return rpoint;
}

void CRect::SwapLeftRight() {
	long temp;
	temp = right;
	right = left;
	left = temp;
}
void CRect::SwapLeftRight(LPRECT lpRect) {
	long temp;
	temp = lpRect->left;
	lpRect->left = lpRect->right;
	lpRect->right = temp;
}

CRect::operator LPRECT() {
	return this;
}

CRect::operator LPRECT() const {
	return (RECT*) this;
}

BOOL CRect::IsRectEmpty() const {
	if ((bottom - top == 0) && (right - left == 0))
		return TRUE;
	return FALSE;
}

BOOL CRect::IsRectNull() const {
	if ((top == bottom) || (left == right))
		return TRUE;
	return FALSE;
}

BOOL CRect::PtInRect(POINT point) const {
	if ((point.x >= left) && (point.x <= right) && (point.y >= top)
			&& (point.y <= bottom))
		return TRUE;
	return FALSE;
}

void CRect::SetRect(int x1, int y1, int x2, int y2) {
	left = x1;
	top = y1;
	right = x2;
	bottom = y2;
}

void CRect::SetRect(POINT topLeft, POINT bottomRight) {
	left = topLeft.x;
	top = topLeft.y;
	right = bottomRight.x;
	bottom = bottomRight.y;
}

void CRect::SetRectEmpty() {
	left = 0;
	top = 0;
	right = 0;
	bottom = 0;

}

void CRect::CopyRect(LPRECT lpSrcRect) {
	bottom = lpSrcRect->bottom;
	top = lpSrcRect->top;
	left = lpSrcRect->left;
	right = lpSrcRect->right;
}

BOOL CRect::EqualRect(LPRECT lpRect) const {
	if ((lpRect->bottom == bottom) && (lpRect->top == top)
			&& (lpRect->left == left) && (lpRect->right == right))
		return TRUE;
	return FALSE;
}

void CRect::InflateRect(int x, int y) {
	top = top - y;
	left = left - x;
}

void CRect::InflateRect(SIZE size) {
	top = top - size.cy;
	left = left - size.cx;
}

void CRect::InflateRect(LPRECT lpRect) {
	top = top - lpRect->top;
	bottom = bottom + lpRect->bottom;
	left = left - lpRect->left;
	right = right + lpRect->right;
}

void CRect::InflateRect(int l, int t, int r, int b) {
	top = top - t;
	bottom = bottom + b;
	left = left - l;
	right = right + r;

}

void CRect::DeflateRect(int x, int y) {
	top = top + y;
	left = left + x;
}

void CRect::DeflateRect(SIZE size) {
	top = top + size.cy;
	left = left + size.cx;
}

void CRect::DeflateRect(LPRECT lpRect) {
	top = top + lpRect->top;
	bottom = bottom - lpRect->bottom;
	left = left + lpRect->left;
	right = right - lpRect->right;
}

void CRect::DeflateRect(int l, int t, int r, int b) {
	top = top + t;
	bottom = bottom - b;
	left = left + l;
	right = right - r;

}

void CRect::OffsetRect(int x, int y) {
	top = top + y;
	left = left + x;
	right = right + x;
	bottom = bottom + y;
}

void CRect::OffsetRect(SIZE size) {
	top = top + size.cy;
	left = left + size.cx;
	right = right + size.cx;
	bottom = bottom + size.cy;
}

void CRect::OffsetRect(POINT point) {
	top = top + point.y;
	left = left + point.x;
	right = right + point.x;
	bottom = bottom + point.y;
}

void CRect::NormalizeRect() {
	if (top > bottom) {
		long temp;
		temp = bottom;
		bottom = top;
		top = temp;
	}
	if (left > right) {
		long temp;
		temp = right;
		right = left;
		left = temp;
	}
}

BOOL CRect::IntersectRect(LPRECT lpRect1, LPRECT lpRect2) {
	((CRect*) lpRect1)->NormalizeRect();
	((CRect*) lpRect2)->NormalizeRect();
	top = max(lpRect1->top,lpRect2->top);
	left = max(lpRect1->left,lpRect2->left);
	bottom = min(lpRect1->bottom,lpRect2->bottom);
	right = min(lpRect1->right,lpRect2->right);
	if (top >= bottom)
		top = bottom = 0;
	if (left >= right)
		left = right = 0;
	if (top < bottom && left < right)
		return TRUE;
	top = bottom = 0;
	left = right = 0;
	return FALSE;
}

BOOL CRect::UnionRect(LPRECT lpRect1, LPRECT lpRect2) {
	((CRect*) lpRect1)->NormalizeRect();
	((CRect*) lpRect2)->NormalizeRect();
	if (((CRect*) lpRect1)->IsRectNull()) {
		left = lpRect2->left;
		right = lpRect2->right;
		top = lpRect2->top;
		bottom = lpRect2->bottom;
		return TRUE;
	}
	if (((CRect*) lpRect2)->IsRectNull()) {
		left = lpRect1->left;
		right = lpRect1->right;
		top = lpRect1->top;
		bottom = lpRect1->bottom;
		return TRUE;
	}
	if (lpRect2->left > lpRect1->left)
		left = lpRect1->left;
	else
		left = lpRect2->left;

	if (lpRect2->top > lpRect1->top)
		top = lpRect1->top;
	else
		top = lpRect2->top;

	if (lpRect2->right < lpRect1->right)
		right = lpRect1->right;
	else
		right = lpRect2->right;

	if (lpRect2->bottom < lpRect1->bottom)
		bottom = lpRect1->bottom;
	else
		bottom = lpRect2->bottom;
	return TRUE;
}

BOOL CRect::SubtractRect(LPRECT lpRectSrc1, LPRECT lpRectSrc2) {
	if (lpRectSrc2->left > lpRectSrc1->left) {
		left = lpRectSrc1->left;
		right = lpRectSrc2->left;
	} else {
		left = lpRectSrc2->left;
		right = lpRectSrc1->left;
	}

	if (lpRectSrc2->top > lpRectSrc1->top) {
		top = lpRectSrc1->top;
		bottom = lpRectSrc2->top;
	} else {
		top = lpRectSrc2->top;
		bottom = lpRectSrc2->top;
	}
	return TRUE;
}

void CRect::operator=(const RECT& srcRect) {
	top = srcRect.top;
	bottom = srcRect.bottom;
	left = srcRect.left;
	right = srcRect.right;
}

BOOL CRect::operator==(const RECT& rect) const {
	if ((top == rect.top) && (bottom == rect.bottom) && (right == rect.right)
			&& (left == rect.left))
		return TRUE;
	return FALSE;
}

BOOL CRect::operator!=(const RECT& rect) const {
	if ((top == rect.top) && (bottom == rect.bottom) && (right == rect.right)
			&& (left == rect.left))
		return FALSE;
	return TRUE;
}

void CRect::operator+=(POINT point) {
	top = top + point.y;
	left = left + point.x;
	right = right + point.x;
	bottom = bottom + point.y;
}

void CRect::operator+=(SIZE size) {
	top = top + size.cy;
	left = left + size.cx;
	right = right + size.cx;
	bottom = bottom + size.cy;
}

void CRect::operator+=(LPRECT lpRect) {
	left = left - lpRect->left;
	right = right + lpRect->right;
	top = top - lpRect->top;
	bottom = bottom + lpRect->bottom;
}

void CRect::operator-=(POINT point) {
	top = top - point.y;
	left = left - point.x;
	right = right - point.x;
	bottom = bottom - point.y;
}

void CRect::operator-=(SIZE size) {
	top = top - size.cy;
	left = left - size.cx;
	right = right - size.cx;
	bottom = bottom - size.cy;
}

void CRect::operator-=(LPRECT lpRect) {
	left = left + lpRect->left;
	right = right - lpRect->right;
	top = top + lpRect->top;
	bottom = bottom - lpRect->bottom;
}

void CRect::operator&=(const RECT& rect) {
	CRect rect1(left, top, right, bottom);
	IntersectRect((RECT*) &rect1, (RECT*) &rect);
}

void CRect::operator|=(const RECT& rect) {
	CRect rect1(left, top, right, bottom);
	UnionRect((RECT*) &rect1, (RECT*) &rect);
}

CRect CRect::operator+(POINT point) const {
	CRect rect;
	rect.bottom = bottom + point.y;
	rect.top = top + point.y;
	rect.left = left + point.x;
	rect.right = right + point.x;
	return rect;
}

CRect CRect::operator-(POINT point) const {
	CRect rect;
	rect.bottom = bottom - point.y;
	rect.top = top - point.y;
	rect.left = left - point.x;
	rect.right = right - point.x;
	return rect;
}

CRect CRect::operator+(LPRECT lpRect) const {
	CRect rect;
	rect.bottom = bottom + lpRect->bottom;
	rect.top = top - lpRect->top;
	rect.left = left - lpRect->left;
	rect.right = right + lpRect->right;
	return rect;
}

CRect CRect::operator+(SIZE size) const {
	CRect rect;
	rect.bottom = bottom + size.cy;
	rect.top = top + size.cy;
	rect.left = left + size.cx;
	rect.right = right + size.cx;
	return rect;
}

CRect CRect::operator-(SIZE size) const {
	CRect rect;
	rect.bottom = bottom - size.cy;
	rect.top = top - size.cy;
	rect.left = left - size.cx;
	rect.right = right - size.cx;
	return rect;
}

CRect CRect::operator-(LPRECT lpRect) const {
	CRect rect;
	rect.bottom = bottom - lpRect->bottom;
	rect.top = top + lpRect->top;
	rect.left = left + lpRect->left;
	rect.right = right - lpRect->right;
	return rect;
}

CRect CRect::operator&(const RECT& rect2) const {
	CRect rect1(left, top, right, bottom);
	if (rect1.top > rect1.bottom)
		rect1.top = rect1.bottom = 0;
	if (rect1.left > rect1.right)
		rect1.left = rect1.right = 0;

	if (rect2.top > rect2.bottom)
		rect1.top = rect1.bottom = 0;
	if (rect2.left > rect2.right)
		rect1.left = rect1.right = 0;
	if (((rect1.top < rect2.bottom) && (rect1.left < rect2.right))
			|| ((rect2.top < rect1.bottom) && (rect2.left < rect1.right))) {
		if (rect2.left > rect1.left)
			rect1.left = rect2.left;
		else
			rect1.left = rect1.left;

		if (rect2.top > rect1.top)
			rect1.top = rect2.top;
		else
			rect1.top = rect1.top;

		if (rect2.right < rect1.right)
			rect1.right = rect2.right;
		else
			rect1.right = rect1.right;

		if (rect2.bottom < rect1.bottom)
			rect1.bottom = rect2.bottom;
		else
			rect1.bottom = rect1.bottom;
	}
	return rect1;
}

CRect CRect::operator|(const RECT& rect2) const {
	CRect rect1(left, top, right, bottom);
	if (rect2.left > rect1.left)
		rect1.left = rect1.left;
	else
		rect1.left = rect2.left;

	if (rect2.top > rect1.top)
		rect1.top = rect1.top;
	else
		rect1.top = rect2.top;

	if (rect2.right < rect1.right)
		rect1.right = rect1.right;
	else
		rect1.right = rect2.right;

	if (rect2.bottom < rect1.bottom)
		rect1.bottom = rect1.bottom;
	else
		rect1.bottom = rect2.bottom;
	return rect1;
}

CRect CRect::MulDiv(int nMultiplier, int nDivisor) const {
	CRect rect;
	rect.left = left * nMultiplier / nDivisor;
	rect.right = right * nMultiplier / nDivisor;
	rect.bottom = bottom * nMultiplier / nDivisor;
	rect.top = top * nMultiplier / nDivisor;
	return rect;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template<class TYPE, class ARG_TYPE>
CArray<TYPE, ARG_TYPE>::CArray() {
	m_pData = 0x00000000;
	m_nSize = 0;
	m_nMaxSize = 0;
	m_nGrowBy = 0;

	m_pData = new TYPE[m_nSize];
}

template<class TYPE, class ARG_TYPE>
CArray<TYPE, ARG_TYPE>::~CArray() {
	if (m_pData != NULL) {
		delete[] (BYTE*) m_pData;
	}
}

template<class TYPE, class ARG_TYPE>
int CArray<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement) {
	m_pData[m_nSize] = newElement;
	m_nSize = m_nSize + 1;
	if (m_nMaxSize < m_nSize)
		m_nMaxSize = m_nSize + m_nGrowBy;
	return m_nSize;
}

template<class TYPE, class ARG_TYPE>
int CArray<TYPE, ARG_TYPE>::GetSize() const {
	return m_nSize;
}

template<class TYPE, class ARG_TYPE>
int CArray<TYPE, ARG_TYPE>::GetUpperBound() const {
	return m_nSize - 1;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetSize(int nNewSize, int nGrowBy) {
	m_nSize = nNewSize;
	if (m_nMaxSize < m_nSize)
		m_nMaxSize = m_nSize + m_nGrowBy;
	m_nGrowBy = nGrowBy;
	for (int i = 0; i < nNewSize; i++) {
		m_pData[i].x = 0;
		m_pData[i].y = 0;
	}

}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::FreeExtra() {
	m_nMaxSize = m_nSize;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::RemoveAll() {
	for (int i = 0; i < m_nSize; i++)
		m_pData[i] = NULL;
	m_nSize = 0;

}

template<class TYPE, class ARG_TYPE>
TYPE CArray<TYPE, ARG_TYPE>::GetAt(int nIndex) const {
	return m_pData[nIndex];
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetAt(int nIndex, ARG_TYPE newElement) {
	m_pData[nIndex] = newElement;
}

template<class TYPE, class ARG_TYPE>
TYPE& CArray<TYPE, ARG_TYPE>::ElementAt(int nIndex) {
	return (TYPE&) (m_pData[nIndex]);
}

// Direct Access to the element data (may return NULL)
template<class TYPE, class ARG_TYPE>
const TYPE* CArray<TYPE, ARG_TYPE>::GetData() const {
	return (const TYPE*) m_pData;
}

template<class TYPE, class ARG_TYPE>
TYPE* CArray<TYPE, ARG_TYPE>::GetData() {
	return m_pData;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetAtGrow(int nIndex, ARG_TYPE newElement) {

	for (int i = m_nSize; i < nIndex; i++) {
		m_pData[i] = NULL;
	}
	m_pData[nIndex] = newElement;
}

template<class TYPE, class ARG_TYPE>
int CArray<TYPE, ARG_TYPE>::Append(const CArray& src) {
	int m_nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize, 0);
	CopyElements<TYPE>(m_pData + m_nOldSize, src.m_pData, src.m_nSize);
	return m_nOldSize;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE newElement,
		int nCount) {
	if (nIndex >= m_nSize) {
		SetSize(nIndex + nCount, -1);
	} else {
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);
		DestructElements<TYPE>(&m_pData[nOldSize], nCount);
		memmove(&m_pData[nIndex + nCount], &m_pData[nIndex],
				(nOldSize - nIndex) * sizeof(TYPE));
		ConstructElements<TYPE>(&m_pData[nIndex], nCount);
	}
	ASSERT(nIndex + nCount <= m_nSize);
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount) {
	int nMoveCount = m_nSize - (nIndex + nCount);
	DestructElements<TYPE>(&m_pData[nIndex], nCount);
	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
				nMoveCount * sizeof(TYPE));
	m_nSize -= nCount;

}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::InsertAt(int nStartIndex, CArray* pNewArray) {
	if (pNewArray->GetSize() > 0) {
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}

}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::Copy(const CArray& src) {
	SetSize(src.m_nSize, 0);
	CopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);

}

// overloaded operator helpers
template<class TYPE, class ARG_TYPE>
TYPE CArray<TYPE, ARG_TYPE>::operator[](int nIndex) const {
	return GetAt(nIndex);
}

template<class TYPE, class ARG_TYPE>
TYPE& CArray<TYPE, ARG_TYPE>::operator[](int nIndex) {
	return ElementAt(nIndex);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
/*template<class BASE_CLASS, class TYPE>
 TYPE CTypedPtrArray<BASE_CLASS,TYPE>::GetAt(int nIndex) const
 {
 return (TYPE)BASE_CLASS::GetAt(nIndex);
 }
 template<class BASE_CLASS, class TYPE>
 TYPE& CTypedPtrArray<BASE_CLASS,TYPE>::ElementAt(int nIndex)
 {
 return (TYPE&)BASE_CLASS::ElementAt(nIndex);
 }
 template<class BASE_CLASS, class TYPE>
 void CTypedPtrArray<BASE_CLASS,TYPE>::SetAt(int nIndex, TYPE ptr)
 {
 BASE_CLASS::SetAt(nIndex, ptr);
 }

 // Potentially growing the array
 template<class BASE_CLASS, class TYPE>
 void CTypedPtrArray<BASE_CLASS,TYPE>::SetAtGrow(int nIndex, TYPE newElement)
 {
 BASE_CLASS::SetAtGrow(nIndex, newElement);
 }
 template<class BASE_CLASS, class TYPE>
 int CTypedPtrArray<BASE_CLASS,TYPE>::Add(TYPE newElement)
 {
 return BASE_CLASS::Add(newElement);
 }
 template<class BASE_CLASS, class TYPE>
 int CTypedPtrArray<BASE_CLASS,TYPE>::Append(const CTypedPtrArray<BASE_CLASS, TYPE>& src)
 {
 return BASE_CLASS::Append(src);
 }
 template<class BASE_CLASS, class TYPE>
 void CTypedPtrArray<BASE_CLASS,TYPE>::Copy(const CTypedPtrArray<BASE_CLASS, TYPE>& src)
 {
 BASE_CLASS::Copy(src);
 }

 // Operations that move elements around
 template<class BASE_CLASS, class TYPE>
 void CTypedPtrArray<BASE_CLASS,TYPE>::InsertAt(int nIndex, TYPE newElement, int nCount)
 {
 BASE_CLASS::InsertAt(nIndex, newElement, nCount);
 }
 template<class BASE_CLASS, class TYPE>
 void CTypedPtrArray<BASE_CLASS,TYPE>::InsertAt(int nStartIndex, CTypedPtrArray<BASE_CLASS, TYPE>* pNewArray)
 {
 BASE_CLASS::InsertAt(nStartIndex, pNewArray);
 }

 // overloaded operator helpers
 template<class BASE_CLASS, class TYPE>
 TYPE CTypedPtrArray<BASE_CLASS,TYPE>::operator[](int nIndex) const
 {
 return (TYPE)BASE_CLASS::operator[](nIndex);
 }
 template<class BASE_CLASS, class TYPE>
 TYPE& CTypedPtrArray<BASE_CLASS,TYPE>::operator[](int nIndex)
 {
 return (TYPE&)BASE_CLASS::operator[](nIndex);
 }*/

CPtrArray::CPtrArray() {
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

int CPtrArray::GetSize() const {
	return m_nSize;
}

int CPtrArray::GetUpperBound() const {
	return m_nSize - 1;
}

void CPtrArray::SetSize(int nNewSize, int nGrowBy) {
//	AssertValid(this);
//	ASSERT(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy; // set new size

	if (nNewSize == 0) {
		// shrink to nothing
		delete[] (BYTE*) m_pData;
		m_pData = NULL;
		m_nSize = m_nMaxSize = 0;
	} else if (m_pData == NULL) {
		// create one with exact size
#ifdef SIZE_T_MAX
		ASSERT(nNewSize <= SIZE_T_MAX/sizeof(void*)); // no overflow
#endif
		m_pData = (void**) new BYTE[nNewSize * sizeof(void*)];

		memset(m_pData, 0, nNewSize * sizeof(void*)); // zero fill

		m_nSize = m_nMaxSize = nNewSize;
	} else if (nNewSize <= m_nMaxSize) {
		// it fits
		if (nNewSize > m_nSize) {
			// initialize the new elements

			memset(&m_pData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(void*));

		}

		m_nSize = nNewSize;
	} else {
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0) {
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = min(1024, max(4, m_nSize / 8));
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy; // granularity
		else
			nNewMax = nNewSize; // no slush

		ASSERT(nNewMax >= m_nMaxSize);
		// no wrap around
#ifdef SIZE_T_MAX
		ASSERT(nNewMax <= SIZE_T_MAX/sizeof(void*)); // no overflow
#endif
		void** pNewData = (void**) new BYTE[nNewMax * sizeof(void*)];

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(void*));

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);

		memset(&pNewData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(void*));

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*) m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

void CPtrArray::FreeExtra() {

	AssertValid();

	if (m_nSize != m_nMaxSize) {
		// shrink to desired size
#ifdef SIZE_T_MAX
		ASSERT(m_nSize <= SIZE_T_MAX/sizeof(void*)); // no overflow
#endif
		void** pNewData = NULL;
		if (m_nSize != 0) {
			pNewData = (void**) new BYTE[m_nSize * sizeof(void*)];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(void*));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*) m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

void CPtrArray::RemoveAll() {
	SetSize(0);
}

void* CPtrArray::GetAt(int nIndex) const {
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

void CPtrArray::SetAt(int nIndex, void* newElement) {
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = newElement;
}

void*& CPtrArray::ElementAt(int nIndex) {
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

void** CPtrArray::GetData() {
	return (void**) m_pData;
}

void CPtrArray::SetAtGrow(int nIndex, void* newElement) {
	AssertValid();
	ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex + 1);
	m_pData[nIndex] = newElement;
}

int CPtrArray::Add(void* newElement) {
	int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex;
}

int CPtrArray::Append(const CPtrArray& src) {
	AssertValid();
	ASSERT(this != &src);
	// cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);

	memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(void*));

	return nOldSize;
}

void CPtrArray::Copy(const CPtrArray& src) {
	AssertValid();
	ASSERT(this != &src);
	// cannot append to itself

	SetSize(src.m_nSize);

	memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(void*));
}

void* CPtrArray::operator[](int nIndex) const {
	return GetAt(nIndex);
}

void*& CPtrArray::operator[](int nIndex) {
	return ElementAt(nIndex);
}

void CPtrArray::InsertAt(int nIndex, void* newElement, int nCount) {
	AssertValid();
	ASSERT(nIndex >= 0);
	// will expand to meet need
	ASSERT(nCount > 0);
	// zero or negative size not allowed

	if (nIndex >= m_nSize) {
		// adding after the end of the array
		SetSize(nIndex + nCount); // grow so nIndex is valid
	} else {
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount); // grow it to new size
		// shift old data up to fill gap
		memmove(&m_pData[nIndex + nCount], &m_pData[nIndex],
				(nOldSize - nIndex) * sizeof(void*));

		// re-init slots we copied from

		memset(&m_pData[nIndex], 0, nCount * sizeof(void*));

	}

	// insert new value in the gap
	ASSERT(nIndex + nCount <= m_nSize);

	// copy elements into the empty space
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

void CPtrArray::RemoveAt(int nIndex, int nCount) {
	AssertValid();
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	ASSERT(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);

	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
				nMoveCount * sizeof(void*));
	m_nSize -= nCount;
}

void CPtrArray::InsertAt(int nStartIndex, CPtrArray* pNewArray) {
	AssertValid();
	ASSERT(pNewArray != NULL);
//	ASSERT_KINDOF(CPtrArray, pNewArray);
	pNewArray->AssertValid();
	ASSERT(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0) {
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}

CPtrArray::~CPtrArray() {
	AssertValid();

	delete[] (BYTE*) m_pData;
}

void CPtrArray::AssertValid() const {
	if (m_pData == NULL) {
		ASSERT(m_nSize == 0);
		ASSERT(m_nMaxSize == 0);
	} else {
		ASSERT(m_nSize >= 0);
		ASSERT(m_nMaxSize >= 0);
		ASSERT(m_nSize <= m_nMaxSize);
//		ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * sizeof(void*)));
	}
}

CFile::CFile() {
	m_szFileName[0] = 0;

	m_hFile = (FILE*) hFileNull;

}

CFile::~CFile() {
	Close();
}

BOOL CFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags) {
	if (m_hFile != (FILE*) hFileNull)
		return FALSE;
	if (lpszFileName == NULL || mystrlen(lpszFileName) == 0)
		return FALSE;

	strcpy_TCHAR2char(m_szFileName, lpszFileName);
#ifdef _LINUX
	int i;
	for (i = 0; i < strlen(m_szFileName); i++) {
		if (m_szFileName[i] == '\\')
			m_szFileName[i] = '/';
	}
#endif
	if (nOpenFlags & modeRead)
		m_hFile = fopen(m_szFileName, "rb");
	else if ((nOpenFlags & modeWrite) != 0)
		m_hFile = fopen(m_szFileName, "wb");

	if (m_hFile == NULL)
		return FALSE;
	return TRUE;
}

void CFile::strcpy_TCHAR2char(char* dst, LPCTSTR str) {
	int i = 0;
	for (;; ++i) {
		if (str[i] == 0)
			break;
		if (i > 1000)
			break;
		dst[i] = str[i];
	}
	dst[i] = 0;

}

void CFile::Close() {
	if (m_hFile != hFileNull)
		fclose(m_hFile);
	m_szFileName[0] = 0;
	m_hFile = NULL;
}

UINT CFile::Read(void* lpBuf, UINT nCount) {
	if (m_hFile == hFileNull)
		return 0;

	return fread(lpBuf, 1, nCount, m_hFile);
}
char* CFile::ReadString(void* lpBuf, UINT nMax) {
	return fgets((char*) lpBuf, nMax, m_hFile);
}
void CFile::Write(const void* lpBuf, UINT nCount) {
	if (m_hFile == hFileNull)
		return;

	fwrite(lpBuf, 1, nCount, m_hFile);
}

LONG CFile::Seek(LONG lOff, UINT nFrom) {
	if (m_hFile == hFileNull)
		return 0;

	switch (nFrom) {
	case begin:
		fseek(m_hFile, lOff, SEEK_SET);
		break;
	case current:
		fseek(m_hFile, lOff, SEEK_CUR);
		break;
	case end:
		fseek(m_hFile, lOff, SEEK_END);
		break;
	}
	fpos_t pos;
	fgetpos(m_hFile, &pos);
//#ifndef _WIN32
//	return (LONG)pos.__pos;
//#else
//	return (LONG)pos;
//#endif // _LINUX
	return (LONG) pos;
}

DWORD CFile::GetPosition() const {
	if (m_hFile == hFileNull)
		return 0;

	fpos_t pos;
	fgetpos(m_hFile, &pos);
//#ifndef _WIN32
//	return (DWORD)pos.__pos;
//#else
//	return (DWORD)pos;
// #endif // _LINUX
	return (DWORD) pos;
}

DWORD CFile::GetLength() {
	if (m_hFile == hFileNull)
		return 0;

	DWORD dwPos = GetPosition();
	Seek(0, end);
	DWORD dwLen = GetPosition();
	Seek(dwPos, begin);
	return dwLen;
}
void CFile::Flush() {

}
void CFile::SeekToEnd() {
	fseek(m_hFile, 0, SEEK_END);
}
