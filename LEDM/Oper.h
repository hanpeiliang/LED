#pragma once

#include "MySocket.h"
#include <vector>

//����ʱ����жԱ�,�ڶ����󷵻�true
//ֻ�ǶԱ�ǰ5λ��**:**
bool TimeCompere(char *Systime,char *Rectime);
//ϵͳʱ���ȡ��ʱ����жԱ�,��ȷ���true
//ֻ�ǶԱ�ǰ5λ��**:** vs ****
//��һ��������ϵͳʱ�䣬�ڶ�������Ϊȡ��ʱ��
bool TimeEqual(char *Systime,char *Rectime);
//ʱ�� + ��ʱ = ʱ��
char *TimeAddDelay(char *RecTime,int DelayTime);
//1.�жϡ����롢����
//2.������ʱ��
//flag Ĭ��Ϊ false���������������д�붫��
LedFeedBcak InsertLedSendIter(std::vector<LedSend> *ledIter,LedSend ledSend,bool WriteIterflag = false);
//ʱ��ת�� **��** ת��Ϊ ****
char *TimeTarns(char *SysTime);
//���ظ�APP��Ϣ
bool SendFeedBackToAPP(SOCKET sockConn,std::vector<LedFeedBcak> feedBack);