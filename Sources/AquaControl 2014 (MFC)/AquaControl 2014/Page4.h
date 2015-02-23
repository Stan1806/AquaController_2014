#pragma once
#include "afxwin.h"
#include "XPGroupBox.h"

// Page4 dialog

class Page4 : public CDialog
{
	DECLARE_DYNAMIC(Page4)

public:
	Page4(CWnd* pParent = NULL);   // standard constructor
	virtual ~Page4();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
// Dialog Data
	enum { IDD = IDD_PAGE4 };
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
	bool show_koef;
	bool show_koef_k;
	unsigned char condition;
	double m_edit_ph_data;
	float m_edit_ph_calibr_1;
	float m_edit_ph_calibr_2;
	double m_edit_last_k1;
	double m_edit_last_k2;
	float m_edit_new_k1;
	float m_edit_new_k2;
	double m_edit_redox_data;
	double m_edit_last_k;
	float m_edit_new_k;
	int m_edit_redox_calibr;
	CXPGroupBox m_group_calibr_ph;
	CXPGroupBox m_group_calibr_redox;
	CXPGroupBox m_group_calc_koef;
	CXPGroupBox m_group_calibr;
	CXPGroupBox m_group_set_koef;
	CXPGroupBox m_group_new_koef;
	CXPGroupBox m_group_redox_set_koef;
	CXPGroupBox m_group_redox_new_koef;
	afx_msg void OnBnClickedButtonCalcK1();
	afx_msg void OnBnClickedButtonCalcK2();
	afx_msg void OnBnClickedButtonCalcK();
public:
	afx_msg void OnPaint();
	CEdit m_tip_edit_ph_data;
	CEdit m_tip_edit_ph_calibr_1;
	CEdit m_tip_edit_ph_calibr_2;
	CEdit m_tip_edit_last_k1;
	CEdit m_tip_edit_last_k2;
	CEdit m_tip_edit_new_k1;
	CEdit m_tip_edit_new_k2;
	CEdit m_tip_edit_redox_data;
	CEdit m_tip_edit_redox_calibr;
	CEdit m_tip_edit_last_k;
	CEdit m_tip_edit_new_k;
};
