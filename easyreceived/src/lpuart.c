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
/** \file lpuart.c
 **
 ** LPUART function driver API.
 ** @link SampleGroup Some description @endlink
 **
 **   - 2017-05-17  1.0  CJ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#define LPUART_GLOBALS
#include "lpuart.h"
#include "lpm.h"
#include "ddl.h"
#include "wdt.h"
#include "lpt.h"
/**
 *******************************************************************************
 ** \addtogroup LpuartGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/
#define txh     Gpio_SetIO(2,5,TRUE); 
#define txl     Gpio_SetIO(2,5,FALSE); 

#define IS_VALID_IRQSEL(x)  ((LPUartTxIrq == (x)) ||\
                             (LPUartRxIrq == (x))||\
                             (LPUartTxShiftNull ==(x)))

#define IS_VALID_MODE(x)    ((LPUartMode0==(x))||\
                             (LPUartMode1==(x))||\
                             (LPUartMode2==(x))||\
                             (LPUartMode3==(x)))

#define IS_VALID_STATUS(x)   ((LPUartRFRAMEError == (x))||\
                              (LPUartRxFull == (x))||\
                              (LPUartTxEmpty == (x)))

#define IS_VALID_CLK(x)      ((LPUart_Pclk==(x))||\
                              (LPUart_Pclk_1==(x))||\
                              (LPUart_Xtl==(x))||\
                              (LPUart_Rcl==(x))) 

#define IS_VALID_PRS(x)       ((LPUartDiv128==(x))||\
                               (LPUartDiv64==(x))||\
                               (LPUartDiv32==(x))||\
                               (LPUartDiv16==(x))||\
                               (LPUartDiv8==(x))||\
                               (LPUartDiv4==(x))||\
                               (LPUartDiv2==(x))||\
                               (LPUartDiv1==(x)))

/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/
static stc_lpuart_intern_cb_t* LPUartGetInternDataCb(void); 
static stc_lpuart_intern_cb_t stcLPUartIrqCb;

uint8_t u8RxFlg = 0;
uint8_t sendcountstart = 0;
uint8_t sentnum = 0;

uint8_t receivenum = 0;
uint8_t endflag = 0;

uint8_t lpuartbuffstart = 0;
uint8_t lpuartbuffend = 0;
uint8_t aaflag = 0;
 
  //test
 
uint8_t UartTxBuff[10];
uint8_t UartTxLen = 0;



void delay_us(uint32_t timer)
{
   uint32_t ii;
	 for(ii=0;ii<timer-25;ii++)
	 {
		 __nop();
		  __nop();
		  __nop();
		  __nop();
	 }
}

/**
 ******************************************************************************
 ** \brief lpuart发送或者接收使能.
 **
 ** \param enIrqSel发送接收
 **
 ** \retval Ok 设置成功
 **\retval ErrorInvalidParameter设置失败
 ******************************************************************************/
en_result_t LPUart_EnableIrq(en_lpuart_irq_sel_t enIrqSel)
{
    ASSERT(IS_VALID_IRQSEL(enIrqSel));
    switch(enIrqSel)
    {
        case  LPUartTxIrq:
            M0P_LPUART->SCON_f.TIEN= 1u;
            break;
        case  LPUartRxIrq:
            M0P_LPUART->SCON_f.RIEN = 1u;
            break;
        case LPUartTxShiftNull:
            M0P_LPUART->SCON_f.TEEN = 1u;
            break;
        default:
            return (ErrorInvalidParameter);       
    }
    return Ok;
}
/**
 ******************************************************************************
 ** \brief lpuart通信时钟源选择
 **
 ** \param enClk 时钟源选项
 **
 ** \retval Ok 设置成功
 **\retval ErrorInvalidParameter设置失败
 ******************************************************************************/
en_result_t LPUart_SelSclk(en_lpuart_sclksel_t enClk)
{
    ASSERT(IS_VALID_CLK(enClk));
    switch(enClk)
    {
        case LPUart_Pclk:
        case LPUart_Pclk_1:
        case LPUart_Xtl:
        case LPUart_Rcl:
            M0P_LPUART->SCON_f.SCLKSEL = enClk;
            break;
        default:
            return (ErrorInvalidParameter);
    }
    return Ok;
}
/**
 ******************************************************************************
 ** \brief lpuart通信时钟分频选择
 **
 ** \param enPrs时钟预分频选择
 **
 ** \retval Ok 设置成功
 **
 ******************************************************************************/
en_result_t LPUart_SelSclkPrs(en_lpuart_sclkprs_t enPrs)
{
    en_result_t enRet = Error;
    ASSERT(IS_VALID_PRS(enPrs));
    M0P_LPUART->SCON_f.PRS = enPrs;
    enRet = Ok;
    return enRet;
}
/**
 ******************************************************************************
 ** \brief lpuart发送或者接收中断使能处理函数
 **
 ** \param enIrqSel发送或者接收使能
 **
 ** \retval Ok 设置成功
 **\retval ErrorInvalidParameter设置失败
 ******************************************************************************/
en_result_t LPUart_DisableIrq(en_lpuart_irq_sel_t enIrqSel)
{
    ASSERT(IS_VALID_IRQSEL(enIrqSel));
    switch(enIrqSel)
    {
        case  LPUartTxIrq:
            M0P_LPUART->SCON_f.TIEN = 0u;
            break;
        case  LPUartRxIrq:
            M0P_LPUART->SCON_f.RIEN = 0u;
            break;
        case LPUartTxShiftNull:
            M0P_LPUART->SCON_f.TEEN = 0u;
            break;
        default:
            return (ErrorInvalidParameter);       
    }  
    return Ok;
}
/**
 ******************************************************************************
 ** \brief lpuart通信4种模式选择
 **
 ** \param enMode模式选择
 **
 ** \retval Ok 设置成功
 **
 ******************************************************************************/
en_result_t LPUart_SetMode(stc_lpuart_mode_t* pstcMode)
{
    en_result_t enRet = Error;
    ASSERT(IS_VALID_MODE(pstcMode->enMode));
    M0P_LPUART->SCON_f.LPMODE = pstcMode->enLpMode;
    if(LPUartLPMode == M0P_LPUART->SCON_f.LPMODE)//低功耗工作模式
    {   
        switch(pstcMode->enMode)
        {
            //case LPUartMode0:
            //case LPUartMode2:
               // return (ErrorInvalidParameter); 
//                break;
            case LPUartMode1:
            case LPUartMode3:  
                M0P_LPUART->SCON_f.SM01 = pstcMode->enMode;
                break;
            default:
                return (ErrorInvalidParameter); 
        }
    }
    else
    {
      M0P_LPUART->SCON_f.SM01 = pstcMode->enMode; 
    }
    enRet = Ok;
    return enRet;
}
/**
 ******************************************************************************
 ** \brief lpuart多主机通信配置无
 **
 ** \param pstcMultiConfig多主机模式配置选项
 **
 ** \retval Ok 设置成功
 **
 ******************************************************************************/
en_result_t LPUart_SetMultiMode(stc_lpuart_multimode_t* pstcMultiConfig)
{
    en_result_t enRet = Error;
    M0P_LPUART->SCON_f.SM2 = pstcMultiConfig->enMulti_mode;
    if(LPUartMulti == pstcMultiConfig->enMulti_mode)
    {
        M0P_LPUART->SADDR = pstcMultiConfig->u8SlaveAddr;
        M0P_LPUART->SADEN = pstcMultiConfig->u8SaddEn;
    }
    enRet = Ok;
    return enRet;
}

/**
 ******************************************************************************
 ** \brief lpuart通信地址设置
 **
 ** \param u8Addr地址值
 **
 ** \retval Ok 设置成功
 **
 ******************************************************************************/
en_result_t LPUart_SetSaddr(uint8_t u8Addr)
{
    en_result_t enRet = Error;
    M0P_LPUART->SADDR = u8Addr;
    enRet = Ok;
    return enRet;
}
/**
 ******************************************************************************
 ** \brief lpuart多主机通信数据or地址帧设置
 **
 ** \param enTb8地址or数据
 **
 ** \retval Ok 设置成功
 **
 ******************************************************************************/
en_result_t LPUart_SetMMDOrCk(en_lpuart_mmdorck_t enTb8)
{
  en_result_t enRet = Error;
  M0P_LPUART->SCON_f.TB8 = enTb8;
  enRet = Ok;
  return enRet;
}
/**
 ******************************************************************************
 ** \brief 获取RB8数值
 **
 ** \param [in] u8Idx通道号
 **
 ** \retval RB8
 **\retval ErrorInvalidParameter????
 ******************************************************************************/
boolean_t LPUart_GetRb8(void)
{
    return (M0P_LPUART->SCON_f.RB8);
}
/**
 ******************************************************************************
 ** \brief 计算字节数据1的个数
 **
 ** \param u8Data计算数据
 **
 ** \retval 个数
 ******************************************************************************/
en_result_t LPUart_SetTb8(en_uart_check_t enCheck,uint8_t u8Data)
{
	uint8_t cnt=0,i;	
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
			M0P_LPUART->SCON_f.TB8 = 1;
		}
		else
		{
			M0P_LPUART->SCON_f.TB8 = 0;
		}
	}
	else
	{
		if(cnt%2!=0)
		{
			M0P_LPUART->SCON_f.TB8 = 0;
		}
		else
		{
			M0P_LPUART->SCON_f.TB8 = 1;
		}
	}
	return Ok;
}
/**
 ******************************************************************************
 ** \brief 计算接收字奇偶校验是否正确
 **
 ** \param enCheck奇偶校验方式,u8Recv校验数据
 **
 ** \retval Error????Ok
 ******************************************************************************/
en_result_t LPUart_CheckEvenOrOdd(en_uart_check_t enCheck,uint8_t u8Recv)
{
	uint8_t cnt=0,i;

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
			if(M0P_LPUART->SCON_f.RB8 != 1)
			{
				return Error;
			}
		}
		else
		{
			if(M0P_LPUART->SCON_f.RB8 != 0)
			{
				return Error;
			}
		}
	}
	else
	{
		if(cnt%2!=0)
		{
			if(M0P_LPUART->SCON_f.RB8 != 0)
			{
				return Error;
			}
		}
		else
		{
			if(M0P_LPUART->SCON_f.RB8 != 1)
			{
				return Error;
			}
		}
	}
	return Ok;
}
/**
 ******************************************************************************
 ** \brief lpuart多主机通信从机地址掩码设置
 **
 ** \param u8Addren从机地址掩码
 **
 ** \retval Ok 设置成功
 **
 ******************************************************************************/
en_result_t LPUart_SetSaddrEn(uint8_t u8Addren)
{
  en_result_t enRet = Error;
  M0P_LPUART->SADEN = u8Addren;
  enRet = Ok;
  return enRet;  
}
/**
 ******************************************************************************
 ** \brief lpuart波特率设置
 **
 ** \param u32pclk pclk时钟   pstcBaud波特率配置结构
 **
 ** \retval 定时器配置值
 **\retval 0失败
 ******************************************************************************/
uint16_t LPUart_SetBaudRate(uint32_t u32pclk,en_lpuart_sclkprs_t enprs,stc_lpuart_baud_config_t* pstcBaud)
{
    uint16_t u16tmload=0;
    if (NULL == pstcBaud)
    {
        return 0;//test
    }
    M0P_LPUART->SCON_f.DBAUD = pstcBaud->bDbaud;
    switch(pstcBaud->u8Mode)
    {
        case LPUartMode0:
        case LPUartMode2:
            u16tmload = 0;
            break;
        case LPUartMode1:
        case LPUartMode3:
            if(LPUartNoLPMode == pstcBaud->u8LpMode)
            {
                u16tmload = 0x10000-((u32pclk*(pstcBaud->bDbaud+1))/(pstcBaud->u32Baud*32));//??????,?????
            }
            else
            {
                return 0;//固定波特率
            }
            break;
        default:
            break; 
    }
    return u16tmload;
}
/**
 ******************************************************************************
 ** \brief lpuart波特率设置
 **
 ** \param 
 **
 ** \retval 低功耗下mode1/mode3模式下波特率获取
 ** \retval 波特率值
 ******************************************************************************/
uint32_t LPUart_GetBaudRate(void)
{
    uint32_t u32sclk = 0;
    uint32_t u32baud = 0;
    
    if(LPUart_Xtl == M0P_LPUART->SCON_f.SCLKSEL)
    {
      u32sclk = 32768;
    }
    else if(0x000 == (M0P_CLOCK->RCL_CR_f.TRIM&0x200u))
    {
      u32sclk = 32768;
    }
    else if(0x200 == (M0P_CLOCK->RCL_CR_f.TRIM&0x200u))
    {
      u32sclk = 38400;
    }
    else
    {
      return 0;
    }
    u32baud = u32sclk/((1<<(7-M0P_LPUART->SCON_f.PRS))*4);
    return u32baud;
}
/**
 ******************************************************************************
 ** \brief lpuart发送或接收功能配置
 **
 ** \param enFunc发送或者接收
 **
 ** \retval OK
 **
 ******************************************************************************/
en_result_t LPUart_EnableFunc(en_lpuart_func_t enFunc)
{
    en_result_t enRet = Error;
    M0P_LPUART->SCON_f.REN = enFunc;
    enRet = Ok;
    return enRet;  
}
/**
 ******************************************************************************
 ** \brief lpuart通信获取当前状态函数
 ** 
 ** \param enStatus获取哪个状态
 **
 ** \retval 状态值
 **
 ******************************************************************************/
boolean_t LPUart_GetStatus(en_lpuart_status_t enStatus)
{
    boolean_t status=FALSE;
    ASSERT(IS_VALID_STATUS(enStatus));
    switch(enStatus)
    {
        case LPUartRFRAMEError:
            status = (M0P_LPUART->ISR_f.FE == 1) ? TRUE : FALSE;
            break;
        case LPUartRxFull:
            status = (M0P_LPUART->ISR_f.RI == 1) ? TRUE : FALSE;
            break;
        case LPUartTxEmpty:
            status = (M0P_LPUART->ISR_f.TI == 1) ? TRUE : FALSE;
            break;
        default:
            break; 
    }
    return status;
}
/**
 ******************************************************************************
 ** \brief lpuart通信清除当前状态函数
 ** 
 ** \param enStatus清除哪个状态
 **
 ** \retval Ok 
 ** \retval ErrorInvalidParameter
 **
 ******************************************************************************/
en_result_t LPUart_ClrStatus(en_lpuart_status_t enStatus)
{
    ASSERT(IS_VALID_STATUS(enStatus));
    switch(enStatus)
    {
        case LPUartRFRAMEError:
            M0P_LPUART->ICR_f.FECLR = 0;
            break;
        case LPUartRxFull:
            M0P_LPUART->ICR_f.RICLR = 0;
            break;
        case LPUartTxEmpty:
            M0P_LPUART->ICR_f.TICLR = 0;
            break;
        default:
            return ErrorInvalidParameter; 
    }
    return Ok;
}
/**
 ******************************************************************************
 ** \brief lpuart通信发送数据函数，此函数只适用于查询方式发送数据
 ** 
 ** \param u32Data发送数据
 **
 ** \retval Ok
 **
 ******************************************************************************/
en_result_t LPUart_SendData(unsigned char u8Data[],uint8_t length)
{
    en_result_t enRet = Error;
	  for(sentnum=0;sentnum<length;sentnum++)
		{
		  M0P_LPUART->SBUF = u8Data[sentnum];
			while(FALSE == LPUart_GetStatus(LPUartTxEmpty))
			{}
			LPUart_ClrStatus(LPUartTxEmpty);
		}
    enRet = Ok;
    return enRet;
}
/**
 ******************************************************************************
 ** \brief lpuart通信接收数据函数
 ** 
 ** \param 
 **
 ** \retval 接收到的数据
 **
 ******************************************************************************/
void LPUart_ReceiveData(void)
{
	  u8RxData[lpuartbuffstart][receivenum] = M0P_LPUART->SBUF;
	  LPUart_ClrStatus(LPUartRxFull);   //收一个清一下中断	
	  if(receivenum == 0)  //收到帧头 进行判断
	  {
	  	if(u8RxData[lpuartbuffstart][0] == 0xAA)  //先判断帧头是不是0xAA
		{
			aaflag = 1;
			Lpt_ARRSet(0xFB7F);  //开启定时器30ms（检测传输超时）
            Lpt_Run();
		}else
		{
		    receivenum = 0;
			memset(u8RxData[lpuartbuffstart],0,15);
		}
	  }
	  if(aaflag)
      {
	    if(receivenum >=2 && receivenum == u8RxData[lpuartbuffstart][2]+2)  //定长2+8个字节
	    {
	    	receivenum = 0;
	    	lpuartbuffstart++;
	    	if(lpuartbuffstart >= 10)
	    	{
	    		lpuartbuffstart = 0;
	    	}
	    	endflag = 1;  //收完的标志
	    }
	    if(endflag)
	    {
		  aaflag = 0;
		  Lpt_Stop();
	      endflag = 0;
	    }else{
		  receivenum++;   //还没收完就继续收     
	    }
	  }	  
	  
}
/**
 ******************************************************************************
 ** \brief lpuart获取中断处理函数地址
 ** 
 ** \param 
 **
 ** \retval 中断处理函数地址
 **
 ******************************************************************************/
static stc_lpuart_intern_cb_t* LPUartGetInternDataCb(void)
{
    return &stcLPUartIrqCb;
}
/**
 ******************************************************************************
 ** \brief lpuart中断处理函数
 ** 
 ** \param 
 **
 ** \retval 
 **
 ******************************************************************************/
void LpUart_IRQHandler(uint8_t u8Param)
{
    stc_lpuart_irq_cb_t* pstcLPUartInternCb;
    pstcLPUartInternCb = LPUartGetInternDataCb();
    if(1 == M0P_LPUART->ISR_f.FE)
    {
        LPUart_ClrStatus(LPUartRFRAMEError);
        if(NULL != pstcLPUartInternCb->pfnRxErrIrqCb)
        {
            pstcLPUartInternCb->pfnRxErrIrqCb();
        }
    }
    if(1 == M0P_LPUART->ISR_f.RI)
    {
        LPUart_ClrStatus(LPUartRxFull);
        if(NULL != pstcLPUartInternCb->pfnRxIrqCb)
        {
            pstcLPUartInternCb->pfnRxIrqCb();
        }
    }
    if(1 == M0P_LPUART->ISR_f.TI)
    {
        LPUart_ClrStatus(LPUartTxEmpty);
        if(NULL != pstcLPUartInternCb->pfnTxIrqCb)
        {
            pstcLPUartInternCb->pfnTxIrqCb();
        }
    }
}
/**
 ******************************************************************************
 ** \brief lpuart通信NVIC中断使能函数
 ** 
 ** \param ?
 **
 ** \retval ?
 **
 ******************************************************************************/
static void LPUartInitNvic(void)
{
    NVIC_ClearPendingIRQ(LPUART_IRQn);
    NVIC_SetPriority(LPUART_IRQn,DDL_IRQ_LEVEL_DEFAULT);
    NVIC_EnableIRQ(LPUART_IRQn);
    
}
/**
 ******************************************************************************
 ** \brief lpuart通信NVIC中断禁止函数
 ** 
 ** \param ?
 **
 ** \retval ?
 **
 ******************************************************************************/
static void LPUartDeInitNvic(void)
{
    NVIC_ClearPendingIRQ(LPUART_IRQn);
    NVIC_SetPriority(LPUART_IRQn,DDL_IRQ_LEVEL_DEFAULT);
    NVIC_DisableIRQ(LPUART_IRQn);
    
}
/**
 ******************************************************************************
 ** \brief lpuart初始化函数
 ** 
 ** \param pstcConfig初始化结构
 **
 ** \retval Ok
 ** \retval ErrorInvalidParameter
 ******************************************************************************/
en_result_t LPUart_Init(stc_lpuart_config_t* pstcConfig)
{
    stc_lpuart_irq_cb_t* pstcLPUartInternCb;
    pstcLPUartInternCb = LPUartGetInternDataCb();
    if(NULL == pstcConfig)
    {
        return ErrorInvalidParameter;
    }
    if(NULL != pstcConfig->pstcLpuart_clk)
    {
        LPUart_SelSclk(pstcConfig->pstcLpuart_clk->enSclk_sel);
        LPUart_SelSclkPrs(pstcConfig->pstcLpuart_clk->enSclk_Prs);
    }
    LPUart_SetMode(pstcConfig->pstcRunMode);
    if(NULL != pstcConfig->pstcMultiMode)
    {
        LPUart_SetMultiMode(pstcConfig->pstcMultiMode);
    }
    if(NULL != pstcConfig->pstcIrqCb)
    {
        pstcLPUartInternCb->pfnRxErrIrqCb= pstcConfig->pstcIrqCb->pfnRxErrIrqCb;
        pstcLPUartInternCb->pfnRxIrqCb = pstcConfig->pstcIrqCb->pfnRxIrqCb;
        pstcLPUartInternCb->pfnTxIrqCb = pstcConfig->pstcIrqCb->pfnTxIrqCb;
    }
    if(pstcConfig->bTouchNvic == TRUE)
    {
        LPUartInitNvic();
    }
    else
    {
        LPUartDeInitNvic();
    }
    return Ok;
}


//接收中断
void RxIntCallback(void)
{	
  LPUart_ReceiveData();
}

//发送中断
void TxIntCallback(void)
{
}

void ErrIntCallback(void)
{ 
}


void LPUART_INIT(void)
{
  stc_lpuart_config_t   stcConfig;
	stc_lpuart_irq_cb_t   stcLPUartIrqCb;
	stc_lpuart_multimode_t   stcMulti;
	stc_lpuart_sclk_sel_t   stcLpuart_clk;
	stc_lpuart_mode_t   stcRunMode;

	DDL_ZERO_STRUCT(stcConfig);
	DDL_ZERO_STRUCT(stcLPUartIrqCb);
	DDL_ZERO_STRUCT(stcMulti);
	
	
	Clk_SetPeripheralGate(ClkPeripheralLpUart,TRUE);  //使能LPUART时钟
	
	//通道端口配置
	Gpio_InitIOExt(2,5,GpioDirOut,TRUE,FALSE,FALSE,FALSE);
	Gpio_InitIOExt(2,6,GpioDirOut,TRUE,FALSE,FALSE,FALSE);
	
	Gpio_SetFunc_UART2RX_P25();
	Gpio_SetFunc_UART2TX_P26();
	
	stcLpuart_clk.enSclk_sel = LPUart_Rcl;  //内部低速时钟
	stcLpuart_clk.enSclk_Prs = LPUartDiv1;  //1分频
	stcConfig.pstcLpuart_clk = &stcLpuart_clk;  //时钟源的选择
	
	stcRunMode.enLpMode = LPUartLPMode;   //正常工作模式或者低功耗工作模式
	stcRunMode.enMode = LPUartMode1;   //模式3
	stcConfig.pstcRunMode = &stcRunMode;  
	
	stcLPUartIrqCb.pfnRxIrqCb = RxIntCallback;  //接收中断
	stcLPUartIrqCb.pfnTxIrqCb = NULL;  //发送中断
	stcLPUartIrqCb.pfnRxErrIrqCb = ErrIntCallback;  
	stcConfig.pstcIrqCb = &stcLPUartIrqCb;
	stcConfig.bTouchNvic = TRUE;
	
	stcMulti.enMulti_mode = LPUartNormal;  //非多主机模式	
	stcConfig.pstcMultiMode = &stcMulti;
	
	LPUart_EnableIrq(LPUartRxIrq);
	LPUart_Init(&stcConfig);
	
	LPUart_EnableFunc(LPUartRx);
	
}

//@} // LpuartGroup     
      
      

