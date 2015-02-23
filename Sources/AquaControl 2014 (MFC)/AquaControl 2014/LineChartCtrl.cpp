// LineChartCtrl.cpp : implementation file
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

#include "stdafx.h"
#include "LineChartCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineChartCtrl

CLineChartCtrl::CLineChartCtrl()
{
	CLineChartCtrl::RegisterWndClass(AfxGetInstanceHandle());
}

CLineChartCtrl::~CLineChartCtrl()
{
	int nCount = m_items.GetSize();
	
	for (int i = 0; i < nCount; i++)
		delete m_items.GetAt(i);
	m_items.RemoveAll();
}


BEGIN_MESSAGE_MAP(CLineChartCtrl, CWnd)
	//{{AFX_MSG_MAP(CLineChartCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CLineChartCtrl::RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.lpszClassName = _T("LINE_CHART_CTRL"); // matches class name in client
	wc.hInstance = hInstance;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = 0;
	wc.lpszMenuName = NULL;
	wc.hbrBackground = (HBRUSH) ::GetStockObject(LTGRAY_BRUSH);
	wc.style = CS_GLOBALCLASS; // To be modified
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	return (::RegisterClass(&wc) != 0);
}

void CLineChartCtrl::InvalidateCtrl()
{
	CClientDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);

	if (m_MemDC.GetSafeHdc() == NULL)
	{
		m_MemDC.CreateCompatibleDC(&dc);
		m_Bitmap.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
		m_MemDC.SelectObject(m_Bitmap);
		
		// draw scale
		m_MemDC.SetBkColor(RGB(255,255,255));
		CBrush bkBrush(HS_CROSS,RGB(190,190,190));
		m_MemDC.FillRect(rcClient,&bkBrush);
	}

	InvalidateRect(rcClient, FALSE);
}

UINT CLineChartCtrl::SetPos(int nIndex, UINT nPos)
{
	if (nIndex >= m_items.GetSize())
		return 0;

	CLineChartItem* pItem = m_items.GetAt(nIndex);

	if (nPos > pItem->m_nUpper)
		nPos = pItem->m_nUpper;
	if (nPos < pItem->m_nLower)
		nPos = pItem->m_nLower;

	pItem->m_nOldPos = pItem->m_nPos;
	pItem->m_nPos = nPos;

	return pItem->m_nOldPos;
}

void CLineChartCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rcClient;
	GetClientRect(rcClient);

	// draw scale
	if (m_MemDC.GetSafeHdc() != NULL)
		dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &m_MemDC, 0, 0, SRCCOPY);
}

void CLineChartCtrl::DrawSpike()
{
	CRect rcClient;
	GetClientRect(rcClient);

	if (m_MemDC.GetSafeHdc() != NULL)
	{
		m_MemDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &m_MemDC, 4, 0, SRCCOPY);

		// draw scale
		CRect rcRight = rcClient;
		rcRight.left = rcRight.right - 4;
		m_MemDC.SetBkColor(RGB(255,255,255));

		CBrush bkBrush(HS_HORIZONTAL,RGB(190,190,190));  
		m_MemDC.FillRect(rcRight,&bkBrush);

		static BOOL bDrawVerticle = FALSE;
		bDrawVerticle = !bDrawVerticle;

		// Draw Verticle lines only every two steps
		if (bDrawVerticle)
		{
			CPen pen(PS_SOLID, 1, RGB(190,190,190));
			CPen* pOldPen = m_MemDC.SelectObject(&pen);
			m_MemDC.MoveTo(CPoint(rcClient.right-2, rcClient.top));
			m_MemDC.LineTo(CPoint(rcClient.right-2, rcClient.bottom));
			m_MemDC.SelectObject(pOldPen);
		}

		int nCount = m_items.GetSize();
		CLineChartItem* pItem;
		CPoint ptOld, ptNew;
		for (int i=0; i<nCount; i++)
		{
			pItem = m_items.GetAt(i);

			UINT  nRange = pItem->m_nUpper - pItem->m_nLower;
			ptOld.x = rcRight.left-1; // Minus one to make sure to draw inside the area
			ptNew.x = rcRight.right-1;
			ptOld.y = (int)((((float)(nRange - pItem->m_nOldPos))/(float)nRange)
				* (float)rcRight.Height());
			ptNew.y = (int)((((float)(nRange - pItem->m_nPos))/(float)nRange)
				* (float)rcRight.Height());

			CPen pen(PS_SOLID, 1, pItem->m_colorLine);
			CPen* pOldPen = m_MemDC.SelectObject(&pen);
			m_MemDC.MoveTo(ptOld);
			m_MemDC.LineTo(ptNew);
			m_MemDC.SelectObject(pOldPen);
		}
	}
}

BOOL CLineChartCtrl::Add(COLORREF color, UINT Upper, UINT Lower)
{
	CLineChartItem* pItem = new CLineChartItem;
	pItem->m_colorLine = color;
	pItem->m_nLower = Lower;
	pItem->m_nUpper = Upper;
	pItem->m_nPos = 0;
	pItem->m_nOldPos = 0;

	try 
	{
		m_items.Add(pItem);

		InvalidateCtrl();
		return TRUE;
	}
	catch (CMemoryException* e)
	{
		if (pItem !=NULL) 
			delete pItem;
		e->Delete();
		return FALSE;
	}
}

void CLineChartCtrl::Go()
{
	DrawSpike();

	Invalidate(FALSE);
}

UINT CLineChartCtrl::SetLimit(int nIndex, UINT nPos)
{
	if (nIndex >= m_items.GetSize())
		return 0;
	
	CLineChartItem* pItem = m_items.GetAt(nIndex);
	
	if (nPos > pItem->m_nUpper)
		nPos = pItem->m_nUpper;
	if (nPos < pItem->m_nLower)
		nPos = pItem->m_nLower;
		
	pItem->m_nPos = nPos;
	pItem->m_nOldPos = pItem->m_nPos;
	
	return pItem->m_nOldPos;
}
