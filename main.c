                          //////////////////////////////////////////////////////////////////////////////////////////
// Author                : Othman Bouyahiaoui                                     //
//////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>           
#include <ctype.h>   
#include "LPC17xx_Bits.h"	   
#include <stdlib.h>
#include <string.h>
#include <LPC17xx.H>  
#include "Timing.h"			    
#include "fat32.h"
#include "spi.h"
#include "type.h"
#include "sdcard.h"
#include "gpio.h"
#include "uda1380.h"
#include "i2c.h"
#include "dma.h"
#include "RDB1768_AUDIO.h"


///////////////////////////////////////////////////////////////////////////////////
//                              SENSOR VARIABLES                                 //
/////////////////////////////////////////////////////////////////////////////////
//                              HC_SR04_init1();                              //
///////////////////////////////////////////////////////////////////////////////


#define USER_PORTCLR        LPC_GPIO0->FIOCLR
#define USER_PORTSET        LPC_GPIO0->FIOSET
#define USER_DDR            LPC_GPIO0->FIODIR
#define USER_PIN            LPC_GPIO0->FIOPIN
#define USER_PORT1DIR			  LPC_GPIO1->FIODIR

#define USER_ECHO           19   //Echo ist Input
#define USER_TRIG           6  //TRIG ist Output
#define USER_ERROR          0xFFFF
#define USER_NO_OBSTACLE    0xFFFE
#define B_SIZE 512
#define H_SIZE 44 

// Leds
#define LEDS_FIOPIN	LPC_GPIO2->FIOPIN0	// Leds ports
#define LEDS_FIODIR	LPC_GPIO2->FIODIR	 

#define SWITCH1 P1_21 //sd card chip selecet
#define SWITCH2 P3_26 //sd card POWER SUPPLY
#define SWITCH3 P3_25 //sd card detect

//LLI Structure for DMA 
struct
{
    uint32_t source;
    uint32_t destination;
    uint32_t control;
    uint32_t next;
}LLI0;


char done=0;
float new_TimeIntervall,TimeIntervall=0xffffffff, new_TimeValue = 0, old_TimeValue = 0; 
volatile unsigned char wav_header[H_SIZE];
volatile unsigned char buffer1[B_SIZE],buffer2[B_SIZE],str[12],buf[12];
unsigned char turn = 0;
volatile unsigned int restSamples,audioFormat,numChannels,sampleRate,bitsPerSample,numSamples,WavPlayPos = 0,valueSampleCount,sampleCount = 0,distance=0;
unsigned char returnStatus,sdcardType, emptyFlag = 0;
extern unsigned int SystemCoreClock;
char ch,sourceFileName[12],destFileName[12];
fileConfig_st *srcFilePtr;
fileInfo fileList;

////////////Variablen für messung wandlung zu char/////////////

/* The first array is not used, it is to make array indexing simple */
    char *one_digits[] = { "null", "one.wav", "two.wav", "three.wav", "four.wav",
                              "five.wav", "six.wav", "seven.wav", "eight.wav", "nine.wav"};
 
		char *two_digits[] = {"ten.wav", "eleven.wav", "twelve.wav", "thirteen.wav", "fourteen.wav",
                     "fifteen.wav", "sixteen.wav", "seventeen.wav", "eightteen.wav", "nineteen.wav"};
 
    char *tens_multiple[] = {" ","ten.wav", "twenty.wav", "thirty.wav", "fourty.wav", "fifty.wav",
                             "sixty.wav", "seventy.wav", "eighty.wav", "ninety.wav"};
 
    char *tens_power[] = {"hundred.wav", "thausend.wav"};
		
		char *centimeter[]={"cm.wav"};
		char *pist1,*pist2,*pist3,*pist4,*pist5,*pist6,*pist7;


//////////////////////////////////////////////////////////////////////////////////////////
//                                FONCTIONS PROTOTYPES                                  //
//////////////////////////////////////////////////////////////////////////////////////////

void InitializeGPIO(void);			  				  // Initialiser GPIO
void InitializeGLCD(void);			 					 // initialiser écran
void InitializeTimer(void);								// initialiser timer
void distance_value(void);
void error_no_objekt(void);
void playWavFile(char *);
void InitializeDAC(void);
void convert_to_words(unsigned int );

///////////////////////////////////////////////////////////////////////////////////////////
//																DMA IRQ HANDLER																				//		
//////////////////////////////////////////////////////////////////////////////////////////
volatile uint32_t DMATCCount = 0;
volatile uint32_t DMAErrCount = 0;
volatile uint32_t I2SDMA0Done = 0;
volatile uint32_t I2SDMA1Done = 0;	


void DMA_IRQHandler(void) 
{
  uint32_t regVal,k;

  regVal = LPC_GPDMA->DMACIntTCStat;
  if ( regVal )
  {
	DMATCCount++;
	LPC_GPDMA->DMACIntTCClear |= regVal;
	if ( regVal & 0x01 )
	{
		if(srcFilePtr->endOfFileDetected != 1){
								for(k=0;k<5012;k++)	buffer1[k]=FILE_GetCh(srcFilePtr);
								DMA_Init( 0, M2P );
		}
		else I2SDMA0Done = 1;
	}
	else if ( regVal & 0x02 )
	{
	  I2SDMA1Done = 1;
	}
  } 

  regVal = LPC_GPDMA->DMACIntErrStat;
  if ( regVal )
  {
	DMAErrCount++;
	LPC_GPDMA->DMACIntErrClr |= regVal;
  }

}		
		

//////////////////////////////////////////////////////////////////////////////////////////
//                                   INTERRUPTS                                      //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                                 TIMER 1   IRQ                               //
//////////////////////////////////////////////////////////////////////////////////////////
void TIMER1_IRQHandler(void)
{
	new_TimeValue = LPC_TIM1->CR1; //Rising edge detected -> start timer
	new_TimeIntervall = new_TimeValue - old_TimeValue;
	old_TimeValue = new_TimeValue;
	if(new_TimeIntervall < TimeIntervall) {
		TimeIntervall = new_TimeIntervall; //Falling edge detected -> read capture register
	}

	LPC_TIM1->IR = TIMIR_CR1;//delete flag

}

//////////////////////////////////////////////////////////////////////////////////////////
//                                 TIMER 0     IRQ                             //
//////////////////////////////////////////////////////////////////////////////////////////
void TIMER0_IRQHandler(void)
{
	uint32_t isrMask;
	isrMask = LPC_TIM0->IR;

	sampleCount++;
  WavPlayPos++;
//	WavPlayPos+=2;
    if(WavPlayPos == B_SIZE-1)  // if buffer empty
    {
				restSamples = numSamples - sampleCount;
				emptyFlag = 1;
        WavPlayPos = 0;
        turn ^= 1;
    }
	if((buffer1[WavPlayPos+1]) && (buffer2[WavPlayPos+1])  != EOF){
    if (turn == 0)
		 LPC_DAC->DACR	 = ((buffer1[WavPlayPos]) & 0x3ff) << 8 | 1<<16 ;//8bit wave
    else
     LPC_DAC->DACR	 = ( (buffer2[WavPlayPos]) & 0x3ff) << 8 | 1<<16 ;
	}
   
  if(numSamples < sampleCount - 1)
    {
			 done =1 ;
			 restSamples=0;
       sampleCount = 0;
			 emptyFlag = 0; // used when the num. of samples exceed the size of both buffers put together, load new samples during audio playing
       WavPlayPos++;
    }
	LPC_TIM0->IR = isrMask;		
}






int32_t main(void)
{  

  SystemInit(); 
	InitializeGPIO();
	InitializeTimer();
	InitializeDAC();

	LLI0.source = (uint32_t) &buffer1; 
	LLI0.destination = (uint32_t) &LPC_DAC->DACR; 
	LLI0.next = (uint32_t) &buffer2; 
	LLI0.control = 0x800 | (1 << 26)| (2 << 21)| (2 << 18); 
//	convert_to_words(143);
	returnStatus = SD_Init(&sdcardType);
	//probe.wav->8bit@22.05khz****probe1.wav->8bit@8khz****one.wav->8bit@16khz****cm.wav->8bit@8khz****test.wav->16bit@44.1khz
	LPC_SC->PCONP |= (1 << 29);	/* Enable GPDMA clock */
	while(1)										 //main loop
	{
	USER_PORTSET |= (1<<USER_TRIG); 
  DELAY_us(10); 			// wait for 60 us	
	USER_PORTCLR |= (1<<USER_TRIG);
  DELAY_ms(200); 			// wait for 200 ms "measurement cycle"
	distance_value();
	}
}



//////////////////////////////////////////////////////////////////////////////////////////
//                                   FONCTIONS                                          //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                             INITIALISATION DES GPIO                                  //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------

void InitializeGPIO(void)
{

	LPC_PINCON->PINSEL3 |= (1<<6)|(1<<7);		//P1.19 config as CAP1.0 PIN  & P0.6 GPIO
	USER_PORT1DIR &= ~(1 <<USER_ECHO);	         //Echo P1.19 Input
//	LPC_PINCON->PINSEL0 |= (1<<10)|(1<<11);
//	LPC_PINCON->PINSEL0 &= ~ ((1<<13)|(1<<12));
//	USER_DDR |= 0UL <<USER_ECHO;	         //Echo P0.5 Input
	USER_DDR |= 1UL <<USER_TRIG;	        //Trigger P0.6 Output
	USER_PIN |= 0UL <<USER_TRIG;
	LEDS_FIODIR = 0xff;
	LEDS_FIOPIN = 0;
	

}






////////////////////////////////////////////////////////////////////////////////////////////
////                             INITIALISE LE TIMER 2                                    //
////////////////////////////////////////////////////////////////////////////////////////////

void InitializeTimer(void)
{
	
	LPC_TIM0->MCR |= 0x03;     														/* Clear TC on MR0 match and Generate Interrupt*/
	LPC_TIM0->PR  |= getPrescalarForUs(PCLK_TIMER0);     /* Prescalar for 1us */  
	LPC_TIM0->MR0 |= 125;																//Wave sample frequency
	NVIC_EnableIRQ(TIMER0_IRQn);
	
	LPC_TIM1->CCR |= 0x38;     													/* Capture on CAP1.1 both edges: a sequence of 0 then 1 on CAPn.1 will cause CR1 to be loaded with the contents of TC*/
  LPC_TIM1->PR  |= getPrescalarForUs(PCLK_TIMER0);    /* Prescalar for 1us */  
	LPC_TIM1->TCR |= 0x01;
	NVIC_EnableIRQ(TIMER1_IRQn);
	NVIC_SetPriority(TIMER1_IRQn,0);

}

void InitializeDAC(void)
{
	
	LPC_PINCON->PINSEL1 |= (1<<21); //P0.26 as AOUT
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	/*DMA CONFIG FOR DAC*/
//	LPC_DAC->DACCNTVAL |= 3125; //DACCLK 25 MHz, 22Us for fs=44.1Khz 
//	LPC_DAC->DACCTRL |= 0x0E;

}





void distance_value(void)
{
//	dec = get_integer_val(LPC_TIM1->TC,'8');
//	dec = (LPC_TIM0->TC) / 25000000 ; 	
//	time_sek = (dec/7200000)*1000;
	distance = 17*(new_TimeIntervall)/1000;
	if(distance >= 400){
		error_no_objekt();
	}else convert_to_words(distance);

}








void error_no_objekt(void)
{
	playWavFile("error.wav");
	Timing_WaitUs(100);
}

void playWavFile(char *pist){
	
	int i=0;
	valueSampleCount = 6;
	//I2S Init and uda1380 config through i2c
  //vfAudioInit();
	srcFilePtr = FILE_Open(pist, READ ,&returnStatus);
	if(!srcFilePtr)
	{
	 LEDS_FIOPIN = 0;		// could not open wave file->unmatching name or file does not exist 
	}
	else{
		LEDS_FIOPIN = 15;
	}
			while(srcFilePtr->byteCounter != 0x2B){
					ch =FILE_GetCh(srcFilePtr);// Dumping header information on header buffer
					wav_header[i++]=ch;					
			}
			audioFormat = wav_header[20]; //PCM = 1, other values indicate some form of compression
			numChannels = wav_header[22]; //Mono = 1 , Stereo = 2
			sampleRate = wav_header[24] | (wav_header[25] << 8) | (wav_header[26] << 16) | (wav_header[27] << 24);
			bitsPerSample = wav_header[34];
			numSamples = ((wav_header[43]<<24|wav_header[42]<<16|wav_header[41]<<8|wav_header[40])*8)/(bitsPerSample*numChannels);			
			for(i=0;i<512;i++)	buffer1[i]=FILE_GetCh(srcFilePtr);
			for(i=0;i<512;i++)	buffer2[i]=FILE_GetCh(srcFilePtr);
		  
			LPC_TIM0->TCR = 0x01; 
			
			while(!done){
					if(emptyFlag)
					{
						if(restSamples < B_SIZE){
								if(turn == 1)
								{
								for(i=0;i<restSamples;i++)	buffer1[i]=FILE_GetCh(srcFilePtr);
								}
								else{			 
								for(i=0;i<restSamples;i++)	buffer2[i]=FILE_GetCh(srcFilePtr);
								}
						FILE_Close(srcFilePtr);
						}else{
								if(turn == 1)
								{
								for(i=0;i<512;i++)	buffer1[i]=FILE_GetCh(srcFilePtr);
								}
								else{			 
								for(i=0;i<512;i++)	buffer2[i]=FILE_GetCh(srcFilePtr);
								}
						}
						emptyFlag = 0;
					}
				}
				LPC_TIM0->TCR |= 0x02; 
				done=0;
//			LPC_GPDMA->DMACIntTCClear = 0x03;
//			LPC_GPDMA->DMACIntErrClr = 0x03;
//			LPC_GPDMA->DMACConfig = 0x01;	/* Enable DMA channels, little endian */
//			while ( !(LPC_GPDMA->DMACConfig & 0x01) );
//			/* on DMA channel 0, Source is memory LLI, destination is I2S TX FIFO, 
//			Enable channel and IE bit */
//			DMA_Init( 0, M2P );
//			LPC_GPDMACH0->DMACCConfig |= 0x1 | (0x00 << 1) | (0x07 << 6) | (0x01 << 11);
//			NVIC_EnableIRQ(DMA_IRQn);

//			LPC_I2S->I2SDAO &= ~ (1<<4);//start i2s
//			LPC_I2S->I2SDAO &= ~ (1<<3);
//			LPC_I2S->I2SDAO &= ~ (1<<15); 
//			LPC_I2S->I2SDMA1 |= ((0x1 << 1)|(0x4 <<16));/* Channel 1 is for TX. */
//			NVIC_DisableIRQ(TIMER0_IRQn);

//			LPC_I2S->I2SDAO |= 0x1<<3|0x1<<4; //stop i2s

}



void convert_to_words(unsigned int num)
{
		char wavenum=0;
    // Get number of digits in given number
    /* Base cases */
		if( 19 < num && num < 100){
			if(num%10 == 0){//num=X0
				pist1 = tens_multiple[num / 10];
			}else{//num=XX
				pist1 = tens_multiple[(num / 10)];
				pist2 = one_digits[num % 10];
				wavenum=2;
			}			
		}
		else if (99 < num && num < 1000){
			pist1 = one_digits[num / 100];
			pist2 = tens_power[0];
			if((num%10 + num%100 )== 0){//num=X00
			wavenum=2;
			}else if((num%10)== 0){//XX0
			pist3 = tens_multiple[(num % 100)/10];
			wavenum=3;				
			}else if( (num % 100) < 20){
			pist3=two_digits[(num % 100)% 10];
			wavenum=3;				
			}else if((num % 100)<10){
			pist3=one_digits[num % 10];
			wavenum=3;				
			}else{//XXX
			pist3 = tens_multiple[(num % 100)/10];
			pist4 = one_digits[num % 10];
			wavenum=4;				
			}
			
	 }else if (999 < num && num< 4000){
		 	pist1 = one_digits[num / 1000];
			pist2 = tens_power[1];
			if((num%10 + num%100 + num%1000)==0){//num=X000
			wavenum=2;
			}else if((num%10 + num%100 )==0){//num=XX00
			pist3 = one_digits[(num % 1000)/100] ;
			pist4 = tens_power[0];
			wavenum=4;
			}else if((num%10)==0){//num=XXX0
			pist3 = one_digits[(num % 1000)/100] ;	
			pist4 = tens_power[0];
			pist5 = tens_multiple[(num % 100)/10];
			wavenum=5;
			}else{//num=XXXX
			pist3 = one_digits[(num % 1000)/100] ;
			pist4 = tens_power[0];
			wavenum=5;
				if( 10 < (num % 100)&& (num % 100) < 20) {
					pist5=two_digits[(num % 100)% 10];
				}else if((num % 100)<10){
					pist5=one_digits[num % 10];
				}else{
				pist5 =	tens_multiple[(num % 100)/10];
				pist6 = one_digits[num % 10];
				wavenum=6;
				}
			}
		}else if (9 < num && num< 20){ //num=XX
			pist1 = two_digits[num % 10];
		}else{
			pist1=one_digits[num];//num=X
		}
		pist7=centimeter[0];
		playWavFile(pist1);
		switch(wavenum){
			case 2:
						playWavFile(pist2);
						break;
			case 3:
						playWavFile(pist2);
						playWavFile(pist3);
						break;
			case 4:
						playWavFile(pist2);
						playWavFile(pist3);
						playWavFile(pist4);
						break;
			case 5:
						playWavFile(pist2);
						playWavFile(pist3);
						playWavFile(pist4);
						playWavFile(pist5);
						break;
			case 6:
						playWavFile(pist2);
						playWavFile(pist3);
						playWavFile(pist4);
						playWavFile(pist5);
						playWavFile(pist6);
						break;
			}
		playWavFile(pist7);//Unit "cm"
}

uint32_t DMA_Init( uint32_t ChannelNum, uint32_t DMAMode )
{
  if ( ChannelNum == 0 )
  {
	LPC_GPDMA->DMACIntTCClear = 0x01;   
	if ( DMAMode == M2P )
	{
	  /* Ch0 set for M2P transfer from mempry to I2S TX FIFO. */
	  LPC_GPDMACH0->DMACCSrcAddr = (uint32_t)&buffer1;
	  LPC_GPDMACH0->DMACCDestAddr = (uint32_t)&LPC_DAC->DACR;
		LPC_GPDMACH0->DMACCLLI = (uint32_t)&LLI0;
	  /* The burst size is set to 1. Terminal Count Int enable */
	  LPC_GPDMACH0->DMACCControl = (DMA_SIZE & 0x0FFF) | (0x00 << 12) | (0x00 << 15) | (0x02 << 18)| (0x02 << 21)
		| (1 << 26);
	}
	else if ( DMAMode == P2M )
	{
	  /* Ch0 set for P2M transfer from I2S RX FIFO to memory. */
	  LPC_GPDMACH0->DMACCSrcAddr = DMA_SSP0_RX_FIFO;
	  LPC_GPDMACH0->DMACCDestAddr = DMA_DST;
	  /* The burst size is set to 1. Terminal Count Int enable. */
	  LPC_GPDMACH0->DMACCControl = (DMA_SIZE & 0x0FFF) | (0x00 << 12) | (0x00 << 15)
		| (1 << 27) | 0x80000000;
	}
	else if ( DMAMode == P2P )
	{
	  /* Ch0 set for P2P transfer from I2S DAO to I2S DAI. */
	  LPC_GPDMACH0->DMACCSrcAddr = DMA_I2S_TX_FIFO;
	  LPC_GPDMACH0->DMACCDestAddr = DMA_SSP0_RX_FIFO;
	  /* The burst size is set to 32. */
	  LPC_GPDMACH0->DMACCControl = (DMA_SIZE & 0x0FFF) | (0x04 << 12) | (0x04 << 15) 
		| 0x80000000;
	}
	else
	{
	  return ( 0 );
	}
  }
  else if ( ChannelNum == 1 )
  {   
	LPC_GPDMA->DMACIntTCClear = 0x02;
	if ( DMAMode == M2P )
	{
	  /* Ch1 set for M2P transfer from mempry to I2S TX FIFO. */
	  LPC_GPDMACH1->DMACCSrcAddr = DMA_SRC;
	  LPC_GPDMACH1->DMACCDestAddr = DMA_DACR;
	  /* The burst size is set to 1. Terminal Count Int enable. */
	  LPC_GPDMACH1->DMACCControl = (DMA_SIZE & 0x0FFF) | (0x04 << 12) | (0x04 << 15)
		| (1 << 26) | 0x80000000;
	}
	else if ( DMAMode == P2M )
	{
	  /* Ch1 set for P2M transfer from I2S RX FIFO to memory. */
	  LPC_GPDMACH1->DMACCSrcAddr = DMA_SSP0_RX_FIFO;
	  LPC_GPDMACH1->DMACCDestAddr = DMA_DST;
	  /* The burst size is set to 1. Terminal Count Int enable. */
	  LPC_GPDMACH1->DMACCControl = (DMA_SIZE & 0x0FFF) | (0x00 << 12) | (0x00 << 15)
		| (1 << 27) | 0x80000000;
	}
	else if ( DMAMode == P2P )
	{
	  /* Ch1 set for P2P transfer from I2S DAO to I2S DAI. */
	  LPC_GPDMACH1->DMACCSrcAddr = DMA_I2S_TX_FIFO;
	  LPC_GPDMACH1->DMACCDestAddr = DMA_SSP0_RX_FIFO;
	  /* The burst size is set to 32. */
	  LPC_GPDMACH1->DMACCControl = (DMA_SIZE & 0x0FFF) | (0x04 << 12) | (0x04 << 15) 
		| 0x80000000;
	}
	else
	{
	  return ( 0 );
	}
  }
  else
  {
	return ( 0 );
  }
  return( 1 );
}




