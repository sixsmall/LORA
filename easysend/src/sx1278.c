#define SX1278_GLOBALS
#include "sx1278.h"

//p03  LED
//p34  KEY
//P14  DIO0
//p32  DIO1
//p33  DIO3
//p15  RST_P


#define     RST_PORT                 (1)
#define     RST_PIN                  (5)

uint8_t Dbuf;
uint8_t addrbuf;
uint8_t ModemStatbuf;
uint8_t RegFifoRxCurrentaddrbuf;
uint8_t RegRxNbBytesbuf;
uint8_t RegFifoTxBaseAddrbuf;
uint8_t RegPayloadLengthbuf;
uint8_t RegIrqFlagsbuf;
uint8_t versionbuf;
uint8_t databuff;
uint8_t RegOpModebuf;


uint8_t DIO3flag;
uint8_t DIO1flag;

uint16_t txcount = 0;
 
void InitRFio(void)
{
	
	 //RF端口初始化
     Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, FALSE, FALSE);   //配置RST_P为输出  改上啦
	 Gpio_SetIO(1,5,TRUE);
	
	 //配置DIO0中断
	 Gpio_InitIOExt(1, 4, GpioDirIn, FALSE, TRUE, FALSE, FALSE);  //改下拉
	 Gpio_ClearIrq(1, 4);
	 Gpio_EnableIrq(1, 4, GpioIrqRising);
	 EnableNvic(PORT1_IRQn, DDL_IRQ_LEVEL_DEFAULT, TRUE);
	
	
	 //配置DIO3中断
	 Gpio_InitIOExt(3, 3, GpioDirIn, FALSE, TRUE, FALSE, FALSE);  //改下拉
	 Gpio_ClearIrq(3, 3);
	 Gpio_EnableIrq(3, 3, GpioIrqRising);
	
	
	//配置DIO1中断
	 Gpio_InitIOExt(3, 2, GpioDirIn, FALSE, TRUE, FALSE, FALSE);  //改下拉
	 Gpio_ClearIrq(3, 2);
	 Gpio_EnableIrq(3, 2, GpioIrqRising);
	
	 EnableNvic(PORT3_IRQn, DDL_IRQ_LEVEL_DEFAULT, TRUE);
	 
}
 

__weak void SX1278_HW_init() 
{
	Spi_SetCS(TRUE);
	Gpio_SetIO(RST_PORT, RST_PIN, FALSE);
	delay1ms(6);
	Gpio_SetIO(RST_PORT, RST_PIN, TRUE);
}

__weak void SX1278_hw_Reset()
{
  Spi_SetCS(TRUE);	
	Gpio_SetIO(RST_PORT, RST_PIN, FALSE);
	delay1ms(1);
	Gpio_SetIO(RST_PORT, RST_PIN, TRUE);
	delay1ms(100);
}


void SX1278_SPIWrite(uint8_t addr, uint8_t cmd)
{
  uint8_t combuf[2];
	combuf[0] = addr | 0x80;
	combuf[1] = cmd;
	
	Spi_SetCS(FALSE);
	WriteData(combuf,2);
	Spi_SetCS(TRUE);
}

void SX1278_SPIRead(uint8_t addr,uint8_t *databuf,uint32_t datalength)
{
  uint8_t readcombuf;
  readcombuf = addr;
 
	Spi_SetCS(FALSE);
	WriteData(&readcombuf,1);
	ReadData(databuf,datalength);
	Spi_SetCS(TRUE);
  	
	return;
}



void SX1278_config(SX1278_t * module, uint8_t frequency, uint8_t power,uint8_t LoRa_Rate, uint8_t LoRa_BW)
{
	//SX1278_SPIRead(module,REG_LR_VERSION,&RegOpModebuf,1);
    SX1278_sleep(module); //Change modem mode Must in Sleep mode
	delay1ms(15);
	SX1278_entryLoRa();
	SX1278_standby(module); //Entry standby mode
	delay1ms(15);
	SX1278_SPIBurstWrite(LR_RegFrMsb,(uint8_t*)SX1278_Frequency[frequency], 3);  //setting  frequency parameter
	SX1278_SPIWrite(LR_RegPaConfig, SX1278_Power[power]);  //Setting output power parameter(我算出来输出功率是14dbm)
	SX1278_SPIWrite(LR_RegOcp, 0x0B);			//RegOcp,Close Ocp
	SX1278_SPIWrite(LR_RegLna, 0x23);		//RegLNA,High & LNA Enable
	if(SX1278_SpreadFactor[LoRa_Rate] == 6)
	{
	  uint8_t tmp;
		SX1278_SPIWrite(LR_RegModemConfig1,((SX1278_LoRaBandwidth[LoRa_BW] << 4) + (SX1278_CR << 1) + 0x01)); //Implicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SX1278_SPIWrite(LR_RegModemConfig2,((SX1278_SpreadFactor[LoRa_Rate] << 4) + (SX1278_CRC << 2)+ 0x03));
		SX1278_SPIRead(0x31,&Dbuf,1);
		tmp = Dbuf;
		tmp &= 0xF8;
		tmp |= 0x05;
		SX1278_SPIWrite(0x31, tmp);
		SX1278_SPIWrite(0x37, 0x0C);
	}else{
	  SX1278_SPIWrite(LR_RegModemConfig1,((SX1278_LoRaBandwidth[LoRa_BW] << 4) + (SX1278_CR << 1) + 0x00)); //Explicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
	  SX1278_SPIWrite(LR_RegModemConfig2,((SX1278_SpreadFactor[LoRa_Rate] << 4) + (SX1278_CRC << 2)+ 0x03)); //SFactor &  LNA gain set by the internal AGC loop
	}
	SX1278_SPIWrite(LR_RegSymbTimeoutLsb, 0xFF); //RegSymbTimeoutLsb Timeout = 0x3FF(Max)
	SX1278_SPIWrite(LR_RegPreambleMsb, 0x00); //RegPreambleMsb
	SX1278_SPIWrite(LR_RegPreambleLsb, 12); //RegPreambleLsb 8+4=12byte Preamble
	SX1278_SPIWrite(REG_LR_DIOMAPPING2, 0x01); //RegDioMapping2 DIO5=00, DIO4=01
	module->readBytes = 0;
	SX1278_standby(module); //Entry standby mode
//	delay1ms(15);
}

void SX1278_SPIBurstWrite(uint8_t addr, uint8_t* txBuf,uint8_t length) 
{
  uint8_t bigcombuf[37];
	int i;
	bigcombuf[0] = addr | 0x80;
	for(i=0;i<=length;i++)
	{
	  bigcombuf[i+1] =  *(txBuf + i);
	}
	
	Spi_SetCS(FALSE);
	WriteData(bigcombuf,length+1);
	Spi_SetCS(TRUE);
}

void SX1278_standby(SX1278_t * module)	
{
	SX1278_SPIWrite(LR_RegOpMode, 0x09);
	module->status = STANDBY;
}

void SX1278_sleep(SX1278_t * module)
{
  SX1278_SPIWrite(LR_RegOpMode, 0x00);
	module->status = SLEEP;
}

void SX1278_entryLoRa() 
{
	SX1278_SPIWrite(LR_RegOpMode, 0x88);
}

void SX1278_clearLoRaIrq() 
{
	SX1278_SPIWrite(LR_RegIrqFlags, 0xFF);
}


//CAD检测
int SX1278_LoRaCadPacket(SX1278_t * module,uint32_t timeout)
{
	 SX1278_standby(module); //Entry standby mode
   SX1278_SPIWrite(LR_RegIrqFlagsMask, 0xFA);
	 SX1278_clearLoRaIrq();
//	  SX1278_SPIRead(module, REG_LR_DIOMAPPING2, &RegOpModebuf,1);
	 SX1278_SPIWrite(REG_LR_DIOMAPPING1, 0x20);//DIO0=00,DIO1=10,DIO2=00, DIO3=00
	 //SX1278_SPIWrite(module, REG_LR_DIOMAPPING2, 0x00);//DIO4=00,DIO5=00
	
	 SX1278_SPIWrite(LR_RegOpMode, 0x8F);  //CAD模式
	 while(1)
	 {
	   if(DIO3flag)  //DIO3
	   {
			 DIO3flag = 0;
			 //SX1278_clearLoRaIrq(module);
			 SX1278_SPIWrite(LR_RegIrqFlags, 0x04);
			// SX1278_standby(module); //Entry standby mode
			 if(DIO1flag)  //DIO1
			 {
				 Gpio_SetIO(0,3,FALSE);
				 DIO1flag =0;
				 //SX1278_clearLoRaIrq(module);
				 SX1278_SPIWrite(LR_RegIrqFlags, 0x01);
				 return 0;
			 }else{
				 return 1;
			 }
	   }
		 
		 if(--timeout == 0)
		{
		  SX1278_hw_Reset();
			SX1278_defaultConfig(module);
			return 0;
		}
		delay1ms(1);
	 
	 }
	 
	 
	 
}


int SX1278_LoRaEntryRx(SX1278_t * module, uint8_t length, uint32_t timeout)
{
    uint8_t addr;
	module->packetLength = length;
	
	SX1278_defaultConfig(module);
	SX1278_SPIWrite(REG_LR_PADAC, 0x84);	//Normal and RX
	SX1278_SPIWrite(LR_RegHopPeriod, 0xFF);	//No FHSS
	SX1278_SPIWrite(REG_LR_DIOMAPPING1, 0x01);//DIO=00,DIO1=00,DIO2=00, DIO3=01
    SX1278_SPIWrite(LR_RegIrqFlagsMask, 0x3F);//Open RxDone interrupt & Timeout
	SX1278_clearLoRaIrq();
	SX1278_SPIWrite(LR_RegPayloadLength, length);//Payload Length 21byte(this register must difine when the data long of one byte in SF is 6)
	SX1278_SPIRead(LR_RegFifoRxBaseAddr,&addrbuf,1);
	addr = addrbuf;
	SX1278_SPIWrite(LR_RegFifoAddrPtr, addr); //RxBaseAddr->FiFoAddrPtr
	SX1278_SPIWrite(LR_RegOpMode, 0x8d);	//Mode//Low Frequency Mode;
	module->readBytes = 0;
	
	while(1)
	{
		uint8_t RegModemStat;
		SX1278_SPIRead(LR_RegModemStat,&ModemStatbuf,1);
		RegModemStat = ModemStatbuf;
	  if((RegModemStat & 0x04 ) == 0x04)
		{
		  module->status = RX;
			return 1;
		}
		
		if(--timeout == 0)
		{
		  SX1278_hw_Reset();
			SX1278_defaultConfig(module);
			return 0;
		}
		delay1ms(1);
	}
}


uint8_t SX1278_LoRaRxPacket(SX1278_t * module)
{
    unsigned char addr;
    unsigned char packet_size;
	
	if(LoRaNodeRxFlag)
	{
	  memset(module->rxBuffer, 0x00, SX1278_MAX_PACKET);
		SX1278_SPIRead(LR_RegFifoRxCurrentaddr,&RegFifoRxCurrentaddrbuf,1);
		addr = RegFifoRxCurrentaddrbuf;
		SX1278_SPIWrite(LR_RegFifoAddrPtr, addr); //RxBaseAddr -> FiFoAddrPtr
		if (module->LoRa_Rate == SX1278_LORA_SF_6)
		{
		  packet_size = module->packetLength;
		}else{
		  SX1278_SPIRead(LR_RegRxNbBytes,&RegRxNbBytesbuf,1); 
			packet_size = RegRxNbBytesbuf;
		}
		
		SX1278_SPIRead(0x00, module->rxBuffer, packet_size);
		module->readBytes = packet_size;
		SX1278_clearLoRaIrq();
	}
	return module->readBytes;
}

int SX1278_LoRaEntryTx(SX1278_t * module, uint8_t length, uint32_t timeout)
{
  uint8_t addr;
	uint8_t temp;
	module->packetLength = length;
	SX1278_defaultConfig(module); //setting base parameter
	SX1278_SPIWrite(REG_LR_PADAC, 0x87);	//Tx for 20dBm
	SX1278_SPIWrite(LR_RegHopPeriod, 0x00); //RegHopPeriod NO FHSS
	SX1278_SPIWrite(REG_LR_DIOMAPPING1, 0x41); //DIO0=00, DIO1=00,DIO2=00, DIO3=01
	SX1278_clearLoRaIrq();
	SX1278_SPIWrite(LR_RegIrqFlagsMask, 0xF7); //Open TxDone interrupt
	SX1278_SPIWrite(LR_RegPayloadLength, length); //RegPayloadLength 21byte
	SX1278_SPIRead(LR_RegFifoTxBaseAddr,&RegFifoTxBaseAddrbuf,1);
	addr = RegFifoTxBaseAddrbuf;
	SX1278_SPIWrite(LR_RegFifoAddrPtr, addr); //RegFifoAddrPtr
	while(1)
	{
	    SX1278_SPIRead(LR_RegPayloadLength,&RegPayloadLengthbuf,1);
		temp = RegPayloadLengthbuf;
		if(temp == length)
		{
		   module->status = TX;
			return 1;
		}
		
		if(--timeout == 0)
		{
		  SX1278_hw_Reset();
			SX1278_defaultConfig(module);
			return 0;
		}
	}
}

int SX1278_LoRaTxPacket(SX1278_t * module, uint8_t* txBuffer, uint8_t length,uint32_t timeout)
{
	SX1278_SPIBurstWrite(0x00, txBuffer, length);
	SX1278_SPIWrite(LR_RegOpMode, 0x8b);	//Tx Mode
//	Gpio_SetIO(0,3,TRUE);
	while(1)
	{
	  if(LoRaNodeTxFlag)
		{
			Gpio_SetIO(0,3,TRUE);
			LoRaNodeTxFlag  = 0;
		    SX1278_SPIRead(LR_RegIrqFlags,&RegIrqFlagsbuf,1);
			SX1278_clearLoRaIrq(); //Clear irq
//			Gpio_SetIO(0,3,FALSE);
			SX1278_standby(module); //Entry Standby mode
			return 1;
		}
		
		if(--timeout == 0)
		{
		  SX1278_hw_Reset();
			SX1278_defaultConfig(module);
			return 0;
		}
		delay1ms(1);
	}		
}


void SX1278_defaultConfig(SX1278_t * module) {
	SX1278_config(module, module->frequency, module->power, module->LoRa_Rate,module->LoRa_BW);
}

void SX1278_begin(SX1278_t *module, uint8_t frequency, uint8_t power,uint8_t LoRa_Rate, uint8_t LoRa_BW, uint8_t packetLength)
{
	 SX1278_HW_init();
	 module->frequency = frequency;
	 module->power = power;
	 module->LoRa_Rate = LoRa_Rate;
	 module->LoRa_BW = LoRa_BW;
	 module->packetLength = packetLength;
	 SX1278_defaultConfig(module);
}


uint8_t SX1278_read(SX1278_t * module, uint8_t* rxBuf, uint8_t length)
{
  if(length != module->readBytes)
    length = module->readBytes;
	memcpy(rxBuf, module->rxBuffer, length);
	//rxBuf[length] = '\0';
	module->readBytes = 0;
	return length;
} 

