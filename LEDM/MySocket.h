#include <atlstr.h>
#if !defined(AFX_MYSOCKET_H__896A4B82_2FBD_4EE2_9483_D256927EC47E__INCLUDED_)
#define AFX_MYSOCKET_H__896A4B82_2FBD_4EE2_9483_D256927EC47E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define DELAY_LENGTH 6
//返回状态定义
//时间和其他人冲突
//#define TIME_CONFLICT_OTHER 1
////时间和系统时间冲突
//#define TIME_CONFLICT_SYSTEM 2
////审核时间过长
//#define CHECKED_TIME_OVER 3
////审核失败
//#define CHECKED_FAILE 4
////审核成功
//#define CHECKED_SUCCEED 5

//不同状态的定义
enum FEED_BACK_STATUS
{
	//初始化的值
	INIT = 0,
	//时间和系统时间冲突
	TIME_CONFLICT_SYSTEM = 1,
	//时间和其他人冲突
	TIME_CONFLICT_OTHER = 2,
	//审核时间过长
	CHECKED_TIME_OVER = 3,
	//审核失败
	CHECKED_FAILE = 4,
	//审核成功
	CHECKED_SUCCEED = 5,
};
enum NET_CMD_TYPE
{	
	NET_CMD_ACK = 0,					// 回复命令时才使用
	NET_CMD_SWITCH_DEVICE,				// 预留
	NET_CMD_SWITCH_DEVICE_SCREEN,		// 开关大屏电源;
									// uCmdParam=0关闭，uCmdParam=1开启

	NET_CMD_GET_CONTENT,				// 得到当前播放文字的内容
									// szStatus前四个字节为文字长度
									// szStatus四个字节之后为文字内容

	NET_CMD_SET_CONTENT,				// 播放文字(可以是RTF格式)
									// uCmdParam显示风格
									// uCmdResult停留时间
									// szStatus前四个字节为文字长度,
									// szStatus四个字节之后为文字内容

	NET_CMD_END_CONTENT,				// 结束文字播放,并加载最近播放节目文件
	NET_CMD_CHECK_STATUS,				// 
	NET_CMD_START_RUNNING,				// 
	NET_CMD_TEST_LCD,					// 
	NET_CMD_TAKE,						// 
	NET_CMD_LCD_SET,					// 
	NET_CMD_CHECK_FLASH_CAP,			// 
	NET_CMD_READ_FLASH_INF,			// 
	NET_CMD_FORMAT_FLASH,				// 
	NET_CMD_LOCK,						// 锁定解锁大屏;
									// uCmdParam=0解锁,uCmdParam=1锁定

	NET_CMD_GET_STATUS,				// 获取电源,锁屏,亮度信息
									// 电源szStatus(1到4字节)
									// 锁屏szStatus(5到8字节)
									// 亮度szStatus(9到12字节)

	NET_CMD_PLAY,						// 播放
	NET_CMD_PAUSE,					// 暂停播放
	NET_CMD_STOP,						// 停止播放
	NET_CMD_PLAY_LAST,					// 播放上一节目页
	NET_CMD_PLAY_NEXT,					// 播放下一节目页
	NET_CMD_PLAY_PROGRAM,				// 播放节目文件
									// 路径放在szStatus里,以'\0'结束

	NET_CMD_PLAY_FILES,				// 播放文件
									// 路径放在szStatus里,以'\0'结束
	NET_CMD_PLAY_SINGLE_TEXT,			// 播放单行文本，属性在CTRL_ATTRIBUTE里设置

	//
	NET_CMD_EN_DOTCR		=	128,		// 允许点校正
									// uCmdParam=0不允许，uCmdParam=1允许  
      
	NET_CMD_GET_SCR_SIZE,				// 获取显示屏大小;
									// szStatus 1到4字节为宽度
									// szStatus 5到8个字节为高度  

	NET_CMD_SET_SCR_BRIGHT,			// 设置显示屏亮度
									// szStatus 1到4字节为要设置的显示屏(只有一块则为0)
									// szStatus 5到8字节为亮度值(0-100)
									// szStatus 9到12字节为色温红值(0-100)
									// szStatus 13到16字节为色温绿值(0-100)     
									// szStatus 17到20字节为色温蓝值(0-100)  

	NET_CMD_SET_SCR_BRIGHTEX,			// 设置显示屏亮度
									// uCmdParam为亮度值0到100    
 
	NET_CMD_SET_SCR_AUTOBRIGHT,			// 设置显示屏自动亮度调节; 
									// uCmdParam=0关闭，uCmdParam=1开启

	NET_CMD_DOTCR_CMD		=	250,		// 远程点校正控制命令
									// 校正数据放在szStatus里
};

#pragma pack(push) //保存对齐状态 
#pragma pack(1)//设定为4字节对齐 
//控制播放的结构体
typedef struct
{	
	char szFlag[8];			// 标记，固定为”LCDCRT”
	UINT uPacketSize;			// 包的大小, NET_CMD总字节数
	BYTE chCmd;				// 命令类型
	WORD wCmdRight;			// 发送命令时，为工作站的权限值，为0时权限最小
	BYTE chOldCmd;				// 回复命令时对应的原来的命令
	union
	{
		BYTE uCmdParam;		// 命令参数
							// 对于开关命令，为0表示关机，非0表示开机（如果关机时能够收到该消息）；
							// 对于开关屏命令，为0表示关屏，非0表示开屏；
		BYTE uCmdResult;		// 命令执行结果，
							// 对于非检测命令，为0表示不成功，为1表示成功，
							// 对于检测命令，请参见设备监控DLL接口描述部分
	};
	char szStatus[512];
							// 设定的显示信息的内容或者返回的状态字符串
							// 对于GET_CONTENT命令，应给出当前设定的消息内容
							// 对于CHECK _STATUS命令，返回内容见下面的描述
	char szError[256];			// 返回的错误描述
	struct
		{
			int iLedID;				//Led屏号，从0开始
			//
			int iPlayNumb;			//循环播放次数
			//

			int iX;					//窗口x起点
			int iY;					//窗口y起点
			int iWidth;				//窗口宽度
			int iHeight;				//窗口高度
			//
			int iStyle;				//显示特技，从0开始
			int iSpeed;				//特技速度
			int iWaitTime;			//停留时间
			int iBkColor;				//背景颜色
			BOOL bIsTransparent;		//透明，0无，1开启
			//
			int iFontColor;			//字体颜色
			int iFontSize;			//字体大小
			BOOL bIsBlack;			//字体黑体，0无，1开启
			BOOL bIsUnderline;			//字体下画线，0无，1开启
			BOOL bIsItalic;			//字体斜体，0无，1开启
			char szFontName[16];		//字体名称
		}CTRL_ATTRIBUTE;
}NET_CMD;

#pragma pack(pop)//恢复对齐状态 
//发送延时结构体
typedef struct
{
	//播放时刻 0000
	char delayTime[6];
	//播放时长 分钟
	int lastTime;

} PlayTime;
//发送信息结构体
typedef struct
{
	NET_CMD cmd;
	PlayTime playTime;
}LedSend;
//反馈信息结构体
//全部是整型，方便jni数据传递
typedef struct
{
	//返回的标志量，代表不同的意义
	FEED_BACK_STATUS Statue;
	int iLed;
	//和标志量对应的时间
	int timeBegin; 
}LedFeedBcak;
#endif // !defined(AFX_MYSOCKET_H__896A4B82_2FBD_4EE2_9483_D256927EC47E__INCLUDED_)
