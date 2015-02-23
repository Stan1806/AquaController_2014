// MainOption.cpp : implementation file
//

#include "stdafx.h"
#include "AquaControl 2014.h"
#include "MainOption.h"
#include "AquaControl 2014Dlg.h"

// CMainOption dialog
char option_data[5]={0x30,0x30,0x30,0x30,0x30};

IMPLEMENT_DYNAMIC(CMainOption, CDialog)

CMainOption::CMainOption(CWnd* pParent /*=NULL*/)
	: CDialog(CMainOption::IDD, pParent)
	, SHOW_TEXT(false)
{

}

CMainOption::~CMainOption()
{
	CFile file;
	CString fName=_T("option.txt");
	if(file.Open(fName,CFile::modeWrite|CFile::shareDenyNone|CFile::modeCreate,NULL))
	{
		file.Write(option_data,5);
		file.Close();
	}
	else
	{
		MessageBox(_T("Ошибка создания файла"));
	}
}

void CMainOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MAIN_OPTION, m_group_main_option);
	DDX_Control(pDX, IDC_COMBO_COM_PORT_NUM, m_combo_com_port_num);
}


BEGIN_MESSAGE_MAP(CMainOption, CDialog)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CHECK_TRAY, &CMainOption::OnBnClickedCheckTray)
	ON_BN_CLICKED(IDC_CHECK_SAVE_EEPROM, &CMainOption::OnBnClickedCheckSaveEeprom)
	ON_BN_CLICKED(IDC_CHECK_TIME_COMPARE, &CMainOption::OnBnClickedCheckTimeCompare)
	ON_BN_CLICKED(IDC_CHECK_COM_PORT_OPEN, &CMainOption::OnBnClickedCheckComPortOpen)
END_MESSAGE_MAP()


// CMainOption message handlers

void CMainOption::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==0)
	{
		if(!SHOW_TEXT)
		{
			m_group_main_option.SetCatptionTextColor(RGB(16,140,231)).SetFontBold(FALSE);
			m_group_main_option.SetAlignment(SS_LEFT);
			
			if(file_buf[0]==0x31) {CheckDlgButton(IDC_CHECK_TRAY,1); option_data[0]=0x31;}
			else {CheckDlgButton(IDC_CHECK_TRAY,0); option_data[0]=0x30;}
			if(file_buf[1]==0x31) {CheckDlgButton(IDC_CHECK_SAVE_EEPROM,1); option_data[1]=0x31;}
			else {CheckDlgButton(IDC_CHECK_SAVE_EEPROM,0); option_data[1]=0x30;}
			if(file_buf[2]==0x31) {CheckDlgButton(IDC_CHECK_TIME_COMPARE,1); option_data[2]=0x31;}
			else {CheckDlgButton(IDC_CHECK_TIME_COMPARE,0); option_data[2]=0x30;}
			
			option_data[4]=file_buf[4];
			m_combo_com_port_num.SetCurSel(file_buf[4]-0x31);
			
			if(file_buf[3]==0x31) 
			{
				GetDlgItem(IDC_CHECK_TIME_COMPARE)->EnableWindow(TRUE);
				GetDlgItem(IDC_COMBO_COM_PORT_NUM)->EnableWindow(FALSE);
				CheckDlgButton(IDC_CHECK_COM_PORT_OPEN,1); 
				option_data[3]=0x31;
			}
			else 
			{
				GetDlgItem(IDC_CHECK_TIME_COMPARE)->EnableWindow(FALSE);
				GetDlgItem(IDC_COMBO_COM_PORT_NUM)->EnableWindow(TRUE);
				CheckDlgButton(IDC_CHECK_COM_PORT_OPEN,0); 
				option_data[3]=0x30;
			}
			
			SHOW_TEXT=true;
		}
	}
	CDialog::OnTimer(nIDEvent);
}

int CMainOption::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFile file;
	CString fName=_T("option.txt");
	if(file.Open(fName,CFile::modeRead|CFile::shareDenyNone,NULL))
	{
		file.Read(file_buf,5);
	}
	
	SetTimer(0,1,NULL);
	// TODO:  Add your specialized creation code here

	return 0;
}

void CMainOption::OnBnClickedCheckTray()
{
	tray=(CButton*)GetDlgItem(IDC_CHECK_TRAY);
	if(tray->GetCheck()) 
	{
		option_data[0]=0x31;
	}
	else
	{
		option_data[0]=0x30;
	}
}

void CMainOption::OnBnClickedCheckSaveEeprom()
{
	eeprom=(CButton*)GetDlgItem(IDC_CHECK_SAVE_EEPROM);
	if(eeprom->GetCheck()) 
	{
		file_buf[1]=0x31;
		option_data[1]=0x31;
	}
	else
	{
		file_buf[1]=0x30;
		option_data[1]=0x30;
	}
}

void CMainOption::OnBnClickedCheckTimeCompare()
{
	time_compare=(CButton*)GetDlgItem(IDC_CHECK_TIME_COMPARE);
	if(time_compare->GetCheck()) 
	{
		option_data[2]=0x31;
	}
	else
	{
		option_data[2]=0x30;
	}
}

void CMainOption::OnBnClickedCheckComPortOpen()
{
	com_port=(CButton*)GetDlgItem(IDC_CHECK_COM_PORT_OPEN);
	if(com_port->GetCheck()) 
	{
		GetDlgItem(IDC_CHECK_TIME_COMPARE)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_COM_PORT_NUM)->EnableWindow(FALSE);
		option_data[3]=0x31;
		option_data[4]=0x30+m_combo_com_port_num.GetCurSel()+1;
	}
	else
	{
		GetDlgItem(IDC_CHECK_TIME_COMPARE)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_COM_PORT_NUM)->EnableWindow(TRUE);
		option_data[3]=0x30;
		CheckDlgButton(IDC_CHECK_TIME_COMPARE,0);
		option_data[2]=0x30;
	}
}
