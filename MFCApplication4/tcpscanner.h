/**
*
* ���������������޳�û������������
* ��������������������
* ���������ߩ��������ߩ�
* ����������������������
* ����������������������
* ���������ש������ס���
* ����������������������
* �������������ߡ�������
* ����������������������
* ����������������������Code is far away from bug with the animal protecting
* ������������������ ���ޱ���,������bug
* ������������������
* ��������������������������
* �������������������������ǩ�
* ����������������������������
* �������������������ש�����
* �������������ϩϡ����ϩ�
* �������������ߩ������ߩ�
*
* �������������о������թ�����������
*/

//2015.09.27
//ͷ�ļ���Ϊ��Ҫдͷ�ļ��أ�
//��Ϊ���˶�д������Ҳд��

#pragma once

#include <iostream>
#include <set>
#include <afxsock.h>
#include <Windows.h>

using namespace std;

typedef union MultiByteStruct
{//IP��ַ������
	int iInt;
	float fFloat;
	UINT uInt;
	DWORD dwDword;
	WORD wWord[2];
	UCHAR ucByte[4];
}UNIONIP, *PUNIONIP;

typedef struct ThreadParamStruct
{
	CString strIP;
	UINT uPort;
	BOOL bNoOrder;
	class TCPScanner* ptr;		//�ⶫ����ʵ����thisָ�룬��Ϊ��Ϊ��Ա���������ص�thisָ�룬
							//����ֻ�ܽ��̺߳�������Ϊstatic���ͣ���ʵ������Ҫthisָ�룬
							//���Ծ����⴫����
	HTREEITEM TreeItem;
}THREADPARAM;
//THREADPARAM ThreadParam;



class TCPScanner {
private:
	BOOL inOrder;		//һ������
	CString BeginIP;	//��һ������
	CString EndIP;		//����һ������
	UINT	BeginPort;	//ͬ����һ������
	UINT	EndPort;	//��Ҳ����һ������
	BOOL	run;		//�����߳�����
	int		ThreadCount;	//�̼߳���
	THREADPARAM		ThreadParam;	//���ݸ��̵߳Ĳ���
	set< UINT > OpenPort;
	CTreeCtrl*	m_ctlTreeResult;
	HANDLE single;
	typedef void(*CallBack)(int, void*);
	CallBack destory;
	void* caller;
public:
	TCPScanner() {};
	//���溯���������еĶ�Ӧ��������public���ͣ���������public����Ҳû��ʲô����
	TCPScanner(CString sIP, CString eIP, UINT sPort, UINT ePort, BOOL order, CTreeCtrl* tree,CallBack p,void* ptr) :
		inOrder(order), BeginIP(sIP), EndIP(eIP), BeginPort(sPort), EndPort(ePort), 
		run(true), ThreadCount(0), m_ctlTreeResult(tree),destory(p),caller(ptr)/*��ʼ���б�*/
		//ΪʲôҪ�ó�ʼ���б��أ��㲻���ú�����ô��������������
		//�ðɣ���ʵ����ΪЧ�ʸߣ�������������˵����ν��
		//����������˵ʹ�ó�ʼ���б�����һ�ε���Ĭ�Ϲ��캯���Ĺ���
	{
		if (!AfxSocketInit())
		{
			MessageBox(NULL, "Socket��ʼ��ʧ�ܣ����������С�", "����", MB_OK);
		}
		single = CreateEvent(NULL, TRUE, FALSE, NULL);
		SetEvent(single);
	}

	void start();
	static UINT ThreadCSocketScan(LPVOID pParam);


	~TCPScanner() {	};

	CString IPIntToStr(UINT IPInt);
	UINT IPStrToInt(CString IPStr);
	set< UINT > GetOpenPort();
	int getThread()
	{
		return ThreadCount;
	}
	void cancle() {
		run = false;
	}
};
