// Process.cpp : ���� �����Դϴ�.
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


// Process ��� �Լ�
void Process::getEdge(CRect rect, double *t, double *a, double *b)
{
	gEdge edge;

	int *pData; //����1��(1��) ¥�� �迭
	double *pX;
	double *pY;
	double dEdge;
	int dir;
	int nSlope, nDir;

	//gEdge::EdgeType edgeoption = gEdge::EdgeType::W2B;
	gEdge::EdgeType edgeoption = gEdge::EdgeType::ABS;
	
		pData = new int[rect.Width()]; //����1��(1��) ¥�� �迭
		pX = new double[rect.Height()];
		pY = new double[rect.Height()];

		for (int j = rect.top; j < rect.bottom; j++) {
			for (int i = rect.left; i < rect.right; i++)
			{
				pData[i - rect.left] = m_fm[j*m_nPitch + i];
			}

			edge.LineFindEdge(edgeoption, rect.Width(), pData, &dEdge, &nSlope, &dir);


			pX[j - rect.top] = dEdge + rect.left;
			pY[j - rect.top] = j;	//+rect.top ������ǥ�� ������ǥ�� ��ȯ�ϱ� ���ػ��
		}

		double dErrorLimit = 1;
		int nRemoveN = rect.Height()*0.2;
		edge.LineFitting(rect.Height(), pX, pY, nRemoveN, dErrorLimit, t, a, b);	//input(x,y)���� ����� ������ ������

	delete pY, pX, pData;
}

#include <algorithm>
#include <math.h>
int Process::getEdgePoint(CRect rect) {
	gEdge edge;

	int *pData; //����1��(1��) ¥�� �迭
	double *pX;
	int *pXInt;
	double dEdge;
	int dir;
	int nSlope, nDir;

	gEdge::EdgeType edgeoption = gEdge::EdgeType::ABS;

	pData = new int[rect.Width()]; //����1��(1��) ¥�� �迭
	pX = new double[rect.Height()];
	pXInt = new int[rect.Height()];
	
	for (int j = rect.top; j < rect.bottom; j++) {
		for (int i = rect.left; i < rect.right; i++)
		{
			pData[i - rect.left] = m_fm[j*m_nPitch + i];
		}
		edge.LineFindEdge(edgeoption, rect.Width(), pData, &dEdge, &nSlope, &dir);

		pXInt[j - rect.top] = dEdge + rect.left;

		//this->m_fm[int(pY[j - rect.top] *m_nPitch + pX[j - rect.top])] = 255;
	}
	//pX�� �ֺ��� x�� ���
	sort(pXInt, pXInt+rect.Height());

	double result = pXInt[rect.Height() / 2]; //�߾Ӱ� edge
	delete pX, pData;
	return result;
}

/*���� ���������� �Ÿ�*/
double Process::measureDistance(CPoint point, Line line)
{
	double x = point.x;
	double y = point.y;

					//���� ���������� �Ÿ�  d = |ax0+by0+b| / root(a^2+b^2)
	return abs(line.a * x + (-line.t)*y + line.b) / sqrt(pow(line.a, 2) + pow(line.t, 2));
}