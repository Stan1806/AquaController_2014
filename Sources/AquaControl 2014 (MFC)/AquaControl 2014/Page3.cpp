// Page3.cpp : implementation file
//

#include "stdafx.h"
#include "AquaControl 2014.h"
#include "AquaControl 2014Dlg.h"
#include "Page3.h"

bool SPIN_T1_START[9];
bool SPIN_T1_STOP[9];
bool SPIN_T2_START[9];
bool SPIN_T2_STOP[9];
bool SPIN_T3_START[9];
bool SPIN_T3_STOP[9];
// Page3 dialog

IMPLEMENT_DYNAMIC(Page3, CDialog)

Page3::Page3(CWnd* pParent /*=NULL*/)
	: CDialog(Page3::IDD, pParent)
{
	//начальная инициализация указателя
	m_pTip=NULL;
}

Page3::~Page3()
{
	//выделяли память...
	if (m_pTip!=NULL) delete m_pTip;
	//...очищаем память
}

void Page3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_LAMP1_T1_SET, m_edit_t1_set[0]);
	DDX_Control(pDX, IDC_SPIN_LAMP1_T1_SET, m_spin_t1_set[0]);
	DDX_Control(pDX, IDC_EDIT_LAMP2_T1_SET, m_edit_t1_set[1]);
	DDX_Control(pDX, IDC_EDIT_LAMP3_T1_SET, m_edit_t1_set[2]);
	DDX_Control(pDX, IDC_EDIT_LAMP4_T1_SET, m_edit_t1_set[3]);
	DDX_Control(pDX, IDC_EDIT_COMPRES_T1_SET, m_edit_t1_set[4]);
	DDX_Control(pDX, IDC_EDIT_HEATER_T1_SET, m_edit_t1_set[5]);
	DDX_Control(pDX, IDC_EDIT_CO2_T1_SET, m_edit_t1_set[6]);
	DDX_Control(pDX, IDC_EDIT_MINERAL_T1_SET, m_edit_t1_set[7]);
	DDX_Control(pDX, IDC_EDIT_COOLER_T1_SET, m_edit_t1_set[8]);

	DDX_Control(pDX, IDC_EDIT_LAMP1_T2_SET, m_edit_t2_set[0]);
	DDX_Control(pDX, IDC_SPIN_LAMP1_T2_SET, m_spin_t2_set[0]);
	DDX_Control(pDX, IDC_EDIT_LAMP2_T2_SET, m_edit_t2_set[1]);
	DDX_Control(pDX, IDC_EDIT_LAMP3_T2_SET, m_edit_t2_set[2]);
	DDX_Control(pDX, IDC_EDIT_LAMP4_T2_SET, m_edit_t2_set[3]);
	DDX_Control(pDX, IDC_EDIT_COMPRES_T2_SET, m_edit_t2_set[4]);
	DDX_Control(pDX, IDC_EDIT_HEATER_T2_SET, m_edit_t2_set[5]);
	DDX_Control(pDX, IDC_EDIT_CO2_T2_SET, m_edit_t2_set[6]);
	DDX_Control(pDX, IDC_EDIT_MINERAL_T2_SET, m_edit_t2_set[7]);
	DDX_Control(pDX, IDC_EDIT_COOLER_T2_SET, m_edit_t2_set[8]);

	DDX_Control(pDX, IDC_EDIT_LAMP1_T3_SET, m_edit_t3_set[0]);
	DDX_Control(pDX, IDC_SPIN_LAMP1_T3_SET, m_spin_t3_set[0]);
	DDX_Control(pDX, IDC_EDIT_LAMP2_T3_SET, m_edit_t3_set[1]);
	DDX_Control(pDX, IDC_EDIT_LAMP3_T3_SET, m_edit_t3_set[2]);
	DDX_Control(pDX, IDC_EDIT_LAMP4_T3_SET, m_edit_t3_set[3]);
	DDX_Control(pDX, IDC_EDIT_COMPRES_T3_SET, m_edit_t3_set[4]);
	DDX_Control(pDX, IDC_EDIT_HEATER_T3_SET, m_edit_t3_set[5]);
	DDX_Control(pDX, IDC_EDIT_CO2_T3_SET, m_edit_t3_set[6]);
	DDX_Control(pDX, IDC_EDIT_MINERAL_T3_SET, m_edit_t3_set[7]);
	DDX_Control(pDX, IDC_EDIT_COOLER_T3_SET, m_edit_t3_set[8]);
	////////////////////////////////////////////////////////////////
	DDX_Control(pDX, IDC_EDIT_LAMP1_T1_STOP, m_edit_t1_stop[0]);
	DDX_Control(pDX, IDC_SPIN_LAMP1_T1_STOP, m_spin_t1_stop[0]);
	DDX_Control(pDX, IDC_EDIT_LAMP2_T1_STOP, m_edit_t1_stop[1]);
	DDX_Control(pDX, IDC_EDIT_LAMP3_T1_STOP, m_edit_t1_stop[2]);
	DDX_Control(pDX, IDC_EDIT_LAMP4_T1_STOP, m_edit_t1_stop[3]);
	DDX_Control(pDX, IDC_EDIT_COMPRES_T1_STOP, m_edit_t1_stop[4]);
	DDX_Control(pDX, IDC_EDIT_HEATER_T1_STOP, m_edit_t1_stop[5]);
	DDX_Control(pDX, IDC_EDIT_CO2_T1_STOP, m_edit_t1_stop[6]);
	DDX_Control(pDX, IDC_EDIT_MINERAL_T1_STOP, m_edit_t1_stop[7]);
	DDX_Control(pDX, IDC_EDIT_COOLER_T1_STOP, m_edit_t1_stop[8]);

	DDX_Control(pDX, IDC_EDIT_LAMP1_T2_STOP, m_edit_t2_stop[0]);
	DDX_Control(pDX, IDC_SPIN_LAMP1_T2_STOP, m_spin_t2_stop[0]);
	DDX_Control(pDX, IDC_EDIT_LAMP2_T2_STOP, m_edit_t2_stop[1]);
	DDX_Control(pDX, IDC_EDIT_LAMP3_T2_STOP, m_edit_t2_stop[2]);
	DDX_Control(pDX, IDC_EDIT_LAMP4_T2_STOP, m_edit_t2_stop[3]);
	DDX_Control(pDX, IDC_EDIT_COMPRES_T2_STOP, m_edit_t2_stop[4]);
	DDX_Control(pDX, IDC_EDIT_HEATER_T2_STOP, m_edit_t2_stop[5]);
	DDX_Control(pDX, IDC_EDIT_CO2_T2_STOP, m_edit_t2_stop[6]);
	DDX_Control(pDX, IDC_EDIT_MINERAL_T2_STOP, m_edit_t2_stop[7]);
	DDX_Control(pDX, IDC_EDIT_COOLER_T2_STOP, m_edit_t2_stop[8]);

	DDX_Control(pDX, IDC_EDIT_LAMP1_T3_STOP, m_edit_t3_stop[0]);
	DDX_Control(pDX, IDC_SPIN_LAMP1_T3_STOP, m_spin_t3_stop[0]);
	DDX_Control(pDX, IDC_EDIT_LAMP2_T3_STOP, m_edit_t3_stop[1]);
	DDX_Control(pDX, IDC_EDIT_LAMP3_T3_STOP, m_edit_t3_stop[2]);
	DDX_Control(pDX, IDC_EDIT_LAMP4_T3_STOP, m_edit_t3_stop[3]);
	DDX_Control(pDX, IDC_EDIT_COMPRES_T3_STOP, m_edit_t3_stop[4]);
	DDX_Control(pDX, IDC_EDIT_HEATER_T3_STOP, m_edit_t3_stop[5]);
	DDX_Control(pDX, IDC_EDIT_CO2_T3_STOP, m_edit_t3_stop[6]);
	DDX_Control(pDX, IDC_EDIT_MINERAL_T3_STOP, m_edit_t3_stop[7]);
	DDX_Control(pDX, IDC_EDIT_COOLER_T3_STOP, m_edit_t3_stop[8]);

	/////////////////////////////////////////////////////////////////

	DDX_Control(pDX, IDC_SPIN_LAMP2_T1_SET, m_spin_t1_set[1]);
	DDX_Control(pDX, IDC_SPIN_LAMP3_T1_SET, m_spin_t1_set[2]);
	DDX_Control(pDX, IDC_SPIN_LAMP4_T1_SET, m_spin_t1_set[3]);
	DDX_Control(pDX, IDC_SPIN_COMPRES_T1_SET, m_spin_t1_set[4]);
	DDX_Control(pDX, IDC_SPIN_HEATER_T1_SET, m_spin_t1_set[5]);
	DDX_Control(pDX, IDC_SPIN_CO2_T1_SET, m_spin_t1_set[6]);
	DDX_Control(pDX, IDC_SPIN_MINERAL_T1_SET, m_spin_t1_set[7]);
	DDX_Control(pDX, IDC_SPIN_COOLER_T1_SET, m_spin_t1_set[8]);

	DDX_Control(pDX, IDC_SPIN_LAMP2_T2_SET, m_spin_t2_set[1]);
	DDX_Control(pDX, IDC_SPIN_LAMP3_T2_SET, m_spin_t2_set[2]);
	DDX_Control(pDX, IDC_SPIN_LAMP4_T2_SET, m_spin_t2_set[3]);
	DDX_Control(pDX, IDC_SPIN_COMPRES_T2_SET, m_spin_t2_set[4]);
	DDX_Control(pDX, IDC_SPIN_HEATER_T2_SET, m_spin_t2_set[5]);
	DDX_Control(pDX, IDC_SPIN_CO2_T2_SET, m_spin_t2_set[6]);
	DDX_Control(pDX, IDC_SPIN_MINERAL_T2_SET, m_spin_t2_set[7]);
	DDX_Control(pDX, IDC_SPIN_COOLER_T2_SET, m_spin_t2_set[8]);

	DDX_Control(pDX, IDC_SPIN_LAMP2_T3_SET, m_spin_t3_set[1]);
	DDX_Control(pDX, IDC_SPIN_LAMP3_T3_SET, m_spin_t3_set[2]);
	DDX_Control(pDX, IDC_SPIN_LAMP4_T3_SET, m_spin_t3_set[3]);
	DDX_Control(pDX, IDC_SPIN_COMPRES_T3_SET, m_spin_t3_set[4]);
	DDX_Control(pDX, IDC_SPIN_HEATER_T3_SET, m_spin_t3_set[5]);
	DDX_Control(pDX, IDC_SPIN_CO2_T3_SET, m_spin_t3_set[6]);
	DDX_Control(pDX, IDC_SPIN_MINERAL_T3_SET, m_spin_t3_set[7]);
	DDX_Control(pDX, IDC_SPIN_COOLER_T3_SET, m_spin_t3_set[8]);
	///////////////////////////////////////////////////////////////
	DDX_Control(pDX, IDC_SPIN_LAMP2_T1_STOP, m_spin_t1_stop[1]);
	DDX_Control(pDX, IDC_SPIN_LAMP3_T1_STOP, m_spin_t1_stop[2]);
	DDX_Control(pDX, IDC_SPIN_LAMP4_T1_STOP, m_spin_t1_stop[3]);
	DDX_Control(pDX, IDC_SPIN_COMPRES_T1_STOP, m_spin_t1_stop[4]);
	DDX_Control(pDX, IDC_SPIN_HEATER_T1_STOP, m_spin_t1_stop[5]);
	DDX_Control(pDX, IDC_SPIN_CO2_T1_STOP, m_spin_t1_stop[6]);
	DDX_Control(pDX, IDC_SPIN_MINERAL_T1_STOP, m_spin_t1_stop[7]);
	DDX_Control(pDX, IDC_SPIN_COOLER_T1_STOP, m_spin_t1_stop[8]);

	DDX_Control(pDX, IDC_SPIN_LAMP2_T2_STOP, m_spin_t2_stop[1]);
	DDX_Control(pDX, IDC_SPIN_LAMP3_T2_STOP, m_spin_t2_stop[2]);
	DDX_Control(pDX, IDC_SPIN_LAMP4_T2_STOP, m_spin_t2_stop[3]);
	DDX_Control(pDX, IDC_SPIN_COMPRES_T2_STOP, m_spin_t2_stop[4]);
	DDX_Control(pDX, IDC_SPIN_HEATER_T2_STOP, m_spin_t2_stop[5]);
	DDX_Control(pDX, IDC_SPIN_CO2_T2_STOP, m_spin_t2_stop[6]);
	DDX_Control(pDX, IDC_SPIN_MINERAL_T2_STOP, m_spin_t2_stop[7]);
	DDX_Control(pDX, IDC_SPIN_COOLER_T2_STOP, m_spin_t2_stop[8]);

	DDX_Control(pDX, IDC_SPIN_LAMP2_T3_STOP, m_spin_t3_stop[1]);
	DDX_Control(pDX, IDC_SPIN_LAMP3_T3_STOP, m_spin_t3_stop[2]);
	DDX_Control(pDX, IDC_SPIN_LAMP4_T3_STOP, m_spin_t3_stop[3]);
	DDX_Control(pDX, IDC_SPIN_COMPRES_T3_STOP, m_spin_t3_stop[4]);
	DDX_Control(pDX, IDC_SPIN_HEATER_T3_STOP, m_spin_t3_stop[5]);
	DDX_Control(pDX, IDC_SPIN_CO2_T3_STOP, m_spin_t3_stop[6]);
	DDX_Control(pDX, IDC_SPIN_MINERAL_T3_STOP, m_spin_t3_stop[7]);
	DDX_Control(pDX, IDC_SPIN_COOLER_T3_STOP, m_spin_t3_stop[8]);
	DDX_Control(pDX, IDC_STATIC_DEVICE, m_group_device);
	DDX_Control(pDX, IDC_STATIC_TIMER1, m_group_timer1);
	DDX_Control(pDX, IDC_STATIC_TIMER2, m_group_timer2);
	DDX_Control(pDX, IDC_STATIC_TIMER3, m_group_timer3);
}


BEGIN_MESSAGE_MAP(Page3, CDialog)
	ON_WM_CREATE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP1_T1_SET, &Page3::OnDeltaposSpinLamp1T1Set)
	ON_WM_TIMER()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP1_T1_STOP, &Page3::OnDeltaposSpinLamp1T1Stop)
	ON_BN_CLICKED(IDC_BUTTON1, &Page3::OnBnClickedButton1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP2_T1_SET, &Page3::OnDeltaposSpinLamp2T1Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP3_T1_SET, &Page3::OnDeltaposSpinLamp3T1Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP4_T1_SET, &Page3::OnDeltaposSpinLamp4T1Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COMPRES_T1_SET, &Page3::OnDeltaposSpinCompresT1Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEATER_T1_SET, &Page3::OnDeltaposSpinHeaterT1Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CO2_T1_SET, &Page3::OnDeltaposSpinCo2T1Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINERAL_T1_SET, &Page3::OnDeltaposSpinMineralT1Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COOLER_T1_SET, &Page3::OnDeltaposSpinCoolerT1Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP2_T1_STOP, &Page3::OnDeltaposSpinLamp2T1Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP3_T1_STOP, &Page3::OnDeltaposSpinLamp3T1Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP4_T1_STOP, &Page3::OnDeltaposSpinLamp4T1Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COMPRES_T1_STOP, &Page3::OnDeltaposSpinCompresT1Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEATER_T1_STOP, &Page3::OnDeltaposSpinHeaterT1Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CO2_T1_STOP, &Page3::OnDeltaposSpinCo2T1Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINERAL_T1_STOP, &Page3::OnDeltaposSpinMineralT1Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COOLER_T1_STOP, &Page3::OnDeltaposSpinCoolerT1Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP1_T2_SET, &Page3::OnDeltaposSpinLamp1T2Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP2_T2_SET, &Page3::OnDeltaposSpinLamp2T2Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP3_T2_SET, &Page3::OnDeltaposSpinLamp3T2Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP4_T2_SET, &Page3::OnDeltaposSpinLamp4T2Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COMPRES_T2_SET, &Page3::OnDeltaposSpinCompresT2Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEATER_T2_SET, &Page3::OnDeltaposSpinHeaterT2Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CO2_T2_SET, &Page3::OnDeltaposSpinCo2T2Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINERAL_T2_SET, &Page3::OnDeltaposSpinMineralT2Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COOLER_T2_SET, &Page3::OnDeltaposSpinCoolerT2Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP1_T2_STOP, &Page3::OnDeltaposSpinLamp1T2Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP2_T2_STOP, &Page3::OnDeltaposSpinLamp2T2Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP3_T2_STOP, &Page3::OnDeltaposSpinLamp3T2Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP4_T2_STOP, &Page3::OnDeltaposSpinLamp4T2Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COMPRES_T2_STOP, &Page3::OnDeltaposSpinCompresT2Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEATER_T2_STOP, &Page3::OnDeltaposSpinHeaterT2Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CO2_T2_STOP, &Page3::OnDeltaposSpinCo2T2Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINERAL_T2_STOP, &Page3::OnDeltaposSpinMineralT2Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COOLER_T2_STOP, &Page3::OnDeltaposSpinCoolerT2Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP1_T3_SET, &Page3::OnDeltaposSpinLamp1T3Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP2_T3_SET, &Page3::OnDeltaposSpinLamp2T3Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP3_T3_SET, &Page3::OnDeltaposSpinLamp3T3Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP4_T3_SET, &Page3::OnDeltaposSpinLamp4T3Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COMPRES_T3_SET, &Page3::OnDeltaposSpinCompresT3Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEATER_T3_SET, &Page3::OnDeltaposSpinHeaterT3Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CO2_T3_SET, &Page3::OnDeltaposSpinCo2T3Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINERAL_T3_SET, &Page3::OnDeltaposSpinMineralT3Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COOLER_T3_SET, &Page3::OnDeltaposSpinCoolerT3Set)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP1_T3_STOP, &Page3::OnDeltaposSpinLamp1T3Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP2_T3_STOP, &Page3::OnDeltaposSpinLamp2T3Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP3_T3_STOP, &Page3::OnDeltaposSpinLamp3T3Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAMP4_T3_STOP, &Page3::OnDeltaposSpinLamp4T3Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COMPRES_T3_STOP, &Page3::OnDeltaposSpinCompresT3Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEATER_T3_STOP, &Page3::OnDeltaposSpinHeaterT3Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CO2_T3_STOP, &Page3::OnDeltaposSpinCo2T3Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINERAL_T3_STOP, &Page3::OnDeltaposSpinMineralT3Stop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COOLER_T3_STOP, &Page3::OnDeltaposSpinCoolerT3Stop)
	ON_BN_CLICKED(IDC_BUTTON_READ_MC, &Page3::OnBnClickedButtonReadMc)
END_MESSAGE_MAP()


// Page3 message handlers
int Page3::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetTimer(0,1,NULL);
	return 0;
}

BOOL Page3::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (!m_pTip)
	{
		m_pTip =new CToolTipCtrl;
		m_pTip->Create(this,TTS_ALWAYSTIP); // подсказки активны во всех диалогах
		m_pTip->SetMaxTipWidth(200); // многострочный текст, количество символов
		m_pTip->SetDelayTime(900); // время отображения подсказок
		for(int i=0;i<9;i++)
		{
			m_pTip->AddTool(&m_edit_t1_set[i],_T("Время начала работы таймера 1. Формат <XX:XX:XX> (чч:мм:сс)."));
			m_pTip->AddTool(&m_edit_t2_set[i],_T("Время начала работы таймера 2. Формат <XX:XX:XX> (чч:мм:сс)."));
			m_pTip->AddTool(&m_edit_t3_set[i],_T("Время начала работы таймера 3. Формат <XX:XX:XX> (чч:мм:сс)."));
			//
			m_pTip->AddTool(&m_edit_t1_stop[i],_T("Время завершения работы таймера 1. Формат <XX:XX:XX> (чч:мм:сс)."));
			m_pTip->AddTool(&m_edit_t2_stop[i],_T("Время завершения работы таймера 2. Формат <XX:XX:XX> (чч:мм:сс)."));
			m_pTip->AddTool(&m_edit_t3_stop[i],_T("Время завершения работы таймера 3. Формат <XX:XX:XX> (чч:мм:сс)."));
		}
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

BOOL Page3::PreTranslateMessage(MSG* pMsg) 
{
	//перегрузка виртуальной функции PreTranslateMessage
	//если подсказки определены, их отображние
	if (m_pTip!=NULL)
            m_pTip->RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}

void Page3::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void Page3::OnDeltaposSpinLamp1T1Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);	
	SPIN_T1_START[0]=1;
	*pResult = 0;
}

void Page3::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CString text;
	for(int i=0;i<9;i++)
	{
		////////////////////////////////////////////
		if(SPIN_T1_START[i])
		{
			m_edit_t1_set[i].GetWindowText(text);
			
			timer_1.timer_set_sec[i]=(text[6]-48)*10+(text[7]-48);
			timer_1.timer_set_min[i]=(text[3]-48)*10+(text[4]-48); 
			timer_1.timer_set_hour[i]=(text[0]-48)*10+(text[1]-48);
			
			if(m_spin_t1_set[i].GetPos32()) timer_1.timer_set_min[i]=timer_1.timer_set_min[i]+m_spin_t1_set[i].GetPos32();
			else timer_1.timer_set_min[i]=timer_1.timer_set_min[i]-1;
			
			if(timer_1.timer_set_min[i]<0) 
			{
				timer_1.timer_set_min[i]=0;
				timer_1.timer_set_hour[i]--;
			}
			if(timer_1.timer_set_hour[i]<0) 
			{
				timer_1.timer_set_hour[i]=0;
			}
			if(timer_1.timer_set_min[i]==60)
			{
				m_spin_t1_set[i].SetPos(0);
				timer_1.timer_set_min[i]=0;
				timer_1.timer_set_hour[i]++;
			}
			if(timer_1.timer_set_hour[i]==24)
			{
				timer_1.timer_set_min[i]=0;
				timer_1.timer_set_hour[i]=0;
			}
			
			timer_1.timer_set_text[i].Format(_T("%02d:%02d:%02d\n"),timer_1.timer_set_hour[i],timer_1.timer_set_min[i],timer_1.timer_set_sec[i]);
			m_edit_t1_set[i].SetWindowTextW(timer_1.timer_set_text[i]);
			
			m_spin_t1_set[i].SetPos(0);

			SPIN_T1_START[i]=0;
		}
		/////////////////////////////////////////////////
		if(SPIN_T2_START[i])
		{
			m_edit_t2_set[i].GetWindowText(text);
			
			timer_2.timer_set_sec[i]=(text[6]-48)*10+(text[7]-48);
			timer_2.timer_set_min[i]=(text[3]-48)*10+(text[4]-48); 
			timer_2.timer_set_hour[i]=(text[0]-48)*10+(text[1]-48);
			
			if(m_spin_t2_set[i].GetPos32()) timer_2.timer_set_min[i]=timer_2.timer_set_min[i]+m_spin_t2_set[i].GetPos32();
			else timer_2.timer_set_min[i]=timer_2.timer_set_min[i]-1;
			
			if(timer_2.timer_set_min[i]<0) 
			{
				timer_2.timer_set_min[i]=0;
				timer_2.timer_set_hour[i]--;
			}
			if(timer_2.timer_set_hour[i]<0) 
			{
				timer_2.timer_set_hour[i]=0;
			}
			if(timer_2.timer_set_min[i]==60)
			{
				m_spin_t2_set[i].SetPos(0);
				timer_2.timer_set_min[i]=0;
				timer_2.timer_set_hour[i]++;
			}
			if(timer_2.timer_set_hour[i]==24)
			{
				timer_2.timer_set_min[i]=0;
				timer_2.timer_set_hour[i]=0;
			}
			
			timer_2.timer_set_text[i].Format(_T("%02d:%02d:%02d\n"),timer_2.timer_set_hour[i],timer_2.timer_set_min[i],timer_2.timer_set_sec[i]);
			m_edit_t2_set[i].SetWindowTextW(timer_2.timer_set_text[i]);
			
			m_spin_t2_set[i].SetPos(0);

			SPIN_T2_START[i]=0;
		}
		/////////////////////////////////////////////////
		if(SPIN_T3_START[i])
		{
			m_edit_t3_set[i].GetWindowText(text);
			
			timer_3.timer_set_sec[i]=(text[6]-48)*10+(text[7]-48);
			timer_3.timer_set_min[i]=(text[3]-48)*10+(text[4]-48); 
			timer_3.timer_set_hour[i]=(text[0]-48)*10+(text[1]-48);
			
			if(m_spin_t3_set[i].GetPos32()) timer_3.timer_set_min[i]=timer_3.timer_set_min[i]+m_spin_t3_set[i].GetPos32();
			else timer_3.timer_set_min[i]=timer_3.timer_set_min[i]-1;
			
			if(timer_3.timer_set_min[i]<0) 
			{
				timer_3.timer_set_min[i]=0;
				timer_3.timer_set_hour[i]--;
			}
			if(timer_3.timer_set_hour[i]<0) 
			{
				timer_3.timer_set_hour[i]=0;
			}
			if(timer_3.timer_set_min[i]==60)
			{
				m_spin_t3_set[i].SetPos(0);
				timer_3.timer_set_min[i]=0;
				timer_3.timer_set_hour[i]++;
			}
			if(timer_3.timer_set_hour[i]==24)
			{
				timer_3.timer_set_min[i]=0;
				timer_3.timer_set_hour[i]=0;
			}
			
			timer_3.timer_set_text[i].Format(_T("%02d:%02d:%02d\n"),timer_3.timer_set_hour[i],timer_3.timer_set_min[i],timer_3.timer_set_sec[i]);
			m_edit_t3_set[i].SetWindowTextW(timer_3.timer_set_text[i]);
			
			m_spin_t3_set[i].SetPos(0);

			SPIN_T3_START[i]=0;
		}
		/////////////////////////////////////////////////
		if(SPIN_T1_STOP[i])	
		{
			m_edit_t1_stop[i].GetWindowText(text);
			
			timer_1.timer_stop_sec[i]=(text[6]-48)*10+(text[7]-48);
			timer_1.timer_stop_min[i]=(text[3]-48)*10+(text[4]-48); 
			timer_1.timer_stop_hour[i]=(text[0]-48)*10+(text[1]-48);
			
			if(m_spin_t1_stop[i].GetPos32()) timer_1.timer_stop_min[i]=timer_1.timer_stop_min[i]+m_spin_t1_stop[i].GetPos32();
			else timer_1.timer_stop_min[i]=timer_1.timer_stop_min[i]-1;
			
			if(timer_1.timer_stop_min[i]<0) 
			{
				timer_1.timer_stop_min[i]=0;
				timer_1.timer_stop_hour[i]--;
			}
			if(timer_1.timer_stop_hour[i]<0) 
			{
				timer_1.timer_stop_hour[i]=0;
			}
			if(timer_1.timer_stop_min[i]==60)
			{
				m_spin_t1_stop[i].SetPos(0);
				timer_1.timer_stop_min[i]=0;
				timer_1.timer_stop_hour[i]++;
			}
			if(timer_1.timer_stop_hour[i]==24)
			{
				timer_1.timer_stop_min[i]=0;
				timer_1.timer_stop_hour[i]=0;
			}
			
			timer_1.timer_stop_text[i].Format(_T("%02d:%02d:%02d\n"),timer_1.timer_stop_hour[i],timer_1.timer_stop_min[i],timer_1.timer_stop_sec[i]);
			m_edit_t1_stop[i].SetWindowTextW(timer_1.timer_stop_text[i]);
			
			m_spin_t1_stop[i].SetPos(0);

			SPIN_T1_STOP[i]=0;
		}
		/////////////////////////////////////////////////
		if(SPIN_T2_STOP[i])	
		{
			m_edit_t2_stop[i].GetWindowText(text);
			
			timer_2.timer_stop_sec[i]=(text[6]-48)*10+(text[7]-48);
			timer_2.timer_stop_min[i]=(text[3]-48)*10+(text[4]-48); 
			timer_2.timer_stop_hour[i]=(text[0]-48)*10+(text[1]-48);
			
			if(m_spin_t2_stop[i].GetPos32()) timer_2.timer_stop_min[i]=timer_2.timer_stop_min[i]+m_spin_t2_stop[i].GetPos32();
			else timer_2.timer_stop_min[i]=timer_2.timer_stop_min[i]-1;
			
			if(timer_2.timer_stop_min[i]<0) 
			{
				timer_2.timer_stop_min[i]=0;
				timer_2.timer_stop_hour[i]--;
			}
			if(timer_2.timer_stop_hour[i]<0) 
			{
				timer_2.timer_stop_hour[i]=0;
			}
			if(timer_2.timer_stop_min[i]==60)
			{
				m_spin_t2_stop[i].SetPos(0);
				timer_2.timer_stop_min[i]=0;
				timer_2.timer_stop_hour[i]++;
			}
			if(timer_2.timer_stop_hour[i]==24)
			{
				timer_2.timer_stop_min[i]=0;
				timer_2.timer_stop_hour[i]=0;
			}
			
			timer_2.timer_stop_text[i].Format(_T("%02d:%02d:%02d\n"),timer_2.timer_stop_hour[i],timer_2.timer_stop_min[i],timer_2.timer_stop_sec[i]);
			m_edit_t2_stop[i].SetWindowTextW(timer_2.timer_stop_text[i]);
			
			m_spin_t2_stop[i].SetPos(0);

			SPIN_T2_STOP[i]=0;
		}
		/////////////////////////////////////////////////
		if(SPIN_T3_STOP[i])	
		{
			m_edit_t3_stop[i].GetWindowText(text);
			
			timer_3.timer_stop_sec[i]=(text[6]-48)*10+(text[7]-48);
			timer_3.timer_stop_min[i]=(text[3]-48)*10+(text[4]-48); 
			timer_3.timer_stop_hour[i]=(text[0]-48)*10+(text[1]-48);
			
			if(m_spin_t3_stop[i].GetPos32()) timer_3.timer_stop_min[i]=timer_3.timer_stop_min[i]+m_spin_t3_stop[i].GetPos32();
			else timer_3.timer_stop_min[i]=timer_3.timer_stop_min[i]-1;
			
			if(timer_3.timer_stop_min[i]<0) 
			{
				timer_3.timer_stop_min[i]=0;
				timer_3.timer_stop_hour[i]--;
			}
			if(timer_3.timer_stop_hour[i]<0) 
			{
				timer_3.timer_stop_hour[i]=0;
			}
			if(timer_3.timer_stop_min[i]==60)
			{
				m_spin_t3_stop[i].SetPos(0);
				timer_3.timer_stop_min[i]=0;
				timer_3.timer_stop_hour[i]++;
			}
			if(timer_3.timer_stop_hour[i]==24)
			{
				timer_3.timer_stop_min[i]=0;
				timer_3.timer_stop_hour[i]=0;
			}
			
			timer_3.timer_stop_text[i].Format(_T("%02d:%02d:%02d\n"),timer_3.timer_stop_hour[i],timer_3.timer_stop_min[i],timer_3.timer_stop_sec[i]);
			m_edit_t3_stop[i].SetWindowTextW(timer_3.timer_stop_text[i]);
			
			m_spin_t3_stop[i].SetPos(0);

			SPIN_T3_STOP[i]=0;
		}
	}
	CDialog::OnTimer(nIDEvent);
}
void Page3::OnDeltaposSpinLamp1T1Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_STOP[0]=1;
	*pResult = 0;
}

void Page3::OnBnClickedButton1()
{
	TRANSMIT_ERROR=1;
	CString text;
	UpdateData(true);
	
	for(int i=0;i<9;i++)
	{
		m_edit_t1_set[i].GetWindowText(text);
			
		timer_1.timer_set_sec[i]=(text[6]-48)*10+(text[7]-48);
		timer_1.timer_set_min[i]=(text[3]-48)*10+(text[4]-48); 
		timer_1.timer_set_hour[i]=(text[0]-48)*10+(text[1]-48);

		m_edit_t2_set[i].GetWindowText(text);
			
		timer_2.timer_set_sec[i]=(text[6]-48)*10+(text[7]-48);
		timer_2.timer_set_min[i]=(text[3]-48)*10+(text[4]-48); 
		timer_2.timer_set_hour[i]=(text[0]-48)*10+(text[1]-48);

		m_edit_t3_set[i].GetWindowText(text);
			
		timer_3.timer_set_sec[i]=(text[6]-48)*10+(text[7]-48);
		timer_3.timer_set_min[i]=(text[3]-48)*10+(text[4]-48); 
		timer_3.timer_set_hour[i]=(text[0]-48)*10+(text[1]-48);

		///////////////////////////

		m_edit_t1_stop[i].GetWindowText(text);
			
		timer_1.timer_stop_sec[i]=(text[6]-48)*10+(text[7]-48);
		timer_1.timer_stop_min[i]=(text[3]-48)*10+(text[4]-48); 
		timer_1.timer_stop_hour[i]=(text[0]-48)*10+(text[1]-48);

		m_edit_t2_stop[i].GetWindowText(text);
			
		timer_2.timer_stop_sec[i]=(text[6]-48)*10+(text[7]-48);
		timer_2.timer_stop_min[i]=(text[3]-48)*10+(text[4]-48); 
		timer_2.timer_stop_hour[i]=(text[0]-48)*10+(text[1]-48);

		m_edit_t3_stop[i].GetWindowText(text);
			
		timer_3.timer_stop_sec[i]=(text[6]-48)*10+(text[7]-48);
		timer_3.timer_stop_min[i]=(text[3]-48)*10+(text[4]-48); 
		timer_3.timer_stop_hour[i]=(text[0]-48)*10+(text[1]-48);
	}
	UpdateData(false);
	
	condition=0x05;
	port_send=TRUE;
}

void Page3::OnDeltaposSpinLamp2T1Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_START[1]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp3T1Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_START[2]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp4T1Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_START[3]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCompresT1Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_START[4]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinHeaterT1Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_START[5]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCo2T1Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_START[6]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinMineralT1Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_START[7]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCoolerT1Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_START[8]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp2T1Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_STOP[1]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp3T1Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_STOP[2]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp4T1Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_STOP[3]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCompresT1Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_STOP[4]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinHeaterT1Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_STOP[5]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCo2T1Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_STOP[6]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinMineralT1Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_STOP[7]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCoolerT1Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T1_STOP[8]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp1T2Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_START[0]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp2T2Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_START[1]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp3T2Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_START[2]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp4T2Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_START[3]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCompresT2Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_START[4]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinHeaterT2Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_START[5]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCo2T2Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_START[6]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinMineralT2Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_START[7]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCoolerT2Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_START[8]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp1T2Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_STOP[0]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp2T2Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_STOP[1]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp3T2Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_STOP[2]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp4T2Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_STOP[3]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCompresT2Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_STOP[4]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinHeaterT2Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_STOP[5]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCo2T2Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_STOP[6]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinMineralT2Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_STOP[7]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCoolerT2Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T2_STOP[8]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp1T3Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_START[0]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp2T3Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_START[1]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp3T3Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_START[2]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp4T3Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_START[3]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCompresT3Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_START[4]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinHeaterT3Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_START[5]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCo2T3Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_START[6]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinMineralT3Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_START[7]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCoolerT3Set(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_START[8]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp1T3Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_STOP[0]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp2T3Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_STOP[1]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp3T3Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_STOP[2]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinLamp4T3Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_STOP[3]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCompresT3Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_STOP[4]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinHeaterT3Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_STOP[5]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCo2T3Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_STOP[6]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinMineralT3Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_STOP[7]=1;
	*pResult = 0;
}

void Page3::OnDeltaposSpinCoolerT3Stop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SPIN_T3_STOP[8]=1;
	*pResult = 0;
}

void Page3::OnBnClickedButtonReadMc()
{
	TRANSMIT_ERROR=1;
	condition=0x14;
	port_send=TRUE;
}
