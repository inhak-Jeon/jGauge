
// jGaugeDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "jGauge.h"
#include "jGaugeDlg.h"
#include "afxdialogex.h"
#include "Process.h"
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


// CjGaugeDlg ��ȭ ����



CjGaugeDlg::CjGaugeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JGAUGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_lineBase.t = 0;
	m_lineBase.a = 0;
	m_lineBase.b = 0;
	m_LinePlate.t = 0;
	m_LinePlate.a = 0;
	m_LinePlate.b = 0;
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
	ON_BN_CLICKED(IDC_BTN_MEASURE, &CjGaugeDlg::OnBnClickedBtnMeasure)
	ON_BN_CLICKED(IDC_BTN_CAPTURE, &CjGaugeDlg::OnBnClickedBtnCapture)
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

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	this->MoveWindow(0, 0, 1920, 1080);//���̾�α� ũ������
	this->InitCam();//ī�޶� �ʱ�ȭ
	

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

	gLogger logger("ihj", "C:/Users/USER/Documents/Visual Studio 2015/Projects/ihjExam/Log/log.log", true, 1024 * 30000, 5);
	//ķ �׷�
	bool bErrChk = m_cam->startGrabbing([this](unsigned char *imgPtr) {_callback(imgPtr); });
	if (bErrChk == FALSE)
	{
		logger.info("��������");
	}
	else
		logger.info("��������");

	unsigned char** pImgData = m_cam->getPointer();	//grab�� �������� �̹��� �ּҸ� �����ɴϴ�. ���н� nullptr

	if (pImgData == nullptr)
	{
		logger.info("camera null error");
	}
	else
		logger.info("camera started");

	logger.info("camera: {}, {}, {}", CAM_WIDTH, CAM_HEIGHT, CAM_BPP);
	logger.end("ī�޶� ������� �ɸ��� �ð� : ");
	
	gString strScale= gString(CString("Scale : "))+gString(CAM_WIDTH) +gString(CString(" x "))+gString(CAM_HEIGHT);
	m_infoScale.SetWindowTextW(strScale.toCString());
}
void CjGaugeDlg::_callback(unsigned char *imgPtr)
{
	//ī�޶� �ʱ�ȭ�մϴ�.
	m_imgDisplay.gSetImage(imgPtr, CAM_WIDTH, CAM_HEIGHT, CAM_BPP);

	Process	process(&m_imgDisplay);
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

	m_imgDisplay.gDrawClear();
	//rectRoi �׸���
	for (int i = 0; i < MAX_OBJECT; i++)
	{
		if (m_rectRoi[i] != NULL) {
			if (i == 0 || i == 1)
				m_imgDisplay.gDrawRect(m_rectRoi[i], COLOR_RED);
			else
				m_imgDisplay.gDrawRect(m_rectRoi[i], COLOR_BLUE);
		}
	}
	//���� �׸���
	DrawEdge();
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CjGaugeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CjGaugeDlg::DrawEdge()
{
	//BaseLine ������ �׸���
	double t = m_lineBase.t;
	double a = m_lineBase.a;
	double b = m_lineBase.b;
	if (t == 0 && a == 0 && b == 0)
		return;
	CPoint p1, p2;
	p1.x = 0;
	p2.x = m_imgDisplay.gGetWidth();
	p1.y = (a*p1.x + b) / t;
	p2.y = (a*p2.x + b) / t;
	m_imgDisplay.gDrawLine(p1, p2);

	t = m_LinePlate.t;
	a = m_LinePlate.a;
	b = m_LinePlate.b;
	if (t == 0 && a == 0 && b == 0)
		return;
	p1, p2;
	p1.x = 0;
	p2.x = m_imgDisplay.gGetWidth();
	p1.y = (a*p1.x + b) / t;
	p2.y = (a*p2.x + b) / t;
	m_imgDisplay.gDrawLine(p1, p2);
}


void CjGaugeDlg::OnBnClickedBtnCapture()
{
	m_cam->stopGrabbing();
}


void CjGaugeDlg::OnBnClickedBtnRoi1()
{
	m_rectRoi[PLATE_LEFT] = m_imgDisplay.gGetRoi();

	Process process(&m_imgDisplay);
	process.getEdge(m_rectRoi[PLATE_LEFT], &m_lineBase.t, &m_lineBase.a, &m_lineBase.b);

	OnPaint();
}

void CjGaugeDlg::OnBnClickedBtnRoi2()
{
	m_rectRoi[PLATE_RIGHT] = m_imgDisplay.gGetRoi();

	Process process(&m_imgDisplay);
	process.getEdge(m_rectRoi[PLATE_RIGHT], &m_LinePlate.t, &m_LinePlate.a, &m_LinePlate.b);

	OnPaint();
}


void CjGaugeDlg::OnBnClickedBtnRoi3()
{
	m_rectRoi[FLAG_LEFT] = m_imgDisplay.gGetRoi();
	OnPaint();
}


void CjGaugeDlg::OnBnClickedBtnRoi4()
{
	m_rectRoi[FLAG_RIGHT] = m_imgDisplay.gGetRoi();
	OnPaint();
}


void CjGaugeDlg::OnBnClickedBtnMeasure()
{
	//PLATE_Right ������ �Ÿ� ���ϱ�
	int PlatePointY = m_rectRoi[PLATE_RIGHT].CenterPoint().y;
	int PlatePointX = (m_LinePlate.t * PlatePointY - m_LinePlate.b) / m_LinePlate.a;	// x = (ty-b)/a
																							
	//���� ���������� �Ÿ�  d = |ax0+by0+b| / root(a^2+b^2)
	int x = PlatePointX;
	int y = PlatePointY;
	double d = abs(m_lineBase.a * x + (-m_lineBase.t)*y + m_lineBase.b) / sqrt(pow(m_lineBase.a, 2) + pow(m_lineBase.t, 2));

	std::cout << "d�� : " << d<<std::endl;

	//TODO �׽�Ʈ�� �׸���
	m_imgDisplay.gDrawLine(CPoint(PlatePointX, PlatePointY), CPoint(PlatePointX, PlatePointY), COLOR_RED, 5);	
	m_imgDisplay.gDrawLine(CPoint(PlatePointX-d, PlatePointY), CPoint(PlatePointX, PlatePointY), COLOR_RED, 2);
}

