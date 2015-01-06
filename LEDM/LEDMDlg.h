
// LEDMDlg.h : 头文件
//

#pragma once
#include "MySocket.h"
#include <process.h>
#include "afxwin.h"
#include <queue>
#include "Oper.h"
#include <iostream>

#define WM_RECQUEST WM_USER+10
//android端端口对应
#define PLAY_FILE_PORT 9900
#define LEDSEND_SIZE sizeof(LedSend)
//led屏幕数
#define LED_NUM 9
//审核时间限定
#define CHECKED_TIME 30
//服务器地址
#define PLAY_IP "192.168.100.110"//"192.168.1.104"
//服务器某个文件
#define LOCAL_FILE "C:\\FTP下载\\flower.jpg"



// CLEDMDlg 对话框
class CLEDMDlg : public CDialogEx
{
// 构造
public:
	CLEDMDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LEDM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//发送的操作
	afx_msg LRESULT OnRecFile(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	//点击 审核
	afx_msg void OnBnClickedButtonChecked();
	//点击 未审核
	afx_msg void OnBnClickedButtonUnChecked();
	//接收播放延时信息的线程
	static void  SocketLedPlayThread(void*);
	//预览图片
	void loadPic(CString str);
	//预览文字
	void loadWord(CString str);
	//计时器
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	//接收信息
	static LedSend ledSend;
	//当前正在审核的接收到的信息
	LedSend ledSendCheck;
	//反馈信息迭代器
	std::vector<LedFeedBcak> feedBack;
	//计数自增变量
	//初始化值为-1
	int timeCount[9];
	//开启时间倒计时标志量
	//初始化为true，使用一次
	bool timeCountflag;
	//存放主要信息的结构体
	//每块屏幕单独进行管理,需要改变成为数组
	std::vector<LedSend> ledIter[9];
	//当前正在操作的是那一块屏幕
	//值介于 0-8
	//int ledNum;
	//存放审核排队的结构体
	std::vector<LedSend> ledCheckedIter;
	//触发WM_RECTIME消息标志位
	static bool recTimeEventflag;
	//对方发送延时的socket
	static SOCKET sockConn;
	//两分钟是否审核判定
	int checkedTime;
	//审核排队标志量
	//审核排队需要放入永久运行的计时器
	bool checkedLineflag;
public:
	//显示图片、文字
	CStatic m_pic;
};

