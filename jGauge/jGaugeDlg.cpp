
// jGaugeDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "jGauge.h"
#include "jGaugeDlg.h"
#include "afxdialogex.h"
#include "Process.h"
#include "Label.h"
#include "gLogger.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CjGaugeDlg ��ȭ ����



CjGaugeDlg::CjGaugeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JGAUGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_lineBase.t = 0;
	m_lineBase.a = 0;
	m_lineBase.b = 0;
	m_linePlate.t = 0;
	m_linePlate.a = 0;
	m_linePlate.b = 0;
	m_dsf = 0.032822757111;
	//m_process.init(&m_imgDisplay);
}

void CjGaugeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISPLAY, m_imgDisplay);
	//  DDX_Control(pDX, IDC_INFO_SCALE, m_infoScale);
	DDX_Control(pDX, IDC_BTN_CAPTURE, m_btnCapture);
	DDX_Control(pDX, IDC_STATIC_INFO, m_labelInfo);
	DDX_Control(pDX, IDC_CHECK_SIMPLE, m_checkSimple);
}

BEGIN_MESSAGE_MAP(CjGaugeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ROI1, &CjGaugeDlg::OnBnClickedBtnRoi1)
	ON_BN_CLICKED(IDC_BTN_ROI2, &CjGaugeDlg::OnBnClickedBtnRoi2)
	ON_BN_CLICKED(IDC_BTN_ROI3, &CjGaugeDlg::OnBnClickedBtnRoi3)
	ON_BN_CLICKED(IDC_BTN_ROI4, &CjGaugeDlg::OnBnClickedBtnRoi4)
	ON_BN_CLICKED(IDC_BTN_MEASURE, &CjGaugeDlg::OnBnClickedBtnMeasure)
	ON_BN_CLICKED(IDC_BTN_CAPTURE, &CjGaugeDlg::OnBnClickedBtnCapture)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_IMGSAVE, &CjGaugeDlg::OnBnClickedBtnImgsave)
	ON_BN_CLICKED(IDC_BTN_IMGLOAD, &CjGaugeDlg::OnBnClickedBtnImgload)
END_MESSAGE_MAP()


// CjGaugeDlg �޽��� ó����
BOOL CjGaugeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.


	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	loadCfg();	//Cfg���Ͽ��� �ε�
	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	this->MoveWindow(0, 0, 1920, 1080);//���̾�α� ũ������
	this->InitCam();//ī�޶� �ʱ�ȭ
	m_logResult = new gLogger("Log_Result", "c:/glim/Result.log");

	SetTimer(0, 100, NULL);
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

//ī�޶� �ʱ�ȭ�մϴ�.
void CjGaugeDlg::InitCam() {
	//��¿� ���÷��� ����
	m_imgDisplay.gCreate(CAM_WIDTH, CAM_HEIGHT, CAM_BPP);
	m_imgDisplay.gSetUseRoi(TRUE);	//ROI�� ����մϴ�.

	// TODO: ī�޶� ���� �׽�Ʈ
	m_cam = new gCamDahua(CAM_NAME);
	m_cam->init(G_CAM_NONE);	// �ش�ķ �ʱ�ȭ

	//gLogger logger("ihj", "C:/Users/USER/Documents/Visual Studio 2015/Projects/ihjExam/Log/log.log", true, 1024 * 30000, 5);
	//ķ �׷�
	bool bErrChk = m_cam->startGrabbing([this](unsigned char *imgPtr) {_callback(imgPtr); });
	//if (bErrChk == FALSE)
	//{
	//	logger.info("��������");
	//}
	//else
	//	logger.info("��������");

	//unsigned char** pImgData = m_cam->getPointer();	//grab�� �������� �̹��� �ּҸ� �����ɴϴ�. ���н� nullptr

	//if (pImgData == nullptr)
	//{
	//	logger.info("camera null error");
	//}
	//else
	//	logger.info("camera started");

	//logger.info("camera: {}, {}, {}", CAM_WIDTH, CAM_HEIGHT, CAM_BPP);
	//logger.end("ī�޶� ������� �ɸ��� �ð� : ");
	
}
void CjGaugeDlg::_callback(unsigned char *imgPtr)
{
	//ī�޶� �ʱ�ȭ�մϴ�.
	m_imgDisplay.gSetImage(imgPtr, CAM_WIDTH, CAM_HEIGHT, CAM_BPP);

	OnBnClickedBtnMeasure();

}

void CjGaugeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CjGaugeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CjGaugeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CjGaugeDlg::DrawRects() {
	for (int i = 0; i < MAX_OBJECT; i++)
	{
		if (m_rectRoi[i] != NULL) {
			if (i == PLATE_LEFT || i == PLATE_RIGHT)
				m_imgDisplay.gDrawRect(m_rectRoi[i], COLOR_RED);
			else if (i == FLAG_LEFT)
				m_imgDisplay.gDrawRect(m_rectRoi[i], COLOR_BLUE);
			else if (i == FLAG_RIGHT)
				m_imgDisplay.gDrawRect(m_rectRoi[i], COLOR_GREEN);;
		}
	}
}
void CjGaugeDlg::DrawEdge(bool overRect)
{
	double t, a, b;
	CPoint p1, p2;

	if (overRect)
	{
		//BaseLine ���� �׸���
		if (!LineIsNull(m_lineBase))
		{
			t = m_lineBase.t;
			a = m_lineBase.a;
			b = m_lineBase.b;
			p1.y = 0;
			p2.y = m_imgDisplay.gGetHeight();
			p1.x = (t*p1.y - b) / a;
			p2.x = (t*p2.y - b) / a;
			m_imgDisplay.gDrawLine(p1, p2);
		}
		//PlateLine ���� �׸���
		if (!LineIsNull(m_linePlate))
		{
			t = m_linePlate.t;
			a = m_linePlate.a;
			b = m_linePlate.b;
			p1.y = 0;
			p2.y = m_imgDisplay.gGetHeight();
			p1.x = (t*p1.y - b) / a;
			p2.x = (t*p2.y - b) / a;
			m_imgDisplay.gDrawLine(p1, p2);
		}
	}else {
		//BaseLine ���� �׸���
		if (!LineIsNull(m_lineBase))
		{
			t = m_lineBase.t;
			a = m_lineBase.a;
			b = m_lineBase.b;
			p1.y = m_rectRoi[0].top;
			p2.y = m_rectRoi[0].bottom;
			p1.x = (t*p1.y - b) / a;
			p2.x = (t*p2.y - b) / a;
			m_imgDisplay.gDrawLine(p1, p2);
		}
		//PlateLine ���� �׸���
		if (!LineIsNull(m_linePlate))
		{
			t = m_linePlate.t;
			a = m_linePlate.a;
			b = m_linePlate.b;
			p1.y = m_rectRoi[1].top;
			p2.y = m_rectRoi[1].bottom;
			p1.x = (t*p1.y - b) / a;
			p2.x = (t*p2.y - b) / a;
			m_imgDisplay.gDrawLine(p1, p2);
		}
	}
}


void CjGaugeDlg::DrawDiffPixels() {
	m_imgDisplay.gDrawLine(CPoint(m_measuredInfo[PLATE_RIGHT].x, m_measuredInfo[PLATE_RIGHT].y),
		CPoint(m_measuredInfo[PLATE_RIGHT].x- m_measuredInfo[PLATE_RIGHT].distancePixel, m_measuredInfo[PLATE_RIGHT].y), COLOR_RED, 2);


	m_imgDisplay.gDrawLine(CPoint(m_measuredInfo[FLAG_LEFT].x, m_measuredInfo[FLAG_LEFT].y),
		CPoint(m_measuredInfo[FLAG_LEFT].x - m_measuredInfo[FLAG_LEFT].distancePixel, m_measuredInfo[FLAG_LEFT].y), COLOR_BLUE, 2);


	m_imgDisplay.gDrawLine(CPoint(m_measuredInfo[FLAG_RIGHT].x, m_measuredInfo[FLAG_RIGHT].y),
		CPoint(m_measuredInfo[FLAG_RIGHT].x - m_measuredInfo[FLAG_RIGHT].distancePixel, m_measuredInfo[FLAG_RIGHT].y), COLOR_GREEN, 2);
}

void CjGaugeDlg::DrawInfomation() {
	string strDistance = "";
	for (int i = PLATE_RIGHT; i <= FLAG_RIGHT; i++)
	{
		strDistance.append(getStringDistance(i));
		strDistance.append(" (mm)\n\n");
	}

	//design ����
	m_labelInfo.SetText(strDistance);
	m_labelInfo.SetFontSize(20);
	//�α����
	m_logResult->info("{}", strDistance);
}

gString CjGaugeDlg::getStringDistance(int i) {
	double dResult = m_measuredInfo[i].distancePixel*m_dsf;
	if (m_checkSimple.GetCheck())
	{
		return gString().format("{:0.2f}", dResult);
	}
	else
	{
		return gString(dResult);
	}
}

bool CjGaugeDlg::LineIsNull(Line line)
{
	if (line.t == 0 && line.a == 0 && line.b == 0)
		return true;
	else
		return false;
}

bool isStopCam = false;
void CjGaugeDlg::OnBnClickedBtnCapture()
{
	if (isStopCam)
	{
		m_cam->startGrabbing([this](unsigned char *imgPtr) {_callback(imgPtr); });
		
		m_btnCapture.SetWindowTextW(CString("Grab"));
	}
	else {
		m_cam->stopGrabbing();
		m_btnCapture.SetWindowTextW(CString("Live"));
	}
	isStopCam = !isStopCam;
}


void CjGaugeDlg::OnBnClickedBtnRoi1()
{
	m_rectRoi[PLATE_LEFT] = m_imgDisplay.gGetRoi();
}

void CjGaugeDlg::OnBnClickedBtnRoi2()
{
	m_rectRoi[PLATE_RIGHT] = m_imgDisplay.gGetRoi();
}

void CjGaugeDlg::OnBnClickedBtnRoi3()
{
	m_rectRoi[FLAG_LEFT] = m_imgDisplay.gGetRoi();
}

void CjGaugeDlg::OnBnClickedBtnRoi4()
{
	m_rectRoi[FLAG_RIGHT] = m_imgDisplay.gGetRoi();
}

void CjGaugeDlg::OnBnClickedBtnMeasure()
{
	//testFunc();
	//return;

	//for(int k=0; k)

	if (m_rectRoi[1].Width() == 0)	return;
	Process process(&m_imgDisplay);
	//Plate-left Roi
	process.getEdge(m_rectRoi[PLATE_LEFT], &m_lineBase.t, &m_lineBase.a, &m_lineBase.b);
	//Plate-right Roi
	process.getEdge(m_rectRoi[PLATE_RIGHT], &m_linePlate.t, &m_linePlate.a, &m_linePlate.b);

	//PLATE_Right ������ �Ÿ� ���ϱ�
	m_measuredInfo[PLATE_RIGHT].y = m_rectRoi[PLATE_RIGHT].CenterPoint().y;
	m_measuredInfo[PLATE_RIGHT].x = (m_linePlate.t * m_measuredInfo[PLATE_RIGHT].y - m_linePlate.b) / m_linePlate.a;
	m_measuredInfo[PLATE_RIGHT].distancePixel = process.measureDistance(
		m_measuredInfo[PLATE_RIGHT].x, m_measuredInfo[PLATE_RIGHT].y, m_lineBase);


	//flag-left point
	m_measuredInfo[FLAG_LEFT].y = m_rectRoi[FLAG_LEFT].CenterPoint().y;
	m_measuredInfo[FLAG_LEFT].x = process.getEdgePoint(m_rectRoi[FLAG_LEFT]);	// x = (ty-b)/a
	m_measuredInfo[FLAG_LEFT].distancePixel = process.measureDistance(
		m_measuredInfo[FLAG_LEFT].x, m_measuredInfo[FLAG_LEFT].y, m_lineBase);

	//FLAG_Left ������ �Ÿ� ���ϱ�
	m_measuredInfo[FLAG_RIGHT].y = m_rectRoi[FLAG_RIGHT].CenterPoint().y;
	m_measuredInfo[FLAG_RIGHT].x = process.getEdgePoint(m_rectRoi[FLAG_RIGHT]);	// x = (ty-b)/a
	m_measuredInfo[FLAG_RIGHT].distancePixel = process.measureDistance(
		m_measuredInfo[FLAG_RIGHT].x, m_measuredInfo[FLAG_RIGHT].y, m_lineBase);

	DrawInfomation();
	DrawEdge();
	DrawRects();
	m_imgDisplay.UpdateDisplay();

}

#include "gEdge.h"
void CjGaugeDlg::testFunc()
{
	int nWidth = m_imgDisplay.gGetWidth();
	int nHeight = m_imgDisplay.gGetHeight();
	int nPitch = m_imgDisplay.gGetPitch();
	unsigned char* fm = m_imgDisplay.gGetImgPtr();

	CRect rect = m_imgDisplay.gGetRoi();

	int* pProj= new int[rect.Width()]{ 0, };
	for (int i = rect.left; i < rect.right; i++) {
		for (int j = rect.top; j < rect.bottom; j++) {
			pProj[i - rect.left] += fm[j*nPitch + i];
		}
	}

	double dOff = 43.5;
	double dEdge = 0;
	int nSlope = 0;
	gEdge edge;
	gEdge::EdgeType opt= gEdge::EdgeType::W2B;
	for (int i = 0; i < 50; i ++) {
		int k = int(i*dOff);
		edge.LineFindEdge(opt, int(dOff), pProj + k, &dEdge, &nSlope);
		cout << dEdge + k << endl;
	}



	delete pProj;
}

void CjGaugeDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	this->saveCfg();
	delete m_logResult;
	delete m_cam;
}


void CjGaugeDlg::OnTimer(UINT_PTR nIDEvent)
{
	int pixel;
	CPoint ptImg, ptMouse;
	string info;
	CString str;
	pixel = m_imgDisplay.gGetPixelInfo(ptMouse, ptImg);
	info = "x: " + to_string(ptImg.x) + " y: " + to_string(ptImg.y) + " pixel: " + to_string(pixel);
	//m_logger.info("{}", info);
	str = info.c_str();
	GetDlgItem(IDC_STATIC_MOUSE_INFO)->SetWindowTextW(str);


	CDialogEx::OnTimer(nIDEvent);
}


void CjGaugeDlg::OnBnClickedBtnImgsave()
{
	m_imgDisplay.gSave("c:/glim/jGauge.bmp", true);
}


void CjGaugeDlg::OnBnClickedBtnImgload()
{
	m_imgDisplay.gLoad("c:/glim/jGauge.bmp");
}

void CjGaugeDlg::saveCfg()
{
	CString path = _T("c:/glim/jGauge.ini");
	CString key = _T("c:/glim/jGauge.ini");

	gCfg cfg(path, key);

	//ScaleFactor
	cfg.SerGet(0, m_dsf, _T("ScaleFactor"));

	//rects ����
	string strRects[4] = { "PLATE_LEFT", "PLATE_RIGHT", "FLAG_LEFT", "FLAG_RIGHT" };
	//rects ����
	for (int i = 0; i < MAX_OBJECT; i++)
	{
		cfg.SerGet(0, m_rectRoi[i], gString(strRects[i]).toCString());
	}
}

void CjGaugeDlg::loadCfg()
{
	CString path = _T("c:/glim/jGauge.ini");
	CString key = _T("c:/glim/jGauge.ini");

	gCfg cfg(path, key);

	//ScaleFactor
	cfg.SerGet(1, m_dsf, _T("ScaleFactor"));

	string strRects[4] = { "PLATE_LEFT", "PLATE_RIGHT", "FLAG_LEFT", "FLAG_RIGHT" };
	//rects ����
	for (int i = 0; i < MAX_OBJECT; i++)
	{
		cfg.SerGet(1, m_rectRoi[i], gString(strRects[i]).toCString());
	}
}



