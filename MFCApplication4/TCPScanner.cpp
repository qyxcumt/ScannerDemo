//                            _ooOoo_  
//                           o8888888o  
//                           88" . "88  
//                           (| -_- |)  
//                            O\ = /O  
//                        ____/`---'\____  
//                      .   ' \\| |// `.  
//                       / \\||| : |||// \  
//                     / _||||| -:- |||||- \  
//                       | | \\\ - /// | |  
//                     | \_| ''\---/'' | |  
//                      \ .-\__ `-` ___/-. /  
//                   ___`. .' /--.--\ `. . __  
//                ."" '< `.___\_<|>_/___.' >'"".  
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
//                 \ \ `-. \_ __\ /__ _/ .-` / /  
//         ======`-.____`-.___\_____/___.-`____.-'======  
//                            `=---='  
//  
//         .............................................  
//                  佛祖保佑             永无BUG 
//          佛曰:  
//                  写字楼里写字间，写字间里程序员；  
//                  程序人员写程序，又拿程序换酒钱。  
//                  酒醒只在网上坐，酒醉还来网下眠；  
//                  酒醉酒醒日复日，网上网下年复年。  
//                  但愿老死电脑间，不愿鞠躬老板前；  
//                  奔驰宝马贵者趣，公交自行程序员。  
//                  别人笑我忒疯癫，我笑自己命太贱；  
//                  不见满街漂亮妹，哪个归得程序员？ 

//When I wrote this, only God and I understood what I was doing
//Now, God only knows


// TCPScanner.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TCPScanner.h"
#include <sstream>
using namespace std;

#include "MyDBG.h"

//扫描线程
UINT TCPScanner::ThreadCSocketScan(LPVOID pParam)
{
	THREADPARAM* threadparam = (THREADPARAM*)pParam;
	CString strIP = threadparam->strIP;
	UINT uPort = threadparam->uPort;
	TCPScanner *ptr = threadparam->ptr;
	HTREEITEM TreeItem = threadparam->TreeItem;
	BOOL bNoOrder = threadparam->bNoOrder;

	ptr->ThreadCount++;		//运行线程总数
	if (ptr->ThreadCount >= 250)	//最多250个线程
		ResetEvent(ptr->single);	//复位事件，停止创建新线程

	if (bNoOrder)
		Sleep(rand() % 3000);		//随机延时一段时间，乱序扫描

	CSocket sock;
	CString strTemp;
	if (sock.Create())
	{
		//AfxMessageBox("1");
		if (sock.Connect(strIP, uPort))
		{
			//AfxMessageBox("2");
			if (uPort<1024)
			{
				ptr->OpenPort.insert(uPort);
				struct servent *se;
				se = getservbyport(htons(uPort), "tcp");
				if (se != NULL)
					strTemp.Format("%d %s", uPort, se->s_name);
				else
					strTemp.Format("%d", uPort);
			}else
				strTemp.Format("%d", uPort);
			//将结果插入到树中
			ptr->m_ctlTreeResult->InsertItem(strTemp, 2, 2, TreeItem);
//调试用的代码，去掉MyDBG宏就不会编译这段代码
#ifdef MyDBG
			stringstream ss;
			ss<<"线程ID："<< GetCurrentThreadId() << " 开放端口：" << uPort << endl;
			MessageBox(NULL, ss.str().c_str(), "DEBUG Information", MB_OK);
#endif

			sock.ShutDown(2);
		}
		sock.Close();
	}

	SetEvent(ptr->single);//退出一个线程数目必然少于250个，可以再运行新的线程

	ptr->ThreadCount--;
	//析构，回调函数通知完成扫描
	if (!ptr->ThreadCount) {
		int tmp = -1;
		if (ptr->run)
			tmp = 0;
		ptr->destory(tmp, ptr->caller);
	}

	return 0;
}
//整数地址转换成点分十进制
CString TCPScanner::IPIntToStr(UINT IPInt)
{
	UNIONIP IP;
	CString IPStr;

	IP.uInt = IPInt;
	IPStr.Format("%d.%d.%d.%d", IP.ucByte[3], IP.ucByte[2], IP.ucByte[1], IP.ucByte[0]);
	return IPStr;
}
//点分十进制转换成整型
UINT TCPScanner::IPStrToInt(CString IPStr)
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
//实际没用…返回的是开放端口集合
set< UINT > TCPScanner::GetOpenPort()
{
	return OpenPort;
}
//开始扫描，设置一些参数
void TCPScanner::start()
{
	CString strIP;
	UNIONIP uIPBegin, uIPEnd;
	uIPBegin.uInt = IPStrToInt(BeginIP);
	uIPEnd.uInt = IPStrToInt(EndIP);

	for (UINT ip = uIPBegin.uInt;ip <= uIPEnd.uInt;ip++)
	{

		strIP = IPIntToStr(ip);
		HTREEITEM curr = m_ctlTreeResult->InsertItem(strIP, 1, 1, TVI_ROOT); //插入树根节点

		ThreadParam.strIP = strIP;
		ThreadParam.bNoOrder = inOrder;
		ThreadParam.ptr = this;
		ThreadParam.TreeItem = curr;
		for (UINT port = BeginPort;port <= EndPort;port++)
		{
			WaitForSingleObject(single, INFINITE);
			if (!run)
				return;
			ThreadParam.uPort = port;
			AfxBeginThread(ThreadCSocketScan, &ThreadParam, THREAD_PRIORITY_IDLE);
		}
	}
}
//A328-1
//A315
