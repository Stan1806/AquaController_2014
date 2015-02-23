#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "XPGroupBox.h"

extern bool port_send;

struct rele
{
	unsigned char lamp1;
	unsigned char lamp2;
	unsigned char lamp3;
	unsigned char lamp4;
	unsigned char compres;
	unsigned char heater;
	unsigned char co2;
	unsigned char pompa;
	unsigned char cooler;
	unsigned char cooler_data;
};

class Page1 : public CDialog
{
	DECLARE_DYNAMIC(Page1)

public:
	Page1(CWnd* pParent = NULL);   // standard constructor
	virtual ~Page1();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Dialog Data
	enum { IDD = IDD_PAGE1 };
private:
	COLORREF m_TipFront;
	COLORREF m_TipBack;
	CToolTipCtrl* m_pTip;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	double m_temp_1;
	double m_temp_2;
	double m_temp_3;
	double m_edit_ph;
	double m_edit_redox;
	unsigned char condition;
	CSliderCtrl m_slider_cooler;
	afx_msg void OnBnClickedButtonSync();
	struct rele rele_condition;
public:
	CButton *lamp1;
	CButton *lamp2;
	CButton *lamp3;
	CButton *lamp4;
	CButton *compres;
	CButton *heater;
	CButton *co2;
	CButton *pompa;
	CButton *cooler;
	
	int m_edit_hour;
	int m_edit_min;
	int m_edit_sec;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CXPGroupBox m_group_temp;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CXPGroupBox m_group_water_param;
	CXPGroupBox m_group_ds1307;
	CXPGroupBox m_group_device_select;
	CXPGroupBox m_group_com;
	CXPGroupBox m_group_led;
	CXPGroupBox m_group_cooler;
	CXPGroupBox m_group_option;
	CEdit m_edit_lamp1;
	CEdit m_edit_lamp2;
	CEdit m_edit_lamp3;
	CEdit m_edit_lamp4;
	CEdit m_edit_compres;
	CEdit m_edit_heater;
	CEdit m_edit_co2;
	CEdit m_edit_pompa;
	CEdit m_edit_cooler;
	CComboBox m_combo_port;
	int m_baudrate;
	int m_num_ports;
	int m_edit_led_stop;
	int m_edit_led_start;
	int m_edit_cooler_stop;
	int m_edit_cooler_start;
	bool port_btn_open; // детекция открытия порта
	bool port_btn_close; // детекция открытия порта
	bool port_btn_num;  
	unsigned char rele_chanel;
	afx_msg void OnBnClickedOpenPort();
	afx_msg void OnBnClickedClosePort();
	afx_msg void OnBnClickedFindPort();
	afx_msg void OnBnClickedCheckLamp1();
	afx_msg void OnBnClickedCheckLamp2();
	afx_msg void OnBnClickedCheckLamp3();
	afx_msg void OnBnClickedCheckLamp4();
	afx_msg void OnBnClickedCheckCompres();
	afx_msg void OnBnClickedCheckHeater();
	afx_msg void OnBnClickedCheckCo2();
	afx_msg void OnBnClickedCheckCooler();
	afx_msg void OnNMCustomdrawSliderCooler(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonLedSet();
	afx_msg void OnBnClickedButtonCoolerSet();
	afx_msg void OnBnClickedButtonSaveSend();
public:
	afx_msg void OnPaint();
	CButton m_tip_check_lamp_1;
	CButton m_tip_check_lamp_2;
	CButton m_tip_check_lamp_3;
	CButton m_tip_check_lamp_4;
	CButton m_tip_check_heater;
	CButton m_tip_check_co2;
	CButton m_tip_check_pompa;
	CButton m_tip_check_compres;
	CButton m_tip_check_cooler;
	CEdit m_tip_edit_t1;
	CEdit m_tip_edit_t2;
	CEdit m_tip_edit_t3;
	CEdit m_tip_edit_ph;
	CEdit m_tip_edit_redox;
	CEdit m_tip_edit_hour;
	CEdit m_tip_edit_min;
	CEdit m_tip_edit_sec;
	CEdit m_tip_edit_baudrate;
	CEdit m_tip_edit_num_ports;
	CEdit m_tip_edit_led_stop;
	CEdit m_tip_edit_led_start;
	CEdit m_tip_edit_cooler_stop;
	CEdit m_tip_edit_cooler_start;
	afx_msg void OnBnClickedCheckPompa();
};
