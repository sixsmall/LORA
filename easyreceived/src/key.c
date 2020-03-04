#define KEY_GLOBALS
#include "key.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
 
//p03  LED
//p34  KEY
//P14  DIO0
//p32  DIO1
//p33  DIO3
//p15  RST_P


uint8_t ret = 0;
uint8_t keyflag = 0;
uint8_t keycount = 0;
uint8_t START = 0;


/**
 * \brief   Gpio_IRQHandler 
 *          any interrput blinking a led.
 *  
 * \param   [in]  u8Param            port index
 * \retval  void             
 */


void Gpio_IRQHandler(uint8_t u8Param)
{
	
switch (u8Param)
	{
	  case 0:
		{
		  *((uint32_t *)((uint32_t)&M0P_GPIO->P0ICLR + u8Param * 0x40 )) = 0;
			break;
		}
		case 1:
		{
//		 *((uint32_t *)((uint32_t)&M0P_GPIO->P0ICLR + u8Param * 0x40 )) = 0;
				
			if(Gpio_GetIrqStat(1, 4))
			{
			  Gpio_ClearIrq(1, 4);
				if(SX1278.status == RX)
				{
					  LoRaNodeRxFlag = 1;
					  ret =  SX1278_LoRaRxPacket(&SX1278);
					  if(ret > 38)
						{
						   ret = 38;
						}
						if(ret > 0)
						{
							SX1278_read(&SX1278, (uint8_t *)LoRaNodeRecvDataBuf[START], ret);
						    START++;
							if(START >= 30)
							{
								START = 0;
							}
						}
				}else{
				  LoRaNodeTxFlag = 1;

					
				}
				 
			}
			
			
			break;
		}
		case 2:
		{
		  *((uint32_t *)((uint32_t)&M0P_GPIO->P0ICLR + u8Param * 0x40 )) = 0;
			break;
		}
		case 3:
		{
		 // *((uint32_t *)((uint32_t)&M0P_GPIO->P3ICLR + u8Param * 0x40 )) = 0;
//			//按键中断
//			if(Gpio_GetIrqStat(3, 4))
//			{	
//				//按键消抖
//				delay1ms(10);
//				if(Gpio_GetIrqStat(3, 4))
//				{
//				  for(keycount = 0;keycount < 100;keycount++)
//					{
//						if(Gpio_GetIrqStat(3, 4))
//						{
//				
//							Gpio_ClearIrq(3, 4);
//							keyflag = 1;
//				
//							//M0P_GPIO->P3OUT ^= 0x04;
//						}
//					}
//				}	
//			}
			
			if(Gpio_GetIrqStat(3, 3))
			{
               Gpio_ClearIrq(3, 3);
			   DIO3flag = 1;
			}
			
			if(Gpio_GetIrqStat(3, 2))
			{
               Gpio_ClearIrq(3, 2);
			   DIO1flag = 1;
			}
			break;
		}
		default:
			break;
	}
    
}														

												
/**
 * \brief   KEY_INIT 
 *  
 * \retval  void             
 */
											
void KEY_INIT(void)
{

	//配置led
	Gpio_InitIO(0,3,GpioDirOut);
	Gpio_SetIO(0,3,FALSE);
	
	//注：此单片机没有按键，所以无须配置按键
	
}




												
