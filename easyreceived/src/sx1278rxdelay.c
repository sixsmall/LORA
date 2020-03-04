#define SX1278RXDELAY_GLOBALS
#include "sx1278rxdelay.h"
#include "lpuart.h"
#include "wdt.h"




uint8_t CADresult;

//变量初始化
uint8_t Retreatflag = 1;  //退避标志
uint8_t Retreatcount = 4;  //退避次数
uint32_t Retreattime = 0;  //退避时间
uint16_t timeoutdelay = 0;  //等待时间

uint8_t END = 0;
uint8_t Controlleridflag = 0;  //主机ID写入标志
uint8_t sendlength = 0;
uint8_t check = 0;
uint8_t sumnum = 0;  //校验和
uint8_t gonghzuang = 0;

uint8_t successflag = 0;  //接收成功标志
uint8_t TEST = 0;

//开启定时器
void TIMESTART(uint16_t u16BTArrData)
{
	  Bt_ARRSet(TIM1,u16BTArrData);
		Bt_Run(TIM1);
}

//获得随机数
uint16_t GetRand(uint16_t min,uint16_t max)
{
	uint16_t r =0;
  //srand((unsigned int)(time(NULL)));
	r = rand()%(max-min+1)+min;
	return r;
}

//得到2的k次幂
uint16_t TwoPowerK(int k)
{
  int a = 2;
	int f = 1;
	while(k--)
	{
	  f *= a;
	}
	return f;
}

//获得退避时间
uint16_t SetRetreatTime()
{
  uint16_t Retreatrand= TwoPowerK(Retreatcount);
	uint16_t Retreatresult = GetRand(0,Retreatrand);
	return Retreatresult;
}


//发送状态机函数()
uint8_t StatusProcess(void)
{
	//此函数是数据处理函数，大家可以自己根据需要编写
	return 0;
}





