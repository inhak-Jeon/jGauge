
// jGaugeDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "jGauge.h"
#include "jGaugeDlg.h"
#include "afxdialogex.h"

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
	this->InitConfig();
	this->MoveWindow(0, 0, 1920, 1080);//���̾�α� ũ������
	this->InitCam();//ī�޶� �ʱ�ȭ
	

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CjGaugeDlg::InitConfig() {
	this->roiRects = new CRect[4];
	for (int i = 0; i < ROI_ARR_SIZE; i++)
	{
		roiRects[i] = NULL;
	}

	//TODO ���α׷� ����� �޸� ���� �ڵ� �߰��� ��
}
//ī�޶� �ʱ�ȭ�մϴ�.
void CjGaugeDlg::InitCam() {
	//��¿� ���÷��� ����
	m_imgDisplay.gCreate(CAM_WIDTH, CAM_HEIGHT, CAM_BPP);
	m_imgDisplay.gSetUseRoi(TRUE);	//ROI�� ����մϴ�.

	// TODO: ī�޶� ���� �׽�Ʈ
	gCamDahua *dahua_cam1 = new gCamDahua(CAM_NAME);
	dahua_cam1->init(G_CAM_NONE);	// �ش�ķ �ʱ�ȭ

	gLogger logger("ihj", "C:/Users/USER/Documents/Visual Studio 2015/Projects/ihjExam/Log/log.log", true, 1024 * 30000, 5);
	//ķ �׷�
	bool bErrChk = dahua_cam1->startGrabbing([this](unsigned char *imgPtr) {_callback(imgPtr); });
	if (bErrChk == FALSE)
	{
		logger.info("��������");
	}
	else
		logger.info("��������");

	unsigned char** pImgData = dahua_cam1->getPointer();	//grab�� �������� �̹��� �ּҸ� �����ɴϴ�. ���н� nullptr

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



	//���÷��� Overlay .
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
