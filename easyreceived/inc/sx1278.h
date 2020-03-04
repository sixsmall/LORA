#ifndef  __SX1278__H__
#define  __SX1278__H__

#ifdef  SX1278_GLOBALS
#define SX1278_EXT
#else
#define SX1278_EXT  extern
#endif

#include <string.h>
#include <stdint.h>
#include "gpio.h"
#include "spi.h"


#define   SX1278_MAX_PACKET	        40
#define   SX1278_DEFAULT_TIMEOUT		1000

SX1278_EXT uint8_t LoRaNodeRxFlag;
SX1278_EXT uint8_t LoRaNodeTxFlag;
SX1278_EXT uint8_t DIO3flag;
SX1278_EXT uint8_t DIO1flag;


//Error Coding rate (CR)setting
#define   SX1278_CR_4_5
//#define SX1278_CR_4_6
//#define SX1278_CR_4_7
//#define SX1278_CR_4_8
#ifdef    SX1278_CR_4_5
#define   SX1278_CR	     0x01
#else
#ifdef    SX1278_CR_4_6
#define   SX1278_CR      0x02
#else
#ifdef    SX1278_CR_4_7
#define   SX1278_CR      0x03
#else
#ifdef    SX1278_CR_4_8
#define   SX1278_CR      0x04
#endif
#endif
#endif
#endif


//CRC Enable
#define   SX1278_CRC_EN
#ifdef    SX1278_CRC_EN
#define   SX1278_CRC     0x01
#else
#define   SX1278_CRC     0x00
#endif

/********************LoRa mode***************************/
#define   LR_RegFifo                    0x00
// Common settings
#define   LR_RegOpMode                  0x01
#define   LR_RegFrMsb                   0x06
#define   LR_RegFrMid                   0x07
#define   LR_RegFrLsb                   0x08
// Tx settings
#define   LR_RegPaConfig                0x09
#define   LR_RegPaRamp                  0x0A
#define   LR_RegOcp                     0x0B
// Rx settings
#define   LR_RegLna                     0x0C
// LoRa registers
#define   LR_RegFifoAddrPtr             0x0D
#define   LR_RegFifoTxBaseAddr          0x0E
#define   LR_RegFifoRxBaseAddr          0x0F
#define   LR_RegFifoRxCurrentaddr       0x10
#define   LR_RegIrqFlagsMask            0x11
#define   LR_RegIrqFlags                0x12
#define   LR_RegRxNbBytes               0x13
#define   LR_RegRxHeaderCntValueMsb     0x14
#define   LR_RegRxHeaderCntValueLsb     0x15
#define   LR_RegRxPacketCntValueMsb     0x16
#define   LR_RegRxPacketCntValueLsb     0x17
#define   LR_RegModemStat               0x18
#define   LR_RegPktSnrValue             0x19
#define   LR_RegPktRssiValue            0x1A
#define   LR_RegRssiValue               0x1B
#define   LR_RegHopChannel              0x1C
#define   LR_RegModemConfig1            0x1D
#define   LR_RegModemConfig2            0x1E
#define   LR_RegSymbTimeoutLsb          0x1F
#define   LR_RegPreambleMsb             0x20
#define   LR_RegPreambleLsb             0x21
#define   LR_RegPayloadLength           0x22
#define   LR_RegMaxPayloadLength        0x23
#define   LR_RegHopPeriod               0x24
#define   LR_RegFifoRxByteAddr          0x25
// I/O settings
#define   REG_LR_DIOMAPPING1            0x40
#define   REG_LR_DIOMAPPING2            0x41
// Version
#define   REG_LR_VERSION                0x42
// Additional settings
#define   REG_LR_PLLHOP                 0x44
#define   REG_LR_TCXO                   0x4B
#define   REG_LR_PADAC                  0x4D
#define   REG_LR_FORMERTEMP             0x5B
#define   REG_LR_AGCREF                 0x61
#define   REG_LR_AGCTHRESH1             0x62
#define   REG_LR_AGCTHRESH2             0x63
#define   REG_LR_AGCTHRESH3             0x64




#define SX1278_433MHZ			0
static const uint8_t SX1278_Frequency[1][3] = { { 0x6C, 0x80, 0x00 }, //434MHz
		};

#define SX1278_POWER_20DBM		0
#define SX1278_POWER_17DBM		1
#define SX1278_POWER_14DBM		2
#define SX1278_POWER_11DBM		3

static const uint8_t SX1278_Power[4] = { 
	    0xFF, //20dbm
		0xFC, //17dbm
		0xF9, //14dbm
		0xF6, //11dbm
		};

#define SX1278_LORA_SF_6		      0
#define SX1278_LORA_SF_7		      1
#define SX1278_LORA_SF_8		      2
#define SX1278_LORA_SF_9	      	3
#define SX1278_LORA_SF_10	      	4
#define SX1278_LORA_SF_11	      	5
#define SX1278_LORA_SF_12	      	6

static const uint8_t SX1278_SpreadFactor[7] = { 6, 7, 8, 9, 10, 11, 12 };

#define	SX1278_LORA_BW_7_8KHZ		  0
#define	SX1278_LORA_BW_10_4KHZ		1
#define	SX1278_LORA_BW_15_6KHZ		2
#define	SX1278_LORA_BW_20_8KHZ		3
#define	SX1278_LORA_BW_31_2KHZ		4
#define	SX1278_LORA_BW_41_7KHZ		5
#define	SX1278_LORA_BW_62_5KHZ		6
#define	SX1278_LORA_BW_125KHZ		  7
#define	SX1278_LORA_BW_250KHZ		  8
#define	SX1278_LORA_BW_500KHZ		  9

static const uint8_t SX1278_LoRaBandwidth[10] = { 0, //   7.8KHz,
		1, //  10.4KHz,
		2, //  15.6KHz,
		3, //  20.8KHz,
		4, //  31.2KHz,
		5, //  41.7KHz,
		6, //  62.5KHz,
		7, // 125.0KHz,
		8, // 250.0KHz,
		9  // 500.0KHz
		};

//typedef enum _SX1278_STATUS {
//	SLEEP, STANDBY, TX, RX
//} SX1278_Status_t;
		
#define SLEEP   0
#define STANDBY   1
#define TX   2
#define RX   3

typedef struct {
//	SX1278_HW_t * hw;

	uint8_t frequency;
	uint8_t power;
	uint8_t LoRa_Rate;
	uint8_t LoRa_BW;
	uint8_t packetLength;

	uint8_t status;

	uint8_t rxBuffer[SX1278_MAX_PACKET];
	uint8_t readBytes;
	uint8_t RxFlag;
} SX1278_t;



//hardware
__weak void SX1278_HW_init(void);
__weak void SX1278_hw_Reset(void);

//logic
void SX1278_SPIWrite(uint8_t addr, uint8_t cmd);
void SX1278_SPIRead(uint8_t addr,uint8_t *databuf,uint32_t datalength);
void SX1278_SPIBurstWrite(uint8_t addr, uint8_t* txBuf,uint8_t length) ;
void InitRFio(void);
void SX1278_standby(SX1278_t * module);
void SX1278_sleep(SX1278_t * module);
void SX1278_entryLoRa(void);
void SX1278_clearLoRaIrq(void);
int SX1278_LoRaCadPacket(SX1278_t * module,uint32_t timeout);
int SX1278_LoRaEntryRx(SX1278_t * module, uint8_t length, uint32_t timeout);
uint8_t SX1278_LoRaRxPacket(SX1278_t * module);
int SX1278_LoRaEntryTx(SX1278_t * module, uint8_t length, uint32_t timeout);
int SX1278_LoRaTxPacket(SX1278_t * module, uint8_t* txBuffer, uint8_t length,uint32_t timeout);
void SX1278_config(SX1278_t * module, uint8_t frequency, uint8_t power,uint8_t LoRa_Rate, uint8_t LoRa_BW);
void SX1278_defaultConfig(SX1278_t * module);
void SX1278_begin(SX1278_t * module, uint8_t frequency, uint8_t power,uint8_t LoRa_Rate, uint8_t LoRa_BW, uint8_t packetLength);
uint8_t SX1278_read(SX1278_t * module, uint8_t* rxBuf, uint8_t length);
#endif

