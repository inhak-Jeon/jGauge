// Process.cpp : ���� �����Դϴ�.
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

			/* edge ǥ���ϱ� ���� �׽�Ʈ �ڵ�*/
			/*m_fm[int(pY[j - rect.top]) * m_nPitch + int(pX[j - rect.top])] = 0xff;*/
		}

		double dErrorLimit = 1;
		int nRemoveN = rect.Height()*0.2;
		edge.LineFitting(rect.Height(), pX, pY, nRemoveN, dErrorLimit, t, a, b);	//input(x,y)���� ����� ������ ������

	delete pY, pX, pData;
}

int Process::getEdgePoint(CRect rect, bool bDrawPoint) {
	gEdge edge;

	int *pData; //����1��(1��) ¥�� �迭
	int *pXInt;
	double dEdge;
	int dir;
	int nSlope, nDir;

	//gEdge::EdgeType edgeoption = gEdge::EdgeType::ABS;
	gEdge::EdgeType edgeoption = gEdge::EdgeType::W2B;

	pData = new int[rect.Width()]; //����1��(1��) ¥�� �迭
	pXInt = new int[rect.Height()];
	
	for (int j = rect.top; j < rect.bottom; j++) {
		for (int i = rect.right; i > rect.left; i--)
		{
			pData[rect.right - i] = m_fm[j*m_nPitch + i];	//�����ʺ��� ���ʹ���, ����1�� ������
		}
		edge.LineFindEdge(edgeoption, rect.Width(), pData, &dEdge, &nSlope, &dir);

		pXInt[j - rect.top] = rect.right - dEdge;	//pXInt = [j, edge]

		/* edge ǥ���ϱ� ���� �׽�Ʈ �ڵ�*/
		if (bDrawPoint) {
			m_fm[int(j*m_nPitch + (rect.right - dEdge))] = 0xff;
		}
			
	}

	double result = findMedianValue(pXInt, rect.Height()); //edge���� �߾Ӱ�
	/* edges�� �߾Ӱ��� ǥ���ϱ� ���� �׽�Ʈ �ڵ�*/
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

/*���� ���������� �Ÿ�*/
double Process::measureDistance(double x, double y, Line line)
{
					//���� ���������� �Ÿ�  d = |ax0+by0+b| / root(a^2+b^2)
	return abs(line.a * x + (-line.t)*y + line.b) / sqrt(pow(line.a, 2) + pow(line.t, 2));
}