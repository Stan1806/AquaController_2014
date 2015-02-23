#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "XPGroupBox.h"


// Page3 dialog
struct timer
{
	int timer_set_hour[9];
	int timer_set_min[9];
	int timer_set_sec[9];
	int timer_stop_hour[9];
	int timer_stop_min[9];
	int timer_stop_sec[9];
	CString timer_set_text[9];
	CString timer_stop_text[9];
};

class Page3 : public CDialog
{
	DECLARE_DYNAMIC(Page3)

public:
	Page3(CWnd* pParent = NULL);   // standard constructor
	virtual ~Page3();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
// Dialog Data
	enum { IDD = IDD_PAGE3 };
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
	struct timer timer_1;
	struct timer timer_2;
	struct timer timer_3;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDeltaposSpinLamp1T1Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	CEdit m_edit_t1_set[9];
	CEdit m_edit_t2_set[9];
	CEdit m_edit_t3_set[9];
	////////////////////////////
	CEdit m_edit_t1_stop[9];
	CEdit m_edit_t2_stop[9];
	CEdit m_edit_t3_stop[9];
	////////////////////////////
	CSpinButtonCtrl m_spin_t1_set[9];
	CSpinButtonCtrl m_spin_t2_set[9];
	CSpinButtonCtrl m_spin_t3_set[9];
	////////////////////////////////////
	CSpinButtonCtrl m_spin_t1_stop[9];
	CSpinButtonCtrl m_spin_t2_stop[9];
	CSpinButtonCtrl m_spin_t3_stop[9];
	
	CXPGroupBox m_group_device;
	CXPGroupBox m_group_timer1;
	CXPGroupBox m_group_timer2;
	CXPGroupBox m_group_timer3;
	afx_msg void OnDeltaposSpinLamp1T1Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();

	unsigned char condition;
	afx_msg void OnDeltaposSpinLamp2T1Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp3T1Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp4T1Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCompresT1Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinHeaterT1Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCo2T1Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMineralT1Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCoolerT1Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp2T1Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp3T1Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp4T1Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCompresT1Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinHeaterT1Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCo2T1Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMineralT1Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCoolerT1Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp1T2Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp2T2Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp3T2Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp4T2Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCompresT2Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinHeaterT2Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCo2T2Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMineralT2Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCoolerT2Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp1T2Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp2T2Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp3T2Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp4T2Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCompresT2Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinHeaterT2Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCo2T2Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMineralT2Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCoolerT2Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp1T3Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp2T3Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp3T3Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp4T3Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCompresT3Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinHeaterT3Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCo2T3Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMineralT3Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCoolerT3Set(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp1T3Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp2T3Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp3T3Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLamp4T3Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCompresT3Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinHeaterT3Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCo2T3Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMineralT3Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCoolerT3Stop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonReadMc();
public:
	afx_msg void OnPaint();
};
