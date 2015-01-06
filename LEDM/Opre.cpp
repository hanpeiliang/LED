
#include "StdAfx.h"
#include "Oper.h"


//两个时间进行对比,第二个大返回true
//只是对比前5位：****
bool TimeCompere(char *Systime,char *Rectime)
{
	int timeSys = atoi(Systime);
	int timeRec = atoi(Rectime);
	if (timeRec >= timeSys)
		return true;

	return false;
}
//系统时间和取得时间进行对比,相等返回true
//只是对比前5位：**:** vs ****00 精确到秒
//因为播放迭代器必须到播放完成才能释放，所以直接使用精确到秒的时刻比较
//第一个参数是系统时间，第二个参数为取得时间
bool TimeEqual(char *Systime,char *Rectime)
{
	if ((Rectime[0] == Systime[0]) && (Rectime[1] == Systime[1]) && (Rectime[2] == Systime[3]) && (Rectime[3] == Systime[4]) && ('0' == Systime[6]) && ('0' == Systime[7]))
		return true;
	return false;
}
//加上延时之后的时间
char *TimeAddDelay(char *RecTime,int DelayTime)
{
	char strTemp[6] = "";
	strcpy(strTemp,RecTime);
	int Hour = DelayTime/60;
	int Min = DelayTime%60;
	int time = atoi(strTemp);
	int qian = time/1000;
	int bai = time/100- qian*10;
	int shi = time/10 - bai*10 - qian*100;
	int ge = time %10;

	int temp = 0;
	temp = ge + Min%10;
	ge = (temp)%10;
	int carry = (temp)/10;
	temp = shi + Min/10 + carry;
	shi = (temp)%6;
	carry = (temp)/6;
	temp = bai + Hour%10 + carry;
	bai = (temp)%10;
	carry = (temp)/10;
	qian =  ( qian + Hour/10 + carry);
	int timeAdd = qian*1000 + bai*100 + shi*10 + ge;
	if (qian == 0)
	{
		sprintf(strTemp, "%d%d",0,timeAdd );
	}
	else
	{
		sprintf(strTemp, "%d",timeAdd );
	}
	return strTemp;
}
//1.判断、插入、反馈
//2.开启计时器
LedFeedBcak InsertLedSendIter(std::vector<LedSend> *ledIter,LedSend ledSend,bool WriteIterflag)
{
	//初始化屏幕
	int ledNum = ledSend.cmd.CTRL_ATTRIBUTE.iLedID;
	//反馈 初始化
	LedFeedBcak feedBack;
	feedBack.Statue = INIT; 
	feedBack.timeBegin = atoi(ledSend.playTime.delayTime);
	feedBack.iLed = ledNum;
	//临时存放时间的字符串
	char timeAddTemp[6] = "";
	//服务器时间审核
	time_t t = time(0); 
	char tmp[64]; 
	strftime( tmp, sizeof(tmp), "%X",localtime(&t)); 
	//如果大于系统时间 为真
	//这里一定要反着使用，可以解决一分钟内仍然审核通过的逻辑错误
	if (!TimeCompere(ledSend.playTime.delayTime,strcpy(timeAddTemp,TimeTarns(tmp))));
	else
	{
		//和系统时间冲突
		feedBack.Statue = TIME_CONFLICT_SYSTEM; 
		feedBack.timeBegin = atoi(timeAddTemp);
		return feedBack;
	}

	int item =0;
	//列表为空 || 接收的时间 + 播放 小于第一个时间
	if (ledIter[ledNum].empty() || TimeCompere(strcpy(timeAddTemp,TimeAddDelay(ledSend.playTime.delayTime,ledSend.playTime.lastTime)),ledIter[ledNum][0].playTime.delayTime))
	{
		//每次都在队列最前面插入
		if (WriteIterflag)
		ledIter[ledNum].insert(ledIter[ledNum].begin(),ledSend);
		//RecTimePos = 0;
		return feedBack;
	}
	else
	{
		for (;item < (ledIter[ledNum].size() - 1); ++item)
		{
			//接收到时间大于某个 && 接收到的时间小于下一个	
			if (TimeCompere(strcpy(timeAddTemp,TimeAddDelay(ledIter[ledNum][item].playTime.delayTime,ledIter[ledNum][item].playTime.lastTime)),ledSend.playTime.delayTime))
			{
				if (TimeCompere(strcpy(timeAddTemp,TimeAddDelay(ledSend.playTime.delayTime,ledSend.playTime.lastTime)),ledIter[ledNum][item + 1].playTime.delayTime))
				{
				//	RecTimePos = item + 1;
					//找到可以插入的区间
					//feedBack.SendFlag = true;
					//insert 需要加两次
					if (WriteIterflag)
					ledIter[ledNum].insert(ledIter[ledNum].begin() + item + 1,ledSend);
					return feedBack;
				}
			}
		}
		//除了最后一个之外，循环完毕，没有找到
		if (item == (ledIter[ledNum].size() - 1))
		{
			//接收的时间大于最后一个时间
			if (TimeCompere(strcpy(timeAddTemp,TimeAddDelay(ledIter[ledNum][item].playTime.delayTime,ledIter[ledNum][item].playTime.lastTime)),ledSend.playTime.delayTime))
			{
				if (WriteIterflag)
				ledIter[ledNum].push_back(ledSend);
				//RecTimePos = item + 1;
				return feedBack;
			}
			else
			{
				//队列最后一个 时间 + 时长 + 1
				feedBack.timeBegin = atoi(strcpy(timeAddTemp,TimeAddDelay(ledIter[ledNum][item].playTime.delayTime,ledIter[ledNum][item].playTime.lastTime + 1)));
				feedBack.Statue = TIME_CONFLICT_OTHER;
				return feedBack;
			}			
		}
	}
	return feedBack;
}
//时间转换 **：** 转化为 ****
char *TimeTarns(char *SysTime)
{
	char strTemp[6] = "";
	//严格保证了 ****
	sprintf(strTemp, "%c%c%c%c",SysTime[0],SysTime[1],SysTime[3],SysTime[4]);
	return strTemp;
}
//返回给APP信息
bool SendFeedBackToAPP(SOCKET sockConn,std::vector<LedFeedBcak> feedBack)
{
	//sennd
	int sendRet = send(sockConn,(char*)&feedBack.at(0),sizeof(LedFeedBcak),0);
	if (sendRet == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}