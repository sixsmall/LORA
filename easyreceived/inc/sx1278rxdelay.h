#ifndef  __SX1278RXDELAY__H__
#define  __SX1278RXDELAY__H__

#ifdef  SX1278RXDELAY_GLOBALS
#define SX1278RXDELAY_EXT
#else
#define SX1278RXDELAY_EXT  extern
#endif

#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "sx1278.h"
#include "ddl.h"
#include "lpt.h"
#include "bt.h"
#include "key.h"
#include "flash.h"
#include "sx1278.h"

#define  nLoRaSendBuffLength     38

SX1278RXDELAY_EXT uint8_t currentstate;

SX1278RXDELAY_EXT unsigned char LoRaNodeSendDataBuf[nLoRaSendBuffLength];
SX1278RXDELAY_EXT unsigned char  LoRaNodeRecvDataBuf[30][nLoRaSendBuffLength];
SX1278RXDELAY_EXT uint8_t Controlleridflag;
SX1278RXDELAY_EXT uint8_t sendlength;
SX1278RXDELAY_EXT uint8_t END;
SX1278RXDELAY_EXT uint8_t check;
SX1278RXDELAY_EXT uint8_t sumnum;
SX1278RXDELAY_EXT uint8_t Retreatflag;
SX1278RXDELAY_EXT uint8_t gonghzuang;
SX1278RXDELAY_EXT uint8_t debugbuf[4];
SX1278RXDELAY_EXT uint8_t verbuf[4];

SX1278RXDELAY_EXT uint8_t Retreatcount;

SX1278RXDELAY_EXT SX1278_t    SX1278;


typedef enum
{
    CADReady,
    TxReady,
    CADRunning,
    TXDone,
    RetreatReady,
    RetreatRunning,
    RetransmissionReady,
	  //WaitCommand,
}tStatus;


typedef enum
{
   TXFAILURE,
   TXSUCCESS,
   TXRUNNING,

   GONGZHUANGSUCESS,
   GONGZHUANGFAILURE, 	
}RxStatus;

uint16_t GetRand(uint16_t min,uint16_t max);

uint8_t StatusProcess(void);
#endif



