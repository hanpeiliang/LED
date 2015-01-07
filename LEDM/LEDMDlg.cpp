
// LEDMDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LEDM.h"
#include "LEDMDlg.h"
//#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//静态变量初始化
bool CLEDMDlg::recTimeEventflag = false;
LedSend CLEDMDlg::ledSend;
SOCKET CLEDMDlg::sockConn;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
bool PlayLocalFile();
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLEDMDlg 对话框




CLEDMDlg::CLEDMDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLEDMDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//init
	//初始化为true
	timeCountflag = true;
	//无人审核实时间初始化 120s
	checkedTime = CHECKED_TIME;
	checkedLineflag = false;
	for (int i = 0;i < LED_NUM;++i)
	{
		timeCount[i] = -1;
	}

}

void CLEDMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_DIS, m_pic);
}

BEGIN_MESSAGE_MAP(CLEDMDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECQUEST,&CLEDMDlg::OnRecFile)
	ON_BN_CLICKED(IDC_BUTTON_CHECKED, &CLEDMDlg::OnBnClickedButtonChecked)
	ON_BN_CLICKED(IDC_BUTTON_UNCHECKED, &CLEDMDlg::OnBnClickedButtonUnChecked)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CLEDMDlg 消息处理程序
BOOL CLEDMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//隐藏主窗口，隐藏失败
	//ShowWindow(SW_HIDE);
	//开启定时器
	//socket 接收信息
	SetTimer(1,500,NULL);
	//时间点对比，播放图片
	//开启播放时长计时器
	SetTimer(2,1000,NULL);
	//开启分钟计时器
	//播放时长
	SetTimer(3,1000,NULL);

	//开启新线程
	_beginthread(SocketLedPlayThread,0,NULL);

	//ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLEDMDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLEDMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLEDMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CLEDMDlg::SocketLedPlayThread(void*)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {                                
		return ;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		HIBYTE( wsaData.wVersion ) != 2 ) {
			WSACleanup( );
			return ; 
	}
	SOCKET sockSrv = socket(AF_INET,SOCK_STREAM,0);

	sockaddr_in addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(PLAY_FILE_PORT);

	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	listen(sockSrv,5);

	sockaddr_in addClient;
	int len = sizeof(SOCKADDR);
	while (1)
	{
		sockConn = accept(sockSrv,(SOCKADDR*)&addClient,&len);
		recv(sockConn,(char*)&ledSend,LEDSEND_SIZE,0);
		recTimeEventflag = true;
	}
}
void CLEDMDlg::loadPic(CString str)
{
	CRect rect;
	m_pic.GetClientRect(&rect); //m_picture为Picture Control控件变量，获得控件的区域对象
	CDC* pDC = m_pic.GetWindowDC(); //获得显示控件的DC
	pDC->FillRect(rect,RGB(0,0,0));

	CImage image; //使用图片类
	image.Load(str); //装载路径下图片信息到图片类
	::SetStretchBltMode(pDC->m_hDC, HALFTONE);  
	::SetBrushOrgEx(pDC->m_hDC, 0, 0, NULL);  
	image.Draw(pDC->m_hDC,rect); 
	ReleaseDC(pDC);

}
void CLEDMDlg::loadWord(CString str)
{
	CRect rect;
	m_pic.GetClientRect(&rect); //m_picture为Picture Control控件变量，获得控件的区域对象
	CDC* pDC = m_pic.GetWindowDC(); //获得显示控件的DC

	pDC->FillRect(rect,RGB(0,0,0));
	pDC->DrawText(str,rect,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_TOP|DT_WORDBREAK);
	ReleaseDC(pDC);

}
//通过审核按钮点击事件
void CLEDMDlg::OnBnClickedButtonChecked()
{
	// TODO: 在此添加控件通知处理程序代码
	//android 回复
	feedBack.at(0).Statue = CHECKED_SUCCEED;
	//int sendRet = send(sockConn,(char*)&feedBack,sizeof(feedBack),0);
	//if (sendRet == SOCKET_ERROR)
	//{
	//	//未处理
	//}
	////开启分钟计时器
	//if (timeCountflag == true)
	//{
	//	timeCountflag = false;
	//	SetTimer(2,1000,NULL);
	//}

	ShowWindow(SW_HIDE);
}

//未通过审核按钮点击事件
void CLEDMDlg::OnBnClickedButtonUnChecked()
{
	// TODO: 在此添加控件通知处理程序代码
	//android 回复
	//因为审核失败，所以直接删除加入的元素
	//ledIter.erase(ledIter.begin());
	//返回消息
	feedBack.at(0).Statue = CHECKED_FAILE;
	////int sendRet = send(sockConn,(char*)&feedBack,sizeof(feedBack),0);
	//if (sendRet == SOCKET_ERROR)
	//{
	//	//未处理
	//}
	ShowWindow(SW_HIDE);
}


void CLEDMDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1://1ms
		//收到信息，处理信息
		//审核排队
		//隐藏主窗口
		if (recTimeEventflag)
		{
			recTimeEventflag = false;
			//播放信息插入
			//审核信息入队
			TRACE("----》接收\n");
			feedBack.push_back(InsertLedSendIter(ledIter,ledSend));
			TRACE("****反馈入队\n");
			if (feedBack.at(0).Statue == TIME_CONFLICT_SYSTEM || feedBack.at(0).Statue == TIME_CONFLICT_OTHER)
			{
				TRACE("****时间冲突####结束####\n");
				SendFeedBackToAPP(sockConn,feedBack);
				feedBack.erase(feedBack.begin());
				TRACE("****反馈出队\n");
			}
			else
			{
				//进入审核排队
				checkedLineflag = true;
			}
		}
		//审核排队处理
		if (checkedLineflag)
		{
			//审核队列入队
			TRACE("****审核入队\n");
			ledCheckedIter.push_back(ledSend);
			checkedLineflag = false;
			//审核排队处理
			SetTimer(5,1000,NULL);
		}
		//隐藏窗口
		if (timeCountflag == true)
		{
			timeCountflag = false;
			ShowWindow(SW_HIDE);
		}
		break;
	case 2://1s
		//时间点对比，播放图片
		//开启播放时长计时器
		{
			//获取当前时间
			time_t t = time(0); 
			char tmp[64]; 
			strftime( tmp, sizeof(tmp), "%X",localtime(&t) );
			for (int i = 0;i < LED_NUM;++i)
			{
				//先进行为空判断，提高效率
				if (!ledIter[i].empty() && TimeEqual(tmp,ledIter[i].at(0).playTime.delayTime))
				{
					//到达预约时间
					//1.开始倒计时2.发送
					//本机发送
					SOCKET sockClient = socket(AF_INET,SOCK_STREAM,0);
					sockaddr_in addrSrv;
					addrSrv.sin_addr.S_un.S_addr = inet_addr(PLAY_IP);
					addrSrv.sin_family = AF_INET;
					addrSrv.sin_port = htons(7610);
					connect(sockClient,(sockaddr*)&addrSrv,sizeof(sockaddr));
					send(sockClient,(char*)&(ledIter[i].at(0).cmd),sizeof(NET_CMD),0);
					closesocket(sockClient);
					//开始倒计时
					timeCount[i] = ledIter[i].at(0).playTime.lastTime * 60;
					//开启分钟计时器
					//SetTimer(3,1000*60,NULL);
					//删掉队列元素
					//ledIter[i].erase(ledIter[i].begin());
					TRACE("****播放开始\n");
				}
			}
			//KillTimer(2);
			//每天00:00自动清空所有
			if (TimeEqual(tmp,"0000"))
			{
				//返回信息迭代器初始化
				feedBack.empty();
				//播放时长数组初始化
				for (int i = 0;i < LED_NUM;++i)
				{
					timeCount[i] = -1;
				}
				//播放信息迭代器数组初始化
				for (int i = 0;i < LED_NUM;++i)
				{
					ledIter[i].empty();
				}
				//审核排队迭代器初始化
				ledCheckedIter.empty();
				//删除C:\\FTP上传\\*.png保存的图片
				system("del C:\\FTP上传\\*.png /a /s /f /q");

			}
		}
		break;
	case 3://1s
		//播放时长控制
		for (int i = 0;i < LED_NUM;++i)
		{
			if (timeCount[i] == -1);
			else
				--timeCount[i];
		}
		for (int i = 0;i < LED_NUM;++i)
		{
			if (timeCount[i] == 0)
			{
				//删掉队列元素
				ledIter[i].erase(ledIter[i].begin());
				TRACE("****播放完成\n");
				TRACE("****播放队列出队\n");
				//播放完毕，回复初始值
				timeCount[i] = -1;
				//播放完成处理
				//播放其他东西
				NET_CMD cmd;
				cmd.szFlag[0]='L';
				cmd.szFlag[1]='E';
				cmd.szFlag[2]='D';
				cmd.szFlag[3]='T';
				cmd.szFlag[4]='R';
				cmd.szFlag[5]='L';
				cmd.szFlag[6]='\0';
				cmd.uPacketSize=sizeof(NET_CMD);
				cmd.chCmd=NET_CMD_PLAY_FILES;
				cmd.uCmdParam=0;		//显示特技
				cmd.uCmdResult=0;
				//led号是对应的
				cmd.CTRL_ATTRIBUTE.iLedID=i;
				char *buf=cmd.szStatus;
				CString str=LOCAL_FILE;
				memcpy(buf,str,str.GetLength()+1);	
				SOCKET sockClient = socket(AF_INET,SOCK_STREAM,0);
				sockaddr_in addrSrv;
				addrSrv.sin_addr.S_un.S_addr = inet_addr(PLAY_IP);
				addrSrv.sin_family = AF_INET;
				addrSrv.sin_port = htons(7610);
				connect(sockClient,(sockaddr*)&addrSrv,sizeof(sockaddr));
				int sendRet = send(sockClient,(char*)&(cmd),sizeof(NET_CMD),0);
				if (sendRet == SOCKET_ERROR)
				{
					//未处理
				}
				closesocket(sockClient);
				TRACE("****播放其他内容\n");
			}
		}
		break;
	case 4://1s
		--checkedTime;
		//审核超时计时器
		if (feedBack.at(0).Statue == CHECKED_FAILE || feedBack.at(0).Statue == CHECKED_SUCCEED || feedBack.at(0).Statue == INIT)
		{
			//在规定时间内完成了
			if (checkedTime >= 0)
			{
				if (feedBack.at(0).Statue == CHECKED_SUCCEED)
				{
					//停止计数
					TRACE("#######通过\n");
					KillTimer(4);
					SendFeedBackToAPP(sockConn,feedBack);
					TRACE("****反馈出队\n");
					feedBack.erase(feedBack.begin());
					//只有审核通过了才进入
					InsertLedSendIter(ledIter,ledSendCheck,true);
					TRACE("****播放队列入队\n");
				}
				else if (feedBack.at(0).Statue == CHECKED_FAILE)
				{
					//停止计数
					TRACE("#######未通过\n");
					KillTimer(4);
					SendFeedBackToAPP(sockConn,feedBack);
					TRACE("****反馈出队\n");
					feedBack.erase(feedBack.begin());
				}
				
			}
			else//在规定给的时间内没有完成
			{
				feedBack.at(0).Statue = CHECKED_TIME_OVER; 

				TRACE("#######超时\n");
				SendFeedBackToAPP(sockConn,feedBack);	
				feedBack.erase(feedBack.begin());
				TRACE("****反馈出队\n");
				checkedTime = CHECKED_TIME;
				KillTimer(4);
				ShowWindow(SW_HIDE);
			}
		}
		break;
	case 5://1s
		//审核排队处理
		if (!IsWindowVisible())
		{
			//从队列理取出当前需要审核的内容
			ledSendCheck = ledCheckedIter.at(0);
			//删除第一个
			TRACE("****审核出队\n");
			ledCheckedIter.erase(ledCheckedIter.begin());
			//两分钟审核倒计时
			SetTimer(4,1000,NULL);
			//进入审核
			SendMessage(OnRecFile(0,0));
		}
		if (ledCheckedIter.empty())
		{
			KillTimer(5);
		}
			break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
//处理
LRESULT CLEDMDlg::OnRecFile(WPARAM wParam, LPARAM lParam)
{

	ShowWindow(SW_SHOW);
	//查找当前的数据，并且赋值
	NET_CMD cmd = ledSendCheck.cmd;
	PlayTime playTime = ledSendCheck.playTime;

	CString str;
	switch (cmd.chCmd)
	{
	case NET_CMD_SET_CONTENT:
		{
			char* strDis = cmd.szStatus;
			strDis = strDis + 4;
			loadWord(strDis);
			CString strTemp;
			strTemp.Format("%d",cmd.CTRL_ATTRIBUTE.iLedID + 1);
			str = str + strTemp + "号LED大屏幕收到推送。\n";
			break;
		}
	case NET_CMD_PLAY_FILES:
		{
			char* strDis = cmd.szStatus;
			loadPic(strDis);
			CString strTemp;
			strTemp.Format("%d",cmd.CTRL_ATTRIBUTE.iLedID + 1);
			str = str + strTemp + "号LED大屏幕收到推送。\n";
			break;
		}
	case NET_CMD_PLAY_SINGLE_TEXT:
		{
			char* strDis = cmd.szStatus;
			strDis = strDis + 4;
			loadWord(strDis);
			CString strTemp;
			strTemp.Format("%d",cmd.CTRL_ATTRIBUTE.iLedID + 1);
			str = str + strTemp + "号LED大屏幕收到推送。\n";
			break;
		}
	default:
		break;
	}
	//延时效果显示
	CString strTemp;
	strTemp.Format("预约时间为：%c%c:%c%c。\n播放时长为：%d分钟。", playTime.delayTime[0], playTime.delayTime[1], playTime.delayTime[2], playTime.delayTime[3],playTime.lastTime);
	str = str + strTemp;

	GetDlgItem(IDC_STATIC_TIP)->SetWindowTextA(str);

	return TRUE;
}
void PlayOther(int iLed)
{
	NET_CMD cmd;
	cmd.szFlag[0]='L';
	cmd.szFlag[1]='E';
	cmd.szFlag[2]='D';
	cmd.szFlag[3]='T';
	cmd.szFlag[4]='R';
	cmd.szFlag[5]='L';
	cmd.szFlag[6]='\0';
	cmd.uPacketSize=sizeof(NET_CMD);
	cmd.chCmd=NET_CMD_PLAY_FILES;
	cmd.uCmdParam=0;		//显示特技
	cmd.uCmdResult=0;
	//led号是对应的
	cmd.CTRL_ATTRIBUTE.iLedID=iLed;
	char *buf=cmd.szStatus;
	CString str=LOCAL_FILE;
	memcpy(buf,str,str.GetLength()+1);	
	SOCKET sockClient = socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(PLAY_IP);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(7610);
	connect(sockClient,(sockaddr*)&addrSrv,sizeof(sockaddr));
	int sendRet = send(sockClient,(char*)&(cmd),sizeof(NET_CMD),0);
	if (sendRet == SOCKET_ERROR)
	{
		//未处理
	}
	closesocket(sockClient);
}