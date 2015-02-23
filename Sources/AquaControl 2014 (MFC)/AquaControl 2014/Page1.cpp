// Page1.cpp : implementation file
//

#include "stdafx.h"
#include "AquaControl 2014.h"
#include "AquaControl 2014Dlg.h"
#include "Page1.h"

IMPLEMENT_DYNAMIC(Page1, CDialog)

Page1::Page1(CWnd* pParent /*=NULL*/)
	: CDialog(Page1::IDD, pParent)
	, m_temp_1(0)
	, m_temp_2(0)
	, m_temp_3(0)
	, m_edit_ph(0)
	, m_edit_redox(0)
	, m_edit_hour(0)
	, m_edit_min(0)
	, m_edit_sec(0)
	, m_baudrate(9600)
	, m_num_ports(0)
	, m_edit_led_stop(23)
	, m_edit_led_start(7)
	, m_edit_cooler_stop(23)
	, m_edit_cooler_start(7)
	, condition(0)
{
	//начальная инициализация указателя
	m_pTip=NULL;
}

Page1::~Page1()
{
	//выделяли память...
	if (m_pTip!=NULL) delete m_pTip;
	//...очищаем память
}

void Page1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_T1, m_temp_1);
	DDX_Text(pDX, IDC_EDIT_T2, m_temp_2);
	DDX_Text(pDX, IDC_EDIT_T3, m_temp_3);
	DDX_Text(pDX, IDC_EDIT_PH, m_edit_ph);
	DDX_Text(pDX, IDC_EDIT_REDOX, m_edit_redox);
	DDX_Control(pDX, IDC_SLIDER_COOLER, m_slider_cooler);
	DDX_Text(pDX, IDC_EDIT_HOUR, m_edit_hour);
	DDX_Text(pDX, IDC_EDIT_MIN, m_edit_min);
	DDX_Text(pDX, IDC_EDIT_SEC, m_edit_sec);
	DDX_Control(pDX, IDC_STATIC_GROUP, m_group_temp);
	DDX_Control(pDX, IDC_STATIC_WATER_PARAM, m_group_water_param);
	DDX_Control(pDX, IDC_STATIC_DS1307, m_group_ds1307);
	DDX_Control(pDX, IDC_STATIC_DEVICE_SELECT, m_group_device_select);
	DDX_Control(pDX, IDC_STATIC_COM, m_group_com);
	DDX_Control(pDX, IDC_STATIC_LED, m_group_led);
	DDX_Control(pDX, IDC_STATIC_COOLER, m_group_cooler);
	DDX_Control(pDX, IDC_STATIC_OPTION, m_group_option);
	DDX_Control(pDX, IDC_EDIT_LAMP1, m_edit_lamp1);
	DDX_Control(pDX, IDC_EDIT_LAMP2, m_edit_lamp2);
	DDX_Control(pDX, IDC_EDIT_LAMP3, m_edit_lamp3);
	DDX_Control(pDX, IDC_EDIT_LAMP4, m_edit_lamp4);
	DDX_Control(pDX, IDC_EDIT_COMPRES, m_edit_compres);
	DDX_Control(pDX, IDC_EDIT_HEATER, m_edit_heater);
	DDX_Control(pDX, IDC_EDIT_CO2, m_edit_co2);
	DDX_Control(pDX, IDC_EDIT_POMPA, m_edit_pompa);
	DDX_Control(pDX, IDC_EDIT_COOLER, m_edit_cooler);
	DDX_Control(pDX, IDC_COMBO_PORT, m_combo_port);
	DDX_Text(pDX, IDC_EDIT_BAUDRATE, m_baudrate);
	DDX_Text(pDX, IDC_EDIT_NUM_PORTS, m_num_ports);
	DDX_Text(pDX, IDC_EDIT_LED_STOP, m_edit_led_stop);
	DDX_Text(pDX, IDC_EDIT_LED_START, m_edit_led_start);
	DDX_Text(pDX, IDC_EDIT_COOLER_STOP, m_edit_cooler_stop);
	DDX_Text(pDX, IDC_EDIT_COOLER_START, m_edit_cooler_start);
	DDX_Control(pDX, IDC_CHECK_LAMP1, m_tip_check_lamp_1);
	DDX_Control(pDX, IDC_CHECK_LAMP2, m_tip_check_lamp_2);
	DDX_Control(pDX, IDC_CHECK_LAMP3, m_tip_check_lamp_3);
	DDX_Control(pDX, IDC_CHECK_LAMP4, m_tip_check_lamp_4);
	DDX_Control(pDX, IDC_CHECK_HEATER, m_tip_check_heater);
	DDX_Control(pDX, IDC_CHECK_CO2, m_tip_check_co2);
	DDX_Control(pDX, IDC_CHECK_POMPA, m_tip_check_pompa);
	DDX_Control(pDX, IDC_CHECK_COMPRES, m_tip_check_compres);
	DDX_Control(pDX, IDC_CHECK_COOLER, m_tip_check_cooler);
	DDX_Control(pDX, IDC_EDIT_T1, m_tip_edit_t1);
	DDX_Control(pDX, IDC_EDIT_T2, m_tip_edit_t2);
	DDX_Control(pDX, IDC_EDIT_T3, m_tip_edit_t3);
	DDX_Control(pDX, IDC_EDIT_PH, m_tip_edit_ph);
	DDX_Control(pDX, IDC_EDIT_REDOX, m_tip_edit_redox);
	DDX_Control(pDX, IDC_EDIT_HOUR, m_tip_edit_hour);
	DDX_Control(pDX, IDC_EDIT_MIN, m_tip_edit_min);
	DDX_Control(pDX, IDC_EDIT_SEC, m_tip_edit_sec);
	DDX_Control(pDX, IDC_EDIT_BAUDRATE, m_tip_edit_baudrate);
	DDX_Control(pDX, IDC_EDIT_NUM_PORTS, m_tip_edit_num_ports);
	DDX_Control(pDX, IDC_EDIT_LED_STOP, m_tip_edit_led_stop);
	DDX_Control(pDX, IDC_EDIT_LED_START, m_tip_edit_led_start);
	DDX_Control(pDX, IDC_EDIT_COOLER_STOP, m_tip_edit_cooler_stop);
	DDX_Control(pDX, IDC_EDIT_COOLER_START, m_tip_edit_cooler_start);
}


BEGIN_MESSAGE_MAP(Page1, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SYNC, &Page1::OnBnClickedButtonSync)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_OPEN_PORT, &Page1::OnBnClickedOpenPort)
	ON_BN_CLICKED(IDC_CLOSE_PORT, &Page1::OnBnClickedClosePort)
	ON_BN_CLICKED(IDC_FIND_PORT, &Page1::OnBnClickedFindPort)
	ON_BN_CLICKED(IDC_CHECK_LAMP1, &Page1::OnBnClickedCheckLamp1)
	ON_BN_CLICKED(IDC_CHECK_LAMP2, &Page1::OnBnClickedCheckLamp2)
	ON_BN_CLICKED(IDC_CHECK_LAMP3, &Page1::OnBnClickedCheckLamp3)
	ON_BN_CLICKED(IDC_CHECK_LAMP4, &Page1::OnBnClickedCheckLamp4)
	ON_BN_CLICKED(IDC_CHECK_COMPRES, &Page1::OnBnClickedCheckCompres)
	ON_BN_CLICKED(IDC_CHECK_HEATER, &Page1::OnBnClickedCheckHeater)
	ON_BN_CLICKED(IDC_CHECK_CO2, &Page1::OnBnClickedCheckCo2)
	ON_BN_CLICKED(IDC_CHECK_COOLER, &Page1::OnBnClickedCheckCooler)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_COOLER, &Page1::OnNMCustomdrawSliderCooler)
	ON_BN_CLICKED(IDC_BUTTON_READ, &Page1::OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_LED_SET, &Page1::OnBnClickedButtonLedSet)
	ON_BN_CLICKED(IDC_BUTTON_COOLER_SET, &Page1::OnBnClickedButtonCoolerSet)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_SEND, &Page1::OnBnClickedButtonSaveSend)
	ON_BN_CLICKED(IDC_CHECK_POMPA, &Page1::OnBnClickedCheckPompa)
END_MESSAGE_MAP()

CAquaControl2014Dlg* AquaControl2014Dlg;
// Page1 message handlers

void Page1::OnBnClickedButtonSync()
{
	condition=0x02;
	port_send=TRUE;
}

HBRUSH Page1::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

BOOL Page1::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (!m_pTip)
	{
		m_pTip =new CToolTipCtrl;
		m_pTip->Create(this,TTS_ALWAYSTIP); // подсказки активны во всех диалогах
		m_pTip->SetMaxTipWidth(200); // многострочный текст, количество символов
		m_pTip->SetDelayTime(900); // время отображения подсказок
		m_pTip->AddTool(&m_tip_check_lamp_1,_T("Ручное управление каналом <Освещение 1>."));
		m_pTip->AddTool(&m_tip_check_lamp_2,_T("Ручное управление каналом <Освещение 2>."));
		m_pTip->AddTool(&m_tip_check_lamp_3,_T("Ручное управление каналом <Освещение 3>."));
		m_pTip->AddTool(&m_tip_check_lamp_4,_T("Ручное управление каналом <Освещение 4>."));
		m_pTip->AddTool(&m_tip_check_heater,_T("Ручное управление каналом <Нагреватель>."));
		m_pTip->AddTool(&m_tip_check_co2,_T("Ручное управление каналом <Клапан CO2>."));
		m_pTip->AddTool(&m_tip_check_pompa,_T("Ручное управление каналом <Помпа>."));
		m_pTip->AddTool(&m_tip_check_compres,_T("Ручное управление каналом <Компрессор>."));
		m_pTip->AddTool(&m_tip_check_cooler,_T("Ручное управление каналом <Кулера 12В>."));
		//
		m_pTip->AddTool(&m_edit_lamp1,_T("Текущий статус канала <Освещение 1>."));
		m_pTip->AddTool(&m_edit_lamp2,_T("Текущий статус канала <Освещение 2>."));
		m_pTip->AddTool(&m_edit_lamp3,_T("Текущий статус канала <Освещение 3>."));
		m_pTip->AddTool(&m_edit_lamp4,_T("Текущий статус канала <Освещение 4>."));
		m_pTip->AddTool(&m_edit_heater,_T("Текущий статус канала <Нагреватель>."));
		m_pTip->AddTool(&m_edit_co2,_T("Текущий статус канала <Клапан CO2>."));
		m_pTip->AddTool(&m_edit_pompa,_T("Текущий статус канала <Помпа>."));
		m_pTip->AddTool(&m_edit_compres,_T("Текущий статус канала <Компрессор>."));
		m_pTip->AddTool(&m_edit_cooler,_T("Текущий статус канала <Кулера 12В>."));
		//
		m_pTip->AddTool(&m_slider_cooler,_T("Мощность кулеров в ручном режиме."));
		//
		m_pTip->AddTool(&m_tip_edit_t1,_T("Температура 1-го датчика."));
		m_pTip->AddTool(&m_tip_edit_t2,_T("Температура 2-го датчика."));
		m_pTip->AddTool(&m_tip_edit_t3,_T("Температура 3-го датчика."));
		//
		m_pTip->AddTool(&m_tip_edit_ph,_T("Значение PH-электрода."));
		m_pTip->AddTool(&m_tip_edit_redox,_T("Значение Redox-электрода."));
		//
		m_pTip->AddTool(&m_tip_edit_hour,_T("Системное время: Часы."));
		m_pTip->AddTool(&m_tip_edit_min,_T("Системное время: Минуты."));
		m_pTip->AddTool(&m_tip_edit_sec,_T("Системное время: Секунды."));
		//
		m_pTip->AddTool(&m_combo_port,_T("COM-порт канала связи с устройством."));
		m_pTip->AddTool(&m_tip_edit_baudrate,_T("Скорость передачи данных."));
		m_pTip->AddTool(&m_tip_edit_num_ports,_T("Количество активных COM-портов в системе."));
		//
		m_pTip->AddTool(&m_tip_edit_led_stop,_T("Время отключения подсветки ЖКИ. Формат <XX> ч."));
		m_pTip->AddTool(&m_tip_edit_led_start,_T("Время включения подсветки ЖКИ. Формат <XX> ч."));
		//
		m_pTip->AddTool(&m_tip_edit_cooler_stop,_T("Время отключения кулеров."));
		m_pTip->AddTool(&m_tip_edit_cooler_start,_T("Время включения кулеров."));

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

BOOL Page1::PreTranslateMessage(MSG* pMsg) 
{
	//перегрузка виртуальной функции PreTranslateMessage
	//если подсказки определены, их отображние
	if (m_pTip!=NULL)
            m_pTip->RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}

int Page1::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	port_btn_open=NULL;
	port_btn_close=NULL;
	port_btn_num=NULL;
	return 0;
}

void Page1::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void Page1::OnBnClickedOpenPort()
{
	port_btn_open=TRUE;
}

void Page1::OnBnClickedClosePort()
{
	port_btn_close=TRUE;
}

void Page1::OnBnClickedFindPort()
{
	port_btn_num=TRUE;
}

void Page1::OnBnClickedCheckLamp1()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	lamp1=(CButton*)GetDlgItem(IDC_CHECK_LAMP1);
	if(lamp1->GetCheck()) 
	{
		rele_condition.lamp1=0x31;
		port_send=TRUE;
	}
	else
	{
		rele_condition.lamp1=0x30;
		port_send=TRUE;
	}
}

void Page1::OnBnClickedCheckLamp2()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	lamp2=(CButton*)GetDlgItem(IDC_CHECK_LAMP2);
	if(lamp2->GetCheck()) 
	{
		rele_condition.lamp2=0x31;
		port_send=TRUE;
	}
	else
	{
		rele_condition.lamp2=0x30;
		port_send=TRUE;
	}
}

void Page1::OnBnClickedCheckLamp3()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	lamp3=(CButton*)GetDlgItem(IDC_CHECK_LAMP3);
	if(lamp3->GetCheck()) 
	{
		rele_condition.lamp3=0x31;
		port_send=TRUE;
	}
	else
	{
		rele_condition.lamp3=0x30;
		port_send=TRUE;
	}
}

void Page1::OnBnClickedCheckLamp4()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	lamp4=(CButton*)GetDlgItem(IDC_CHECK_LAMP4);
	if(lamp4->GetCheck()) 
	{
		rele_condition.lamp4=0x31;
		port_send=TRUE;
	}
	else
	{
		rele_condition.lamp4=0x30;
		port_send=TRUE;
	}
}

void Page1::OnBnClickedCheckCompres()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	compres=(CButton*)GetDlgItem(IDC_CHECK_COMPRES);
	if(compres->GetCheck()) 
	{
		rele_condition.compres=0x31;
		port_send=TRUE;
	}
	else
	{
		rele_condition.compres=0x30;
		port_send=TRUE;
	}
}

void Page1::OnBnClickedCheckHeater()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	heater=(CButton*)GetDlgItem(IDC_CHECK_HEATER);
	if(heater->GetCheck()) 
	{
		rele_condition.heater=0x31;
		port_send=TRUE;
	}
	else
	{
		rele_condition.heater=0x30;
		port_send=TRUE;
	}
}

void Page1::OnBnClickedCheckCo2()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	co2=(CButton*)GetDlgItem(IDC_CHECK_CO2);
	if(co2->GetCheck()) 
	{
		rele_condition.co2=0x31;
		port_send=TRUE;
	}
	else
	{
		rele_condition.co2=0x30;
		port_send=TRUE;
	}
}

void Page1::OnBnClickedCheckCooler()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	cooler=(CButton*)GetDlgItem(IDC_CHECK_COOLER);
	if(cooler->GetCheck()) 
	{
		rele_condition.cooler=0x31;
		port_send=TRUE;
	}
	else
	{
		rele_condition.cooler=0x30;
		rele_condition.cooler_data=0;
		port_send=TRUE;
	}
}

void Page1::OnNMCustomdrawSliderCooler(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	rele_condition.cooler_data=m_slider_cooler.GetPos();

	*pResult = 0;
}

void Page1::OnBnClickedButtonRead()
{
	
}

void Page1::OnBnClickedButtonLedSet()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	port_send=TRUE;
}

void Page1::OnBnClickedButtonCoolerSet()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	port_send=TRUE;
}

void Page1::OnBnClickedButtonSaveSend()
{
	TRANSMIT_ERROR=1;
	condition=0x13;
	port_send=TRUE;
}

void Page1::OnBnClickedCheckPompa()
{
	TRANSMIT_ERROR=1;
	condition=0x01;
	pompa=(CButton*)GetDlgItem(IDC_CHECK_MINERAL);
	if(pompa->GetCheck()) 
	{
		rele_condition.pompa=0x31;
		port_send=TRUE;
	}
	else
	{
		rele_condition.pompa=0x30;
		port_send=TRUE;
	}
}
