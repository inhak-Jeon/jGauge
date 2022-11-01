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
void Process::getEdge(CRect rect, double *t, double *a, double *b, int option)
{
	gEdge edge;

	int *pData; //����1��(1��) ¥�� �迭
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
		pData = new int[rect.Height()]; //����1��(1��) ¥�� �迭
		pX = new double[rect.Width()];
		pY = new double[rect.Width()];

		for (int i = rect.left; i < rect.right; i++) {	//���ʿ��� ����������
			for (int j = rect.top; j < rect.bottom; j++)	//������ �Ʒ���
			{
				pData[j - rect.top] = m_fm[j*m_nPitch + i];	//ROI���� ���� ���� ����
															//rect������ ���������� ������ ������, ���� ������(�ȼ���)�� fm�� ���ؼ� ����
			}

			edge.LineFindEdge(edgeoption, rect.Height(), pData, &dEdge, &nSlope);
			pX[i - rect.left] = i;
			pY[i - rect.left] = dEdge + rect.top;	//+rect.top ������ǥ�� ������ǥ�� ��ȯ�ϱ� ���ػ��
		}
		double dErrorLimit = 1;
		int nRemoveN = rect.Width()*0.2;
		edge.LineFitting(rect.Width(), pX, pY, nRemoveN, dErrorLimit, t, a, b);	//input(x,y)���� ����� ������ ������

	}


	if ((option & VERTICAL) == true)
	{
		pData = new int[rect.Width()]; //����1��(1��) ¥�� �迭
		pX = new double[rect.Height()];
		pY = new double[rect.Height()];

		for (int j = rect.top; j < rect.bottom; j++) {
			for (int i = rect.left; i < rect.right; i++)
			{
				pData[i - rect.left] = m_fm[j*m_nPitch + i];
			}

			edge.LineFindEdge(edgeoption, rect.Width(), pData, &dEdge, &nSlope);


			pX[j - rect.top] = dEdge + rect.left;
			pY[j - rect.top] = j;	//+rect.top ������ǥ�� ������ǥ�� ��ȯ�ϱ� ���ػ��

			//TODO ��꿡 ���Ǵ� �ȼ� ǥ�� (�׽�Ʈ��)
			m_fm[int(pY[j - rect.top] * m_nPitch + pX[j - rect.top])] = 255;
		}

		double dErrorLimit = 1;
		int nRemoveN = rect.Height()*0.2;
		edge.LineFitting(rect.Height(), pX, pY, nRemoveN, dErrorLimit, t, a, b);	//input(x,y)���� ����� ������ ������



	}

	delete pY, pX, pData;
}

//�������� ����ã��
void Process::getEdgeBox(CRect rect, double *t, double *a, double *b)
{
	//�� �𼭸�(p1, p2, p3, p4) �� pixel��Ȯ���Ͽ� ����/�������� Ȯ�� (�ĺ��� �����۾�?)
	CPoint p1 = CPoint(rect.left, rect.top);				// p1 ��-------��p2
	CPoint p2 = CPoint(rect.right - 1, rect.top);			//    ��       ��
	CPoint p3 = CPoint(rect.left, rect.bottom - 1);			//    ��       ��
	CPoint p4 = CPoint(rect.right - 1, rect.bottom - 1);	// p3 ��-------��p4
	int nTh = 70;
	//pixel�� Ȯ��	(��ġȭ ���Ѽ� 0 �Ǵ� 255�� ����)
	int pixel1 = (m_fm[p1.y * m_nPitch + p1.x] < nTh) ? 0 : 255;
	int pixel2 = (m_fm[p2.y * m_nPitch + p2.x] < nTh) ? 0 : 255;
	int pixel3 = (m_fm[p3.y * m_nPitch + p3.x] < nTh) ? 0 : 255;
	int pixel4 = (m_fm[p4.y * m_nPitch + p4.x] < nTh) ? 0 : 255;

	//���������� ���������� �Ǵ� (getEdgeBox�� �ѱ涧 �ؼ� ���ص� ��)
	if (pixel1 == pixel2 && pixel3 == pixel4 && pixel1 != pixel3)
	{
		//������ W2B
		if (pixel1 == 255) {
			getEdge(rect, t, a, b, edgeType::HORIZONTAL);
		}//������ B2W
		else if (pixel1 == 0) {
			getEdge(rect, t, a, b, edgeType::HORIZONTAL | edgeType::CHANGE_B2W);
		}
	}
	if (pixel1 == pixel3 && pixel2 == pixel4 && pixel1 != pixel2)
	{
		//������ W2B
		if (pixel1 == 255) {
			getEdge(rect, t, a, b, edgeType::VERTICAL);
		}//������ B2W
		else if (pixel1 == 0) {
			getEdge(rect, t, a, b, edgeType::VERTICAL | edgeType::CHANGE_B2W);
		}
	}
}

