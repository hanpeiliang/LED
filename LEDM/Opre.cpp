
#include "StdAfx.h"
#include "Oper.h"


//����ʱ����жԱ�,�ڶ����󷵻�true
//ֻ�ǶԱ�ǰ5λ��****
bool TimeCompere(char *Systime,char *Rectime)
{
	int timeSys = atoi(Systime);
	int timeRec = atoi(Rectime);
	if (timeRec >= timeSys)
		return true;

	return false;
}
//ϵͳʱ���ȡ��ʱ����жԱ�,��ȷ���true
//ֻ�ǶԱ�ǰ5λ��**:** vs ****00 ��ȷ����
//��Ϊ���ŵ��������뵽������ɲ����ͷţ�����ֱ��ʹ�þ�ȷ�����ʱ�̱Ƚ�
//��һ��������ϵͳʱ�䣬�ڶ�������Ϊȡ��ʱ��
bool TimeEqual(char *Systime,char *Rectime)
{
	if ((Rectime[0] == Systime[0]) && (Rectime[1] == Systime[1]) && (Rectime[2] == Systime[3]) && (Rectime[3] == Systime[4]) && ('0' == Systime[6]) && ('0' == Systime[7]))
		return true;
	return false;
}
//������ʱ֮���ʱ��
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
//1.�жϡ����롢����
//2.������ʱ��
LedFeedBcak InsertLedSendIter(std::vector<LedSend> *ledIter,LedSend ledSend,bool WriteIterflag)
{
	//��ʼ����Ļ
	int ledNum = ledSend.cmd.CTRL_ATTRIBUTE.iLedID;
	//���� ��ʼ��
	LedFeedBcak feedBack;
	feedBack.Statue = INIT; 
	feedBack.timeBegin = atoi(ledSend.playTime.delayTime);
	feedBack.iLed = ledNum;
	//��ʱ���ʱ����ַ���
	char timeAddTemp[6] = "";
	//������ʱ�����
	time_t t = time(0); 
	char tmp[64]; 
	strftime( tmp, sizeof(tmp), "%X",localtime(&t)); 
	//�������ϵͳʱ�� Ϊ��
	//����һ��Ҫ����ʹ�ã����Խ��һ��������Ȼ���ͨ�����߼�����
	if (!TimeCompere(ledSend.playTime.delayTime,strcpy(timeAddTemp,TimeTarns(tmp))));
	else
	{
		//��ϵͳʱ���ͻ
		feedBack.Statue = TIME_CONFLICT_SYSTEM; 
		feedBack.timeBegin = atoi(timeAddTemp);
		return feedBack;
	}

	int item =0;
	//�б�Ϊ�� || ���յ�ʱ�� + ���� С�ڵ�һ��ʱ��
	if (ledIter[ledNum].empty() || TimeCompere(strcpy(timeAddTemp,TimeAddDelay(ledSend.playTime.delayTime,ledSend.playTime.lastTime)),ledIter[ledNum][0].playTime.delayTime))
	{
		//ÿ�ζ��ڶ�����ǰ�����
		if (WriteIterflag)
		ledIter[ledNum].insert(ledIter[ledNum].begin(),ledSend);
		//RecTimePos = 0;
		return feedBack;
	}
	else
	{
		for (;item < (ledIter[ledNum].size() - 1); ++item)
		{
			//���յ�ʱ�����ĳ�� && ���յ���ʱ��С����һ��	
			if (TimeCompere(strcpy(timeAddTemp,TimeAddDelay(ledIter[ledNum][item].playTime.delayTime,ledIter[ledNum][item].playTime.lastTime)),ledSend.playTime.delayTime))
			{
				if (TimeCompere(strcpy(timeAddTemp,TimeAddDelay(ledSend.playTime.delayTime,ledSend.playTime.lastTime)),ledIter[ledNum][item + 1].playTime.delayTime))
				{
				//	RecTimePos = item + 1;
					//�ҵ����Բ��������
					//feedBack.SendFlag = true;
					//insert ��Ҫ������
					if (WriteIterflag)
					ledIter[ledNum].insert(ledIter[ledNum].begin() + item + 1,ledSend);
					return feedBack;
				}
			}
		}
		//�������һ��֮�⣬ѭ����ϣ�û���ҵ�
		if (item == (ledIter[ledNum].size() - 1))
		{
			//���յ�ʱ��������һ��ʱ��
			if (TimeCompere(strcpy(timeAddTemp,TimeAddDelay(ledIter[ledNum][item].playTime.delayTime,ledIter[ledNum][item].playTime.lastTime)),ledSend.playTime.delayTime))
			{
				if (WriteIterflag)
				ledIter[ledNum].push_back(ledSend);
				//RecTimePos = item + 1;
				return feedBack;
			}
			else
			{
				//�������һ�� ʱ�� + ʱ�� + 1
				feedBack.timeBegin = atoi(strcpy(timeAddTemp,TimeAddDelay(ledIter[ledNum][item].playTime.delayTime,ledIter[ledNum][item].playTime.lastTime + 1)));
				feedBack.Statue = TIME_CONFLICT_OTHER;
				return feedBack;
			}			
		}
	}
	return feedBack;
}
//ʱ��ת�� **��** ת��Ϊ ****
char *TimeTarns(char *SysTime)
{
	char strTemp[6] = "";
	//�ϸ�֤�� ****
	sprintf(strTemp, "%c%c%c%c",SysTime[0],SysTime[1],SysTime[3],SysTime[4]);
	return strTemp;
}
//���ظ�APP��Ϣ
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