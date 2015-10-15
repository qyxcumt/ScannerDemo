
// MFCApplication4Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCApplication4.h"
#include "MFCApplication4Dlg.h"
#include "afxdialogex.h"

#include <winsock2.h> 
#include <afxsock.h>
#include <ws2tcpip.h>

#include <Windows.h>
#include "TCPScanner.h"
#pragma comment(lib,"ws2_32")

#include "MyDBG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define INIFILENAME ".\\PortScan.ini"

//#define MyDBG


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
CString CMFCApplication4Dlg::IPIntToStr(UINT IPInt)
{
	UNIONIP IP;
	CString IPStr;

	IP.uInt = IPInt;
	IPStr.Format("%d.%d.%d.%d", IP.ucByte[3], IP.ucByte[2], IP.ucByte[1], IP.ucByte[0]);
	return IPStr;
}

CString CMFCApplication4Dlg::IPDwordToStr(UINT IPDword)
{
	UNIONIP IP;
	CString IPStr;

	IP.dwDword = IPDword;
	IPStr.Format("%d.%d.%d.%d", IP.ucByte[3], IP.ucByte[2], IP.ucByte[1], IP.ucByte[0]);
	return IPStr;
}

UINT CMFCApplication4Dlg::IPStrToInt(CString IPStr)
{
	UNIONIP IP;

	int i, j = 0;
	IPStr.TrimLeft(" ");
	IPStr.TrimRight(" ");
	for (i = 0;i<IPStr.GetLength();i++)
	{
		if (IPStr.GetAt(i) <'0' || IPStr.GetAt(i)>'9')
			if (IPStr.GetAt(i) == '.')
				j++;
			else
				return 0;
	}
	if (j != 3)
		return 0;
	i = 0;
	IPStr += ".";
	CString temp;
	for (int m = 0;m<4;m++)
	{
		temp = "";
		while (IPStr.GetAt(i) != '.')
		{
			temp += IPStr.GetAt(i);
			i++;
		}
		i++;
		if (temp == "" || atoi(temp) > 0xFF)
			return 0;
		else
			IP.ucByte[3 - m] = atoi(temp);
	}
	return IP.uInt;
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication4Dlg �Ի���



CMFCApplication4Dlg::CMFCApplication4Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCAPPLICATION4_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_Result, m_ctlTreeResult);
	DDX_Text(pDX, IDC_EDIT_IPEnd, m_strIPEnd);
	DDX_Text(pDX, IDC_EDIT_IPBegin, m_strIPBegin);
	DDX_Text(pDX, IDC_EDIT_PortBegin, m_uPortBegin);
	DDX_Text(pDX, IDC_EDIT_PortEnd, m_uPortEnd);
	DDX_Check(pDX, IDC_CHECK_NoOrder, m_bNoOrder);
}

BEGIN_MESSAGE_MAP(CMFCApplication4Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Scan, &CMFCApplication4Dlg::OnBnClickedButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_ScanStop, &CMFCApplication4Dlg::OnBnClickedButtonScanstop)
END_MESSAGE_MAP()


// CMFCApplication4Dlg ��Ϣ�������

BOOL CMFCApplication4Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������


	srand((unsigned)time(NULL));


	CString strTemp;
	char buff[MAX_PATH];
	GetPrivateProfileString("Param", "IPBegin", "127.0.0.1", buff, MAX_PATH, INIFILENAME);
	m_strIPBegin.Format("%s", buff);
	GetPrivateProfileString("Param", "IPEnd", "127.0.0.1", buff, MAX_PATH, INIFILENAME);
	m_strIPEnd.Format("%s", buff);
	m_uPortBegin = GetPrivateProfileInt("Param", "PortBegin", 1, INIFILENAME);
	m_uPortEnd = GetPrivateProfileInt("Param", "PortEnd", 1024, INIFILENAME);
	test = NULL;
	UpdateData(FALSE);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
					//Ȼ����û��ʲô�ã������꽹�㷵��FALSEΪʲô����Ҳ���ԣ�������
}

void CMFCApplication4Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCApplication4Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCApplication4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}







void CMFCApplication4Dlg::OnCancel()
{
	// TODO: Add extra cleanup here
	if (MessageBox("ȷʵҪ�˳���?", "�˳�ϵͳ", MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDOK)
	{
		CString strTemp;
		WritePrivateProfileString("Param", "IPBegin", m_strIPBegin, INIFILENAME);
		WritePrivateProfileString("Param", "IPEnd", m_strIPEnd, INIFILENAME);
		strTemp.Format("%d", m_uPortBegin);
		WritePrivateProfileString("Param", "PortBegin", strTemp, INIFILENAME);
		strTemp.Format("%d", m_uPortEnd);
		WritePrivateProfileString("Param", "PortEnd", strTemp, INIFILENAME);

		WSACleanup();

		if (test) {
			delete test;
			test = NULL;
		}

		CDialog::OnCancel();
	}
}

void CMFCApplication4Dlg::OnBnClickedButtonScan()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (test != NULL) {
		test->cancle();
	}
	if (m_uPortBegin<1 || m_uPortBegin>65535 || m_uPortEnd<1 || m_uPortEnd>65535 || m_uPortBegin>m_uPortEnd)
	{
		MessageBox("�˿�ֵ���ԣ���ֵ��ΧӦ����1~65535�����ҿ�ʼ�˿�Ӧ��С�ڻ���ڽ����˿ڡ�", "������ʾ", MB_OK | MB_ICONERROR);
		return;
	}
	UNIONIP uIPBegin, uIPEnd;
	uIPBegin.uInt = IPStrToInt(m_strIPBegin);
	uIPEnd.uInt = IPStrToInt(m_strIPEnd);
	if (uIPBegin.uInt>uIPEnd.uInt)
	{
		MessageBox("IP��ַ���ԣ�����IPӦ�ô��ڻ���ڿ�ʼIP", "������ʾ", MB_OK | MB_ICONERROR);
		return;
	}

	m_ctlTreeResult.DeleteAllItems();

	AfxBeginThread(ScanThread, (void*)this, THREAD_PRIORITY_IDLE);

	UpdateData(FALSE);
}

//ɨ���̣߳����߳�ʵ����TCPScanner�ಢ��ʼɨ��
//�˺���Ϊstatic���ͣ���Ϊ��Ա��������������thisָ�룬������
UINT CMFCApplication4Dlg::ScanThread(LPVOID p)
{
	CMFCApplication4Dlg* tmp = (CMFCApplication4Dlg*)p;
	tmp->test = new TCPScanner(tmp->m_strIPBegin, 
		tmp->m_strIPEnd, 
		tmp->m_uPortBegin, 
		tmp->m_uPortEnd, 
		tmp->m_bNoOrder, 
		&(tmp->m_ctlTreeResult),
		FreeScanner,
		(void*)p);
	tmp->test->start();
	return 0;
}

//��ֹɨ��
void CMFCApplication4Dlg::OnBnClickedButtonScanstop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(test!=NULL)
		test->cancle();
}

void CMFCApplication4Dlg::FreeScanner(int no, void* p)
{
	CMFCApplication4Dlg* tmp = (CMFCApplication4Dlg*)p;
	if (tmp->test) {
		delete tmp->test;
		tmp->test = NULL;
	}

	switch (no) {
	case 0:
		::MessageBox(NULL, "ɨ�����", "��ʾ", NULL);
		break;
	case -1:
		::MessageBox(NULL, "ɨ����ֹ", "��ʾ", NULL);
		break;
	}
}