package com.szOCR.general;

public class RecogResult {
    public final int NUMBER_LEN = 12;
    public int m_nResultCount;

    public char[] m_szNumber;
    public char[] m_szNumber1;
    //public Rect[]   m_rcRecogRect;
    public int[] m_diffPos;//equal:0,diff=1 between First and second string
    public double m_nRecogDis;
    public long m_nRecogTime;

    public RecogResult() {
        m_nResultCount = 0;
        m_szNumber = new char[NUMBER_LEN];
        m_szNumber1 = new char[NUMBER_LEN];
        m_diffPos = new int[NUMBER_LEN];
        m_nRecogDis = 0;
        m_nRecogTime = 0;
    }

    public RecogResult(int nCount) {
        m_nResultCount = nCount;
        m_szNumber = new char[NUMBER_LEN];
        m_szNumber1 = new char[NUMBER_LEN];
        m_diffPos = new int[NUMBER_LEN];
        m_nRecogDis = 0;
        m_nRecogTime = 0;
    }
}
