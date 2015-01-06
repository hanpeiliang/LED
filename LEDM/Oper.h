#pragma once

#include "MySocket.h"
#include <vector>

//两个时间进行对比,第二个大返回true
//只是对比前5位：**:**
bool TimeCompere(char *Systime,char *Rectime);
//系统时间和取得时间进行对比,相等返回true
//只是对比前5位：**:** vs ****
//第一个参数是系统时间，第二个参数为取得时间
bool TimeEqual(char *Systime,char *Rectime);
//时间 + 延时 = 时间
char *TimeAddDelay(char *RecTime,int DelayTime);
//1.判断、插入、反馈
//2.开启计时器
//flag 默认为 false，不向迭代器里面写入东西
LedFeedBcak InsertLedSendIter(std::vector<LedSend> *ledIter,LedSend ledSend,bool WriteIterflag = false);
//时间转换 **：** 转化为 ****
char *TimeTarns(char *SysTime);
//返回给APP信息
bool SendFeedBackToAPP(SOCKET sockConn,std::vector<LedFeedBcak> feedBack);