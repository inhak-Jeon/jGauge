#include <string>
#if !defined(AFX_LABEL_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_)
#define AFX_LABEL_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Label.h : header file
//

#include <string>
using namespace std;

#define	DEF_FLASH_COLOR		RGB(255,0,0)
#define	DEF_FLASH_INTERVAL	250		// msec
#define	COLOR_NONE		DWORD_MAX	// COLORREF 인자로 주고받을 수 있도록

/////////////////////////////////////////////////////////////////////////////
// CLabel window

enum FlashType {None, Text, Background };
class FlashStat {
public:
	FlashType	m_type;
	COLORREF	m_crCurrent, m_crFlash1, m_crFlash2;
	bool	bToggle;
	FlashStat() {
		Clear();
	};
	void	Set(FlashType type, COLORREF crCurrent, COLORREF crFlash1, COLORREF crFlash2) {
		m_type = type;	m_crCurrent = crCurrent;	m_crFlash1 = crFlash1;	m_crFlash2 = crFlash2;	bToggle = false;
	};
	void	Clear() {
		m_type = None;
	};
};

class CLabel : public CStatic
{
// Construction
public:
	CLabel();
	CLabel& SetBkColor(COLORREF crBkgnd);
	CLabel& SetTextColor(COLORREF crText);
	CLabel& SetText(const string& strText);
	CLabel& SetText(const CString& strText);
	CLabel& SetFontBold(BOOL bBold);
	CLabel& SetFontName(const string& strFont, int charset = ANSI_CHARSET);
	CLabel& SetFontName(const CString& strFont, int charset = ANSI_CHARSET);
	CLabel& SetFontUnderline(BOOL bSet);
	CLabel& SetFontItalic(BOOL bSet);
	CLabel& SetFontSize(int nSize);
	CLabel& SetSunken(BOOL bSet);
	CLabel& SetBorder(BOOL bSet);
	CLabel& FlashText(BOOL bActivate, COLORREF crFlash = DEF_FLASH_COLOR, int intv = DEF_FLASH_INTERVAL);
	CLabel& FlashText(BOOL bActivate, COLORREF crFlash1, COLORREF crFlash2, int intv = DEF_FLASH_INTERVAL);
	CLabel& FlashBackground(BOOL bActivate, COLORREF crFlash = DEF_FLASH_COLOR, int intv = DEF_FLASH_INTERVAL);
	CLabel& FlashBackground(BOOL bActivate, COLORREF crFlash1, COLORREF crFlash2, int intv = DEF_FLASH_INTERVAL);
	CLabel& SetLink(BOOL bLink);
	CLabel& SetLinkCursor(HCURSOR hCursor);

// Attributes
public:
protected:
	void ReconstructFont();
	COLORREF	m_crText;
	COLORREF	m_crBkgnd;
	HBRUSH		m_hBrush;
	HBRUSH		m_hwndBrush;
	LOGFONT		m_lf;
	CFont		m_font;
	CString		m_strText;
	BOOL		m_bLink;
	FlashStat	m_flashStat;
	HCURSOR		m_hCursor;
			// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabel)
	//}}AFX_VIRTUAL

// Implementation
public:
	CString GetText();
	virtual ~CLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLabel)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABEL_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_)
