// Process.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "jGauge.h"
#include "gEdge.h"
#include "Process.h"


// Process
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
void Process::getEdge(CRect rect, double *t, double *a, double *b, int option)
{
	gEdge edge;

	int *pData; //세로1줄(1열) 짜리 배열
	double *pX;
	double *pY;
	double dEdge;
	int nSlope, nDir;

	gEdge::EdgeType edgeoption = gEdge::EdgeType::W2B;
	//if (option == 2 || option == 3)
	if ((option & CHANGE_B2W) == CHANGE_B2W)
	{
		edgeoption = gEdge::EdgeType::B2W;
	}

	if ((option & VERTICAL) == false)
	{
		pData = new int[rect.Height()]; //세로1줄(1열) 짜리 배열
		pX = new double[rect.Width()];
		pY = new double[rect.Width()];

		for (int i = rect.left; i < rect.right; i++) {	//왼쪽에서 오른쪽으로
			for (int j = rect.top; j < rect.bottom; j++)	//위에서 아래로
			{
				pData[j - rect.top] = m_fm[j*m_nPitch + i];	//ROI에서 세로 한줄 복사
															//rect변수는 영역정보만 가지고 있으며, 실제 데이터(픽셀값)은 fm을 통해서 얻어옴
			}

			edge.LineFindEdge(edgeoption, rect.Height(), pData, &dEdge, &nSlope);
			pX[i - rect.left] = i;
			pY[i - rect.left] = dEdge + rect.top;	//+rect.top 상태좌표를 절대좌표로 변환하기 위해사용
		}
		double dErrorLimit = 1;
		int nRemoveN = rect.Width()*0.2;
		edge.LineFitting(rect.Width(), pX, pY, nRemoveN, dErrorLimit, t, a, b);	//input(x,y)들의 평균의 직선의 방정식

	}


	if ((option & VERTICAL) == true)
	{
		pData = new int[rect.Width()]; //가로1줄(1열) 짜리 배열
		pX = new double[rect.Height()];
		pY = new double[rect.Height()];

		for (int j = rect.top; j < rect.bottom; j++) {
			for (int i = rect.left; i < rect.right; i++)
			{
				pData[i - rect.left] = m_fm[j*m_nPitch + i];
			}

			edge.LineFindEdge(edgeoption, rect.Width(), pData, &dEdge, &nSlope);


			pX[j - rect.top] = dEdge + rect.left;
			pY[j - rect.top] = j;	//+rect.top 상태좌표를 절대좌표로 변환하기 위해사용

			//TODO 계산에 사용되는 픽셀 표시 (테스트용)
			m_fm[int(pY[j - rect.top] * m_nPitch + pX[j - rect.top])] = 255;
		}

		double dErrorLimit = 1;
		int nRemoveN = rect.Height()*0.2;
		edge.LineFitting(rect.Height(), pX, pY, nRemoveN, dErrorLimit, t, a, b);	//input(x,y)들의 평균의 직선의 방정식



	}

	delete pY, pX, pData;
}

//영역으로 엣지찾기
void Process::getEdgeBox(CRect rect, double *t, double *a, double *b)
{
	//각 모서리(p1, p2, p3, p4) 의 pixel값확인하여 세로/가로인지 확인 (후보군 선정작업?)
	CPoint p1 = CPoint(rect.left, rect.top);				// p1 ┌-------┐p2
	CPoint p2 = CPoint(rect.right - 1, rect.top);			//    │       │
	CPoint p3 = CPoint(rect.left, rect.bottom - 1);			//    │       │
	CPoint p4 = CPoint(rect.right - 1, rect.bottom - 1);	// p3 └-------┘p4
	int nTh = 70;
	//pixel값 확인	(이치화 시켜서 0 또는 255로 저장)
	int pixel1 = (m_fm[p1.y * m_nPitch + p1.x] < nTh) ? 0 : 255;
	int pixel2 = (m_fm[p2.y * m_nPitch + p2.x] < nTh) ? 0 : 255;
	int pixel3 = (m_fm[p3.y * m_nPitch + p3.x] < nTh) ? 0 : 255;
	int pixel4 = (m_fm[p4.y * m_nPitch + p4.x] < nTh) ? 0 : 255;

	//가로줄인지 세로줄인지 판단 (getEdgeBox로 넘길때 해서 안해도 됨)
	if (pixel1 == pixel2 && pixel3 == pixel4 && pixel1 != pixel3)
	{
		//가로줄 W2B
		if (pixel1 == 255) {
			getEdge(rect, t, a, b, edgeType::HORIZONTAL);
		}//가로줄 B2W
		else if (pixel1 == 0) {
			getEdge(rect, t, a, b, edgeType::HORIZONTAL | edgeType::CHANGE_B2W);
		}
	}
	if (pixel1 == pixel3 && pixel2 == pixel4 && pixel1 != pixel2)
	{
		//세로줄 W2B
		if (pixel1 == 255) {
			getEdge(rect, t, a, b, edgeType::VERTICAL);
		}//세로줄 B2W
		else if (pixel1 == 0) {
			getEdge(rect, t, a, b, edgeType::VERTICAL | edgeType::CHANGE_B2W);
		}
	}
}

