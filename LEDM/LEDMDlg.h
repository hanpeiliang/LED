
// LEDMDlg.h : ͷ�ļ�
//

#pragma once
#include "MySocket.h"
#include <process.h>
#include "afxwin.h"
#include <queue>
#include "Oper.h"
#include <iostream>

#define WM_RECQUEST WM_USER+10
//android�˶˿ڶ�Ӧ
#define PLAY_FILE_PORT 9900
#define LEDSEND_SIZE sizeof(LedSend)
//led��Ļ��
#define LED_NUM 9
//���ʱ���޶�
#define CHECKED_TIME 30
//��������ַ
#define PLAY_IP "192.168.100.110"//"192.168.1.104"
//������ĳ���ļ�
#define LOCAL_FILE "C:\\FTP����\\flower.jpg"



// CLEDMDlg �Ի���
class CLEDMDlg : public CDialogEx
{
// ����
public:
	CLEDMDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LEDM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//���͵Ĳ���
	afx_msg LRESULT OnRecFile(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	//��� ���
	afx_msg void OnBnClickedButtonChecked();
	//��� δ���
	afx_msg void OnBnClickedButtonUnChecked();
	//���ղ�����ʱ��Ϣ���߳�
	static void  SocketLedPlayThread(void*);
	//Ԥ��ͼƬ
	void loadPic(CString str);
	//Ԥ������
	void loadWord(CString str);
	//��ʱ��
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	//������Ϣ
	static LedSend ledSend;
	//��ǰ������˵Ľ��յ�����Ϣ
	LedSend ledSendCheck;
	//������Ϣ������
	std::vector<LedFeedBcak> feedBack;
	//������������
	//��ʼ��ֵΪ-1
	int timeCount[9];
	//����ʱ�䵹��ʱ��־��
	//��ʼ��Ϊtrue��ʹ��һ��
	bool timeCountflag;
	//�����Ҫ��Ϣ�Ľṹ��
	//ÿ����Ļ�������й���,��Ҫ�ı��Ϊ����
	std::vector<LedSend> ledIter[9];
	//��ǰ���ڲ���������һ����Ļ
	//ֵ���� 0-8
	//int ledNum;
	//�������ŶӵĽṹ��
	std::vector<LedSend> ledCheckedIter;
	//����WM_RECTIME��Ϣ��־λ
	static bool recTimeEventflag;
	//�Է�������ʱ��socket
	static SOCKET sockConn;
	//�������Ƿ�����ж�
	int checkedTime;
	//����Ŷӱ�־��
	//����Ŷ���Ҫ�����������еļ�ʱ��
	bool checkedLineflag;
public:
	//��ʾͼƬ������
	CStatic m_pic;
};

