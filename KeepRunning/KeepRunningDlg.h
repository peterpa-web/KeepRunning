
// KeepRunningDlg.h: Headerdatei
//

#pragma once
#include "afxcmn.h"
#include "ListProc.h"


// CKeepRunningDlg-Dialogfeld
class CKeepRunningDlg : public CDialogEx
{
// Konstruktion
public:
	CKeepRunningDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KEEPRUNNING_DIALOG };
#endif



protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung

	static LPCTSTR s_section;

	CListProc m_listApp;

// Implementierung
protected:
	HICON m_hIcon;
	HICON m_hIconBusy;
	NOTIFYICONDATA m_tnd;
	CMenu m_menuPopup;
	UINT_PTR m_nTimer;
	BOOL m_bVisible;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void SetState(BOOL bKeep);
	void UpdateList(BOOL bShow);

public:
	CListCtrl m_listCtrl;
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPopupShow();
	afx_msg void OnClose();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
