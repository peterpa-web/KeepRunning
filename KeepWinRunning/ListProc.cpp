#include "stdafx.h"
#include "ListProc.h"
#include "Resource.h"
#include <tlhelp32.h>

CListProc::CListProc(void)
{
}


CListProc::~CListProc(void)
{
}

void CListProc::ReadProfile( LPCTSTR pszSection )
{
	HKEY hSecKey = AfxGetApp()->GetSectionKey( pszSection );
	if ( hSecKey == NULL )
		return;

	RemoveAll();
	int n;
	for ( n = 1; TRUE; ++n )
	{
		TCHAR szEntry[10];
		_itot_s( n, szEntry, 10, 10 );
		CString strValue;
		DWORD dwType, dwCount;
		LONG lResult = RegQueryValueEx(hSecKey, szEntry, NULL, &dwType,
			NULL, &dwCount);
		if (lResult != ERROR_SUCCESS)
			break;

		ASSERT(dwType == REG_SZ);
		lResult = RegQueryValueEx(hSecKey, szEntry, NULL, &dwType,
			(LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
		strValue.ReleaseBuffer();

		CProc el;
		el.sExe = strValue;
		el.bReg = TRUE;
		AddTail( el );
	}
	RegCloseKey( hSecKey );
}

void CListProc::WriteProfile( LPCTSTR pszSection ) const
{
	AfxGetApp()->WriteProfileString( pszSection, NULL, NULL );	// delete section

	HKEY hSecKey = AfxGetApp()->GetSectionKey( pszSection );
	ASSERT( hSecKey != NULL );
	if (hSecKey == NULL)
		return;

	int n = 1;
	POSITION pos = GetHeadPosition();
	while ( pos != NULL )
	{
		const CProc &el = GetNext( pos );
		if (!el.bReg)
			continue;

		TCHAR szEntry[10];
		_itot_s( n++, szEntry, 10, 10 );
		LPCTSTR pszValue = el.sExe;
		LONG lResult = RegSetValueEx(hSecKey, szEntry, NULL, REG_SZ,
			(LPBYTE)pszValue, (lstrlen(pszValue)+1)*sizeof(TCHAR));
	}
	RegCloseKey(hSecKey);
}

BOOL CListProc::UpdRegElements( const CProc &el )
{
	POSITION pos = GetHeadPosition();
	while ( pos != NULL )
	{
		POSITION posCurr = pos;
		const CProc &elReg = GetNext( pos );
		if ( !elReg.bReg )
			return FALSE;	// no more reg elements

		if ( el.sExe.Compare( elReg.sExe ) == 0 ) {
			CProc &elUpd = GetAt( posCurr );
			elUpd.dwProcId = el.dwProcId;
			elUpd.dwParentId = el.dwParentId;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CListProc::IsAnyRegProcess() const
{
	POSITION pos = GetHeadPosition();
	while ( pos != NULL )
	{
		const CProc &el = GetNext( pos );
		if ( !el.bReg )
			return FALSE;	// no more reg elements

		if ( el.dwProcId != 0 ) {
			return TRUE;
		}
	}
	return FALSE;
}

DWORD CListProc::FindExplorerPID() const
{
	POSITION pos = GetHeadPosition();
	while ( pos != NULL )
	{
		const CProc &el = GetNext( pos );
		if ( el.sExe.Compare( _T("explorer.exe")) == 0 ) {
			return el.dwProcId;
		}
	}
	return 0;
}

void CListProc::Snap()
{
    HANDLE         hProcessSnap = NULL; 
    PROCESSENTRY32 pe32      = {0}; 
 
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    if (hProcessSnap == INVALID_HANDLE_VALUE) 
        return; 
 
	DWORD dwMyPID = GetCurrentProcessId();
    pe32.dwSize = sizeof(PROCESSENTRY32); 
 
    if (Process32First(hProcessSnap, &pe32)) 
    { 
        do 
        {
			if (pe32.th32ProcessID == dwMyPID)
				continue;	// ignore me

			CProc el;
			el.sExe = pe32.szExeFile;
			el.dwProcId = pe32.th32ProcessID;
			el.dwParentId = pe32.th32ParentProcessID;
			if ( !UpdRegElements( el ) )
				AddTail( el );
		} 
        while (Process32Next(hProcessSnap, &pe32)); 
    } 
    CloseHandle (hProcessSnap);
}

void CListProc::RemoveServices()
{
	DWORD dwExplorerPID = FindExplorerPID();

	POSITION pos = GetHeadPosition();
	while ( pos != NULL )
	{
		POSITION posCurr = pos;
		const CProc &el = GetNext( pos );
		if ( el.bReg )
			continue;	// keep reg elements

		if ( el.dwParentId == 0 ||
			 el.dwParentId != dwExplorerPID ) {
			RemoveAt( posCurr );
		}
	}
}

void CListProc::ToCntl(CListCtrl &ctrl)
{
	ctrl.DeleteAllItems();
	POSITION pos = GetHeadPosition();
	int nItem = 0;
	while (pos != NULL)
	{
		POSITION posCurr = pos;
		const CProc &el = GetNext(pos);
		ctrl.InsertItem(nItem, el.sExe);
		ctrl.SetItemData(nItem, (DWORD_PTR)posCurr);
		SetRemark(ctrl, nItem++, el);
	}

}

void CListProc::SetRemark(CListCtrl &ctrl, int nItem, const CProc &el)
{
	CString str;
	if (el.bReg) {
		el.dwProcId != 0 ? str.LoadStringW(IDS_RUNNING) : str.LoadStringW(IDS_MONITORED);
	}
	ctrl.SetItemText(nItem, 1, str);
}