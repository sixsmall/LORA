#ifndef __KEY_H__
#define __KEY_H__

#ifdef  KEY_GLOBALS
#define KEY_EXT
#else
#define KEY_EXT  extern
#endif

#include <string.h>
#include <stdint.h>

#include <string.h>
#include <stdint.h> 
#include "sx1278rxdelay.h"
#include "sx1278.h"
#include "gpio.h"
#include "lpuart.h"

KEY_EXT uint8_t START;
KEY_EXT uint8_t keyflag;

void KEY_INIT(void);

#endif

