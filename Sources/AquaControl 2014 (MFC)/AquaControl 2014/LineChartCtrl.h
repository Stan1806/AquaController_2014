// LineChartCtrl.h : header file
//
// Written by Yuheng Zhao (yuheng@ministars.com) 
// http://www.ministars.com
// The original idea and part of the code from Ken C. Len's CHistogramCtrl
// http://www.codeguru.com/controls/histogram_control.shtml
//
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
// It's free - so you get what you pay for.
//

#ifndef __LINECHARTCTRL_H__
#define __LINECHARTCTRL_H__

#include <afxtempl.h>

class CLineChartItem:public CObject
{
public:
	CLineChartItem() {;}

	COLORREF m_colorLine;
	UINT     m_nLower;		// lower bounds
	UINT     m_nUpper;		// upper bounds
	
	UINT     m_nPos;		// current position within bounds
	UINT     m_nOldPos;		// last position within bounds
};

typedef CTypedPtrArray <CObArray, CLineChartItem*> CItemArray;

/////////////////////////////////////////////////////////////////////////////
// CLineChartCtrl window

class CLineChartCtrl : public CWnd
{
// Construction
public:
	static BOOL RegisterWndClass(HINSTANCE hInstance);

	CLineChartCtrl();
	UINT m_nVertical;

// Attributes
public:
	CItemArray m_items;
	UINT SetPos(int nIndex, UINT nPos);
	void InvalidateCtrl();
	void DrawSpike();

	// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineChartCtrl)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	UINT SetLimit(int nIndex, UINT nPos);
	void Go();
	BOOL Add(COLORREF color, UINT Upper, UINT Lower );
	virtual ~CLineChartCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLineChartCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CDC      m_MemDC;
	CBitmap  m_Bitmap;
};

/////////////////////////////////////////////////////////////////////////////
#endif
