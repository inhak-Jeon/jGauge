#pragma once
#include "gImage.h"
// Process 명령 대상입니다.

class Process : public CObject
{
private:
	int m_nWidth;
	int m_nHeight;
	int m_nPitch;
	unsigned char * m_fm;

public:
	enum edgeType
	{
		HORIZONTAL = 0b00,
		VERTICAL = 0b01,
		CHANGE_W2B = 0b00,
		CHANGE_B2W = 0b10
	};

	Process(unsigned char* fm, int nWidth, int nHeight, int nPitch);
	Process(gImage *pGImage);
	virtual ~Process();

	void getEdge(CRect rect, double *t, double *a, double *b, int options);
	void getEdgeBox(CRect rect, double *t, double *a, double *b);
};


