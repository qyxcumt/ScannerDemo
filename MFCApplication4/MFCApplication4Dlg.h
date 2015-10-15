
// MFCApplication4Dlg.h : 头文件
//

#include "tcpscanner.h"
#pragma once

;

// CMFCApplication4Dlg 对话框
class CMFCApplication4Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication4Dlg(CWnd* pParent = NULL);	// 标准构造函数

	CString GetPortName(UINT uPort, BOOL bIsTCP);
	CString IPIntToStr(UINT IPInt);
	CString IPDwordToStr(UINT IPDword);
	UINT IPStrToInt(CString IPStr);
	static UINT ScanThread( LPVOID );
	void OnCancel();
	static void FreeScanner(int, void*);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION4_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	enum { IDD = IDD_MFCAPPLICATION4_DIALOG };
	CTreeCtrl	m_ctlTreeResult;
	CString	m_strIPEnd;
	CString	m_strIPBegin;
	CString	m_strComment;
	UINT	m_uPortBegin;
	UINT	m_uPortEnd;
	BOOL	m_bNoOrder;
	TCPScanner* test;


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonScan();
	afx_msg void OnBnClickedButtonScanstop();
};
