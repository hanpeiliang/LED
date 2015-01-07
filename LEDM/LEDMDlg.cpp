
// LEDMDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LEDM.h"
#include "LEDMDlg.h"
//#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//��̬������ʼ��
bool CLEDMDlg::recTimeEventflag = false;
LedSend CLEDMDlg::ledSend;
SOCKET CLEDMDlg::sockConn;
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
bool PlayLocalFile();
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
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


// CLEDMDlg �Ի���




CLEDMDlg::CLEDMDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLEDMDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//init
	//��ʼ��Ϊtrue
	timeCountflag = true;
	//�������ʵʱ���ʼ�� 120s
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


// CLEDMDlg ��Ϣ�������
BOOL CLEDMDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//���������ڣ�����ʧ��
	//ShowWindow(SW_HIDE);
	//������ʱ��
	//socket ������Ϣ
	SetTimer(1,500,NULL);
	//ʱ���Աȣ�����ͼƬ
	//��������ʱ����ʱ��
	SetTimer(2,1000,NULL);
	//�������Ӽ�ʱ��
	//����ʱ��
	SetTimer(3,1000,NULL);

	//�������߳�
	_beginthread(SocketLedPlayThread,0,NULL);

	//ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLEDMDlg::OnPaint()
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
	m_pic.GetClientRect(&rect); //m_pictureΪPicture Control�ؼ���������ÿؼ����������
	CDC* pDC = m_pic.GetWindowDC(); //�����ʾ�ؼ���DC
	pDC->FillRect(rect,RGB(0,0,0));

	CImage image; //ʹ��ͼƬ��
	image.Load(str); //װ��·����ͼƬ��Ϣ��ͼƬ��
	::SetStretchBltMode(pDC->m_hDC, HALFTONE);  
	::SetBrushOrgEx(pDC->m_hDC, 0, 0, NULL);  
	image.Draw(pDC->m_hDC,rect); 
	ReleaseDC(pDC);

}
void CLEDMDlg::loadWord(CString str)
{
	CRect rect;
	m_pic.GetClientRect(&rect); //m_pictureΪPicture Control�ؼ���������ÿؼ����������
	CDC* pDC = m_pic.GetWindowDC(); //�����ʾ�ؼ���DC

	pDC->FillRect(rect,RGB(0,0,0));
	pDC->DrawText(str,rect,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_TOP|DT_WORDBREAK);
	ReleaseDC(pDC);

}
//ͨ����˰�ť����¼�
void CLEDMDlg::OnBnClickedButtonChecked()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//android �ظ�
	feedBack.at(0).Statue = CHECKED_SUCCEED;
	//int sendRet = send(sockConn,(char*)&feedBack,sizeof(feedBack),0);
	//if (sendRet == SOCKET_ERROR)
	//{
	//	//δ����
	//}
	////�������Ӽ�ʱ��
	//if (timeCountflag == true)
	//{
	//	timeCountflag = false;
	//	SetTimer(2,1000,NULL);
	//}

	ShowWindow(SW_HIDE);
}

//δͨ����˰�ť����¼�
void CLEDMDlg::OnBnClickedButtonUnChecked()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//android �ظ�
	//��Ϊ���ʧ�ܣ�����ֱ��ɾ�������Ԫ��
	//ledIter.erase(ledIter.begin());
	//������Ϣ
	feedBack.at(0).Statue = CHECKED_FAILE;
	////int sendRet = send(sockConn,(char*)&feedBack,sizeof(feedBack),0);
	//if (sendRet == SOCKET_ERROR)
	//{
	//	//δ����
	//}
	ShowWindow(SW_HIDE);
}


void CLEDMDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case 1://1ms
		//�յ���Ϣ��������Ϣ
		//����Ŷ�
		//����������
		if (recTimeEventflag)
		{
			recTimeEventflag = false;
			//������Ϣ����
			//�����Ϣ���
			TRACE("----������\n");
			feedBack.push_back(InsertLedSendIter(ledIter,ledSend));
			TRACE("****�������\n");
			if (feedBack.at(0).Statue == TIME_CONFLICT_SYSTEM || feedBack.at(0).Statue == TIME_CONFLICT_OTHER)
			{
				TRACE("****ʱ���ͻ####����####\n");
				SendFeedBackToAPP(sockConn,feedBack);
				feedBack.erase(feedBack.begin());
				TRACE("****��������\n");
			}
			else
			{
				//��������Ŷ�
				checkedLineflag = true;
			}
		}
		//����ŶӴ���
		if (checkedLineflag)
		{
			//��˶������
			TRACE("****������\n");
			ledCheckedIter.push_back(ledSend);
			checkedLineflag = false;
			//����ŶӴ���
			SetTimer(5,1000,NULL);
		}
		//���ش���
		if (timeCountflag == true)
		{
			timeCountflag = false;
			ShowWindow(SW_HIDE);
		}
		break;
	case 2://1s
		//ʱ���Աȣ�����ͼƬ
		//��������ʱ����ʱ��
		{
			//��ȡ��ǰʱ��
			time_t t = time(0); 
			char tmp[64]; 
			strftime( tmp, sizeof(tmp), "%X",localtime(&t) );
			for (int i = 0;i < LED_NUM;++i)
			{
				//�Ƚ���Ϊ���жϣ����Ч��
				if (!ledIter[i].empty() && TimeEqual(tmp,ledIter[i].at(0).playTime.delayTime))
				{
					//����ԤԼʱ��
					//1.��ʼ����ʱ2.����
					//��������
					SOCKET sockClient = socket(AF_INET,SOCK_STREAM,0);
					sockaddr_in addrSrv;
					addrSrv.sin_addr.S_un.S_addr = inet_addr(PLAY_IP);
					addrSrv.sin_family = AF_INET;
					addrSrv.sin_port = htons(7610);
					connect(sockClient,(sockaddr*)&addrSrv,sizeof(sockaddr));
					send(sockClient,(char*)&(ledIter[i].at(0).cmd),sizeof(NET_CMD),0);
					closesocket(sockClient);
					//��ʼ����ʱ
					timeCount[i] = ledIter[i].at(0).playTime.lastTime * 60;
					//�������Ӽ�ʱ��
					//SetTimer(3,1000*60,NULL);
					//ɾ������Ԫ��
					//ledIter[i].erase(ledIter[i].begin());
					TRACE("****���ſ�ʼ\n");
				}
			}
			//KillTimer(2);
			//ÿ��00:00�Զ��������
			if (TimeEqual(tmp,"0000"))
			{
				//������Ϣ��������ʼ��
				feedBack.empty();
				//����ʱ�������ʼ��
				for (int i = 0;i < LED_NUM;++i)
				{
					timeCount[i] = -1;
				}
				//������Ϣ�����������ʼ��
				for (int i = 0;i < LED_NUM;++i)
				{
					ledIter[i].empty();
				}
				//����Ŷӵ�������ʼ��
				ledCheckedIter.empty();
				//ɾ��C:\\FTP�ϴ�\\*.png�����ͼƬ
				system("del C:\\FTP�ϴ�\\*.png /a /s /f /q");

			}
		}
		break;
	case 3://1s
		//����ʱ������
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
				//ɾ������Ԫ��
				ledIter[i].erase(ledIter[i].begin());
				TRACE("****�������\n");
				TRACE("****���Ŷ��г���\n");
				//������ϣ��ظ���ʼֵ
				timeCount[i] = -1;
				//������ɴ���
				//������������
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
				cmd.uCmdParam=0;		//��ʾ�ؼ�
				cmd.uCmdResult=0;
				//led���Ƕ�Ӧ��
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
					//δ����
				}
				closesocket(sockClient);
				TRACE("****������������\n");
			}
		}
		break;
	case 4://1s
		--checkedTime;
		//��˳�ʱ��ʱ��
		if (feedBack.at(0).Statue == CHECKED_FAILE || feedBack.at(0).Statue == CHECKED_SUCCEED || feedBack.at(0).Statue == INIT)
		{
			//�ڹ涨ʱ���������
			if (checkedTime >= 0)
			{
				if (feedBack.at(0).Statue == CHECKED_SUCCEED)
				{
					//ֹͣ����
					TRACE("#######ͨ��\n");
					KillTimer(4);
					SendFeedBackToAPP(sockConn,feedBack);
					TRACE("****��������\n");
					feedBack.erase(feedBack.begin());
					//ֻ�����ͨ���˲Ž���
					InsertLedSendIter(ledIter,ledSendCheck,true);
					TRACE("****���Ŷ������\n");
				}
				else if (feedBack.at(0).Statue == CHECKED_FAILE)
				{
					//ֹͣ����
					TRACE("#######δͨ��\n");
					KillTimer(4);
					SendFeedBackToAPP(sockConn,feedBack);
					TRACE("****��������\n");
					feedBack.erase(feedBack.begin());
				}
				
			}
			else//�ڹ涨����ʱ����û�����
			{
				feedBack.at(0).Statue = CHECKED_TIME_OVER; 

				TRACE("#######��ʱ\n");
				SendFeedBackToAPP(sockConn,feedBack);	
				feedBack.erase(feedBack.begin());
				TRACE("****��������\n");
				checkedTime = CHECKED_TIME;
				KillTimer(4);
				ShowWindow(SW_HIDE);
			}
		}
		break;
	case 5://1s
		//����ŶӴ���
		if (!IsWindowVisible())
		{
			//�Ӷ�����ȡ����ǰ��Ҫ��˵�����
			ledSendCheck = ledCheckedIter.at(0);
			//ɾ����һ��
			TRACE("****��˳���\n");
			ledCheckedIter.erase(ledCheckedIter.begin());
			//��������˵���ʱ
			SetTimer(4,1000,NULL);
			//�������
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
//����
LRESULT CLEDMDlg::OnRecFile(WPARAM wParam, LPARAM lParam)
{

	ShowWindow(SW_SHOW);
	//���ҵ�ǰ�����ݣ����Ҹ�ֵ
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
			str = str + strTemp + "��LED����Ļ�յ����͡�\n";
			break;
		}
	case NET_CMD_PLAY_FILES:
		{
			char* strDis = cmd.szStatus;
			loadPic(strDis);
			CString strTemp;
			strTemp.Format("%d",cmd.CTRL_ATTRIBUTE.iLedID + 1);
			str = str + strTemp + "��LED����Ļ�յ����͡�\n";
			break;
		}
	case NET_CMD_PLAY_SINGLE_TEXT:
		{
			char* strDis = cmd.szStatus;
			strDis = strDis + 4;
			loadWord(strDis);
			CString strTemp;
			strTemp.Format("%d",cmd.CTRL_ATTRIBUTE.iLedID + 1);
			str = str + strTemp + "��LED����Ļ�յ����͡�\n";
			break;
		}
	default:
		break;
	}
	//��ʱЧ����ʾ
	CString strTemp;
	strTemp.Format("ԤԼʱ��Ϊ��%c%c:%c%c��\n����ʱ��Ϊ��%d���ӡ�", playTime.delayTime[0], playTime.delayTime[1], playTime.delayTime[2], playTime.delayTime[3],playTime.lastTime);
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
	cmd.uCmdParam=0;		//��ʾ�ؼ�
	cmd.uCmdResult=0;
	//led���Ƕ�Ӧ��
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
		//δ����
	}
	closesocket(sockClient);
}