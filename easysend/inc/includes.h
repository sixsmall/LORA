#include <string.h>
#include <stdint.h>/*
*********************************************************************************************************
* filename:INCLUDES.H
* Programmer:  
* DATE:       2011.3.9
*********************************************************************************************************
*/

#ifndef __INCLUDES__
#define __INCLUDES__


#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "system_hc32l110.h"
#include "ddl.h"
#include "clk.h"
#include "wdt.h"
#include "lpt.h"
#include "bt.h"
#include "lpm.h"
#include "spi.h"
#include "lpuart.h"
#include "key.h"
#include "spi.h"
#include "sx1278.h"
#include "sx1278rxdelay.h"



#define __DEBUG__   1

void Bt0Int(void);
void BTinitialization(void);

#endif


