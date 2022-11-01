
// jGaugeDlg.h : 헤더 파일
//

#pragma once
#include "gImage.h"
#include "gCamDahua.h"
#include "afxwin.h"
#include "gString.h"
#include "Line.h"
#pragma comment(lib, "/gImage.lib")  
#pragma comment(lib, "/gLogger.lib")
#pragma comment(lib, "/gCam.lib")
#pragma comment(lib, "/gEdge.lib")

#define CAM_WIDTH 2448
#define CAM_HEIGHT 2048
#define CAM_BPP 8
#define CAM_NAME "Cam6LO"

#define MAX_OBJECT	4
enum {
	PLATE_LEFT,
	PLATE_RIGHT,
	FLAG_LEFT,
	FLAG_RIGHT
};
// CjGaugeDlg 대화 상자

class CjGaugeDlg : public CDialogEx
{
private:
	CRect m_rectRoi[MAX_OBJECT];
	void InitCam();
	void _callback(unsigned char *imgPtr);

// 생성입니다.
public:
	Line m_lineBase;
	Line m_LinePlate;
	CjGaugeDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JGAUGE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	gCamDahua *m_cam;
	gImage m_imgDisplay;
	CStatic m_infoScale;
	afx_msg void OnBnClickedBtnRoi1();
	void DrawEdge();
	afx_msg void OnBnClickedBtnRoi2();
	afx_msg void OnBnClickedBtnRoi3();
	afx_msg void OnBnClickedBtnRoi4();
	afx_msg void OnBnClickedBtnMeasure();
	afx_msg void OnBnClickedBtnCapture();
};
