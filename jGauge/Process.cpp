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

void Process::findDistance(Line line, CPoint point, Line distanceLine, gImage *display)
{
	//����(point)�� ������ ������ ����	{y-y1 = m(x-x1) +b}
	Line orthoLine;
	orthoLine.t = line.t*(line.t*point.y);
	orthoLine.a = 1 / (-line.a)*(-point.x);
	orthoLine.b = line.b;

	//������ �������� ����
	gEdge edge = gEdge();
	double x, y;
	edge.FindCrossPoint(line.t, line.a, line.b, orthoLine.t, orthoLine.a, orthoLine.b, &x, &y);
	CPoint crossP = CPoint(x, y);


	//BaseLine ������ �׸���
	double t = orthoLine.t;
	double a = orthoLine.a;
	double b = orthoLine.b ;
	if (t == 0 && a == 0 && b == 0)
		return;
	CPoint p1, p2;
	p1.x = 0;
	p2.x = display->gGetWidth();
	p1.y = (a*p1.x + b) / t;
	p2.y = (a*p2.x + b) / t;
	display->gDrawLine(p1, p2,COLOR_BLUE);
}