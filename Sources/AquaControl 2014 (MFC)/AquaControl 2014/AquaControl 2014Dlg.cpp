// AquaControl 2014Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "AquaControl 2014.h"
#include "AquaControl 2014Dlg.h"
#include "Page1.h"
#include "Page2.h"
#include "Page3.h"
#include "Page4.h"
#include "LineChartCtrl.h"
#include "SerialPort.h"
#include "MainOption.h"

bool port_send;
unsigned int water_lvl;
unsigned int doliv_porog1;
unsigned int doliv_porog2;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	WM_ICON_NOTIFY			WM_APP+10

Page1 m_page1;
Page2 m_page2;
Page3 m_page3;
Page4 m_page4;
CString sys_time;
bool port_open; // детекция открытия порта
int port_num=0; // количество доступных портов
unsigned char data_to_send[512];
unsigned char com_data_in[512];
unsigned int com_data_count=0;
bool com_data_read=false;
unsigned char com_package=0xff;
bool compare=true;
CString str;
bool get_chanel_status=true;
char file_buf[5]={0x30,0x30,0x30,0x30,0x30};
bool COM_ON_START=false;
bool TIME_SYNC=false;
unsigned char TRANSMIT_ERROR=1;
bool TIMERS_START_READ=false;
bool EVENT_PARAM_START_READ=false;
// CAboutDlg dialog used for App About

struct grafic
{
	double temperature[3];
	double ph;
	double redox;
};

struct grafic graf_data;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAquaControl2014Dlg dialog

CAquaControl2014Dlg::CAquaControl2014Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAquaControl2014Dlg::IDD, pParent)
	,m_graf(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//начальная инициализация указателя
	m_pTip=NULL;
}

CAquaControl2014Dlg::~CAquaControl2014Dlg()
{
	//выделяли память...
	if (m_pTip!=NULL) delete m_pTip;
	//...очищаем память
	if(file_buf[1]==0x31) 
	{
		data_to_send[0]=0x3b;
		data_to_send[1]=0x13;
		data_to_send[2]=0xb3;

		if(port_open==TRUE) // проверка состояния порта
		{
			m_port->WriteToPort((char*)data_to_send, 3);
		}
	}
	if (!m_port)
	{
		delete m_port; // возвращение выделенной памяти
	}
}

void CAquaControl2014Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tab);
	DDX_Control(pDX, IDC_STATIC_DINAMIC, m_group_dinamic);
	DDX_Control(pDX, IDC_STATIC_DINAMIC_TEMP, m_group_dinamic_temp);
	DDX_Control(pDX, IDC_STATIC_DINAMIC_PH, m_group_dinamic_ph);
	DDX_Control(pDX, IDC_STATIC_DINAMIC_REDOX, m_group_dinamic_redox);
	DDX_Control(pDX, IDC_EDIT_SYSTEM_TIME, m_edit_system_time);
	DDX_Control(pDX, IDC_SLIDER_GRAF, m_slider_graf);
}

BEGIN_MESSAGE_MAP(CAquaControl2014Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CAquaControl2014Dlg::OnTcnSelchangeTab)
	ON_MESSAGE(WM_COMM_RXCHAR, OnCommunication)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_GRAF, &CAquaControl2014Dlg::OnNMCustomdrawSliderGraf)
	ON_COMMAND(ID_RECIEVE_DATA, OnRecieveData)
	ON_COMMAND(ID_SPRAVKA, OnSpravka)
	ON_COMMAND(ID_TRAY, OnTray)
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	ON_COMMAND(ID_DEMO_MINIMISE, OnMinimise)
	ON_COMMAND(ID_DEMO_MAXIMISE, OnMaximise)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CAquaControl2014Dlg message handlers

BOOL CAquaControl2014Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_page1.m_slider_cooler.SetRange(25,100,1);
	m_page2.m_slider_cooler.SetRange(25,100,1);

	m_tab.InsertItem(0,_T("Отображение информации"));
	m_tab.InsertItem(1,_T("Режимы/События"));
	m_tab.InsertItem(2,_T("Ежедневные Таймеры"));
	m_tab.InsertItem(3,_T("Калибровка электродов"));
	m_page1.Create(IDD_PAGE1);
	m_page2.Create(IDD_PAGE2);
	m_page3.Create(IDD_PAGE3);
	m_page4.Create(IDD_PAGE4);
	RECT R=GetPageRect(m_tab);
	m_page1.MoveWindow(&R, FALSE);
	m_page2.MoveWindow(&R, FALSE);
	m_page3.MoveWindow(&R, FALSE);
	m_page4.MoveWindow(&R, FALSE);
	m_page1.ShowWindow(SW_NORMAL);

	m_wndLineChart_1.SubclassDlgItem(IDC_CUSTOM_GRAF1, this);
	m_wndLineChart_2.SubclassDlgItem(IDC_CUSTOM_GRAF2, this);
	m_wndLineChart_3.SubclassDlgItem(IDC_CUSTOM_GRAF3, this);
	m_wndLineChart_1.Add(RGB(255,0,0),200, 0); // создание черного графика
	m_wndLineChart_1.Add(RGB(0,255,0),200, 1); // создание черного графика
	m_wndLineChart_1.Add(RGB(0,0,255),200, 2); // создание черного графика
	m_wndLineChart_2.Add(RGB(255,0,0),200, 0); // создание черного графика
	m_wndLineChart_3.Add(RGB(255,0,0),200, 0); // создание черного графика

	CString text;
	text.Format(_T("%02d:%02d:%02d\n"),0,0,0);
	
	for(int i=0;i<9;i++) m_page3.m_edit_t1_set[i].SetWindowTextW(text);
	for(int i=0;i<9;i++) m_page3.m_edit_t2_set[i].SetWindowTextW(text);
	for(int i=0;i<9;i++) m_page3.m_edit_t3_set[i].SetWindowTextW(text);
	//////////////////////////////////////////////////
	for(int i=0;i<9;i++) m_page3.m_edit_t1_stop[i].SetWindowTextW(text);
	for(int i=0;i<9;i++) m_page3.m_edit_t2_stop[i].SetWindowTextW(text);
	for(int i=0;i<9;i++) m_page3.m_edit_t3_stop[i].SetWindowTextW(text);

	text.Format(_T("%02d:%02d\n"),5,0);
	m_page2.m_edit_produv_dlit.SetWindowTextW(text);

	///////////////////////////////////////////////////

	m_page2.m_spin_produv_dlit.SetRange(0,99);

	for(int i=0;i<9;i++) m_page3.m_spin_t1_set[i].SetRange(0,99);
	for(int i=0;i<9;i++) m_page3.m_spin_t2_set[i].SetRange(0,99);
	for(int i=0;i<9;i++) m_page3.m_spin_t3_set[i].SetRange(0,99);
	
	//////////////////////////////////////////////////
	
	for(int i=0;i<9;i++) m_page3.m_spin_t1_stop[i].SetRange(0,99);
	for(int i=0;i<9;i++) m_page3.m_spin_t2_stop[i].SetRange(0,99);
	for(int i=0;i<9;i++) m_page3.m_spin_t3_stop[i].SetRange(0,99);

	m_group_dinamic.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_group_dinamic_temp.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_group_dinamic_ph.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_group_dinamic_redox.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	
	m_page1.m_group_temp.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page1.m_group_water_param.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page1.m_group_ds1307.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page1.m_group_device_select.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page1.m_group_com.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page1.m_group_led.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page1.m_group_cooler.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page1.m_group_option.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);

	m_page2.m_group_temp_mode.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page2.m_group_ph_mode.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page2.m_group_mineral_mode.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page2.m_static_dop_param.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page2.m_group_doliv.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);

	m_page3.m_group_device.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page3.m_group_timer1.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page3.m_group_timer2.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page3.m_group_timer3.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);

	m_page4.m_group_calibr_ph.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page4.m_group_calibr_redox.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page4.m_group_calc_koef.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page4.m_group_calibr.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page4.m_group_set_koef.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page4.m_group_new_koef.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page4.m_group_redox_set_koef.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
	m_page4.m_group_redox_new_koef.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);

	m_page1.m_group_temp.SetAlignment(SS_CENTER);
	m_page1.m_group_water_param.SetAlignment(SS_CENTER);
	m_page1.m_group_ds1307.SetAlignment(SS_CENTER);
	
	m_page3.m_group_timer1.SetAlignment(SS_CENTER);
	m_page3.m_group_timer2.SetAlignment(SS_CENTER);
	m_page3.m_group_timer3.SetAlignment(SS_CENTER);
	m_page3.m_group_device.SetAlignment(SS_CENTER);

	m_page4.m_group_set_koef.SetAlignment(SS_CENTER);
	m_page4.m_group_new_koef.SetAlignment(SS_CENTER);
	m_page4.m_group_calibr.SetAlignment(SS_CENTER);
	m_page4.m_group_redox_set_koef.SetAlignment(SS_CENTER);
	m_page4.m_group_redox_new_koef.SetAlignment(SS_CENTER);

	UDACCEL YAccell;
	YAccell.nSec=1000;
	YAccell.nInc=1;
	
	for(int i=0;i<9;i++) m_page3.m_spin_t1_set[i].SetAccel(1,&YAccell);
	
	m_page2.m_spin_produv_dlit.SetAccel(1,&YAccell);

	m_page1.m_edit_lamp1.SetWindowText(_T("NA"));
	m_page1.m_edit_lamp2.SetWindowText(_T("NA"));
	m_page1.m_edit_lamp3.SetWindowText(_T("NA"));
	m_page1.m_edit_lamp4.SetWindowText(_T("NA"));
	m_page1.m_edit_compres.SetWindowText(_T("NA"));
	m_page1.m_edit_heater.SetWindowText(_T("NA"));
	m_page1.m_edit_co2.SetWindowText(_T("NA"));
	m_page1.m_edit_pompa.SetWindowText(_T("NA"));
	m_page1.m_edit_cooler.SetWindowText(_T("NA"));

	m_page1.m_combo_port.SetCurSel(0);
	m_page2.m_combo_period.SetCurSel(0);
	m_page2.m_combo_temp_sensor.SetCurSel(0);

	m_page1.GetDlgItem(IDC_CLOSE_PORT)->EnableWindow(FALSE);
	m_page4.GetDlgItem(IDC_BUTTON_CALC_K2)->EnableWindow(FALSE);
	m_page2.GetDlgItem(IDC_BUTTON_POROG1)->EnableWindow(FALSE);
	m_page2.GetDlgItem(IDC_BUTTON_POROG2)->EnableWindow(FALSE);

	m_page1.m_temp_1=20;
	m_page1.m_temp_2=20;
	m_page1.m_temp_3=20;
	m_page1.m_edit_ph=4;
	m_page1.m_edit_redox=0;
	
	m_slider_graf.SetRangeMin(1,false);
	m_slider_graf.SetRangeMax(60,false);
	
	SetTimer(0,1,NULL);

	if (!m_pTip)
	{
		m_pTip =new CToolTipCtrl;
		m_pTip->Create(this,TTS_ALWAYSTIP); // подсказки активны во всех диалогах
		m_pTip->SetMaxTipWidth(200); // многострочный текст, количество символов
		m_pTip->SetDelayTime(900); // время отображения подсказок
		m_pTip->AddTool(&m_slider_graf,_T("Время обновления данных графика."));
		m_pTip->AddTool(&m_edit_system_time,_T("Системное время."));

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

	// обнуление переменной порта
	m_port=NULL;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CAquaControl2014Dlg::PreTranslateMessage(MSG* pMsg) 
{
	//перегрузка виртуальной функции PreTranslateMessage
	//если подсказки определены, их отображние
	if (m_pTip!=NULL)
            m_pTip->RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}

void CAquaControl2014Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAquaControl2014Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	
	CClientDC dc(this); // контекст устройства (клиентская область)
	
	dc.MoveTo(55,370); dc.LineTo(55,466); // линия (от предидущего к последующему значению)
	dc.LineTo(205,466); dc.LineTo(205,370);  dc.LineTo(55,370);
	
	dc.MoveTo(257,370); dc.LineTo(257,466); // линия (от предидущего к последующему значению)
	dc.LineTo(407,466); dc.LineTo(407,370);  dc.LineTo(257,370);

	dc.MoveTo(460,370); dc.LineTo(460,466); // линия (от предидущего к последующему значению)
	dc.LineTo(610,466); dc.LineTo(610,370);  dc.LineTo(460,370);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAquaControl2014Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CAquaControl2014Dlg::OnCommunication(WPARAM ch, LPARAM port)
{
	if(port_open)
	{
		if (port==(m_page1.m_combo_port.GetCurSel()+1))
		{		
			if(ch==0x3B) com_data_read=true;	
			if(ch==0xB3) 
			{
				com_data_in[com_data_count]=ch;
				com_data_read=false;
				com_data_count=0;

				switch(com_data_in[1])
				{
					case 0x01: com_package=0x01; break;
					case 0x02: com_package=0x02; break;
					case 0x03: com_package=0x03; break;
					case 0x04: com_package=0x04; break;
					case 0x05: com_package=0x05; break;
					case 0x06: com_package=0x06; break;
					case 0x07: com_package=0x07; break;
					case 0x08: com_package=0x08; break;
					case 0x09: com_package=0x09; break;
					case 0x10: com_package=0x10; break;
					case 0x11: com_package=0x11; break;
					case 0x12: com_package=0x12; break;
					case 0x13: com_package=0x13; break;
					case 0x14: com_package=0x14; break;
					case 0x15: com_package=0x15; break;
					case 0x16: com_package=0x16; break;
					case 0x17: com_package=0x17; break;
					case 0x18: com_package=0x18; break;
					case 0xa1: com_package=0xa1; break;
					case 0xa2: com_package=0xa2; break;
					case 0xa3: com_package=0xa3; break;
					default:;break;
				}
			}
			if(com_data_read)
			{
				com_data_in[com_data_count]=ch;			
				com_data_count++;
			}
		}
	}
	return S_OK;
}

CRect CAquaControl2014Dlg::GetPageRect(CTabCtrl &T)
{
	WINDOWPLACEMENT Pos;
	T.GetWindowPlacement(&Pos);
	CRect Win=Pos.rcNormalPosition;
	T.AdjustRect(FALSE, Win);
	return Win;
}
void CAquaControl2014Dlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{	
	KillTimer(1);
	KillTimer(2);
	KillTimer(3);
	KillTimer(4);
	m_page1.ShowWindow(SW_HIDE);
	m_page2.ShowWindow(SW_HIDE);
	m_page3.ShowWindow(SW_HIDE);
	m_page4.ShowWindow(SW_HIDE);
	switch (m_tab.GetCurSel())
	{
		case 0: m_page1.ShowWindow(SW_SHOW); SetTimer(1,1,NULL); port_send=false; break;
		case 1: 
		{
			m_page2.ShowWindow(SW_SHOW); 
			SetTimer(2,1,NULL); 
			port_send=false;
		}break;
		case 2: 
		{
			m_page3.ShowWindow(SW_SHOW); 
			SetTimer(3,1,NULL); 
			port_send=false; 
		}break;
		case 3: m_page4.ShowWindow(SW_SHOW); SetTimer(4,1,NULL); port_send=false; break;
		*pResult = 0;
	}
}

bool CAquaControl2014Dlg::mas_compare(int data_count)
{
	compare=true;
	for(int i=0;i<data_count;i++)
	{
		if(com_data_in[i]!=data_to_send[i]) compare=false;
	}
	return compare;
}

void CAquaControl2014Dlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==0)
	{
		UpdateData(true);

		switch(com_package)
		{
			case 0x01:
			{
				if((!mas_compare(23))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x01;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x02:
			{
				if((!mas_compare(9))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x02;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x03:
			{
				if((!mas_compare(10))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x03;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x04:
			{
				if((!mas_compare(3))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x04;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x05:
			{
				if((!mas_compare(327))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x05;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x06:
			{
				if((!mas_compare(11))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x06;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x07:
			{
				if((!mas_compare(3))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x07;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x08:
			{
				if((!mas_compare(14))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x08;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x09:
			{
				if((!mas_compare(14))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x09;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x10:
			{
				if((!mas_compare(14))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x10;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x11:
			{
				if((!mas_compare(13))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x11;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x12:
			{
				if((!mas_compare(3))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x12;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x13:
			{
				if((!mas_compare(3))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x13;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x14:
			{
				if((!mas_compare(3))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x14;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x15:
			{
				if((!mas_compare(3))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x15;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x16:
			{
				if((!mas_compare(3))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x16;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x17:
			{
				if((!mas_compare(3))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x17;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x18:
			{
				if((!mas_compare(3))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x18;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			case 0x19:
			{
				if((!mas_compare(3))&(TRANSMIT_ERROR>0))
				{
					TRANSMIT_ERROR++;
					m_page1.condition=0x19;
					port_send=TRUE;
				}
				else com_package=0;
			}break;
			default:; break;
		}
		
		if(TRANSMIT_ERROR==6) 
		{
			TRANSMIT_ERROR=0;
			MessageBox(_T(" Ошибка передачи данных!!!\n Проверьте соединение с устройством!!!"),_T("Информация"), MB_ICONINFORMATION);
		}

		if(COM_ON_START)
		{
			COM_ON_START=false;
			m_page1.m_combo_port.SetCurSel(file_buf[4]-0x31);
			OnBnClickedOpenPort();	
		}
		
		CTime ct=CTime::GetCurrentTime();
		sys_time.Format(_T("%02d:%02d:%02d\n"),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
		m_edit_system_time.SetWindowText(sys_time);
		
		hour_now[0]=ct.GetHour()/10;
		hour_now[1]=ct.GetHour()%10;
		minute_now[0]=ct.GetMinute()/10;
		minute_now[1]=ct.GetMinute()%10;
		second_now[0]=ct.GetSecond()/10;
		second_now[1]=ct.GetSecond()%10;

		if(TIME_SYNC)
		{
			TIME_SYNC=false;
			
			data_to_send[0]=0x3b;
			data_to_send[1]=0x02;
			data_to_send[2]=hour_now[0]+0x30;
			data_to_send[3]=hour_now[1]+0x30;
			data_to_send[4]=minute_now[0]+0x30;
			data_to_send[5]=minute_now[1]+0x30;
			data_to_send[6]=second_now[0]+0x30;
			data_to_send[7]=second_now[1]+0x30;
			data_to_send[8]=0xb3;

			if(port_open==TRUE) // проверка состояния порта
			{
				m_port->WriteToPort((char*)data_to_send, 9);
			}
			port_send=FALSE;
		}
		
		if(m_page1.port_btn_open)
		{
			m_page1.port_btn_open=NULL;
			OnBnClickedOpenPort();
		}
		if(m_page1.port_btn_close)
		{
			m_page1.port_btn_close=NULL;
			OnBnClickedClosePort();
			KillTimer(5);
		}
		if(m_page1.port_btn_num)
		{
			m_page1.port_btn_num=NULL;
			OnBnClickedFindPort();
		}

		if(m_page1.rele_condition.cooler==0x31) m_page1.GetDlgItem(IDC_SLIDER_COOLER)->EnableWindow(FALSE);
		else m_page1.GetDlgItem(IDC_SLIDER_COOLER)->EnableWindow(TRUE);

		if((com_data_in[1]==0xa1)&(com_data_in[66]==0xb3))
		{ 
			m_page1.UpdateData(true);
			m_page4.UpdateData(true);
			m_page1.m_edit_hour=(com_data_in[2]-0x30)*10+(com_data_in[3]-0x30);
			m_page1.m_edit_min=(com_data_in[4]-0x30)*10+(com_data_in[5]-0x30);
			m_page1.m_edit_sec=(com_data_in[6]-0x30)*10+(com_data_in[7]-0x30);
			str.Format(_T("Time: %2d:%2d:%2d"),	m_page1.m_edit_hour, 
												m_page1.m_edit_min, 
												m_page1.m_edit_sec); // преобразование строки

			m_page1.m_temp_1=(com_data_in[8]-0x30)*10+(com_data_in[9]-0x30)+(com_data_in[10]-0x30)*0.1;
			m_page1.m_temp_2=(com_data_in[11]-0x30)*10+(com_data_in[12]-0x30)+(com_data_in[13]-0x30)*0.1;
			m_page1.m_temp_3=(com_data_in[14]-0x30)*10+(com_data_in[15]-0x30)+(com_data_in[16]-0x30)*0.1;

			m_page1.m_edit_ph=(com_data_in[17]-0x30)*10+(com_data_in[18]-0x30)+(com_data_in[19]-0x30)*0.1+(com_data_in[20]-0x30)*0.01; 
			m_page1.m_edit_redox=(com_data_in[21]-0x30)+(com_data_in[22]-0x30)*0.1+(com_data_in[23]-0x30)*0.01+(com_data_in[24]-0x30)*0.001;

			doliv_porog1=(com_data_in[40]-0x30)*1000+(com_data_in[41]-0x30)*100+(com_data_in[42]-0x30)*10+(com_data_in[43]-0x30);
			doliv_porog2=(com_data_in[44]-0x30)*1000+(com_data_in[45]-0x30)*100+(com_data_in[46]-0x30)*10+(com_data_in[47]-0x30);
			water_lvl=(com_data_in[48]-0x30)*1000+(com_data_in[49]-0x30)*100+(com_data_in[50]-0x30)*10+(com_data_in[51]-0x30);

			if(get_chanel_status)
			{
				if((com_data_in[26]&0x01)==0x01) {m_page1.CheckDlgButton(IDC_CHECK_LAMP1,1); m_page1.rele_condition.lamp1=0x31;}
				else {m_page1.CheckDlgButton(IDC_CHECK_LAMP1,0); m_page1.rele_condition.lamp1=0x30;}
				
				if((com_data_in[26]&0x02)==0x02) {m_page1.CheckDlgButton(IDC_CHECK_LAMP2,1); m_page1.rele_condition.lamp2=0x31;}
				else {m_page1.CheckDlgButton(IDC_CHECK_LAMP2,0); m_page1.rele_condition.lamp2=0x30;}
				
				if((com_data_in[26]&0x04)==0x04) {m_page1.CheckDlgButton(IDC_CHECK_LAMP3,1); m_page1.rele_condition.lamp3=0x31;}
				else {m_page1.CheckDlgButton(IDC_CHECK_LAMP3,0); m_page1.rele_condition.lamp3=0x30;}
				
				if((com_data_in[26]&0x08)==0x08) {m_page1.CheckDlgButton(IDC_CHECK_LAMP4,1); m_page1.rele_condition.lamp4=0x31;}
				else {m_page1.CheckDlgButton(IDC_CHECK_LAMP4,0); m_page1.rele_condition.lamp4=0x30;}
				
				if((com_data_in[26]&0x10)==0x10) {m_page1.CheckDlgButton(IDC_CHECK_COMPRES,1); m_page1.rele_condition.compres=0x31;}
				else {m_page1.CheckDlgButton(IDC_CHECK_COMPRES,0); m_page1.rele_condition.compres=0x30;}

				if(com_data_in[65]==0x32) {m_page1.CheckDlgButton(IDC_CHECK_HEATER,1); m_page1.rele_condition.heater=0x31;}
				else {m_page1.CheckDlgButton(IDC_CHECK_HEATER,0); m_page1.rele_condition.heater=0x30;}
				
				if(com_data_in[64]==0x32) {m_page1.CheckDlgButton(IDC_CHECK_CO2,1); m_page1.rele_condition.co2=0x31;}
				else {m_page1.CheckDlgButton(IDC_CHECK_CO2,0); m_page1.rele_condition.co2=0x30;}
				
				if(com_data_in[52]==0x32) {m_page1.CheckDlgButton(IDC_CHECK_POMPA,1); m_page1.rele_condition.pompa=0x31;}
				else {m_page1.CheckDlgButton(IDC_CHECK_POMPA,0); m_page1.rele_condition.pompa=0x30;}
				
				if(com_data_in[27]==0x32) {m_page1.CheckDlgButton(IDC_CHECK_COOLER,1); m_page1.rele_condition.cooler=0x31;}
				else {m_page1.CheckDlgButton(IDC_CHECK_COOLER,0); m_page1.rele_condition.cooler=0x30;}
				
				if((com_data_in[39]&0x01)==0x01)
				{
					m_page2.GetDlgItem(IDC_EDIT_TEMP_SET)->EnableWindow(FALSE);
					m_page2.GetDlgItem(IDC_EDIT_TEMP_POROG)->EnableWindow(FALSE);
					m_page2.GetDlgItem(IDC_COMBO_TEMP_SENSOR)->EnableWindow(FALSE);
					m_page2.temp_mode_set=true;
					m_page2.condition=0x03;
					m_page2.GetDlgItem(IDC_SLIDER_COOLER)->EnableWindow(FALSE);
					m_page2.CheckDlgButton(IDC_CHECK_TEMP_MODE,1);
				}
				if((com_data_in[39]&0x02)==0x02)
				{
					m_page2.GetDlgItem(IDC_EDIT_PRODUV_DLIT)->EnableWindow(FALSE);
					m_page2.GetDlgItem(IDC_SPIN_MINERAL_STOP)->EnableWindow(FALSE);
					m_page2.GetDlgItem(IDC_COMBO_PERIOD)->EnableWindow(FALSE);
					m_page2.GetDlgItem(IDC_EDIT_MIN_POWER_COOLER)->EnableWindow(FALSE);
					m_page2.condition=0x06;
					m_page2.CheckDlgButton(IDC_CHECK_PRODUV,1);
				}
				if((com_data_in[39]&0x04)==0x04)
				{
					m_page2.GetDlgItem(IDC_EDIT_PH_HIGH)->EnableWindow(FALSE);
					m_page2.GetDlgItem(IDC_EDIT_PH_LOW)->EnableWindow(FALSE);
					m_page2.GetDlgItem(IDC_EDIT_CO2_STOP)->EnableWindow(FALSE);
					m_page2.GetDlgItem(IDC_EDIT_CO2_START)->EnableWindow(FALSE);
					m_page2.condition=0x11;
					m_page2.CheckDlgButton(IDC_CHECK_CO2_MODE,1);
				}
				if((com_data_in[39]&0x08)==0x08)
				{
					m_page2.GetDlgItem(IDC_BUTTON_POROG1)->EnableWindow(TRUE);
					m_page2.condition=0x15;
					m_page2.CheckDlgButton(IDC_CHECK_DOLIV,1);
				}
				m_page4.m_edit_last_k1=(com_data_in[28]-0x30)+(com_data_in[29]-0x30)*0.1+(com_data_in[30]-0x30)*0.01+(com_data_in[31]-0x30)*0.001;
				m_page4.m_edit_last_k2=(com_data_in[32]-0x30)+(com_data_in[33]-0x30)*0.1+(com_data_in[34]-0x30)*0.01;
				m_page4.m_edit_last_k=(com_data_in[35]-0x30)+(com_data_in[36]-0x30)*0.1+(com_data_in[37]-0x30)*0.01+(com_data_in[38]-0x30)*0.001;

				///////////////
				m_page1.m_edit_led_stop=(com_data_in[53]-0x30)*10+(com_data_in[54]-0x30);
				m_page1.m_edit_led_start=(com_data_in[55]-0x30)*10+(com_data_in[56]-0x30);
				m_page1.m_edit_cooler_stop=(com_data_in[57]-0x30)*10+(com_data_in[58]-0x30);
				m_page1.m_edit_cooler_start=(com_data_in[59]-0x30)*10+(com_data_in[60]-0x30);
				///////////////
				m_page1.m_slider_cooler.SetPos((com_data_in[61]-0x30)*100+(com_data_in[62]-0x30)*10+(com_data_in[63]-0x30));
				
				get_chanel_status=false; 
			}

			//////////////
			if((com_data_in[26]&0x01)==0x01) m_page1.m_edit_lamp1.SetWindowText(_T("HAND")); 
			else
			{
				if((com_data_in[25]&0x01)==0x01) m_page1.m_edit_lamp1.SetWindowText(_T("TIMER"));
				else m_page1.m_edit_lamp1.SetWindowText(_T("NA"));
			}
			///////////////
			if((com_data_in[26]&0x02)==0x02) m_page1.m_edit_lamp2.SetWindowText(_T("HAND"));
			else
			{
				if((com_data_in[25]&0x02)==0x02) m_page1.m_edit_lamp2.SetWindowText(_T("TIMER"));
				else m_page1.m_edit_lamp2.SetWindowText(_T("NA"));
			}
			///////////////
			if((com_data_in[26]&0x04)==0x04) m_page1.m_edit_lamp3.SetWindowText(_T("HAND"));
			else
			{
				if((com_data_in[25]&0x04)==0x04) m_page1.m_edit_lamp3.SetWindowText(_T("TIMER"));
				else m_page1.m_edit_lamp3.SetWindowText(_T("NA"));
			}
			///////////////
			if((com_data_in[26]&0x08)==0x08) m_page1.m_edit_lamp4.SetWindowText(_T("HAND"));
			else
			{
				if((com_data_in[25]&0x08)==0x08) m_page1.m_edit_lamp4.SetWindowText(_T("TIMER"));
				else m_page1.m_edit_lamp4.SetWindowText(_T("NA"));
			}
			///////////////
			if((com_data_in[26]&0x10)==0x10) m_page1.m_edit_compres.SetWindowText(_T("HAND"));
			else
			{
				if((com_data_in[25]&0x10)==0x10) m_page1.m_edit_compres.SetWindowText(_T("TIMER"));
				else m_page1.m_edit_compres.SetWindowText(_T("NA"));
			}
			///////////////
			if(com_data_in[64]==0x30) m_page1.m_edit_co2.SetWindowText(_T("NA"));
			if(com_data_in[64]==0x31) m_page1.m_edit_co2.SetWindowText(_T("TIMER"));
			if(com_data_in[64]==0x32) m_page1.m_edit_co2.SetWindowText(_T("HAND"));
			if(com_data_in[64]==0x33) m_page1.m_edit_co2.SetWindowText(_T("PH/CO2"));
			///////////////
			if(com_data_in[52]==0x30) m_page1.m_edit_pompa.SetWindowText(_T("NA"));
			if(com_data_in[52]==0x31) m_page1.m_edit_pompa.SetWindowText(_T("TIMER"));
			if(com_data_in[52]==0x32) m_page1.m_edit_pompa.SetWindowText(_T("HAND"));
			if(com_data_in[52]==0x33) m_page1.m_edit_pompa.SetWindowText(_T("DOLIV"));
			if(com_data_in[52]==0x34) m_page1.m_edit_pompa.SetWindowText(_T("ERROR"));
			///////////////
			if(com_data_in[27]==0x30) m_page1.m_edit_cooler.SetWindowText(_T("NA"));
			if(com_data_in[27]==0x31) m_page1.m_edit_cooler.SetWindowText(_T("TIMER"));
			if(com_data_in[27]==0x32) m_page1.m_edit_cooler.SetWindowText(_T("HAND"));
			if(com_data_in[27]==0x33) m_page1.m_edit_cooler.SetWindowText(_T("TEMP_M"));
			if(com_data_in[27]==0x34) m_page1.m_edit_cooler.SetWindowText(_T("PRODUV"));
			///////////////
			if(com_data_in[65]==0x30) m_page1.m_edit_heater.SetWindowText(_T("NA"));
			if(com_data_in[65]==0x31) m_page1.m_edit_heater.SetWindowText(_T("TIMER"));
			if(com_data_in[65]==0x32) m_page1.m_edit_heater.SetWindowText(_T("HAND"));
			if(com_data_in[65]==0x33) m_page1.m_edit_heater.SetWindowText(_T("HEATER"));
			///////////////
			m_page4.m_edit_ph_data=m_page1.m_edit_ph;
			m_page4.m_edit_redox_data=m_page1.m_edit_redox;
			if(m_page4.show_koef)
			{
				m_page4.m_edit_new_k1=(com_data_in[28]-0x30)+(com_data_in[29]-0x30)*0.1+(com_data_in[30]-0x30)*0.01+(com_data_in[31]-0x30)*0.001;
				m_page4.m_edit_new_k2=(com_data_in[32]-0x30)+(com_data_in[33]-0x30)*0.1+(com_data_in[34]-0x30)*0.01;
			}
			if(m_page4.show_koef_k)
			{
				m_page4.m_edit_new_k=(com_data_in[35]-0x30)+(com_data_in[36]-0x30)*0.1+(com_data_in[37]-0x30)*0.01+(com_data_in[38]-0x30)*0.001;
			}
			///////////////
			com_data_in[1]=0x00;
			com_data_in[66]=0x00;
			m_page1.UpdateData(false);
			m_page4.UpdateData(false);
		}
		
		if((com_data_in[1]==0xa2)&(com_data_in[326]==0xb3))
		{
			m_page3.UpdateData(true);
			m_page3.KillTimer(0);
			for(int i=0;i<9;i++)
			{
				m_page3.timer_1.timer_set_hour[i]=(com_data_in[2+36*i]-0x30)*10+(com_data_in[3+36*i]-0x30);
				m_page3.timer_1.timer_set_min[i]=(com_data_in[4+36*i]-0x30)*10+(com_data_in[5+36*i]-0x30);
				m_page3.timer_1.timer_set_sec[i]=(com_data_in[6+36*i]-0x30)*10+(com_data_in[7+36*i]-0x30);
				
				m_page3.timer_2.timer_set_hour[i]=(com_data_in[8+36*i]-0x30)*10+(com_data_in[9+36*i]-0x30);
				m_page3.timer_2.timer_set_min[i]=(com_data_in[10+36*i]-0x30)*10+(com_data_in[11+36*i]-0x30);
				m_page3.timer_2.timer_set_sec[i]=(com_data_in[12+36*i]-0x30)*10+(com_data_in[13+36*i]-0x30);

				m_page3.timer_3.timer_set_hour[i]=(com_data_in[14+36*i]-0x30)*10+(com_data_in[15+36*i]-0x30);
				m_page3.timer_3.timer_set_min[i]=(com_data_in[16+36*i]-0x30)*10+(com_data_in[17+36*i]-0x30);
				m_page3.timer_3.timer_set_sec[i]=(com_data_in[18+36*i]-0x30)*10+(com_data_in[19+36*i]-0x30);
				////////////////////////////////////////////////////////////////////////////////////////////
				m_page3.timer_1.timer_stop_hour[i]=(com_data_in[20+36*i]-0x30)*10+(com_data_in[21+36*i]-0x30);
				m_page3.timer_1.timer_stop_min[i]=(com_data_in[22+36*i]-0x30)*10+(com_data_in[23+36*i]-0x30);
				m_page3.timer_1.timer_stop_sec[i]=(com_data_in[24+36*i]-0x30)*10+(com_data_in[25+36*i]-0x30);
				
				m_page3.timer_2.timer_stop_hour[i]=(com_data_in[26+36*i]-0x30)*10+(com_data_in[27+36*i]-0x30);
				m_page3.timer_2.timer_stop_min[i]=(com_data_in[28+36*i]-0x30)*10+(com_data_in[29+36*i]-0x30);
				m_page3.timer_2.timer_stop_sec[i]=(com_data_in[30+36*i]-0x30)*10+(com_data_in[31+36*i]-0x30);

				m_page3.timer_3.timer_stop_hour[i]=(com_data_in[32+36*i]-0x30)*10+(com_data_in[33+36*i]-0x30);
				m_page3.timer_3.timer_stop_min[i]=(com_data_in[34+36*i]-0x30)*10+(com_data_in[35+36*i]-0x30);
				m_page3.timer_3.timer_stop_sec[i]=(com_data_in[36+36*i]-0x30)*10+(com_data_in[37+36*i]-0x30);	
				
				///////////////////////////////////////////////////////////////

				m_page3.timer_1.timer_set_text[i].Format(_T("%02d:%02d:%02d\n"),m_page3.timer_1.timer_set_hour[i],m_page3.timer_1.timer_set_min[i],m_page3.timer_1.timer_set_sec[i]);
				m_page3.m_edit_t1_set[i].SetWindowTextW(m_page3.timer_1.timer_set_text[i]);

				m_page3.timer_2.timer_set_text[i].Format(_T("%02d:%02d:%02d\n"),m_page3.timer_2.timer_set_hour[i],m_page3.timer_2.timer_set_min[i],m_page3.timer_2.timer_set_sec[i]);
				m_page3.m_edit_t2_set[i].SetWindowTextW(m_page3.timer_2.timer_set_text[i]);

				m_page3.timer_3.timer_set_text[i].Format(_T("%02d:%02d:%02d\n"),m_page3.timer_3.timer_set_hour[i],m_page3.timer_3.timer_set_min[i],m_page3.timer_3.timer_set_sec[i]);
				m_page3.m_edit_t3_set[i].SetWindowTextW(m_page3.timer_3.timer_set_text[i]);
				////////////////////////////
				m_page3.timer_1.timer_stop_text[i].Format(_T("%02d:%02d:%02d\n"),m_page3.timer_1.timer_stop_hour[i],m_page3.timer_1.timer_stop_min[i],m_page3.timer_1.timer_stop_sec[i]);
				m_page3.m_edit_t1_stop[i].SetWindowTextW(m_page3.timer_1.timer_stop_text[i]);

				m_page3.timer_2.timer_stop_text[i].Format(_T("%02d:%02d:%02d\n"),m_page3.timer_2.timer_stop_hour[i],m_page3.timer_2.timer_stop_min[i],m_page3.timer_2.timer_stop_sec[i]);
				m_page3.m_edit_t2_stop[i].SetWindowTextW(m_page3.timer_2.timer_stop_text[i]);

				m_page3.timer_3.timer_stop_text[i].Format(_T("%02d:%02d:%02d\n"),m_page3.timer_3.timer_stop_hour[i],m_page3.timer_3.timer_stop_min[i],m_page3.timer_3.timer_stop_sec[i]);
				m_page3.m_edit_t3_stop[i].SetWindowTextW(m_page3.timer_3.timer_stop_text[i]);
			}
			///////////////
			com_data_in[1]=0x00;
			com_data_in[326]=0x00;
			m_page3.SetTimer(0,1,NULL);
			m_page3.UpdateData(false);
		}
		////////////////
		if((com_data_in[1]==0xa3)&(com_data_in[27]==0xb3))
		{
			m_page2.UpdateData(true);
			
			CString text;

			m_page2.m_combo_temp_sensor.SetCurSel(com_data_in[2]-0x31);
			m_page2.m_edit_temp_set=(com_data_in[3]-0x30)*10+(com_data_in[4]-0x30);
			m_page2.m_edit_temp_porog=(com_data_in[5]-0x30)*0.1;
			m_page2.m_slider_cooler.SetPos(((com_data_in[6]-0x30)*100+(com_data_in[7]-0x30)*10+(com_data_in[8]-0x30))/2.55);
			text.Format(_T("%02d:%02d\n"),(com_data_in[9]-0x30)*10+(com_data_in[10]-0x30),(com_data_in[11]-0x30)*10+(com_data_in[12]-0x30));
			m_page2.m_edit_produv_dlit.SetWindowTextW(text);
			m_page2.m_combo_period.SetCurSel(com_data_in[13]-0x31);
			m_page2.m_edit_min_power_cooler=((com_data_in[14]-0x30)*100+(com_data_in[15]-0x30)*10+(com_data_in[16]-0x30))/2.55;
			m_page2.m_edit_ph_high=(com_data_in[17]-0x30)+(com_data_in[18]-0x30)*0.1+(com_data_in[19]-0x30)*0.01;
			m_page2.m_edit_ph_low=(com_data_in[20]-0x30)+(com_data_in[21]-0x30)*0.1+(com_data_in[22]-0x30)*0.01;
			m_page2.m_edit_co2_stop=(com_data_in[23]-0x30)*10+(com_data_in[24]-0x30);
			m_page2.m_edit_co2_start=(com_data_in[25]-0x30)*10+(com_data_in[26]-0x30);

			com_data_in[1]=0x00;
			com_data_in[27]=0x00;
			m_page2.UpdateData(false);
		}

		UpdateData(false);
	}	
	if(nIDEvent==1)
	{
		if(port_send==TRUE)
		{
			switch(m_page1.condition)
			{
				case 0x01:
				{
					m_page1.UpdateData(true);
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page1.condition;
					data_to_send[2]=m_page1.rele_condition.lamp1;
					data_to_send[3]=m_page1.rele_condition.lamp2;
					data_to_send[4]=m_page1.rele_condition.lamp3;
					data_to_send[5]=m_page1.rele_condition.lamp4;
					data_to_send[6]=m_page1.rele_condition.compres;
					data_to_send[7]=m_page1.rele_condition.heater;
					data_to_send[8]=m_page1.rele_condition.co2;
					data_to_send[9]=m_page1.rele_condition.pompa;
					data_to_send[10]=m_page1.rele_condition.cooler;
					data_to_send[11]=m_page1.rele_condition.cooler_data/100;
					data_to_send[12]=(m_page1.rele_condition.cooler_data%100)/10;
					data_to_send[13]=((m_page1.rele_condition.cooler_data%100)%100)%10;
					data_to_send[14]=m_page1.m_edit_led_stop/10+0x30;
					data_to_send[15]=m_page1.m_edit_led_stop%10+0x30;
					data_to_send[16]=m_page1.m_edit_led_start/10+0x30;
					data_to_send[17]=m_page1.m_edit_led_start%10+0x30;
					data_to_send[18]=m_page1.m_edit_cooler_stop/10+0x30;
					data_to_send[19]=m_page1.m_edit_cooler_stop%10+0x30;
					data_to_send[20]=m_page1.m_edit_cooler_start/10+0x30;
					data_to_send[21]=m_page1.m_edit_cooler_start%10+0x30;
					data_to_send[22]=0xb3;
					m_page1.UpdateData(false);

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 23);
					}
					port_send=FALSE;
				}; break;
				case 0x02:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page1.condition;
					data_to_send[2]=hour_now[0]+0x30;
					data_to_send[3]=hour_now[1]+0x30;
					data_to_send[4]=minute_now[0]+0x30;
					data_to_send[5]=minute_now[1]+0x30;
					data_to_send[6]=second_now[0]+0x30;
					data_to_send[7]=second_now[1]+0x30;
					data_to_send[8]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 9);
					}
					port_send=FALSE;
				}; break;
				case 0x13:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page1.condition;
					data_to_send[2]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 3);
					}
					port_send=FALSE;
				}
				default:; break;
			}
		}
	}
	if(nIDEvent==2)
	{
		if(!EVENT_PARAM_START_READ) 
		{
			m_page2.OnBnClickedButtonEventParam();
			EVENT_PARAM_START_READ=true;
		}
		if(port_send==TRUE)
		{
			switch(m_page2.condition)
			{
				case 0x03:
				{
					m_page2.UpdateData(true);
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=m_page2.m_combo_temp_sensor.GetCurSel()+1+0x30;
					data_to_send[3]=(m_page2.m_edit_temp_set)/10+0x30;
					data_to_send[4]=(m_page2.m_edit_temp_set)%10+0x30;
					data_to_send[5]=m_page2.m_edit_temp_porog*10+0x30;
					data_to_send[6]=m_page2.cooler_data/100+0x30;
					data_to_send[7]=(m_page2.cooler_data%100)/10+0x30;
					data_to_send[8]=((m_page2.cooler_data%100)%100)%10+0x30;
					data_to_send[9]=0xb3;
					m_page2.UpdateData(false);

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 10);
					}
					port_send=FALSE;
				}; break;
				case 0x04:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 3);
					}
					port_send=FALSE;
				}; break;
				case 0x06:
				{
					m_page2.UpdateData(true);
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=m_page2.timer_produv.timer_set_min[0]/10+0x30;
					data_to_send[3]=m_page2.timer_produv.timer_set_min[0]%10+0x30;
					data_to_send[4]=m_page2.timer_produv.timer_set_sec[0]/10+0x30;
					data_to_send[5]=m_page2.timer_produv.timer_set_sec[0]%10+0x30;
					data_to_send[6]=m_page2.m_combo_period.GetCurSel()+1+0x30;
					data_to_send[7]=m_page2.m_edit_min_power_cooler/100+0x30;
					data_to_send[8]=(m_page2.m_edit_min_power_cooler%100)/10+0x30;
					data_to_send[9]=(m_page2.m_edit_min_power_cooler%100)%10+0x30;
					data_to_send[10]=0xb3;
					m_page2.UpdateData(false);

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 11);
					}
					port_send=FALSE;
				}; break;
				case 0x07:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 3);
					}
					port_send=FALSE;
				}; break;
				case 0x11:
				{
					int ph_high=m_page2.m_edit_ph_high*100;
					int ph_low=m_page2.m_edit_ph_low*100;
					m_page2.UpdateData(true);
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=ph_high/100+0x30;
					data_to_send[3]=(ph_high%100)/10+0x30;
					data_to_send[4]=(ph_high%100)%10+0x30;
					data_to_send[5]=ph_low/100+0x30;
					data_to_send[6]=(ph_low%100)/10+0x30;
					data_to_send[7]=(ph_low%100)%10+0x30;
					data_to_send[8]=m_page2.m_edit_co2_stop/10+0x30;
					data_to_send[9]=m_page2.m_edit_co2_stop%10+0x30;
					data_to_send[10]=m_page2.m_edit_co2_start/10+0x30;
					data_to_send[11]=m_page2.m_edit_co2_start%10+0x30;
					data_to_send[12]=0xb3;
					m_page2.UpdateData(false);

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 13);
					}
					port_send=FALSE;
				}; break;
				case 0x12:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 3);
					}
					port_send=FALSE;
				}; break;
				case 0x15:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 3);
					}
					port_send=FALSE;
				}; break;
				case 0x16:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 3);
					}
					port_send=FALSE;
				}; break;
				case 0x17:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 3);
					}
					port_send=FALSE;
				}; break;
				case 0x18:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 3);
					}
					port_send=FALSE;
				}; break;
				case 0x19:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page2.condition;
					data_to_send[2]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 3);
					}
					port_send=FALSE;
				}; break;
				default:; break;
			}
		}
	}
	if(nIDEvent==3)
	{
		if(!TIMERS_START_READ) 
		{
			m_page3.OnBnClickedButtonReadMc();
			TIMERS_START_READ=true;
		}
		if(port_send==TRUE)
		{
			switch(m_page3.condition)
			{
				case 0x05:
				{
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page3.condition;
					for(int i=0;i<9;i++)
					{
						data_to_send[2+36*i]=m_page3.timer_1.timer_set_hour[i]/10+0x30;
						data_to_send[3+36*i]=m_page3.timer_1.timer_set_hour[i]%10+0x30;
						data_to_send[4+36*i]=m_page3.timer_1.timer_set_min[i]/10+0x30;
						data_to_send[5+36*i]=m_page3.timer_1.timer_set_min[i]%10+0x30;
						data_to_send[6+36*i]=m_page3.timer_1.timer_set_sec[i]/10+0x30;
						data_to_send[7+36*i]=m_page3.timer_1.timer_set_sec[i]%10+0x30;
						data_to_send[8+36*i]=m_page3.timer_1.timer_stop_hour[i]/10+0x30;
						data_to_send[9+36*i]=m_page3.timer_1.timer_stop_hour[i]%10+0x30;
						data_to_send[10+36*i]=m_page3.timer_1.timer_stop_min[i]/10+0x30;
						data_to_send[11+36*i]=m_page3.timer_1.timer_stop_min[i]%10+0x30;
						data_to_send[12+36*i]=m_page3.timer_1.timer_stop_sec[i]/10+0x30;
						data_to_send[13+36*i]=m_page3.timer_1.timer_stop_sec[i]%10+0x30;

						data_to_send[14+36*i]=m_page3.timer_2.timer_set_hour[i]/10+0x30;
						data_to_send[15+36*i]=m_page3.timer_2.timer_set_hour[i]%10+0x30;
						data_to_send[16+36*i]=m_page3.timer_2.timer_set_min[i]/10+0x30;
						data_to_send[17+36*i]=m_page3.timer_2.timer_set_min[i]%10+0x30;
						data_to_send[18+36*i]=m_page3.timer_2.timer_set_sec[i]/10+0x30;
						data_to_send[19+36*i]=m_page3.timer_2.timer_set_sec[i]%10+0x30;
						data_to_send[20+36*i]=m_page3.timer_2.timer_stop_hour[i]/10+0x30;
						data_to_send[21+36*i]=m_page3.timer_2.timer_stop_hour[i]%10+0x30;
						data_to_send[22+36*i]=m_page3.timer_2.timer_stop_min[i]/10+0x30;
						data_to_send[23+36*i]=m_page3.timer_2.timer_stop_min[i]%10+0x30;
						data_to_send[24+36*i]=m_page3.timer_2.timer_stop_sec[i]/10+0x30;
						data_to_send[25+36*i]=m_page3.timer_2.timer_stop_sec[i]%10+0x30;

						data_to_send[26+36*i]=m_page3.timer_3.timer_set_hour[i]/10+0x30;
						data_to_send[27+36*i]=m_page3.timer_3.timer_set_hour[i]%10+0x30;
						data_to_send[28+36*i]=m_page3.timer_3.timer_set_min[i]/10+0x30;
						data_to_send[29+36*i]=m_page3.timer_3.timer_set_min[i]%10+0x30;
						data_to_send[30+36*i]=m_page3.timer_3.timer_set_sec[i]/10+0x30;
						data_to_send[31+36*i]=m_page3.timer_3.timer_set_sec[i]%10+0x30;
						data_to_send[32+36*i]=m_page3.timer_3.timer_stop_hour[i]/10+0x30;
						data_to_send[33+36*i]=m_page3.timer_3.timer_stop_hour[i]%10+0x30;
						data_to_send[34+36*i]=m_page3.timer_3.timer_stop_min[i]/10+0x30;
						data_to_send[35+36*i]=m_page3.timer_3.timer_stop_min[i]%10+0x30;
						data_to_send[36+36*i]=m_page3.timer_3.timer_stop_sec[i]/10+0x30;
						data_to_send[37+36*i]=m_page3.timer_3.timer_stop_sec[i]%10+0x30;
					}
					data_to_send[326]=0xb3;
					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 327);
					}
					port_send=FALSE;
				}; break;
				case 0x14:
				{
					com_data_count=0;
					for(int i=0;i<512;i++) com_data_in[i]=0;
					
					data_to_send[0]=0x3b;
					data_to_send[1]=m_page3.condition;
					data_to_send[2]=0xb3;

					if(port_open==TRUE) // проверка состояния порта
					{
						m_port->WriteToPort((char*)data_to_send, 3);
					}
					port_send=FALSE;
				}; break;
				default:; break;
			}
		}
	}
	if(nIDEvent==4)
	{
		if(port_send==TRUE)
		{
			m_page4.UpdateData(true);
			int ph_1_calibr=0;
			int ph_2_calibr=0;
			ph_1_calibr=m_page4.m_edit_ph_calibr_1*100;
			ph_2_calibr=m_page4.m_edit_ph_calibr_2*100;
			data_to_send[0]=0x3b;
			data_to_send[1]=m_page4.condition;
			data_to_send[2]=ph_1_calibr/1000+0x30;
			data_to_send[3]=(ph_1_calibr%1000)/100+0x30;
			data_to_send[4]=((ph_1_calibr%1000)%100)/10+0x30;
			data_to_send[5]=((ph_1_calibr%1000)%100)%10+0x30;
			data_to_send[6]=ph_2_calibr/1000+0x30;
			data_to_send[7]=(ph_2_calibr%1000)/100+0x30;
			data_to_send[8]=((ph_2_calibr%1000)%100)/10+0x30;
			data_to_send[9]=((ph_2_calibr%1000)%100)%10+0x30;
			data_to_send[10]=m_page4.m_edit_redox_calibr/100+0x30;
			data_to_send[11]=(m_page4.m_edit_redox_calibr%100)/10+0x30;
			data_to_send[12]=(m_page4.m_edit_redox_calibr%100)%10+0x30;
			data_to_send[13]=0xb3;
			m_page4.UpdateData(false);
			if(port_open==TRUE) // проверка состояния порта
			{
				m_port->WriteToPort((char*)data_to_send, 14);
			}
			port_send=FALSE;
		}
	}
	if(nIDEvent==5)
	{	
		if(m_page1.m_temp_1<=20) graf_data.temperature[0]=20;
		else graf_data.temperature[0]=m_page1.m_temp_1;
		if(m_page1.m_temp_2<=20) graf_data.temperature[1]=20;
		else graf_data.temperature[1]=m_page1.m_temp_2;
		if(m_page1.m_temp_3<=20) graf_data.temperature[2]=20;
		else graf_data.temperature[2]=m_page1.m_temp_3;
		if(m_page1.m_edit_ph<=4) graf_data.ph=4;
		else graf_data.ph=m_page1.m_edit_ph;
		
		m_wndLineChart_1.SetPos(0,(graf_data.temperature[0]*10-200)); // установка текущей позиция
		m_wndLineChart_1.SetPos(1,(graf_data.temperature[1]*10-200)); // установка текущей позиция
		m_wndLineChart_1.SetPos(2,(graf_data.temperature[2]*10-200)); // установка текущей позиция
		m_wndLineChart_1.Go(); // обновляем элемент управления с новыми значениями

		m_wndLineChart_2.SetPos(0,(graf_data.ph*33-132)); // установка текущей позиция
		m_wndLineChart_2.Go(); // обновляем элемент управления с новыми значениями

		m_wndLineChart_3.SetPos(0,(m_page1.m_edit_redox*200)); // установка текущей позиция
		m_wndLineChart_3.Go(); // обновляем элемент управления с новыми значениями
	}
	CDialog::OnTimer(nIDEvent);
}

void CAquaControl2014Dlg::OnBnClickedOpenPort()
{
	if (m_port==NULL)
	{	
		m_port = new CSerialPort;
	}
	UpdateData(true); // обновление значенией переменных интерфейса
	if(port_open==TRUE) // проверка состояния порта
	{
		MessageBox(_T("Порт уже открыт"),_T("Информация"), MB_ICONINFORMATION);
		goto END; // если порт уже окрыт - выход из обработчика
	}
	// если порт активен, установлена скорость
	if (m_port->InitPort(this, m_page1.m_combo_port.GetCurSel()+1, m_page1.m_baudrate, 'N', 8, 1, EV_RXCHAR))
	{		
		get_chanel_status=true;
		TRANSMIT_ERROR=1;
		SetTimer(1,1,NULL);
		SetTimer(5,1000,NULL);
		port_open=TRUE; // порт активен
		MessageBox(_T("Порт открыт"),_T("Информация"), MB_ICONINFORMATION);
		m_page1.GetDlgItem(IDC_CLOSE_PORT)->EnableWindow(TRUE); // кнопка ЗАКРЫТЬ активна
		m_page1.GetDlgItem(IDC_FIND_PORT)->EnableWindow(FALSE); // кнопка НАЛИЧИЕ ПОРТОВ не активна
		m_port->StartMonitoring();
	}
	else // порт не определен или не проинициализирован
	{		
		m_port = NULL;
		delete m_port;
		port_open=NULL; // порт не активен
		MessageBox(_T("Ошибка открытия порта"), _T("Ошибка"), MB_ICONERROR);
		goto END_F;
	}
END:
	UpdateData(false); // обновление значенией переменных интерфейса
	m_page1.GetDlgItem(IDC_OPEN_PORT)->EnableWindow(FALSE); // кнопка ОТКРЫТЬ не активна
	m_page1.GetDlgItem(IDC_COMBO_PORT)->EnableWindow(FALSE); 
	m_page1.GetDlgItem(IDC_EDIT_BAUDRATE)->EnableWindow(FALSE);
END_F:;
}

void CAquaControl2014Dlg::OnBnClickedClosePort()
{
	port_open=NULL; // порт не активен
	m_port->StopMonitoring();
	MessageBox(_T("Порт закрыт"),_T("Информация"), MB_ICONINFORMATION);
	m_port->InitPort(this, m_page1.m_combo_port.GetCurSel()+1, m_page1.m_baudrate, 'N', 8, 1, EV_RXCHAR);
	m_page1.GetDlgItem(IDC_CLOSE_PORT)->EnableWindow(FALSE); // кнопка ЗАКРЫТЬ не активна
	m_page1.GetDlgItem(IDC_FIND_PORT)->EnableWindow(TRUE); // кнопка НАЛИЧИЕ ПОРТОВ активна
	m_page1.GetDlgItem(IDC_OPEN_PORT)->EnableWindow(TRUE); // кнопка ОТКРЫТЬ активна
	m_page1.GetDlgItem(IDC_COMBO_PORT)->EnableWindow(TRUE); 
	m_page1.GetDlgItem(IDC_EDIT_BAUDRATE)->EnableWindow(TRUE);
}

void CAquaControl2014Dlg::OnBnClickedFindPort()
{
	port_num=0; // кол-во портов
	if (m_port==NULL)
	{	
		m_port = new CSerialPort;
	}
	// опрос наличия портов
	for(int i=0;i<10;i++)
	{
		if(m_port->InitPort(this, i, m_page1.m_baudrate, 'N', 8, 1, EV_RXCHAR)) port_num=i;
	}
	// если ни одного порта найдено не было
	if(!port_num)
	{
		delete m_port; // очистка памяти
		m_port = NULL; // порт закрыт
	}
	UpdateData(true); // обновление значенией переменных интерфейса
	m_page1.m_num_ports=port_num; // вывод кол-ва имеющихся на ПК портов
	m_page1.UpdateData(true); // обновление значенией переменных интерфейса
	m_page1.m_num_ports=port_num; // вывод кол-ва имеющихся на ПК портов
	m_page1.UpdateData(false); // обновление значенией переменных интерфейса
	MessageBox(_T("Указывается количество физических и виртуальных COM - портов"),_T("Информация"), MB_ICONINFORMATION);
}

void CAquaControl2014Dlg::OnNMCustomdrawSliderGraf(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData(true);
	m_graf=m_slider_graf.GetPos();
	if(port_open==TRUE) // проверка состояния порта
	{
		SetTimer(5,(1000*m_graf),NULL);
	}
	UpdateData(false);
	*pResult = 0;
}

void CAquaControl2014Dlg::OnRecieveData()
{
	m_main_option.SHOW_TEXT=false;
	CMainOption recievedata;
	recievedata.DoModal();
}

void CAquaControl2014Dlg::OnSpravka()
{
	ShellExecute(0, _T("open"), _T("Help.chm"),NULL, NULL, SW_SHOWNORMAL);
}

void CAquaControl2014Dlg::OnTray()
{
	OnMinimise();
}

void CAquaControl2014Dlg::OnAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CAquaControl2014Dlg::OnMinimise() 
{
#ifndef _WIN32_WCE
#ifdef USING_NONMFC_TRAY  // This is set in project settings
    CSystemTray::MinimiseToTray(GetSafeHwnd());
#else
    CSystemTray::MinimiseToTray(this);
#endif
    m_TrayIcon.SetMenuDefaultItem(ID_DEMO_MAXIMISE, FALSE);
#endif
}

void CAquaControl2014Dlg::OnMaximise() 
{
#ifndef _WIN32_WCE
#ifdef USING_NONMFC_TRAY  // This is set in project settings
    CSystemTray::MaximiseFromTray(GetSafeHwnd());
#else
    CSystemTray::MaximiseFromTray(this);
#endif
    m_TrayIcon.SetMenuDefaultItem(ID_DEMO_MINIMISE, FALSE);
#endif
}

int CAquaControl2014Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)); 
	// Create the tray icon
#ifdef USING_NONMFC_TRAY  // This is set in project settings
	if (!m_TrayIcon.Create(AfxGetInstanceHandle(),
#else
	if (!m_TrayIcon.Create(
#endif
						NULL,                            // Let icon deal with its own messages
                        WM_ICON_NOTIFY,                  // Icon notify message to use
                        _T("-=AQUA CONTROLLER 2014=-\n Дистанционное управление аквариумом"),  // tooltip
                        hIcon,
                        IDR_POPUP_MENU,                  // ID of tray icon
                        FALSE,
                        _T("Ожидание команды..."), // balloon tip
                        _T("Система активирована."),               // balloon title
                        NIIF_NONE,                    // balloon icon
                        20 ))                            // balloon timeout
    {
		return -1;
    }

	// MFC version of CSystemTray automatically sends messages to the main app window
#ifdef USING_NONMFC_TRAY  // This is set in project settings
	m_TrayIcon.SetTargetWnd(GetSafeHwnd());		// Send all menu messages here.
#endif

    m_TrayIcon.SetMenuDefaultItem(3, TRUE);

	CFile file;
	CString fName=_T("option.txt");
	if(file.Open(fName,CFile::modeRead|CFile::shareDenyNone,NULL))
	{
		file.Read(file_buf,5);
	}
	
	if(file_buf[0]==0x31) {OnMinimise(); option_data[0]=0x31;}
	else option_data[0]=0x30;
	if(file_buf[1]==0x31) option_data[1]=0x31;
	else option_data[1]=0x30;
	if(file_buf[2]==0x31) {TIME_SYNC=true; option_data[2]=0x31;}
	else option_data[2]=0x30;
	if(file_buf[3]==0x31) {COM_ON_START=true; option_data[3]=0x31;}
	else option_data[3]=0x30;
	option_data[4]=file_buf[4]; 

	return 0;
}
