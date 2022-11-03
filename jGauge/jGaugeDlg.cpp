
// jGaugeDlg.cpp : 구현 파일
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


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CjGaugeDlg 대화 상자



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
	m_dsf = DEFAULT_SCALE_FACTOR;
	//m_process.init(&m_imgDisplay);
}

void CjGaugeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISPLAY, m_imgDisplay);
	DDX_Control(pDX, IDC_BTN_CAPTURE, m_btnCapture);
	DDX_Control(pDX, IDC_STATIC_INFO_PLATE_R, m_labelPlateR);
	DDX_Control(pDX, IDC_STATIC_INFO_FLAG_R, m_labelFlagR);
	DDX_Control(pDX, IDC_STATIC_INFO_FLAG_L, m_labelFlagL);
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


// CjGaugeDlg 메시지 처리기
BOOL CjGaugeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.


	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	this->CallCfg(FILE_LOAD);	//Cfg파일에서 로드
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	this->MoveWindow(0, 0, 1920, 1080);//다이얼로그 크기조절
	this->InitCam();//카메라 초기화
	m_logResult = new gLogger("Log_Result", "c:/glim/Result.log");
	m_isStopCam = false;


	//design 초기화
	m_labelPlateR.SetFontSize(20).SetTextColor(COLOR_RED);
	m_labelFlagL.SetFontSize(20).SetTextColor(COLOR_BLUE);
	m_labelFlagR.SetFontSize(20).SetTextColor(COLOR_GREEN);

	SetTimer(0, 100, NULL);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

//카메라를 초기화합니다.
void CjGaugeDlg::InitCam() {
	//출력용 디스플레이 설정
	m_imgDisplay.gCreate(CAM_WIDTH, CAM_HEIGHT, CAM_BPP);
	m_imgDisplay.gSetUseRoi(TRUE);	//ROI를 사용합니다.

	// TODO: 카메라 관련 테스트
	m_cam = new gCamDahua(CAM_NAME);
	m_cam->init(G_CAM_NONE);	// 해당캠 초기화

	//gLogger logger("ihj", "C:/Users/USER/Documents/Visual Studio 2015/Projects/ihjExam/Log/log.log", true, 1024 * 30000, 5);
	//캠 그랩
	bool bErrChk = m_cam->startGrabbing([this](unsigned char *imgPtr) {_callback(imgPtr); });
	//if (bErrChk == FALSE)
	//{
	//	logger.info("문제있음");
	//}
	//else
	//	logger.info("문제없음");

	//unsigned char** pImgData = m_cam->getPointer();	//grab된 프레임의 이미지 주소를 가져옵니다. 실패시 nullptr

	//if (pImgData == nullptr)
	//{
	//	logger.info("camera null error");
	//}
	//else
	//	logger.info("camera started");

	//logger.info("camera: {}, {}, {}", CAM_WIDTH, CAM_HEIGHT, CAM_BPP);
	//logger.end("카메라 연결까지 걸리는 시간 : ");
	
}
void CjGaugeDlg::_callback(unsigned char *imgPtr)
{
	//카메라를 초기화합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CjGaugeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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
		//BaseLine 엣지 그리기
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
		//PlateLine 엣지 그리기
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
		//BaseLine 엣지 그리기
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
		//PlateLine 엣지 그리기
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
	for (int i = PLATE_RIGHT; i <= FLAG_RIGHT; i++)
	{
		string strDistance = "";
		double dResult = GetMM(m_measuredInfo[i].distancePixel);
		strDistance.append(gString().format("{:0.3f}(mm)", dResult));
		//design 변경
		switch (i)
		{
			case PLATE_RIGHT:
				m_labelPlateR.SetText(strDistance);
				break;

			case FLAG_LEFT:
				m_labelFlagL.SetText(strDistance);
				break;

			case FLAG_RIGHT:
				m_labelFlagR.SetText(strDistance);
				break;
		}
	}
	//로그출력 logger
	//m_logResult->info("PLATE_RIGHT {}", GetMM(m_measuredInfo[PLATE_RIGHT].distancePixel));
	//mlogResult->info("FLAG_LEFT {}", GetMM(m_measuredInfo[FLAG_LEFT].distancePixel));
	//m_logResult->info("FLAG_RIGHT {}", GetMM(m_measuredInfo[FLAG_RIGHT].distancePixel));
}
double CjGaugeDlg::GetMM(int pixel)
{
	return pixel * m_dsf;
}

bool CjGaugeDlg::LineIsNull(Line line)
{
	if (line.t == 0 && line.a == 0 && line.b == 0)
		return true;
	else
		return false;
}


void CjGaugeDlg::OnBnClickedBtnCapture()
{
	ChangeCamState();
}

void CjGaugeDlg::ChangeCamState()
{
	if (m_isStopCam)
	{
		m_cam->startGrabbing([this](unsigned char *imgPtr) {_callback(imgPtr); });
		m_btnCapture.SetWindowTextW(CString("Grab"));
	}
	else {
		m_cam->stopGrabbing();
		m_btnCapture.SetWindowTextW(CString("Live"));
	}
	m_isStopCam = !m_isStopCam;
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

	if (m_rectRoi[1].Width() == 0)	return;
	Process process(&m_imgDisplay);
	//Plate-left Roi
	process.getEdge(m_rectRoi[PLATE_LEFT], &m_lineBase.t, &m_lineBase.a, &m_lineBase.b);
	//Plate-right Roi
	process.getEdge(m_rectRoi[PLATE_RIGHT], &m_linePlate.t, &m_linePlate.a, &m_linePlate.b);

	//PLATE_Right 까지의 거리 구하기
	m_measuredInfo[PLATE_RIGHT].y = m_rectRoi[PLATE_RIGHT].CenterPoint().y;
	m_measuredInfo[PLATE_RIGHT].x = (m_linePlate.t * m_measuredInfo[PLATE_RIGHT].y - m_linePlate.b) / m_linePlate.a;
	m_measuredInfo[PLATE_RIGHT].distancePixel = process.measureDistance(
		m_measuredInfo[PLATE_RIGHT].x, m_measuredInfo[PLATE_RIGHT].y, m_lineBase);


	//flag-left point
	m_measuredInfo[FLAG_LEFT].y = m_rectRoi[FLAG_LEFT].CenterPoint().y;
	m_measuredInfo[FLAG_LEFT].x = process.getEdgePoint(m_rectRoi[FLAG_LEFT]);	// x = (ty-b)/a
	m_measuredInfo[FLAG_LEFT].distancePixel = process.measureDistance(
		m_measuredInfo[FLAG_LEFT].x, m_measuredInfo[FLAG_LEFT].y, m_lineBase);

	//FLAG_Left 까지의 거리 구하기
	m_measuredInfo[FLAG_RIGHT].y = m_rectRoi[FLAG_RIGHT].CenterPoint().y;
	m_measuredInfo[FLAG_RIGHT].x = process.getEdgePoint(m_rectRoi[FLAG_RIGHT]);	// x = (ty-b)/a
	m_measuredInfo[FLAG_RIGHT].distancePixel = process.measureDistance(
		m_measuredInfo[FLAG_RIGHT].x, m_measuredInfo[FLAG_RIGHT].y, m_lineBase);

	m_imgDisplay.gDrawClear();
	DrawEdge();
	DrawRects();
	DrawInfomation();
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
	this->CallCfg(FILE_SAVE);
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
	/*info = "x: " + to_string(ptImg.x) + " y: " + to_string(ptImg.y) + " pixel: " + to_string(pixel);*/
	info = "x: "+to_string(GetMM(ptImg.x))+"mm (" + to_string(ptImg.x) + ")\n"+
		"y: "+ to_string(GetMM(ptImg.y))+"mm ("+to_string(ptImg.y) +")\n"+
		"pixel: " + to_string(pixel);
	//m_logger.info("{}", info);
	str = info.c_str();
	GetDlgItem(IDC_STATIC_MOUSE_INFO)->SetWindowTextW(str);


	CDialogEx::OnTimer(nIDEvent);
}


void CjGaugeDlg::OnBnClickedBtnImgsave()
{
	CString strDPath = _T("c:/glim/jGauge.bmp");
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.gif |모든파일(*.*)|*.*||");

	CFileDialog dlg(FALSE, _T("*.bmp"), _T("jGauge"), OFN_HIDEREADONLY, szFilter);
	dlg.m_ofn.lpstrInitialDir = strDPath;
	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		MessageBox(pathName);
		m_imgDisplay.gSave(pathName, true);
	}
}


void CjGaugeDlg::OnBnClickedBtnImgload()
{
	CString strDPath = _T("c:/glim/jGauge.bmp");
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.gif |모든파일(*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("*.bmp"), _T("jGauge"), OFN_HIDEREADONLY, szFilter);
	dlg.m_ofn.lpstrInitialDir = strDPath;
	if (IDOK == dlg.DoModal())
	{
		if (!m_isStopCam) 
			ChangeCamState();

		CString pathName = dlg.GetPathName();
		MessageBox(pathName);
		m_imgDisplay.gLoad(pathName);
	}
}

void CjGaugeDlg::CallCfg(int mode)
{
	CString path = _T("c:/glim/jGauge.ini");
	CString key = _T("c:/glim/jGauge.ini");

	gCfg cfg(path, key);

	//ScaleFactor
	cfg.SerGet(mode, m_dsf, _T("ScaleFactor"));
	if (m_dsf == 0)
	{
		m_dsf = DEFAULT_SCALE_FACTOR;
	}

	//rects 정보
	string strRects[4] = { "PLATE_LEFT", "PLATE_RIGHT", "FLAG_LEFT", "FLAG_RIGHT" };
	//rects 정보
	for (int i = 0; i < MAX_OBJECT; i++)
	{
		cfg.SerGet(mode, m_rectRoi[i], gString(strRects[i]).toCString());
	}
}