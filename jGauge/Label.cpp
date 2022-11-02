// Label.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "Label.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	FLASH_TIMER_ID		0

string x;

/////////////////////////////////////////////////////////////////////////////
// CLabel

CLabel::CLabel()
{
	m_crText = GetSysColor(COLOR_WINDOWTEXT);
	m_crBkgnd = GetSysColor(COLOR_3DFACE);
	m_hBrush = ::CreateSolidBrush(m_crBkgnd);

	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT),sizeof(m_lf),&m_lf);

	m_font.CreateFontIndirect(&m_lf);
	m_bLink = TRUE;
	m_hCursor = NULL;

	m_hwndBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));
}


CLabel::~CLabel()
{
	m_font.DeleteObject();
	::DeleteObject(m_hBrush);
}

CLabel& CLabel::SetText(const string& strText)
{
	SetWindowText(CString(strText.c_str()));
	return *this;
}

CLabel& CLabel::SetText(const CString& strText)
{
	SetWindowText(strText);
	return *this;
}

CLabel& CLabel::SetTextColor(COLORREF crText)
{
	m_crText = crText;
	RedrawWindow();
	return *this;
}

CLabel& CLabel::SetFontBold(BOOL bBold)
{	
	m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont();
	RedrawWindow();
	return *this;
}

CLabel& CLabel::SetFontUnderline(BOOL bSet)
{	
	m_lf.lfUnderline = bSet;
	ReconstructFont();
	RedrawWindow();
	return *this;
}

CLabel& CLabel::SetFontItalic(BOOL bSet)
{
	m_lf.lfItalic = bSet;
	ReconstructFont();
	RedrawWindow();
	return *this;	
}

CLabel& CLabel::SetSunken(BOOL bSet)
{
	if (!bSet)
		ModifyStyleEx(WS_EX_STATICEDGE,0,SWP_DRAWFRAME);
	else
		ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_DRAWFRAME);
		
	return *this;	
}

CLabel& CLabel::SetBorder(BOOL bSet)
{
	if (!bSet)
		ModifyStyle(WS_BORDER,0,SWP_DRAWFRAME);
	else
		ModifyStyle(0,WS_BORDER,SWP_DRAWFRAME);
		
	return *this;	
}

CLabel& CLabel::SetFontSize(int nSize)
{
	nSize*=-1;
	m_lf.lfHeight = nSize;
	ReconstructFont();
	RedrawWindow();
	return *this;
}


CLabel& CLabel::SetBkColor(COLORREF crBkgnd)
{
	if (m_hBrush)
		::DeleteObject(m_hBrush);
	
	m_crBkgnd = crBkgnd;
	m_hBrush = ::CreateSolidBrush(m_crBkgnd);
	return *this;
}

CLabel& CLabel::SetFontName(const string& strFont, int charset)
{
	return SetFontName(CString(strFont.c_str()), charset);
}

CLabel& CLabel::SetFontName(const CString& strFont, int charset)
{	
	wcscpy_s(m_lf.lfFaceName, strFont);
	//strcpy(m_lf.lfFaceName,(LPCWSTR)strFont);
	m_lf.lfCharSet = charset;
	ReconstructFont();
	RedrawWindow();
	return *this;
}


BEGIN_MESSAGE_MAP(CLabel, CStatic)
	//{{AFX_MSG_MAP(CLabel)
	ON_WM_CTLCOLOR_REFLECT()
	//ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabel message handlers

HBRUSH CLabel::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SelectObject(&m_font);
		pDC->SetTextColor(m_crText);
		pDC->SetBkMode(TRANSPARENT);
	}

	return m_hBrush;
}

void CLabel::ReconstructFont()
{
	m_font.DeleteObject();
	BOOL bCreated = m_font.CreateFontIndirect(&m_lf);

	ASSERT(bCreated);
}


CLabel& CLabel::FlashText(BOOL bActivate, COLORREF crFlash, int intv) {
	if (bActivate) {	// 시작
		// 기존에 있으면 일단 종료
		switch (m_flashStat.m_type) {
		case FlashType::Text:
			FlashText(FALSE);
			break;
		case FlashType::Background:
			FlashBackground(FALSE);
			break;
		}
		// set
		m_flashStat.Set(FlashType::Text, m_crText, crFlash, COLOR_NONE);
		SetTimer(FLASH_TIMER_ID, intv, NULL);
	}
	else if (m_flashStat.m_type==FlashType::Text) {			// 종료
		KillTimer(FLASH_TIMER_ID);
		SetTextColor(m_flashStat.m_crCurrent);
		m_flashStat.Clear();
	}
	return *this;
}

CLabel& CLabel::FlashText(BOOL bActivate, COLORREF crFlash1, COLORREF crFlash2, int intv) {
	if (bActivate) {	// 시작
		// 기존에 있으면 일단 종료
		switch (m_flashStat.m_type) {
		case FlashType::Text:
			FlashText(FALSE);
			break;
		case FlashType::Background:
			FlashBackground(FALSE);
			break;
		}
		// set
		m_flashStat.Set(FlashType::Text, m_crText, crFlash1, crFlash2);
		SetTimer(FLASH_TIMER_ID, intv, NULL);
	}
	else if (m_flashStat.m_type == FlashType::Text) {			// 종료
		KillTimer(FLASH_TIMER_ID);
		SetTextColor(m_flashStat.m_crCurrent);
		m_flashStat.Clear();
	}
	return *this;
}

CLabel& CLabel::FlashBackground(BOOL bActivate, COLORREF crFlash, int intv) {
	if (bActivate) {	// 시작
		// 기존에 있으면 일단 종료
		switch (m_flashStat.m_type) {
		case FlashType::Text:
			FlashText(FALSE);
			break;
		case FlashType::Background:
			FlashBackground(FALSE);
			break;
		}
		// set
		m_flashStat.Set(FlashType::Background, m_crBkgnd, crFlash, COLOR_NONE);
		SetTimer(FLASH_TIMER_ID, intv, NULL);
	}
	else if (m_flashStat.m_type == FlashType::Background) {			// 종료
		KillTimer(FLASH_TIMER_ID);
		SetBkColor(m_flashStat.m_crCurrent);
		m_flashStat.Clear();
	}
	return *this;
}

CLabel& CLabel::FlashBackground(BOOL bActivate, COLORREF crFlash1, COLORREF crFlash2, int intv) {
	if (bActivate) {	// 시작
		// 기존에 있으면 일단 종료
		switch (m_flashStat.m_type) {
		case FlashType::Text:
			FlashText(FALSE);
			break;
		case FlashType::Background:
			FlashBackground(FALSE);
			break;
		}
		// set
		m_flashStat.Set(FlashType::Background, m_crBkgnd, crFlash1, crFlash2);
		SetTimer(FLASH_TIMER_ID, intv, NULL);
	}
	else if (m_flashStat.m_type == FlashType::Background) {			// 종료
		KillTimer(FLASH_TIMER_ID);
		SetBkColor(m_flashStat.m_crCurrent);
		m_flashStat.Clear();
	}
	return *this;
}

CLabel& CLabel::SetLink(BOOL bLink)
{
	m_bLink = bLink;

	if (bLink)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);

	return *this;
}

void CLabel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CString strLink;

	GetWindowText(strLink);
	ShellExecute(NULL,_T("open"),strLink,NULL,NULL,SW_SHOWNORMAL);
		
	CStatic::OnLButtonDown(nFlags, point);
}

BOOL CLabel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

CLabel& CLabel::SetLinkCursor(HCURSOR hCursor)
{
	m_hCursor = hCursor;
	return *this;
}


CString CLabel::GetText()
{
	CString str;
	GetWindowText(str);
	return str;
}


void CLabel::OnTimer(UINT_PTR nIDEvent)
{
	TRACE("%s(%d)\n", __FUNCTION__, nIDEvent);

	if (nIDEvent == FLASH_TIMER_ID) {
		COLORREF	the_color;
		the_color = (m_flashStat.bToggle) ? m_flashStat.m_crFlash1 :
			(m_flashStat.m_crFlash2 == COLOR_NONE) ? m_flashStat.m_crCurrent : m_flashStat.m_crFlash2;
		m_flashStat.bToggle = !m_flashStat.bToggle;
		switch (m_flashStat.m_type) {
		case FlashType::Text:
			SetTextColor(the_color);
			break;
		case FlashType::Background:
			SetBkColor(the_color);
			break;
		}
		RedrawWindow();
	}

	CStatic::OnTimer(nIDEvent);
}
