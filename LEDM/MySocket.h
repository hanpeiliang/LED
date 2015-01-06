#include <atlstr.h>
#if !defined(AFX_MYSOCKET_H__896A4B82_2FBD_4EE2_9483_D256927EC47E__INCLUDED_)
#define AFX_MYSOCKET_H__896A4B82_2FBD_4EE2_9483_D256927EC47E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define DELAY_LENGTH 6
//����״̬����
//ʱ��������˳�ͻ
//#define TIME_CONFLICT_OTHER 1
////ʱ���ϵͳʱ���ͻ
//#define TIME_CONFLICT_SYSTEM 2
////���ʱ�����
//#define CHECKED_TIME_OVER 3
////���ʧ��
//#define CHECKED_FAILE 4
////��˳ɹ�
//#define CHECKED_SUCCEED 5

//��ͬ״̬�Ķ���
enum FEED_BACK_STATUS
{
	//��ʼ����ֵ
	INIT = 0,
	//ʱ���ϵͳʱ���ͻ
	TIME_CONFLICT_SYSTEM = 1,
	//ʱ��������˳�ͻ
	TIME_CONFLICT_OTHER = 2,
	//���ʱ�����
	CHECKED_TIME_OVER = 3,
	//���ʧ��
	CHECKED_FAILE = 4,
	//��˳ɹ�
	CHECKED_SUCCEED = 5,
};
enum NET_CMD_TYPE
{	
	NET_CMD_ACK = 0,					// �ظ�����ʱ��ʹ��
	NET_CMD_SWITCH_DEVICE,				// Ԥ��
	NET_CMD_SWITCH_DEVICE_SCREEN,		// ���ش�����Դ;
									// uCmdParam=0�رգ�uCmdParam=1����

	NET_CMD_GET_CONTENT,				// �õ���ǰ�������ֵ�����
									// szStatusǰ�ĸ��ֽ�Ϊ���ֳ���
									// szStatus�ĸ��ֽ�֮��Ϊ��������

	NET_CMD_SET_CONTENT,				// ��������(������RTF��ʽ)
									// uCmdParam��ʾ���
									// uCmdResultͣ��ʱ��
									// szStatusǰ�ĸ��ֽ�Ϊ���ֳ���,
									// szStatus�ĸ��ֽ�֮��Ϊ��������

	NET_CMD_END_CONTENT,				// �������ֲ���,������������Ž�Ŀ�ļ�
	NET_CMD_CHECK_STATUS,				// 
	NET_CMD_START_RUNNING,				// 
	NET_CMD_TEST_LCD,					// 
	NET_CMD_TAKE,						// 
	NET_CMD_LCD_SET,					// 
	NET_CMD_CHECK_FLASH_CAP,			// 
	NET_CMD_READ_FLASH_INF,			// 
	NET_CMD_FORMAT_FLASH,				// 
	NET_CMD_LOCK,						// ������������;
									// uCmdParam=0����,uCmdParam=1����

	NET_CMD_GET_STATUS,				// ��ȡ��Դ,����,������Ϣ
									// ��ԴszStatus(1��4�ֽ�)
									// ����szStatus(5��8�ֽ�)
									// ����szStatus(9��12�ֽ�)

	NET_CMD_PLAY,						// ����
	NET_CMD_PAUSE,					// ��ͣ����
	NET_CMD_STOP,						// ֹͣ����
	NET_CMD_PLAY_LAST,					// ������һ��Ŀҳ
	NET_CMD_PLAY_NEXT,					// ������һ��Ŀҳ
	NET_CMD_PLAY_PROGRAM,				// ���Ž�Ŀ�ļ�
									// ·������szStatus��,��'\0'����

	NET_CMD_PLAY_FILES,				// �����ļ�
									// ·������szStatus��,��'\0'����
	NET_CMD_PLAY_SINGLE_TEXT,			// ���ŵ����ı���������CTRL_ATTRIBUTE������

	//
	NET_CMD_EN_DOTCR		=	128,		// �����У��
									// uCmdParam=0������uCmdParam=1����  
      
	NET_CMD_GET_SCR_SIZE,				// ��ȡ��ʾ����С;
									// szStatus 1��4�ֽ�Ϊ���
									// szStatus 5��8���ֽ�Ϊ�߶�  

	NET_CMD_SET_SCR_BRIGHT,			// ������ʾ������
									// szStatus 1��4�ֽ�ΪҪ���õ���ʾ��(ֻ��һ����Ϊ0)
									// szStatus 5��8�ֽ�Ϊ����ֵ(0-100)
									// szStatus 9��12�ֽ�Ϊɫ�º�ֵ(0-100)
									// szStatus 13��16�ֽ�Ϊɫ����ֵ(0-100)     
									// szStatus 17��20�ֽ�Ϊɫ����ֵ(0-100)  

	NET_CMD_SET_SCR_BRIGHTEX,			// ������ʾ������
									// uCmdParamΪ����ֵ0��100    
 
	NET_CMD_SET_SCR_AUTOBRIGHT,			// ������ʾ���Զ����ȵ���; 
									// uCmdParam=0�رգ�uCmdParam=1����

	NET_CMD_DOTCR_CMD		=	250,		// Զ�̵�У����������
									// У�����ݷ���szStatus��
};

#pragma pack(push) //�������״̬ 
#pragma pack(1)//�趨Ϊ4�ֽڶ��� 
//���Ʋ��ŵĽṹ��
typedef struct
{	
	char szFlag[8];			// ��ǣ��̶�Ϊ��LCDCRT��
	UINT uPacketSize;			// ���Ĵ�С, NET_CMD���ֽ���
	BYTE chCmd;				// ��������
	WORD wCmdRight;			// ��������ʱ��Ϊ����վ��Ȩ��ֵ��Ϊ0ʱȨ����С
	BYTE chOldCmd;				// �ظ�����ʱ��Ӧ��ԭ��������
	union
	{
		BYTE uCmdParam;		// �������
							// ���ڿ������Ϊ0��ʾ�ػ�����0��ʾ����������ػ�ʱ�ܹ��յ�����Ϣ����
							// ���ڿ��������Ϊ0��ʾ��������0��ʾ������
		BYTE uCmdResult;		// ����ִ�н����
							// ���ڷǼ�����Ϊ0��ʾ���ɹ���Ϊ1��ʾ�ɹ���
							// ���ڼ�������μ��豸���DLL�ӿ���������
	};
	char szStatus[512];
							// �趨����ʾ��Ϣ�����ݻ��߷��ص�״̬�ַ���
							// ����GET_CONTENT���Ӧ������ǰ�趨����Ϣ����
							// ����CHECK _STATUS����������ݼ����������
	char szError[256];			// ���صĴ�������
	struct
		{
			int iLedID;				//Led���ţ���0��ʼ
			//
			int iPlayNumb;			//ѭ�����Ŵ���
			//

			int iX;					//����x���
			int iY;					//����y���
			int iWidth;				//���ڿ��
			int iHeight;				//���ڸ߶�
			//
			int iStyle;				//��ʾ�ؼ�����0��ʼ
			int iSpeed;				//�ؼ��ٶ�
			int iWaitTime;			//ͣ��ʱ��
			int iBkColor;				//������ɫ
			BOOL bIsTransparent;		//͸����0�ޣ�1����
			//
			int iFontColor;			//������ɫ
			int iFontSize;			//�����С
			BOOL bIsBlack;			//������壬0�ޣ�1����
			BOOL bIsUnderline;			//�����»��ߣ�0�ޣ�1����
			BOOL bIsItalic;			//����б�壬0�ޣ�1����
			char szFontName[16];		//��������
		}CTRL_ATTRIBUTE;
}NET_CMD;

#pragma pack(pop)//�ָ�����״̬ 
//������ʱ�ṹ��
typedef struct
{
	//����ʱ�� 0000
	char delayTime[6];
	//����ʱ�� ����
	int lastTime;

} PlayTime;
//������Ϣ�ṹ��
typedef struct
{
	NET_CMD cmd;
	PlayTime playTime;
}LedSend;
//������Ϣ�ṹ��
//ȫ�������ͣ�����jni���ݴ���
typedef struct
{
	//���صı�־��������ͬ������
	FEED_BACK_STATUS Statue;
	int iLed;
	//�ͱ�־����Ӧ��ʱ��
	int timeBegin; 
}LedFeedBcak;
#endif // !defined(AFX_MYSOCKET_H__896A4B82_2FBD_4EE2_9483_D256927EC47E__INCLUDED_)
