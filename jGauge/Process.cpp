// Process.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "jGauge.h"
#include "gEdge.h"
#include "Process.h"
#include <algorithm>
#include <math.h>

Process::Process(unsigned char* fm, int nWidth, int nHeight, int nPitch)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nPitch = nPitch;
	m_fm = fm;
}
Process::Process(gImage* pGImage)
{
	m_nWidth = pGImage->gGetWidth();
	m_nHeight = pGImage->gGetHeight();
	m_nPitch = pGImage->gGetPitch();
	m_fm = pGImage->gGetImgPtr();
}

Process::~Process()
{
}


// Process 멤버 함수
void Process::getEdge(CRect rect, double *t, double *a, double *b)
{
	gEdge edge;

	int *pData; //세로1줄(1열) 짜리 배열
	double *pX;
	double *pY;
	double dEdge;
	int dir;
	int nSlope, nDir;

	gEdge::EdgeType edgeoption = gEdge::EdgeType::ABS;
	
		pData = new int[rect.Width()]; //가로1줄(1열) 짜리 배열
		pX = new double[rect.Height()];
		pY = new double[rect.Height()];

		


		for (int j = rect.top; j < rect.bottom; j++) {
			for (int i = rect.left; i < rect.right; i++)
			{
				pData[i - rect.left] = m_fm[j*m_nPitch + i];
			}

			edge.LineFindEdge(edgeoption, rect.Width(), pData, &dEdge, &nSlope, &dir);
			


			pX[j - rect.top] = dEdge + rect.left;
			pY[j - rect.top] = j;	//+rect.top 상태좌표를 절대좌표로 변환하기 위해사용

			/* edge 표시하기 위한 테스트 코드*/
			/*m_fm[int(pY[j - rect.top]) * m_nPitch + int(pX[j - rect.top])] = 0xff;*/
		}

		double dErrorLimit = 1;
		int nRemoveN = rect.Height()*0.2;
		edge.LineFitting(rect.Height(), pX, pY, nRemoveN, dErrorLimit, t, a, b);	//input(x,y)들의 평균의 직선의 방정식

	delete pY, pX, pData;
}

int Process::getEdgePoint(CRect rect, bool bDrawPoint) {
	gEdge edge;

	int *pData; //세로1줄(1열) 짜리 배열
	int *pXInt;
	double dEdge;
	int dir;
	int nSlope, nDir;

	//gEdge::EdgeType edgeoption = gEdge::EdgeType::ABS;
	gEdge::EdgeType edgeoption = gEdge::EdgeType::W2B;

	pData = new int[rect.Width()]; //가로1줄(1열) 짜리 배열
	pXInt = new int[rect.Height()];
	
	for (int j = rect.top; j < rect.bottom; j++) {
		for (int i = rect.right; i > rect.left; i--)
		{
			pData[rect.right - i] = m_fm[j*m_nPitch + i];	//오른쪽부터 왼쪽방향, 가로1줄 데이터
		}
		edge.LineFindEdge(edgeoption, rect.Width(), pData, &dEdge, &nSlope, &dir);

		pXInt[j - rect.top] = rect.right - dEdge;	//pXInt = [j, edge]

		/* edge 표시하기 위한 테스트 코드*/
		if (bDrawPoint) {
			m_fm[int(j*m_nPitch + (rect.right - dEdge))] = 0xff;
		}
			
	}

	double result = findMedianValue(pXInt, rect.Height()); //edge들의 중앙값
	/* edges의 중앙값을 표시하기 위한 테스트 코드*/
	if (bDrawPoint) {
		m_fm[int(rect.CenterPoint().y*m_nPitch + (result))] = 0;
	}

	delete pData, pXInt;
	return result;
}

double Process::findMedianValue(int *arr, int size)
{
	sort(arr, arr+size);
	return arr[size / 2];
}

/*점과 직선사이의 거리*/
double Process::measureDistance(double x, double y, Line line)
{
					//점과 직선사이의 거리  d = |ax0+by0+b| / root(a^2+b^2)
	return abs(line.a * x + (-line.t)*y + line.b) / sqrt(pow(line.a, 2) + pow(line.t, 2));
}