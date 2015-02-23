// Page2.cpp : implementation file
//

#include "stdafx.h"
#include "AquaControl 2014.h"
#include "AquaControl 2014Dlg.h"
#include "Page2.h"

CButton *temp_mode;
CButton *produv_mode;
CButton *co2_mode;
CButton *doliv;

bool SPIN_NEW=false;

// Page2 dialog

IMPLEMENT_DYNAMIC(Page2, CDialog)

Page2::Page2(CWnd* pParent /*=NULL*/)
	: CDialog(Page2::IDD, pParent)
	, m_edit_temp_set(26)
	, m_edit_temp_porog(0.3)
	, m_edit_cooler_power(50)
	, m_edit_ph_high(7.01)
	, m_edit_ph_low(4.01)
	, m_edit_co2_stop(23)
	, m_edit_co2_start(7)
	, m_edit_min_power_cooler(25)
	, temp_mode_set(false)
{
	//начальная инициализация указателя
	m_pTip=NULL;
}

Page2::~Page2()
{
	//выделяли память...
	if (m_pTip!=NULL) delete m_pTip;
	//...очищаем память
}

void Page2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_COOLER, m_slider_cooler);
	DDX_Text(pDX, IDC_EDIT_TEMP_SET, m_edit_temp_set);
	DDX_Text(pDX, IDC_EDIT_TEMP_POROG, m_edit_temp_porog);
	DDX_Text(pDX, IDC_EDIT_PH_HIGH, m_edit_ph_high);
	DDX_Text(pDX, IDC_EDIT_PH_LOW, m_edit_ph_low);
	DDX_Text(pDX, IDC_EDIT_CO2_STOP, m_edit_co2_stop);
	DDX_Text(pDX, IDC_EDIT_CO2_START, m_edit_co2_start);
	DDX_Control(pDX, IDC_EDIT_PRODUV_DLIT, m_edit_produv_dlit);
	DDX_Control(pDX, IDC_COMBO_PERIOD, m_combo_period);
	DDX_Control(pDX, IDC_STATIC_TEMP_MODE, m_group_temp_mode);
	DDX_Control(pDX, IDC_STATIC_PH_MODE, m_group_ph_mode);
	DDX_Control(pDX, IDC_STATIC_PRODUV_MODE, m_group_mineral_mode);
	DDX_Control(pDX, IDC_SPIN_PRODUV_DLIT, m_spin_produv_dlit);
	DDX_Control(pDX, IDC_COMBO_TEMP_SENSOR, m_combo_temp_sensor);
	DDX_Control(pDX, IDC_STATIC_DOP_PARAM, m_static_dop_param);
	DDX_Text(pDX, IDC_EDIT_MIN_POWER_COOLER, m_edit_min_power_cooler);
	DDX_Control(pDX, IDC_STATIC_DOLIV, m_group_doliv);
	DDX_Control(pDX, IDC_PROGRESS_DOLIV, m_progress_doliv);
	DDX_Control(pDX, IDC_CHECK_TEMP_MODE, m_tip_check_temp_mode);
	DDX_Control(pDX, IDC_EDIT_TEMP_SET, m_tip_edit_temp_set);
	DDX_Control(pDX, IDC_EDIT_TEMP_POROG, m_tip_edit_temp_porog);
	DDX_Control(pDX, IDC_CHECK_PRODUV, m_tip_check_produv);
	DDX_Control(pDX, IDC_CHECK_CO2_MODE, m_tip_check_co2_mode);
	DDX_Control(pDX, IDC_EDIT_PH_HIGH, m_tip_edit_ph_high);
	DDX_Control(pDX, IDC_EDIT_PH_LOW, m_tip_edit_ph_low);
	DDX_Control(pDX, IDC_EDIT_CO2_STOP, m_tip_edit_co2_stop);
	DDX_Control(pDX, IDC_EDIT_CO2_START, m_tip_edit_co2_start);
	DDX_Control(pDX, IDC_CHECK_DOLIV, m_tip_check_doliv);
	DDX_Control(pDX, IDC_EDIT_MIN_POWER_COOLER, m_tip_edit_power_cooler);
}


BEGIN_MESSAGE_MAP(Page2, CDialog)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINERAL_STOP, &Page2::OnDeltaposSpinMineralStop)
	ON_BN_CLICKED(IDC_BUTTON_SEND_MC, &Page2::OnBnClickedButtonSendMc)
	ON_BN_CLICKED(IDC_CHECK_TEMP_MODE, &Page2::OnBnClickedCheckTempMode)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_COOLER, &Page2::OnNMCustomdrawSliderCooler)
	ON_BN_CLICKED(IDC_CHECK_PRODUV, &Page2::OnBnClickedCheckMineral)
	ON_BN_CLICKED(IDC_CHECK_CO2_MODE, &Page2::OnBnClickedCheckCo2Mode)
	ON_BN_CLICKED(IDC_BUTTON_POROG1, &Page2::OnBnClickedButtonPorog1)
	ON_BN_CLICKED(IDC_BUTTON_POROG2, &Page2::OnBnClickedButtonPorog2)
	ON_BN_CLICKED(IDC_CHECK_DOLIV, &Page2::OnBnClickedCheckDoliv)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_PARAM, &Page2::OnBnClickedButtonEventParam)
END_MESSAGE_MAP()

BOOL Page2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (!m_pTip)
	{
		m_pTip =new CToolTipCtrl;
		m_pTip->Create(this,TTS_ALWAYSTIP); // подсказки активны во всех диалогах
		m_pTip->SetMaxTipWidth(200); // многострочный текст, количество символов
		m_pTip->SetDelayTime(900); // время отображения подсказок
		m_pTip->AddTool(&m_tip_check_temp_mode,_T("Температурный режим позволяет автоматически удерживать заданную температуру в пределах порога срабатывания."));
		m_pTip->AddTool(&m_tip_edit_temp_set,_T("Установите температуру для режима в формате <XX> 'C."));
		m_pTip->AddTool(&m_tip_edit_temp_porog,_T("Установите порог срабатывани для режима в формате <0.X> 'C."));
		m_pTip->AddTool(&m_slider_cooler,_T("Установите мощность кулеров для температурного режима."));
		//
		m_pTip->AddTool(&m_tip_check_produv,_T("Режим вентиляции крышки позволяет избавиться от накапливаемого конденсата."));
		m_pTip->AddTool(&m_edit_produv_dlit,_T("Время работы кулеров в формате <XX:XX> (мин:сек)."));
		m_pTip->AddTool(&m_combo_period,_T("Период включения кулеров."));
		//
		m_pTip->AddTool(&m_tip_check_co2_mode,_T("Режим PH/CO2 позволяет управлять подачей CO2 в зависимости от текущего значения PH."));
		m_pTip->AddTool(&m_tip_edit_ph_high,_T("Установите значение PH, при котором включатеся клапан CO2. Формат <X.XX> е.и."));
		m_pTip->AddTool(&m_tip_edit_ph_low,_T("Установите значение PH, при котором отключается клапан CO2. Формат <X.XX> е.и."));
		m_pTip->AddTool(&m_tip_edit_co2_stop,_T("Время отключения работы клапана CO2. Формат <XX> ч."));
		m_pTip->AddTool(&m_tip_edit_co2_start,_T("Время включения работы клапана CO2. Формат <XX> ч."));
		//
		m_pTip->AddTool(&m_tip_check_doliv,_T("Режим автоматического долива следит за уровнем воды в аквариуме, периодически проверяя работоспособность датчика уровня воды."));
		m_pTip->AddTool(&m_progress_doliv,_T("Текущий уровень воды в диапозоне, заданном значениями <Порог 1> и <Порог 2>."));
		//
		m_pTip->AddTool(&m_combo_temp_sensor,_T("Установите датчик для температурного режима."));
		m_pTip->AddTool(&m_tip_edit_power_cooler,_T("Установите значение минимальной мощности для кулеров."));

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

BOOL Page2::PreTranslateMessage(MSG* pMsg) 
{
	//перегрузка виртуальной функции PreTranslateMessage
	//если подсказки определены, их отображние
	if (m_pTip!=NULL)
            m_pTip->RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}

void Page2::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void Page2::OnSpinSet(CEdit &edit_control, CSpinButtonCtrl &spin_control, int timer_param)
{
	if(SPIN_NEW)
	{
		CString text;

		edit_control.GetWindowText(text);
		
		timer_produv.timer_set_sec[timer_param]=(text[3]-48)*10+(text[4]-48); 
		timer_produv.timer_set_min[timer_param]=(text[0]-48)*10+(text[1]-48);
		
		if(spin_control.GetPos32()) timer_produv.timer_set_min[timer_param]=timer_produv.timer_set_min[timer_param]+spin_control.GetPos32();
		else timer_produv.timer_set_min[timer_param]=timer_produv.timer_set_min[timer_param]-1;
		
		if(timer_produv.timer_set_min[timer_param]<0) 
		{
			timer_produv.timer_set_min[timer_param]=0;
		}
		if(timer_produv.timer_set_min[timer_param]==60)
		{
			spin_control.SetPos(0);
			timer_produv.timer_set_min[timer_param]=0;
		}
		
		timer_produv.timer_set_text[timer_param].Format(_T("%02d:%02d\n"),timer_produv.timer_set_min[timer_param],timer_produv.timer_set_sec[timer_param]);
		edit_control.SetWindowTextW(timer_produv.timer_set_text[timer_param]);
		
		spin_control.SetPos(0);
		SPIN_NEW=0;
	}
}

void Page2::OnTimer(UINT_PTR nIDEvent)
{
	OnSpinSet(m_edit_produv_dlit, m_spin_produv_dlit,0);

	m_progress_doliv.SetRange32(doliv_porog2,doliv_porog1);
	m_progress_doliv.SetPos(water_lvl);
	
	CDialog::OnTimer(nIDEvent);
}

int Page2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(0,1,NULL);
	
	return 0;
}

void Page2::OnDeltaposSpinMineralStop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	SPIN_NEW=1;

	*pResult = 0;
}

void Page2::OnBnClickedButtonSendMc()
{
	// TODO: Add your control notification handler code here
}

void Page2::OnBnClickedCheckTempMode()
{
	TRANSMIT_ERROR=1;
	temp_mode=(CButton*)GetDlgItem(IDC_CHECK_TEMP_MODE);
	if(temp_mode->GetCheck()) 
	{
		GetDlgItem(IDC_EDIT_TEMP_SET)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TEMP_POROG)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_TEMP_SENSOR)->EnableWindow(FALSE);
		temp_mode_set=true;
		condition=0x03;
		port_send=TRUE;
		GetDlgItem(IDC_SLIDER_COOLER)->EnableWindow(FALSE);
	}
	else
	{
		m_page1.CheckDlgButton(IDC_CHECK_COOLER,0); m_page1.rele_condition.cooler=0x30;
		GetDlgItem(IDC_SLIDER_COOLER)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_TEMP_SET)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_TEMP_POROG)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_TEMP_SENSOR)->EnableWindow(TRUE);
		temp_mode_set=false;
		condition=0x04;
		port_send=TRUE;
	}
}

void Page2::OnNMCustomdrawSliderCooler(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	if(temp_mode_set) condition=0x03;
	cooler_data=m_slider_cooler.GetPos();
	*pResult = 0;
}

void Page2::OnBnClickedCheckMineral()
{
	TRANSMIT_ERROR=1;
	CString text;
	UpdateData(true);
	m_edit_produv_dlit.GetWindowText(text);
	UpdateData(false);	
	timer_produv.timer_set_sec[0]=(text[3]-48)*10+(text[4]-48); 
	timer_produv.timer_set_min[0]=(text[0]-48)*10+(text[1]-48);

	produv_mode=(CButton*)GetDlgItem(IDC_CHECK_PRODUV);
	if(produv_mode->GetCheck()) 
	{
		GetDlgItem(IDC_EDIT_PRODUV_DLIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_MINERAL_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_PERIOD)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MIN_POWER_COOLER)->EnableWindow(FALSE);
		condition=0x06;
		port_send=TRUE;
	}
	else
	{
		GetDlgItem(IDC_EDIT_PRODUV_DLIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_MINERAL_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_PERIOD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MIN_POWER_COOLER)->EnableWindow(TRUE);
		condition=0x07;
		port_send=TRUE;
	}
}

void Page2::OnBnClickedCheckCo2Mode()
{
	TRANSMIT_ERROR=1;
	UpdateData(true);
	co2_mode=(CButton*)GetDlgItem(IDC_CHECK_CO2_MODE);
	UpdateData(false);	
	if(co2_mode->GetCheck()) 
	{
		GetDlgItem(IDC_EDIT_PH_HIGH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PH_LOW)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CO2_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CO2_START)->EnableWindow(FALSE);
		condition=0x11;
		port_send=TRUE;
	}
	else
	{
		GetDlgItem(IDC_EDIT_PH_HIGH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PH_LOW)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CO2_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CO2_START)->EnableWindow(TRUE);
		condition=0x12;
		port_send=TRUE;
	}
}

void Page2::OnBnClickedButtonPorog1()
{
	TRANSMIT_ERROR=1;
	condition=0x17;
	port_send=TRUE;
	GetDlgItem(IDC_BUTTON_POROG2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_POROG1)->EnableWindow(FALSE);
}

void Page2::OnBnClickedButtonPorog2()
{
	TRANSMIT_ERROR=1;
	condition=0x18;
	port_send=TRUE;
	GetDlgItem(IDC_BUTTON_POROG1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_POROG2)->EnableWindow(FALSE);
}

void Page2::OnBnClickedCheckDoliv()
{
	TRANSMIT_ERROR=1;
	doliv=(CButton*)GetDlgItem(IDC_CHECK_DOLIV);
	if(doliv->GetCheck()) 
	{
		condition=0x15;
		port_send=TRUE;
		GetDlgItem(IDC_BUTTON_POROG1)->EnableWindow(TRUE);
	}
	else
	{
		condition=0x16;
		port_send=TRUE;
		GetDlgItem(IDC_BUTTON_POROG1)->EnableWindow(FALSE);
	}
}

void Page2::OnBnClickedButtonEventParam()
{
	TRANSMIT_ERROR=1;
	condition=0x19;
	port_send=TRUE;
}
