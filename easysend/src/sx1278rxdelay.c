#define SX1278RXDELAY_GLOBALS
#include "sx1278rxdelay.h"
#include "lpuart.h"
#include "wdt.h"




uint8_t CADresult;

//������ʼ��
uint8_t Retreatflag = 1;  //�˱ܱ�־
uint8_t Retreatcount = 4;  //�˱ܴ���
uint32_t Retreattime = 0;  //�˱�ʱ��
uint16_t timeoutdelay = 0;  //�ȴ�ʱ��

uint8_t END = 0;
uint8_t Controlleridflag = 0;  //����IDд���־
uint8_t sendlength = 0;
uint8_t check = 0;
uint8_t sumnum = 0;  //У���
uint8_t gonghzuang = 0;

uint8_t successflag = 0;  //���ճɹ���־
uint8_t TEST = 0;

//������ʱ��
void TIMESTART(uint16_t u16BTArrData)
{
	  Bt_ARRSet(TIM1,u16BTArrData);
		Bt_Run(TIM1);
}

//��������
uint16_t GetRand(uint16_t min,uint16_t max)
{
	uint16_t r =0;
  //srand((unsigned int)(time(NULL)));
	r = rand()%(max-min+1)+min;
	return r;
}

//�õ�2��k����
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

//����˱�ʱ��
uint16_t SetRetreatTime()
{
  uint16_t Retreatrand= TwoPowerK(Retreatcount);
	uint16_t Retreatresult = GetRand(0,Retreatrand);
	return Retreatresult;
}


//����״̬������()
uint8_t StatusProcess(void)
{
	//�˺��������ݴ���������ҿ����Լ�������Ҫ��д
	return 0;
}





