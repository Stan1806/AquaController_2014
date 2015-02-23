#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "XPGroupBox.h"

// CMainOption dialog
extern char file_buf[5];
extern char option_data[5];

class CMainOption : public CDialog
{
	DECLARE_DYNAMIC(CMainOption)

public:
	CMainOption(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainOption();
// Dialog Data
	enum { IDD = IDD_DIALOG_MAIN_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton *tray;
	CButton *eeprom;
	CButton *time_compare;
	CButton *com_port;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CXPGroupBox m_group_main_option;
	bool SHOW_TEXT;
	afx_msg void OnBnClickedCheckTray();
	afx_msg void OnBnClickedCheckSaveEeprom();
	afx_msg void OnBnClickedCheckTimeCompare();
	afx_msg void OnBnClickedCheckComPortOpen();
	CComboBox m_combo_com_port_num;
};
