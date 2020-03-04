/*************************************************************************************
* Copyright (C) 2017, Huada Semiconductor Co.,Ltd All rights reserved.    
*
* This software is owned and published by: 
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND 
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC 
* components. This software is licensed by HDSC to be adapted only 
* for use in systems utilizing HDSC components. HDSC shall not be 
* responsible for misuse or illegal use of this software for devices not 
* supported herein. HDSC is providing this software "AS IS" and will 
* not be responsible for issues arising from incorrect user implementation 
* of the software.  
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS), 
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING, 
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED 
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED 
* WARRANTY OF NONINFRINGEMENT.  
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT, 
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT 
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, 
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR 
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA, 
* SAVINGS OR PROFITS, 
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED 
* FROM, THE SOFTWARE.  
*
* This software may be replicated in part or whole for the licensed use, 
* with the restriction that this Disclaimer and Copyright notice must be 
* included with each copy of this software, whether used in part or whole, 
* at all times.                        
*/
/******************************************************************************/
/** \file uart.c
 **
 ** UART function driver API.
 ** @link SampleGroup Some description @endlink
 **
 **   - 2017-05-17  1.0  CJ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "uart.h"
#include "bt.h"
/**
 ******************************************************************************
 ** \addtogroup UartGroup
 ******************************************************************************/
//@{
/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

#define IS_VALID_CH(x)          ((UARTCH0 == (x)) ||\
                                 (UARTCH1 == (x)))

#define IS_VALID_IRQSEL(x)      ((UartTxIrq == (x)) ||\
                                 (UartRxIrq == (x))) 

#define IS_VALID_MODE(x)        ((UartMode0==(x))||\
                                 (UartMode1==(x))||\
                                 (UartMode2==(x))||\
                                 (UartMode3==(x)))

#define IS_VALID_STATUS(x)      ((UartRFRAMEError == (x))||\
                                 (UartRxFull == (x))||\
                                 (UartTxEmpty == (x)))
/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/
static stc_uart_instance_data_t* UartGetInternDataPtr(uint8_t u8Idx);
static void UartInitNvic(uint8_t u8Idx);
static void UartDeInitNvic(uint8_t u8Idx);
/******************************************************************************/
/* Local variable definitions ('static')                                       */
/******************************************************************************/
static stc_uart_instance_data_t m_astcUartInstanceDataLut[] =
{
    {
        UARTCH0,
        M0P_UART0,       /* pstcInstance */
        {NULL,NULL,NULL},     
    },
    {
        UARTCH1,
        M0P_UART1,       /* pstcInstance */
       {NULL,NULL,NULL},    
    },
};
/**
 ******************************************************************************
 ** \brief  UART0/1?? ??????
 **
 ** \param [in] u8Idx???
 **
 ** \retval ?????????
 **
 ******************************************************************************/
static stc_uart_instance_data_t* UartGetInternDataPtr(uint8_t u8Idx)
{
    stc_uart_instance_data_t* pstcData = NULL;
    uint8_t u8i = 0;
    for (u8i = 0; u8i < ARRAY_SZ(m_astcUartInstanceDataLut); u8i++)
    {
        if (u8Idx == m_astcUartInstanceDataLut[u8i].u32Idx)
        {
            pstcData = &m_astcUartInstanceDataLut[u8i];
            break;
        }
    }

    return (pstcData);
}
/**
 ******************************************************************************
 ** \brief  UART??????????
 **
 ** \param [in] u8Idx???,enIrqSel??or??????
 **
 ** \retval OK????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_EnableIrq(uint8_t u8Idx,
                           en_uart_irq_sel_t enIrqSel)
{
    stc_uart_instance_data_t* pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    ASSERT(IS_VALID_IRQSEL(enIrqSel));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    switch(enIrqSel)
    {
        case  UartTxIrq:
            pstcData->pstcInstance->SCON_f.TIEN = 1u;
            break;
        case  UartRxIrq:
            pstcData->pstcInstance->SCON_f.RIEN = 1u;
            break;
        default:
            return (ErrorInvalidParameter);       
    }
    return Ok;
}
/**
 ******************************************************************************
 ** \brief  UART??????????
 **
 ** \param [in] u8Idx???,enIrqSel??or??????
 **
 ** \retval OK????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_DisableIrq(uint8_t u8Idx, 
                            en_uart_irq_sel_t enIrqSel)
{
    stc_uart_instance_data_t *pstcData = NULL;

    ASSERT(IS_VALID_CH(u8Idx));
    ASSERT(IS_VALID_IRQSEL(enIrqSel));
        
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    switch(enIrqSel)
    {
        case  UartTxIrq:
            pstcData->pstcInstance->SCON_f.TIEN = 0u;
            break;
        case  UartRxIrq:
            pstcData->pstcInstance->SCON_f.RIEN = 0u;
            break;
        default:
            return (ErrorInvalidParameter);       
    }
    
    return Ok;
}
/**
 ******************************************************************************
 ** \brief  UART??4?????
 **
 ** \param [in] u8Idx???,mode????
 **
 ** \retval OK????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_SetMode(uint8_t u8Idx,en_uart_mode_t enMode)
{
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    ASSERT(IS_VALID_MODE(enMode));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    } 
    pstcData->pstcInstance->SCON_f.SM01 = enMode;
    return Ok;
}
/**
 ******************************************************************************
 ** \brief  UART?????????
 **
 ** \param [in] u8Idx???,stcMultiConfig???????
 **
 ** \retval OK????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_SetMultiMode(uint8_t u8Idx,stc_uart_multimode_t* pstcMultiConfig)
{
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    if(NULL != pstcMultiConfig)
    {
        pstcData->pstcInstance->SCON_f.SM2 = pstcMultiConfig->enMulti_mode;
        if(pstcMultiConfig->enMulti_mode == UartMulti)
        {
            pstcData->pstcInstance->SADDR = pstcMultiConfig->u8SlaveAddr;
            pstcData->pstcInstance->SADEN = pstcMultiConfig->u8SaddEn;
        }
    }
    return Ok;
}
/**
 ******************************************************************************
 ** \brief  UART???????????/???????????TB8
 **
 ** \param [in] u8Idx???,tb8??or?????????
 **
 ** \retval OK????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_SetMMDOrCk(uint8_t u8Idx,en_uart_mmdorck_t enTb8)
{
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    pstcData->pstcInstance->SCON_f.TB8 = enTb8;
    return Ok;
}
/**
 ******************************************************************************
 ** \brief ??RB8??
 **
 ** \param [in] u8Idx???
 **
 ** \retval RB8
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
boolean_t Uart_GetRb8(uint8_t u8Idx)
{
     stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    return (pstcData->pstcInstance->SCON_f.RB8);
}
/**
 ******************************************************************************
 ** \brief ??????1???
 **
 ** \param u8Data????
 **
 ** \retval ??
 ******************************************************************************/
en_result_t Uart_SetTb8(uint8_t u8Idx,en_uart_check_t enCheck,uint8_t u8Data)
{
	uint8_t cnt=0,i;
	stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }	
	for(i=0;i<8;i++)
	{
		if((u8Data&0x80)==0x80)
		{
			cnt++;
		}
		u8Data<<=1;
	}
	if(enCheck == Even)
	{
		if(cnt%2!=0)
		{
			pstcData->pstcInstance->SCON_f.TB8 = 1;
		}
		else
		{
			pstcData->pstcInstance->SCON_f.TB8 = 0;
		}
	}
	else
	{
		if(cnt%2!=0)
		{
			pstcData->pstcInstance->SCON_f.TB8 = 0;
		}
		else
		{
			pstcData->pstcInstance->SCON_f.TB8 = 1;
		}
	}
	return Ok;
}
/**
 ******************************************************************************
 ** \brief ??????????????
 **
 ** \param u8Idx??,enCheck??????,u8Recv????
 **
 ** \retval Error????Ok
 ******************************************************************************/
en_result_t Uart_CheckEvenOrOdd(uint8_t u8Idx,en_uart_check_t enCheck,uint8_t u8Recv)
{
	uint8_t cnt=0,i;
	stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
	for(i=0;i<8;i++)
	{
		if((u8Recv&0x80)==0x80)
		{
			cnt++;
		}
		u8Recv<<=1;
	}
	if(enCheck == Even)
	{
		if(cnt%2!=0)
		{
			if(pstcData->pstcInstance->SCON_f.RB8 != 1)
			{
				return Error;
			}
		}
		else
		{
			if(pstcData->pstcInstance->SCON_f.RB8 != 0)
			{
				return Error;
			}
		}
	}
	else
	{
		if(cnt%2!=0)
		{
			if(pstcData->pstcInstance->SCON_f.RB8 != 0)
			{
				return Error;
			}
		}
		else
		{
			if(pstcData->pstcInstance->SCON_f.RB8 != 1)
			{
				return Error;
			}
		}
	}
	return Ok;
}
/**
 ******************************************************************************
 ** \brief  UART???????????????
 **
 ** \param [in] u8Idx???,addr??
 **
 ** \retval OK????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_SetSaddr(uint8_t u8Idx,uint8_t u8Addr)
{
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    pstcData->pstcInstance->SADDR = u8Addr;
    return Ok;
}
/**
 ******************************************************************************
 ** \brief  UART???????????????
 **
 ** \param [in] u8Idx???,addren????
 **
 ** \retval OK????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_SetSaddrEn(uint8_t u8Idx,uint8_t u8Addren)
{
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    pstcData->pstcInstance->SADEN = u8Addren;
    return Ok;  
}
/**
 ******************************************************************************
 ** \brief  UART???????
 **
 ** \param [in] u8Idx???,u32pclk???,pstcBaud???????
 **
 ** \retval ??????
 ** \retval 0,?????
 ******************************************************************************/
uint16_t Uart_SetBaudRate(uint8_t u8Idx,uint32_t u32pclk,stc_uart_baud_config_t* pstcBaud)
{
    uint16_t u16tmload=0;
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if ((NULL == pstcData)||(NULL == pstcBaud))
    {
        return 0;//test
    } 
    pstcData->pstcInstance->SCON_f.DBAUD = pstcBaud->bDbaud;
    switch(pstcBaud->u8Mode)
    {
        case UartMode0:
        case UartMode2:
            break;
        case UartMode1:
        case UartMode3:
            u16tmload = 0x10000-((u32pclk*(pstcBaud->bDbaud+1))/(pstcBaud->u32Baud*32));//??????,?????
            break;
        default:
            break; 
    }
    return u16tmload;
}
/**
 ******************************************************************************
 ** \brief  UART???????????
 **
 ** \param [in] u8Idx???,enFunc?????
 **
 ** \retval OK????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_EnableFunc(uint8_t u8Idx, en_uart_func_t enFunc)
{
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    } 
    pstcData->pstcInstance->SCON_f.REN = enFunc;
    return Ok;
}
/**
 ******************************************************************************
 ** \brief  UART????????
 **
 ** \param [in] u8Idx???,enStatus??????
 **
 ** \retval ???
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
boolean_t Uart_GetStatus(uint8_t u8Idx,en_uart_status_t enStatus)
{
    boolean_t bStatus=FALSE;
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    ASSERT(IS_VALID_STATUS(enStatus));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;//4,??????0?1
    }
    switch(enStatus)
    {
        case UartRFRAMEError:
            bStatus = (pstcData->pstcInstance->ISR_f.FE == 1) ? TRUE : FALSE;
            break;
        case UartRxFull:
            bStatus = (pstcData->pstcInstance->ISR_f.RI == 1) ? TRUE : FALSE;
            break;
        case UartTxEmpty:
            bStatus = (pstcData->pstcInstance->ISR_f.TI == 1) ? TRUE : FALSE;
            break;
        default:
            break; 
    }
    return bStatus;
}
/**
 ******************************************************************************
 ** \brief  UART????????
 **
 ** \param [in] u8Idx???,enStatus??????
 **
 ** \retval ???
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_ClrStatus(uint8_t u8Idx,en_uart_status_t enStatus)
{
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    ASSERT(IS_VALID_STATUS(enStatus));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    switch(enStatus)
    {
        case UartRFRAMEError:
            pstcData->pstcInstance->ICR_f.FECLR = 0;
            break;
        case UartRxFull:
            pstcData->pstcInstance->ICR_f.RICLR = 0;
            break;
        case UartTxEmpty:
            pstcData->pstcInstance->ICR_f.TICLR = 0;
            break;
        default:
            return ErrorInvalidParameter; 
    }
    return Ok;
}
/**
 ******************************************************************************
 ** \brief  UART????????,?????????,?????????
 **
 ** \param [in] u8Idx???,Data????
 **
 ** \retval Ok????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_SendData(uint8_t u8Idx, uint8_t u8Data)
{
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    pstcData->pstcInstance->SBUF =u8Data;
    while(FALSE == Uart_GetStatus(u8Idx,UartTxEmpty))
    {}
    Uart_ClrStatus(u8Idx,UartTxEmpty);       
    return Ok;
}
/**
 ******************************************************************************
 ** \brief  UART????????
 **
 ** \param [in] u8Idx???
 **
 ** \retval ????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
uint8_t Uart_ReceiveData(uint8_t u8Idx)
{
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    return (pstcData->pstcInstance->SBUF);
}
/**
 ******************************************************************************
 ** \brief  UART????????
 **
 ** \param [in] u8Param???
 **
 ** \retval ?
 **
 ******************************************************************************/
void Uart_IRQHandler(uint8_t u8Param)
{
    stc_uart_instance_data_t *pstcData = NULL;
    pstcData = UartGetInternDataPtr(u8Param);
    if (NULL == pstcData)
    {
        return;
    }
    if(1 == pstcData->pstcInstance->ISR_f.FE)
    {
        Uart_ClrStatus(u8Param,UartRFRAMEError);
        if(NULL != pstcData->stcUartInternIrqCb.pfnRxErrIrqCb)
        {
            pstcData->stcUartInternIrqCb.pfnRxErrIrqCb();
        }
        return;//?????????????????
    }
    if(1 == pstcData->pstcInstance->ISR_f.RI)
    {
        Uart_ClrStatus(u8Param,UartRxFull);
        if(NULL != pstcData->stcUartInternIrqCb.pfnRxIrqCb)
        {
            pstcData->stcUartInternIrqCb.pfnRxIrqCb();
        }
    }
    if(1 == pstcData->pstcInstance->ISR_f.TI)
    {
        Uart_ClrStatus(u8Param,UartTxEmpty);
        if(NULL != pstcData->stcUartInternIrqCb.pfnTxIrqCb)
        {
            pstcData->stcUartInternIrqCb.pfnTxIrqCb();
        }
    }
}
/**
 ******************************************************************************
 ** \brief  UART??????NVIC??
 **
 ** \param [in] u8Idx???
 **
 ** \retval ?
 **
 ******************************************************************************/
static void UartInitNvic(uint8_t u8Idx)
{
    IRQn_Type enIrqIndex;
    
    ASSERT(IS_VALID_CH(u8Idx));;
    enIrqIndex = (IRQn_Type)(UART0_IRQn + u8Idx);
    NVIC_ClearPendingIRQ(enIrqIndex);
    NVIC_SetPriority(enIrqIndex,DDL_IRQ_LEVEL_DEFAULT);
    NVIC_EnableIRQ(enIrqIndex);
    
}
/**
 ******************************************************************************
 ** \brief  UART??????NVIC??
 **
 ** \param [in] u8Idx???
 **
 ** \retval ?
 **
 ******************************************************************************/
static void UartDeInitNvic(uint8_t u8Idx)
{
    IRQn_Type enIrqIndex;

    ASSERT(IS_VALID_CH(u8Idx));
    enIrqIndex = (IRQn_Type)(UART0_IRQn + u8Idx);
    NVIC_ClearPendingIRQ(enIrqIndex);
    NVIC_SetPriority(enIrqIndex,DDL_IRQ_LEVEL_DEFAULT);
    NVIC_DisableIRQ(enIrqIndex);
    
}
/**
 ******************************************************************************
 ** \brief  UART???????
 **
 ** \param [in] u8Idx???,pstcConfig??????
 **
 ** \retval OK????
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
en_result_t Uart_Init(uint8_t u8Idx, 
                      stc_uart_config_t* pstcConfig)
{
    en_result_t enRet = Error;
    stc_uart_instance_data_t *pstcData = NULL;
    ASSERT(IS_VALID_CH(u8Idx));
    pstcData = UartGetInternDataPtr(u8Idx);
    if (NULL == pstcData)
    {
        return ErrorInvalidParameter;
    }
    if(NULL == pstcConfig)
    {
        return ErrorInvalidParameter;
    }
    enRet = Uart_SetMode(u8Idx,pstcConfig->enRunMode);

    if(NULL != pstcConfig->pstcMultiMode)
    {
        enRet = Uart_SetMultiMode(u8Idx,pstcConfig->pstcMultiMode);
    }
    if(NULL != pstcConfig->pstcIrqCb)
    {
        pstcData->stcUartInternIrqCb.pfnRxErrIrqCb = pstcConfig->pstcIrqCb->pfnRxErrIrqCb;
        pstcData->stcUartInternIrqCb.pfnRxIrqCb = pstcConfig->pstcIrqCb->pfnRxIrqCb;
        pstcData->stcUartInternIrqCb.pfnTxIrqCb = pstcConfig->pstcIrqCb->pfnTxIrqCb;
    }
    if(pstcConfig->bTouchNvic == TRUE)
    {
        UartInitNvic(u8Idx);
    }
    else
    {
        UartDeInitNvic(u8Idx);
    }
    enRet = Ok;
    return enRet;
}


//UART��ʼ��
void UART_INIT(void)
{
	uint16_t timer=0;
  uint32_t pclk=0;
	
  stc_uart_config_t  stcConfig;
	stc_uart_irq_cb_t stcUartIrqCb;
	stc_uart_multimode_t stcMulti;
	stc_uart_baud_config_t stcBaud;
	stc_bt_config_t stcBtConfig;
	
	DDL_ZERO_STRUCT(stcUartIrqCb);
	DDL_ZERO_STRUCT(stcMulti);
	DDL_ZERO_STRUCT(stcBaud);
	DDL_ZERO_STRUCT(stcBtConfig);
	
	Gpio_InitIOExt(3,5,GpioDirOut,TRUE,FALSE,FALSE,FALSE);   
	Gpio_InitIOExt(3,6,GpioDirOut,TRUE,FALSE,FALSE,FALSE); 
	
	//ͨ���˿�����
	Gpio_SetFunc_UART1TX_P35();
	Gpio_SetFunc_UART1RX_P36();
	
	//����ʱ��ʹ��
	Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);
	Clk_SetPeripheralGate(ClkPeripheralUart1,TRUE);
	
  stcUartIrqCb.pfnRxIrqCb = NULL;
	stcUartIrqCb.pfnTxIrqCb = NULL;
	stcUartIrqCb.pfnRxErrIrqCb = NULL;
	
	stcConfig.pstcIrqCb = &stcUartIrqCb;
	stcConfig.bTouchNvic = TRUE;
	stcConfig.enRunMode = UartMode3;//ģʽ���ã�4�֣�
	
	stcMulti.enMulti_mode = UartNormal;//����ģʽ
	stcConfig.pstcMultiMode = &stcMulti;
	
	stcBaud.bDbaud = 0u;
	stcBaud.u32Baud = 115200u;
	stcBaud.u8Mode = UartMode3; //���㲨������Ҫģʽ����
	
	pclk = Clk_GetPClkFreq();
  timer=Uart_SetBaudRate(UARTCH1,pclk,&stcBaud);
	
	stcBtConfig.enMD = BtMode2;
	stcBtConfig.enCT = BtTimer;
	Bt_Init(TIM1, &stcBtConfig);//����basetimer1��������������
	Bt_ARRSet(TIM1,timer);
	Bt_Cnt16Set(TIM1,timer);
	Bt_Run(TIM1);
	
	Uart_Init(UARTCH1, &stcConfig);
	Uart_EnableIrq(UARTCH1,UartRxIrq);
	Uart_ClrStatus(UARTCH1,UartRxFull);
	Uart_EnableFunc(UARTCH1,UartRx);		
}
//@} // UartGroup      
