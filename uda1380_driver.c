#include "i2c.h" 
#include "uda1380.h" 
 
#define rI2SDAO         (*(volatile unsigned *)(0x400A8000)) 
#define rI2STXFIFO      (*(volatile unsigned *)(0x400A8008)) 
#define rI2STXRATE      (*(volatile unsigned *)(0x400A8020)) 
#define rI2STXBITRATE   (*(volatile unsigned *)(0x400A8028)) 
#define rI2STXMODE      (*(volatile unsigned *)(0x400A8030)) 
 
#define rI2SDMA1        (*(volatile unsigned *)(0x400A8014)) 
#define rI2SDMA2        (*(volatile unsigned *)(0x400A8018)) 
#define rI2SSTATE       (*(volatile unsigned *)(0x400A8010)) 
#define rI2SIRQ         (*(volatile unsigned *)(0x400A801C)) 
 
#define rI2SDAI         (*(volatile unsigned *)(0x400A8004)) 
#define rI2SRXFIFO      (*(volatile unsigned *)(0x400A800C)) 
#define rI2SRXRATE      (*(volatile unsigned *)(0x400A8024)) 
#define rI2SRXBITRATE   (*(volatile unsigned *)(0x400A802C)) 
#define rI2SRXMODE      (*(volatile unsigned *)(0x400A8034)) 
 
#define rIOCON_P0_07    (*(volatile unsigned *)(0x4002C01C)) 
#define rIOCON_P0_08    (*(volatile unsigned *)(0x4002C020)) 
#define rIOCON_P0_09    (*(volatile unsigned *)(0x4002C024)) 
#define rIOCON_P1_16    (*(volatile unsigned *)(0x4002C0C0)) 
 
#define UDA1380_ADDRESS (0x18)  
typedef enum{

UDA1380_EVALMODE_CLK = 0x00,
UDA1380_I2S_BUS_IO = 0x1,
UDA1380_PW_CONTROL = 0x2,
	UDA1380_ANALOG_MIX = 0x3 ,
	UDA1380_HEADPHONE_AMP = 0x4 ,
	UDA1380_MASTER_VOLUME = 0x10 ,
	UDA1380_MIXER_VOLUME = 0x11 ,
	UDA1380_MODE_SELECT = 0x12 ,
	UDA1380_MASTER_CHAIN_SELECT = 0x13 ,
	UDA1380_MIXER_DETECT_OVER = 0x14 ,
	UDA1380_DECIM_VOLUM = 0x20 ,
	UDA1380_PGA = 0x21 ,
	UDA1380_ADC = 0x22 ,
	UDA1380_AGC = 0x23 ,
	UDA1380_SOFT_RESET = 0x7f ,
	UDA1380_FILTER_STATUS = 0x18 ,
	UDA1380_DECIM_STATUS = 0x28 ,
	UDA1380_END = 0xff
}UDA1380_Registers;

struct uda1380_setup
{
UDA1380_Registers reg;
unsigned short int val;
};
const struct uda1380_setup UDA1380_InitPara[]={
{UDA1380_SOFT_RESET, 0x00ff} ,
{UDA1380_PW_CONTROL , 0xA5DF},

{UDA1380_EVALMODE_CLK , 0x0332},
{UDA1380_I2S_BUS_IO , 0x0000},
{UDA1380_ANALOG_MIX , 0x2222 },
{UDA1380_HEADPHONE_AMP , 0x0202 },
	{UDA1380_MASTER_VOLUME , 0x4040 },
	{UDA1380_MIXER_VOLUME , 0xFF00 },
{	UDA1380_MODE_SELECT , 0x0000 },
{	UDA1380_MASTER_CHAIN_SELECT , 0x0000 },
 {UDA1380_MIXER_DETECT_OVER , 0x0000 },
	{UDA1380_DECIM_VOLUM , 0x0000 },
	{UDA1380_PGA , 0x0000 },
	{UDA1380_ADC , 0x0f02 },
	{UDA1380_AGC , 0x0000 },
	
{UDA1380_END , 0xffff }
};



void Uda1380_WriteData(unsigned char reg, unsigned short int data) 
{ 
    unsigned char config[3]; 
     
    config[0] = reg; 
    config[1] = (data >> 8) & 0xFF;    //MS 
    config[2] = data&0xFF;             //LS 
     
     
    I2C0_MasterTransfer(UDA1380_ADDRESS, config, sizeof(config), 0, 0); 
    I2C0_MasterTransfer(UDA1380_ADDRESS, config, 1, &config[1], 2);     //Verify Value by reading it back 
    
	if((config[1]<<8|config[2]) != data) 
    { 
        while(1);   
    } 

} 
 
void Uda1380_config(void) 
{ 
//const struct uda1380_setup *preset;
		I2CInit(); //I2C MASTER MODE

	
//	preset = UDA1380_InitPara;
//	while(preset->reg!=UDA1380_END)
//	{	
//	Uda1380_WriteData(preset->reg, preset->val);
//	preset++;	
//	}
	
    Uda1380_WriteData(0x7F, 0x0);         //restore L3-default values 
    Uda1380_WriteData(0x01, 0x0);         
     
//    Uda1380_WriteData(0x13, 0x0);         
//    Uda1380_WriteData(0x14, 0x0);            
    Uda1380_WriteData(0x00, 0x1|0x1<<4|0x1<<8|0x1<<9);     
    Uda1380_WriteData(0x02, 0x1<<15|0x1<<13|0x1<<10); 

          
 
} 
