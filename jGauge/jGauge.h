
// jGauge.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CjGaugeApp:
// �� Ŭ������ ������ ���ؼ��� jGauge.cpp�� �����Ͻʽÿ�.
//

class CjGaugeApp : public CWinApp
{
public:
	CjGaugeApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CjGaugeApp theApp;