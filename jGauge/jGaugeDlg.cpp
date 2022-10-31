
// jGaugeDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "jGauge.h"
#include "jGaugeDlg.h"
#include "afxdialogex.h"

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
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CjGaugeDlg 대화 상자



CjGaugeDlg::CjGaugeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JGAUGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CjGaugeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISPLAY, m_imgDisplay);
	DDX_Control(pDX, IDC_INFO_SCALE, m_infoScale);
}

BEGIN_MESSAGE_MAP(CjGaugeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ROI1, &CjGaugeDlg::OnBnClickedBtnRoi1)
	ON_BN_CLICKED(IDC_BTN_ROI2, &CjGaugeDlg::OnBnClickedBtnRoi2)
	ON_BN_CLICKED(IDC_BTN_ROI3, &CjGaugeDlg::OnBnClickedBtnRoi3)
	ON_BN_CLICKED(IDC_BTN_ROI4, &CjGaugeDlg::OnBnClickedBtnRoi4)
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	this->InitConfig();
	this->MoveWindow(0, 0, 1920, 1080);//다이얼로그 크기조절
	this->InitCam();//카메라 초기화
	

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CjGaugeDlg::InitConfig() {
	this->roiRects = new CRect[4];
	for (int i = 0; i < ROI_ARR_SIZE; i++)
	{
		roiRects[i] = NULL;
	}

	//TODO 프로그램 종료시 메모리 삭제 코드 추가할 것
}
//카메라를 초기화합니다.
void CjGaugeDlg::InitCam() {
	//출력용 디스플레이 설정
	m_imgDisplay.gCreate(CAM_WIDTH, CAM_HEIGHT, CAM_BPP);
	m_imgDisplay.gSetUseRoi(TRUE);	//ROI를 사용합니다.

	// TODO: 카메라 관련 테스트
	gCamDahua *dahua_cam1 = new gCamDahua(CAM_NAME);
	dahua_cam1->init(G_CAM_NONE);	// 해당캠 초기화

	gLogger logger("ihj", "C:/Users/USER/Documents/Visual Studio 2015/Projects/ihjExam/Log/log.log", true, 1024 * 30000, 5);
	//캠 그랩
	bool bErrChk = dahua_cam1->startGrabbing([this](unsigned char *imgPtr) {_callback(imgPtr); });
	if (bErrChk == FALSE)
	{
		logger.info("문제있음");
	}
	else
		logger.info("문제없음");

	unsigned char** pImgData = dahua_cam1->getPointer();	//grab된 프레임의 이미지 주소를 가져옵니다. 실패시 nullptr

	if (pImgData == nullptr)
	{
		logger.info("camera null error");
	}
	else
		logger.info("camera started");

	logger.info("camera: {}, {}, {}", CAM_WIDTH, CAM_HEIGHT, CAM_BPP);
	logger.end("카메라 연결까지 걸리는 시간 : ");
	
	gString strScale= gString(CString("Scale : "))+gString(CAM_WIDTH) +gString(CString(" x "))+gString(CAM_HEIGHT);
	m_infoScale.SetWindowTextW(strScale.toCString());
}
void CjGaugeDlg::_callback(unsigned char *imgPtr)
{
	//카메라를 초기화합니다.
	m_imgDisplay.gSetImage(imgPtr, CAM_WIDTH, CAM_HEIGHT, CAM_BPP);



	//디스플레이 Overlay .
	for (int i = 0; i < ROI_ARR_SIZE; i++)
	{
		if (roiRects[i] != NULL) {
			if (i == ROI_ONE || i == ROI_TWO)
				m_imgDisplay.gDrawRect(roiRects[i], COLOR_RED);
			else
				m_imgDisplay.gDrawRect(roiRects[i], COLOR_BLUE);
		}
	}
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

void CjGaugeDlg::OnBnClickedBtnRoi1()
{
	CRect roiRect = m_imgDisplay.gGetRoi();
	this->roiRects[ROI_ONE] = roiRect;
	m_imgDisplay.gDrawRect(roiRect);
}

void CjGaugeDlg::OnBnClickedBtnRoi2()
{
	CRect roiRect = m_imgDisplay.gGetRoi();
	this->roiRects[ROI_TWO] = roiRect;
}


void CjGaugeDlg::OnBnClickedBtnRoi3()
{
	CRect roiRect = m_imgDisplay.gGetRoi();
	this->roiRects[ROI_THREE] = roiRect;
}


void CjGaugeDlg::OnBnClickedBtnRoi4()
{
	CRect roiRect = m_imgDisplay.gGetRoi();
	this->roiRects[ROI_FOUR] = roiRect;
}
