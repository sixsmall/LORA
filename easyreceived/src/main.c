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

//test*******可删*******
uint16_t TESTCOUNT = 0;   //发送的次数
uint16_t sucesscount = 0;  //成功的次数
uint16_t falsecount = 0;   //失败的次数
uint8_t flashresult1 = 0;
uint8_t flashresult2 = 0; 
uint8_t flashresult3 = 0; 
uint8_t flashresult4 = 0; 
uint8_t flashresult5 = 0; 
uint8_t flashresult6 = 0;
uint8_t e[200];
uint8_t CAD = 0;
uint32_t testtime = 0;
//可删*******


uint8_t BUFF;

uint32_t flag_power = 10000;  

uint16_t receivedelaytime = 10000;  //等待时间 

uint8_t lpuartsum = 0;  //串口数据校验和
								

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//只在本文件中使用
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
	
	// 不用的端口配置
	 Gpio_SetIO(3, 4, 0); Gpio_InitIO(3, 4, GpioDirOut);
	 Gpio_SetIO(3, 5, 0); Gpio_InitIO(3, 5, GpioDirOut);
	 Gpio_SetIO(3, 6, 0); Gpio_InitIO(3, 6, GpioDirOut);
	 Gpio_SetIO(3, 1, 0); Gpio_InitIO(3, 1, GpioDirOut);
	 Gpio_SetIO(2, 7, 0); Gpio_InitIO(2, 7, GpioDirOut);
	 Gpio_SetIO(0, 0, 0); Gpio_InitIO(0, 0, GpioDirOut);	
	
	 DDL_ZERO_STRUCT(stcLpmCfg);

     stcLpmCfg.enSEVONPEND   = SevPndDisable;
     //stcLpmCfg.enSLEEPDEEP   = SlpDpDisable; // 休眠
     stcLpmCfg.enSLEEPDEEP   = SlpDpEnable;  // 深度休眠
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
	   
	   InitRFio();//端口初始化
	   SX1278_begin(&SX1278, SX1278_433MHZ, SX1278_POWER_20DBM, SX1278_LORA_SF_8,SX1278_LORA_BW_125KHZ, 20);  //lora模块初始化
	   SX1278_LoRaEntryRx(&SX1278, nLoRaSendBuffLength, 1000);  //进入接收模式
	   
	   
	   //使能GPIO口时钟
       Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
		 
	   //使能RCL
	   Clk_Enable(ClkRCL, TRUE);
	   Clk_SetRCLFreq(ClkFreq38_4K);   //设置时钟频率为32.768khz(c注：如果要用到LPUART,最好设为38.4KHZ,得到9600的波特率)
	   Clk_SetFunc(ClkFuncWkupRCH, TRUE);  //时钟
		 
	   //清空发送缓存，并将数据包统一设置成0x11
	   memset(LoRaNodeSendDataBuf,0,nLoRaSendBuffLength);
	   memset(LoRaNodeSendDataBuf,0x11,nLoRaSendBuffLength);
	   

		
	   //深度休眠模式配置
	   sleep_config();
	
       while (1)
	   {
	     
		 if(LoRaNodeRxFlag)
		 {
		   M0P_GPIO->P0OUT ^= 0x08;   //test小灯闪
		   LoRaNodeRxFlag = 0;
		   StatusProcess();//数据处理函数
		   
		 }
		   
	
	}


}




