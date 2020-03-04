#define  MAIN_GLOBALS
#include "includes.h"


//p03  LED
//p34  KEY
//P14  DIO0
//p32  DIO1
//p33  DIO3
//p15  RST_P

uint8_t i = 0;
uint8_t j;

//test*******��ɾ*******
uint16_t TESTCOUNT = 0;   //���͵Ĵ���
uint16_t sucesscount = 0;  //�ɹ��Ĵ���
uint16_t falsecount = 0;   //ʧ�ܵĴ���
uint8_t flashresult1 = 0;
uint8_t flashresult2 = 0; 
uint8_t flashresult3 = 0; 
uint8_t flashresult4 = 0; 
uint8_t flashresult5 = 0; 
uint8_t flashresult6 = 0;
uint8_t e[200];
uint8_t CAD = 0;
uint32_t testtime = 0;
//��ɾ*******


uint8_t BUFF;

uint32_t flag_power = 10000;  

uint16_t receivedelaytime = 10000;  //�ȴ�ʱ�� 

uint8_t lpuartsum = 0;  //��������У���
								

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//ֻ�ڱ��ļ���ʹ��
void delayms(uint32_t timer)
{
	uint32_t ii,jj;
	for( jj=0;jj<timer;jj++ )
	{
		for( ii=0;ii<1000;ii++ )
	  __nop(); 
	  __nop();
	}
}


void sleep_config()
{
   stc_lpm_config_t stcLpmCfg;
	
	// ���õĶ˿�����
	 Gpio_SetIO(3, 4, 0); Gpio_InitIO(3, 4, GpioDirOut);
	 Gpio_SetIO(3, 5, 0); Gpio_InitIO(3, 5, GpioDirOut);
	 Gpio_SetIO(3, 6, 0); Gpio_InitIO(3, 6, GpioDirOut);
	 Gpio_SetIO(3, 1, 0); Gpio_InitIO(3, 1, GpioDirOut);
	 Gpio_SetIO(2, 7, 0); Gpio_InitIO(2, 7, GpioDirOut);
	 Gpio_SetIO(0, 0, 0); Gpio_InitIO(0, 0, GpioDirOut);	
	
	 DDL_ZERO_STRUCT(stcLpmCfg);

     stcLpmCfg.enSEVONPEND   = SevPndDisable;
     //stcLpmCfg.enSLEEPDEEP   = SlpDpDisable; // ����
     stcLpmCfg.enSLEEPDEEP   = SlpDpEnable;  // �������
     stcLpmCfg.enSLEEPONEXIT = SlpExtDisable;
     Lpm_Config(&stcLpmCfg);
	
}


int main()
 {
	   
       //SystemInit();
	   stc_lpm_config_t stcLpmCfg;
	   DDL_ZERO_STRUCT(stcLpmCfg);
       
	   CLK_INIT();
	   
	   KEY_INIT(); 
	   WDT_INIT();
	   LPTIME_INIT();
	   TIME_INIT();
	   LPUART_INIT();
	   SPI_INIT();
	   
	   InitRFio();//�˿ڳ�ʼ��
	   SX1278_begin(&SX1278, SX1278_433MHZ, SX1278_POWER_20DBM, SX1278_LORA_SF_8,SX1278_LORA_BW_125KHZ, 20);  //loraģ���ʼ��
	   SX1278_LoRaEntryRx(&SX1278, nLoRaSendBuffLength, 1000);  //�������ģʽ
	   
	   
	   //ʹ��GPIO��ʱ��
       Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
		 
	   //ʹ��RCL
	   Clk_Enable(ClkRCL, TRUE);
	   Clk_SetRCLFreq(ClkFreq38_4K);   //����ʱ��Ƶ��Ϊ32.768khz(cע�����Ҫ�õ�LPUART,�����Ϊ38.4KHZ,�õ�9600�Ĳ�����)
	   Clk_SetFunc(ClkFuncWkupRCH, TRUE);  //ʱ��
		 
	   //��շ��ͻ��棬�������ݰ�ͳһ���ó�0x11
	   memset(LoRaNodeSendDataBuf,0,nLoRaSendBuffLength);
	   memset(LoRaNodeSendDataBuf,0x11,nLoRaSendBuffLength);
	   

		
	   //�������ģʽ����
	   sleep_config();
	
       while (1)
	   {
	     
		 if(LoRaNodeRxFlag)
		 {
		   M0P_GPIO->P0OUT ^= 0x08;   //testС����
		   LoRaNodeRxFlag = 0;
		   StatusProcess();//���ݴ�����
		   
		 }
		   
	
	}


}




