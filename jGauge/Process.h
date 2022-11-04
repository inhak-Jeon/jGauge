#pragma once
#include "gImage.h"
#include "Line.h"

// Process 명령 대상입니다.

class Process : public CObject
{
private:
	int m_nWidth;
	int m_nHeight;
	int m_nPitch;
	unsigned char * m_fm;

public:
	Process(unsigned char* fm, int nWidth, int nHeight, int nPitch);
	Process(gImage *pGImage);
	virtual ~Process();
	void getEdge(CRect rect, double * t, double * a, double * b, bool bDrawPoint);
	int getEdgePoint(CRect rect, bool bDrawPoint);
	double findMedianValue(int * arr, int size);
	double measureDistance(double x, double y, Line line);
};


