
// jGaugeDlg.h : ��� ����
//

#pragma once
#include "gImage.h"
#include "gCamDahua.h"
#include "afxwin.h"
#include "gString.h"
#pragma comment(lib, "/gImage.lib")  
#pragma comment(lib, "/gLogger.lib")
#pragma comment(lib, "/gCam.lib")

#define CAM_WIDTH 2448
#define CAM_HEIGHT 2048
#define CAM_BPP 8
#define CAM_NAME "Cam6LO"
#define ROI_ARR_SIZE 4
#define ROI_ONE_L 0
#define ROI_ONE_R 1
#define ROI_TWO_L 2
#define ROI_TWO_R 3

// CjGaugeDlg ��ȭ ����
class CjGaugeDlg : public CDialogEx
{
private:
	CRect *roiRects;
	void InitCam();
	void _callback(unsigned char *imgPtr);

// �����Դϴ�.
public:
	CjGaugeDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JGAUGE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	void InitConfig();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	gImage m_imgDisplay;
	CStatic m_infoScale;
	afx_msg void OnBnClickedBtnRoi1l();
	afx_msg void OnBnClickedBtnRoi1r();
	afx_msg void OnBnClickedBtnRoi2l();
	afx_msg void OnBnClickedBtnRoi2r();
};
