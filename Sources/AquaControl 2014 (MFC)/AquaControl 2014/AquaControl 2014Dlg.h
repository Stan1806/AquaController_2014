// AquaControl 2014Dlg.h : header file
//

#pragma once
#include "afxcmn.h"

#include "LineChartCtrl.h"
#include "afxwin.h"
#include "XPGroupBox.h"
#include "SerialPort.h"
#include "MainOption.h"
#include "SystemTray.h"


#include "Page1.h"
#include "Page2.h"
#include "Page3.h"
#include "Page4.h"

extern unsigned char TRANSMIT_ERROR;
extern Page1 m_page1;
extern Page2 m_page2;
extern Page3 m_page3;
extern Page4 m_page4;

// CAquaControl2014Dlg dialog
class CAquaControl2014Dlg : public CDialog
{
// Construction
public:
	CAquaControl2014Dlg(CWnd* pParent = NULL);	// standard constructor
	~CAquaControl2014Dlg();
	CRect GetPageRect(CTabCtrl &T);
	CSystemTray m_TrayIcon;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
// Dialog Data
	enum { IDD = IDD_AQUACONTROL2014_DIALOG };
private:
	COLORREF m_TipFront;
	COLORREF m_TipBack;
	CToolTipCtrl* m_pTip;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnRecieveData();
	afx_msg void OnSpravka();
	afx_msg void OnTray();
	afx_msg void OnAbout();
	afx_msg void OnMinimise();
	afx_msg void OnMaximise();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	HANDLE	hThreadSend;
	DWORD	dwTheardSendID;
	CRITICAL_SECTION csData;
	CTabCtrl m_tab;
	CMainOption m_main_option;
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
private:
	CLineChartCtrl m_wndLineChart_1;
	CLineChartCtrl m_wndLineChart_2;
	CLineChartCtrl m_wndLineChart_3;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOpenPort();
	afx_msg void OnBnClickedClosePort();
	afx_msg void OnBnClickedFindPort();
	CXPGroupBox m_group_dinamic;
	CXPGroupBox m_group_dinamic_temp;
	CXPGroupBox m_group_dinamic_ph;
	CXPGroupBox m_group_dinamic_redox;
	CEdit m_edit_system_time;
	unsigned int m_graf;
	unsigned char hour_now[2];
	unsigned char minute_now[2];
	unsigned char second_now[2];
	bool mas_compare(int data_count);
private:
	HRESULT OnCommunication(WPARAM ch, LPARAM port);
	CSerialPort *m_port;
public:
	CSliderCtrl m_slider_graf;
	afx_msg void OnNMCustomdrawSliderGraf(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};