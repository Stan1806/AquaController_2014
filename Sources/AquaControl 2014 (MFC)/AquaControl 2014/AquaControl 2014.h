// AquaControl 2014.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CAquaControl2014App:
// See AquaControl 2014.cpp for the implementation of this class
//

class CAquaControl2014App : public CWinApp
{
public:
	CAquaControl2014App();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CAquaControl2014App theApp;