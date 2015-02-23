#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "XPGroupBox.h"

struct timer_new
{
	int timer_set_min[1];
	int timer_set_sec[1];
	CString timer_set_text[1];
	CString timer_stop_text[1];
};

extern bool port_send;
extern unsigned int water_lvl;
extern unsigned int doliv_porog1;
extern unsigned int doliv_porog2;
// Page2 dialog

class Page2 : public CDialog
{
	DECLARE_DYNAMIC(Page2)

public:
	Page2(CWnd* pParent = NULL);   // standard constructor
	virtual ~Page2();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Dialog Data
	enum { IDD = IDD_PAGE2 };
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
	struct timer_new timer_produv;
	bool temp_mode_set;
	CSliderCtrl m_slider_cooler;
	unsigned char m_edit_temp_set;
	float m_edit_temp_porog;
	unsigned char cooler_data;
	int m_edit_cooler_power;
	float m_edit_ph_high;
	float m_edit_ph_low;
	unsigned char m_edit_co2_stop;
	unsigned char m_edit_co2_start;
	unsigned char condition;
	bool check_temp_mode;
	CEdit m_edit_produv_dlit;
	CComboBox m_combo_period;
	CXPGroupBox m_group_temp_mode;
	CXPGroupBox m_group_ph_mode;
	CXPGroupBox m_group_mineral_mode;
	CSpinButtonCtrl m_spin_produv_dlit;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDeltaposSpinMineralStart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSpinSet(CEdit &edit_control, CSpinButtonCtrl &spin_control, int timer_param);
	afx_msg void OnSpinStop(CEdit &edit_control, CSpinButtonCtrl &spin_control, int timer_param);
	afx_msg void OnDeltaposSpinMineralStop(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_combo_temp_sensor;
	afx_msg void OnBnClickedButtonSendMc();
	CXPGroupBox m_static_dop_param;
	afx_msg void OnBnClickedCheckTempMode();
	afx_msg void OnNMCustomdrawSliderCooler(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckMineral();
	int m_edit_min_power_cooler;
	afx_msg void OnBnClickedCheckCo2Mode();
	CXPGroupBox m_group_doliv;
	afx_msg void OnBnClickedButtonPorog1();
	afx_msg void OnBnClickedButtonPorog2();
	afx_msg void OnBnClickedCheckDoliv();
	CProgressCtrl m_progress_doliv;
	afx_msg void OnBnClickedButtonEventParam();
	CButton m_tip_check_temp_mode;
public:
	afx_msg void OnPaint();
	CEdit m_tip_edit_temp_set;
	CEdit m_tip_edit_temp_porog;
	CButton m_tip_check_produv;
	CButton m_tip_check_co2_mode;
	CEdit m_tip_edit_ph_high;
	CEdit m_tip_edit_ph_low;
	CEdit m_tip_edit_co2_stop;
	CEdit m_tip_edit_co2_start;
	CButton m_tip_check_doliv;
	CEdit m_tip_edit_power_cooler;
};
