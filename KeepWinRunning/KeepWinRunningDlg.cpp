
// KeepWinRunningDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KeepWinRunning.h"
#include "KeepWinRunningDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MYWM_NOTIFYICON		(WM_USER+100)

LPCTSTR CKeepWinRunningDlg::s_section = _T("procs");

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CKeepWinRunningDlg dialog



CKeepWinRunningDlg::CKeepWinRunningDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_KEEPWINRUNNING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKeepWinRunningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CKeepWinRunningDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CKeepWinRunningDlg::OnNMClickList1)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_MESSAGE( MYWM_NOTIFYICON, OnNotifyIcon )
	ON_COMMAND(ID_POPUP_SHOW, OnPopupShow)
	ON_COMMAND(IDM_ABOUTBOX, OnAbout)
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


// CKeepWinRunningDlg message handlers

BOOL CKeepWinRunningDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

//	CRect rect;
//	GetWindowRect(rect);
//	int nWidth = rect.Width() - 50;
	CString strCol;
	strCol.LoadString(IDS_APPL);
	m_listCtrl.InsertColumn(0, strCol, 0, 150);
	strCol.LoadString(IDS_REM);
	m_listCtrl.InsertColumn(1, strCol, 0, 250);

	m_nTimer = SetTimer(1, 5000, NULL);

	HINSTANCE hInst = AfxGetResourceHandle();
//	m_hIcon     = (HICON)LoadImage( hInst, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0);
	m_hIconBusy = (HICON)LoadImage( hInst, MAKEINTRESOURCE(IDI_BUSY), IMAGE_ICON, 48, 48, 0);

	VERIFY( m_menuPopup.LoadMenu(IDR_MENU1) );
	m_tnd.cbSize		= sizeof(NOTIFYICONDATA);
	m_tnd.hWnd			= m_hWnd;
	m_tnd.uID			= ID_TASKICON;
	m_tnd.uFlags		= NIF_MESSAGE | NIF_ICON;	// NIF_INFO
	m_tnd.uCallbackMessage = MYWM_NOTIFYICON;
	m_tnd.hIcon			= m_hIconBusy;
//	SetTip(CString("No Config"));
	VERIFY( Shell_NotifyIcon( NIM_ADD, &m_tnd ) );

	UpdateList(FALSE);
//	ShowWindow(SW_HIDE);
	m_bVisible = FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKeepWinRunningDlg::UpdateList(BOOL bShow)
{
	m_listApp.ReadProfile(s_section);
	m_listApp.Snap();
	if (bShow) {
		m_listApp.RemoveServices();
		m_listApp.ToCntl(m_listCtrl);
	}
	SetState(m_listApp.IsAnyRegProcess());
}

void CKeepWinRunningDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CKeepWinRunningDlg::OnAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKeepWinRunningDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKeepWinRunningDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CKeepWinRunningDlg::SetState(BOOL bKeep)
{
	EXECUTION_STATE	esFlags = ES_CONTINUOUS | (bKeep ? ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED : 0);
	SetThreadExecutionState(esFlags);
	m_tnd.hIcon = bKeep ? m_hIconBusy : m_hIcon;
	VERIFY(Shell_NotifyIcon(NIM_MODIFY, &m_tnd));
}


void CKeepWinRunningDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	UNREFERENCED_PARAMETER(pResult);

	LPNMITEMACTIVATE pia = (LPNMITEMACTIVATE)pNMHDR;

	// Get the current mouse location and convert it to client
	// coordinates.
	CPoint pos(::GetMessagePos());
	ScreenToClient(&pos);

	// Get indexes of the first and last visible items in 
	// the listview control.
	int index = m_listCtrl.GetTopIndex();
	int last_visible_index = index + m_listCtrl.GetCountPerPage();
	if (last_visible_index > m_listCtrl.GetItemCount())
		last_visible_index = m_listCtrl.GetItemCount();

	// Loop until number visible items has been reached.
	while (index <= last_visible_index)
	{
		// Get the bounding rectangle of an item. If the mouse
		// location is within the bounding rectangle of the item,
		// you know you have found the item that was being clicked.
		CRect r;
		m_listCtrl.GetItemRect(index, &r, LVIR_BOUNDS);
		if (r.PtInRect(pia->ptAction))
		{
//			UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
//			m_listCtrl.SetItemState(index, flag, flag);
			DWORD_PTR dwData = m_listCtrl.GetItemData(index);
			POSITION pos = (POSITION)dwData;
			CProc &el = m_listApp.GetProcAt(pos);
			el.bReg = !el.bReg;
			m_listApp.SetRemark(m_listCtrl, index, el);
			m_listApp.WriteProfile(s_section);
			break;
		}

		// Get the next item in listview control.
		index++;
	}
//	*pResult = 0;
}


void CKeepWinRunningDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	if (nIDEvent == 1)
	{
		UpdateList( IsWindowVisible() );
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CKeepWinRunningDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if ( nType == SIZE_MINIMIZED )
		ShowWindow( SW_HIDE );
}

LRESULT CKeepWinRunningDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
//	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if ( wParam == ID_TASKICON )
		{
			CMenu* pSubMenu = m_menuPopup.GetSubMenu(0);
			if (!pSubMenu) 
				return 0;
			::SetMenuDefaultItem(pSubMenu->m_hMenu, 0, TRUE);
			m_bVisible = TRUE;
			SetForegroundWindow();
			UpdateList(TRUE);
			CPoint mouse;
			GetCursorPos(&mouse);
			::TrackPopupMenu(pSubMenu->m_hMenu, 0, mouse.x, mouse.y, 0,	m_hWnd, NULL);    
			Shell_NotifyIcon(NIM_SETFOCUS, &m_tnd);
			PostMessage( WM_NULL, 0, 0 );
			return 1; // handled
		}
		break;

	case WM_LBUTTONDBLCLK:
		if ( wParam == ID_TASKICON )
		{
			m_bVisible = TRUE;
			ShowWindow( SW_NORMAL );
			SetForegroundWindow();
			UpdateList( TRUE );
		}
		break;

	default:
		break;
	}

	return (LRESULT)0;
}

void CKeepWinRunningDlg::OnPopupShow()
{
	ShowWindow( SW_NORMAL );
}



void CKeepWinRunningDlg::OnClose()
{
	CString strMsg;
	strMsg.LoadString(IDS_EXIT);
	if (AfxMessageBox(strMsg, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2, 0) != IDOK)
		return;

	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	KillTimer(m_nTimer);
	VERIFY(Shell_NotifyIcon(NIM_DELETE, &m_tnd));

	CDialogEx::OnClose();
}


void CKeepWinRunningDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (!m_bVisible)
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	CDialogEx::OnWindowPosChanging(lpwndpos);

	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein.
}
