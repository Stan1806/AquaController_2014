// Page4.cpp : implementation file
//

#include "stdafx.h"
#include "AquaControl 2014.h"
#include "AquaControl 2014Dlg.h"
#include "Page4.h"


// Page4 dialog

IMPLEMENT_DYNAMIC(Page4, CDialog)

Page4::Page4(CWnd* pParent /*=NULL*/)
	: CDialog(Page4::IDD, pParent)
	, m_edit_ph_data(0)
	, m_edit_ph_calibr_1(7.01)
	, m_edit_ph_calibr_2(4.01)
	, m_edit_last_k1(0)
	, m_edit_last_k2(0)
	, m_edit_new_k1(0)
	, m_edit_new_k2(0)
	, m_edit_redox_data(0)
	, m_edit_last_k(0)
	, m_edit_new_k(0)
	, m_edit_redox_calibr(475)
	, show_koef(false)
	, show_koef_k(false)
{
	//начальная инициализация указателя
	m_pTip=NULL;
}

Page4::~Page4()
{
	//выделяли память...
	if (m_pTip!=NULL) delete m_pTip;
	//...очищаем память
}

void Page4::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PH_DATA, m_edit_ph_data);
	DDX_Text(pDX, IDC_EDIT_PH_CALIBR_1, m_edit_ph_calibr_1);
	DDX_Text(pDX, IDC_EDIT_PH_CALIBR_2, m_edit_ph_calibr_2);
	DDX_Text(pDX, IDC_EDIT_LAST_K1, m_edit_last_k1);
	DDX_Text(pDX, IDC_EDIT_LAST_K2, m_edit_last_k2);
	DDX_Text(pDX, IDC_EDIT_NEW_K1, m_edit_new_k1);
	DDX_Text(pDX, IDC_EDIT_NEW_K2, m_edit_new_k2);
	DDX_Text(pDX, IDC_EDIT_REDOX_DATA, m_edit_redox_data);
	DDX_Text(pDX, IDC_EDIT_LAST_K, m_edit_last_k);
	DDX_Text(pDX, IDC_EDIT_NEW_K, m_edit_new_k);
	DDX_Text(pDX, IDC_EDIT_REDOX_CALIBR, m_edit_redox_calibr);
	DDX_Control(pDX, IDC_STATIC_CALIBR_PH, m_group_calibr_ph);
	DDX_Control(pDX, IDC_STATIC_CALIBR_REDOX, m_group_calibr_redox);
	DDX_Control(pDX, IDC_STATIC_CALC_KOEF, m_group_calc_koef);
	DDX_Control(pDX, IDC_STATIC_CALIBR, m_group_calibr);
	DDX_Control(pDX, IDC_STATIC_SET_KOEF, m_group_set_koef);
	DDX_Control(pDX, IDC_STATIC_NEW_KOEF, m_group_new_koef);
	DDX_Control(pDX, IDC_STATIC_REDOX_SET_KOEF, m_group_redox_set_koef);
	DDX_Control(pDX, IDC_STATIC_REDOX_NEW_KOEF, m_group_redox_new_koef);
	DDX_Control(pDX, IDC_EDIT_PH_DATA, m_tip_edit_ph_data);
	DDX_Control(pDX, IDC_EDIT_PH_CALIBR_1, m_tip_edit_ph_calibr_1);
	DDX_Control(pDX, IDC_EDIT_PH_CALIBR_2, m_tip_edit_ph_calibr_2);
	DDX_Control(pDX, IDC_EDIT_LAST_K1, m_tip_edit_last_k1);
	DDX_Control(pDX, IDC_EDIT_LAST_K2, m_tip_edit_last_k2);
	DDX_Control(pDX, IDC_EDIT_NEW_K1, m_tip_edit_new_k1);
	DDX_Control(pDX, IDC_EDIT_NEW_K2, m_tip_edit_new_k2);
	DDX_Control(pDX, IDC_EDIT_REDOX_DATA, m_tip_edit_redox_data);
	DDX_Control(pDX, IDC_EDIT_REDOX_CALIBR, m_tip_edit_redox_calibr);
	DDX_Control(pDX, IDC_EDIT_LAST_K, m_tip_edit_last_k);
	DDX_Control(pDX, IDC_EDIT_NEW_K, m_tip_edit_new_k);
}


BEGIN_MESSAGE_MAP(Page4, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CALC_K1, &Page4::OnBnClickedButtonCalcK1)
	ON_BN_CLICKED(IDC_BUTTON_CALC_K2, &Page4::OnBnClickedButtonCalcK2)
	ON_BN_CLICKED(IDC_BUTTON_CALC_K, &Page4::OnBnClickedButtonCalcK)
END_MESSAGE_MAP()

BOOL Page4::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (!m_pTip)
	{
		m_pTip =new CToolTipCtrl;
		m_pTip->Create(this,TTS_ALWAYSTIP); // подсказки активны во всех диалогах
		m_pTip->SetMaxTipWidth(200); // многострочный текст, количество символов
		m_pTip->SetDelayTime(900); // время отображения подсказок
		m_pTip->AddTool(&m_tip_edit_ph_data,_T("Текущее значение PH-электрода."));
		m_pTip->AddTool(&m_tip_edit_ph_calibr_1,_T("Значение первого калибровочного раствора. Формат <X.XX> е.и."));
		m_pTip->AddTool(&m_tip_edit_ph_calibr_2,_T("Значение второго калибровочного раствора. Формат <X.XX> е.и.."));
		m_pTip->AddTool(&m_tip_edit_last_k1,_T("Значение установленного коэффициента калибровки К1."));
		m_pTip->AddTool(&m_tip_edit_last_k2,_T("Значение установленного коэффициента калибровки К2."));
		m_pTip->AddTool(&m_tip_edit_new_k1,_T("Значение рассчитанного коэффициента калибровки К1."));
		m_pTip->AddTool(&m_tip_edit_new_k2,_T("Значение рассчитанного коэффициента калибровки К2."));
		m_pTip->AddTool(&m_tip_edit_redox_data,_T("Текущее значение Redox-электрода."));
		m_pTip->AddTool(&m_tip_edit_redox_calibr,_T("Значение калибрововчного раствора. Формат <XXX> mV."));
		m_pTip->AddTool(&m_tip_edit_last_k,_T("Значение установленного коэффициента калибровки К."));
		m_pTip->AddTool(&m_tip_edit_new_k,_T("Значение рассчитанного коэффициента калибровки К."));

		m_pTip->Activate(TRUE); 
	}

	//установка цвета текста и фона подсказок
	//с помощью функций WinAPI
	
	//цвет заднего фона подсказки
	m_TipBack=RGB(248,255,183);
	WPARAM wParam=(WPARAM)m_TipBack;
	LPARAM lParam = 0;
	::SendMessage(m_pTip->m_hWnd,TTM_SETTIPBKCOLOR,wParam, lParam);
    
	//цвет текста подсказки
	m_TipFront=RGB(0,0,255);
	wParam=(WPARAM)m_TipFront;
	::SendMessage(m_pTip->m_hWnd,TTM_SETTIPTEXTCOLOR,wParam, lParam);
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL Page4::PreTranslateMessage(MSG* pMsg) 
{
	//перегрузка виртуальной функции PreTranslateMessage
	//если подсказки определены, их отображние
	if (m_pTip!=NULL)
            m_pTip->RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}

void Page4::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}
// Page4 message handlers

void Page4::OnBnClickedButtonCalcK1()
{
	TRANSMIT_ERROR=1;
	condition=0x08;
	port_send=TRUE;
	GetDlgItem(IDC_BUTTON_CALC_K2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CALC_K1)->EnableWindow(FALSE);
}

void Page4::OnBnClickedButtonCalcK2()
{
	TRANSMIT_ERROR=1;
	show_koef=true;
	UpdateData(true);
	if(m_edit_new_k1!=0)
	{
		m_edit_last_k1=m_edit_new_k1;
		m_edit_last_k2=m_edit_new_k2;
	}
	UpdateData(false);
	condition=0x09;
	port_send=TRUE;
	GetDlgItem(IDC_BUTTON_CALC_K2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CALC_K1)->EnableWindow(TRUE);
}

void Page4::OnBnClickedButtonCalcK()
{
	TRANSMIT_ERROR=1;
	show_koef_k=true;
	UpdateData(true);
	if(m_edit_new_k!=0)
	{
		m_edit_last_k=m_edit_new_k;
	}
	UpdateData(false);
	condition=0x10;
	port_send=TRUE;
}
