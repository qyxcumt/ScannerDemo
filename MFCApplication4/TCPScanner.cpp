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
//                  ���汣��             ����BUG 
//          ��Ի:  
//                  д��¥��д�ּ䣬д�ּ������Ա��  
//                  ������Աд�������ó��򻻾�Ǯ��  
//                  ����ֻ���������������������ߣ�  
//                  ��������ո��գ����������긴�ꡣ  
//                  ��Ը�������Լ䣬��Ը�Ϲ��ϰ�ǰ��  
//                  ���۱������Ȥ���������г���Ա��  
//                  ����Ц��߯��񲣬��Ц�Լ���̫����  
//                  ��������Ư���ã��ĸ���ó���Ա�� 

//When I wrote this, only God and I understood what I was doing
//Now, God only knows


// TCPScanner.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "TCPScanner.h"
#include <sstream>
using namespace std;

#include "MyDBG.h"

//ɨ���߳�
UINT TCPScanner::ThreadCSocketScan(LPVOID pParam)
{
	THREADPARAM* threadparam = (THREADPARAM*)pParam;
	CString strIP = threadparam->strIP;
	UINT uPort = threadparam->uPort;
	TCPScanner *ptr = threadparam->ptr;
	HTREEITEM TreeItem = threadparam->TreeItem;
	BOOL bNoOrder = threadparam->bNoOrder;

	ptr->ThreadCount++;		//�����߳�����
	if (ptr->ThreadCount >= 250)	//���250���߳�
		ResetEvent(ptr->single);	//��λ�¼���ֹͣ�������߳�

	if (bNoOrder)
		Sleep(rand() % 3000);		//�����ʱһ��ʱ�䣬����ɨ��

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
			//��������뵽����
			ptr->m_ctlTreeResult->InsertItem(strTemp, 2, 2, TreeItem);
//�����õĴ��룬ȥ��MyDBG��Ͳ��������δ���
#ifdef MyDBG
			stringstream ss;
			ss<<"�߳�ID��"<< GetCurrentThreadId() << " ���Ŷ˿ڣ�" << uPort << endl;
			MessageBox(NULL, ss.str().c_str(), "DEBUG Information", MB_OK);
#endif

			sock.ShutDown(2);
		}
		sock.Close();
	}

	SetEvent(ptr->single);//�˳�һ���߳���Ŀ��Ȼ����250���������������µ��߳�

	ptr->ThreadCount--;
	//�������ص�����֪ͨ���ɨ��
	if (!ptr->ThreadCount) {
		int tmp = -1;
		if (ptr->run)
			tmp = 0;
		ptr->destory(tmp, ptr->caller);
	}

	return 0;
}
//������ַת���ɵ��ʮ����
CString TCPScanner::IPIntToStr(UINT IPInt)
{
	UNIONIP IP;
	CString IPStr;

	IP.uInt = IPInt;
	IPStr.Format("%d.%d.%d.%d", IP.ucByte[3], IP.ucByte[2], IP.ucByte[1], IP.ucByte[0]);
	return IPStr;
}
//���ʮ����ת��������
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
//ʵ��û�á����ص��ǿ��Ŷ˿ڼ���
set< UINT > TCPScanner::GetOpenPort()
{
	return OpenPort;
}
//��ʼɨ�裬����һЩ����
void TCPScanner::start()
{
	CString strIP;
	UNIONIP uIPBegin, uIPEnd;
	uIPBegin.uInt = IPStrToInt(BeginIP);
	uIPEnd.uInt = IPStrToInt(EndIP);

	for (UINT ip = uIPBegin.uInt;ip <= uIPEnd.uInt;ip++)
	{

		strIP = IPIntToStr(ip);
		HTREEITEM curr = m_ctlTreeResult->InsertItem(strIP, 1, 1, TVI_ROOT); //���������ڵ�

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
