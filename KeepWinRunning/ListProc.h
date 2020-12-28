#pragma once
#include <afxtempl.h>
#include "afxcmn.h"

class CProc
{
public:
	DWORD	dwProcId;
	DWORD	dwParentId;
	BOOL	bReg;
	CString	sExe;

public:
	CProc(void)
	{
		dwProcId = 0;
		dwParentId = 0;
		bReg = FALSE;
	}
};




class CListProc : protected CList<CProc>
{
public:
	CListProc(void);
	~CListProc(void);

	void ReadProfile( LPCTSTR pszSection );
	void WriteProfile( LPCTSTR pszSection ) const;
	BOOL UpdRegElements( const CProc &el );
	BOOL IsAnyRegProcess() const;
	DWORD FindExplorerPID() const;
	void Snap();
	void RemoveServices();
	void ToCntl(CListCtrl &ctrl);
	void SetRemark(CListCtrl &ctrl, int nItem, const CProc &el);

	CProc &GetProcAt(POSITION pos) { return GetAt(pos); }
//	void Reset() { RemoveAll(); }
};

