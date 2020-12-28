
// KeepRunningDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "KeepRunning.h"
#include "KeepRunningDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MYWM_NOTIFYICON		(WM_USER+100)

LPCTSTR CKeepRunningDlg::s_section = _T("procs");


// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
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


// CKeepRunningDlg-Dialogfeld



CKeepRunningDlg::CKeepRunningDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_KEEPRUNNING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKeepRunningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CKeepRunningDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CKeepRunningDlg::OnNMClickList1)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_MESSAGE( MYWM_NOTIFYICON, OnNotifyIcon )
	ON_COMMAND(ID_POPUP_SHOW, OnPopupShow)
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


// CKeepRunningDlg-Meldungshandler

BOOL CKeepRunningDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
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

	// Symbol für dieses Dialogfeld festlegen.  Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// TODO: Hier zusätzliche Initialisierung einfügen

//	CRect rect;
//	GetWindowRect(rect);
//	int nWidth = rect.Width() - 50;
	m_listCtrl.InsertColumn(0, _T("Application"), 0, 150);
	m_listCtrl.InsertColumn(1, _T("Remarks"), 0, 250);

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

	return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}

void CKeepRunningDlg::UpdateList(BOOL bShow)
{
	m_listApp.ReadProfile(s_section);
	m_listApp.Snap();
	if (bShow) {
		m_listApp.RemoveServices();
		m_listApp.ToCntl(m_listCtrl);
	}
	SetState(m_listApp.IsAnyRegProcess());
}

void CKeepRunningDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie
//  den nachstehenden Code, um das Symbol zu zeichnen.  Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CKeepRunningDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CKeepRunningDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CKeepRunningDlg::SetState(BOOL bKeep)
{
	EXECUTION_STATE	esFlags = ES_CONTINUOUS | (bKeep ? ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED : 0);
	SetThreadExecutionState(esFlags);
	m_tnd.hIcon = bKeep ? m_hIconBusy : m_hIcon;
	VERIFY(Shell_NotifyIcon(NIM_MODIFY, &m_tnd));
}


void CKeepRunningDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
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


void CKeepRunningDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	if (nIDEvent == 1)
	{
		UpdateList( IsWindowVisible() );
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CKeepRunningDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if ( nType == SIZE_MINIMIZED )
		ShowWindow( SW_HIDE );
}

LRESULT CKeepRunningDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
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

void CKeepRunningDlg::OnPopupShow()
{
	ShowWindow( SW_NORMAL );
}



void CKeepRunningDlg::OnClose()
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	KillTimer(m_nTimer);
	VERIFY(Shell_NotifyIcon(NIM_DELETE, &m_tnd));

	CDialogEx::OnClose();
}


void CKeepRunningDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (!m_bVisible)
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	CDialogEx::OnWindowPosChanging(lpwndpos);

	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein.
}
