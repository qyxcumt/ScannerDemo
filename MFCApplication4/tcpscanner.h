/**
*
* ━━━━━━神兽出没━━━━━━
* 　　　┏┓　　　┏┓
* 　　┏┛┻━━━┛┻┓
* 　　┃　　　　　　　┃
* 　　┃　　　━　　　┃
* 　　┃　┳┛　┗┳　┃
* 　　┃　　　　　　　┃
* 　　┃　　　┻　　　┃
* 　　┃　　　　　　　┃
* 　　┗━┓　　　┏━┛Code is far away from bug with the animal protecting
* 　　　　┃　　　┃ 神兽保佑,代码无bug
* 　　　　┃　　　┃
* 　　　　┃　　　┗━━━┓
* 　　　　┃　　　　　　　┣┓
* 　　　　┃　　　　　　　┏┛
* 　　　　┗┓┓┏━┳┓┏┛
* 　　　　　┃┫┫　┃┫┫
* 　　　　　┗┻┛　┗┻┛
*
* ━━━━━━感觉萌萌哒━━━━━━
*/

//2015.09.27
//头文件，为何要写头文件呢？
//因为别人都写所以我也写了

#pragma once

#include <iostream>
#include <set>
#include <afxsock.h>
#include <Windows.h>

using namespace std;

typedef union MultiByteStruct
{//IP地址联合体
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
	class TCPScanner* ptr;		//这东西其实就是this指针，因为作为成员函数有隐藏的this指针，
							//所以只能讲线程函数声明为static类型，但实际又需要this指针，
							//所以就在这传递它
	HTREEITEM TreeItem;
}THREADPARAM;
//THREADPARAM ThreadParam;



class TCPScanner {
private:
	BOOL inOrder;		//一个变量
	CString BeginIP;	//又一个变量
	CString EndIP;		//还是一个变量
	UINT	BeginPort;	//同样是一个变量
	UINT	EndPort;	//这也还是一个变量
	BOOL	run;		//控制线程运行
	int		ThreadCount;	//线程计数
	THREADPARAM		ThreadParam;	//传递给线程的参数
	set< UINT > OpenPort;
	CTreeCtrl*	m_ctlTreeResult;
	HANDLE single;
	typedef void(*CallBack)(int, void*);
	CallBack destory;
	void* caller;
public:
	TCPScanner() {};
	//下面函数不是所有的都应该声明成public类型，但声明成public类型也没有什么问题
	TCPScanner(CString sIP, CString eIP, UINT sPort, UINT ePort, BOOL order, CTreeCtrl* tree,CallBack p,void* ptr) :
		inOrder(order), BeginIP(sIP), EndIP(eIP), BeginPort(sPort), EndPort(ePort), 
		run(true), ThreadCount(0), m_ctlTreeResult(tree),destory(p),caller(ptr)/*初始化列表*/
		//为什么要用初始化列表呢？你不觉得很厉害么，不明觉厉……
		//好吧，其实是因为效率高，对内置类型来说无所谓，
		//对类类型来说使用初始化列表少了一次调用默认构造函数的过程
	{
		if (!AfxSocketInit())
		{
			MessageBox(NULL, "Socket初始化失败，请重新运行。", "错误", MB_OK);
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
