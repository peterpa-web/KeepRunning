
// KeepRunning.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CKeepRunningApp:
// Siehe KeepRunning.cpp für die Implementierung dieser Klasse
//

class CKeepRunningApp : public CWinApp
{
public:
	CKeepRunningApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CKeepRunningApp theApp;