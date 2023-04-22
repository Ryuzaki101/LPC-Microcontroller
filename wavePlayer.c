#include <ctype.h>   
#include "LPC17xx_Bits.h"	   // toujours présent
#include <stdlib.h>
#include <string.h>
#include <LPC17xx.H>  
#include "Timing.h"			    // accès à la librairie Timing.c
#include "fat32.h"
#include "wavePlayer.h"

unsigned char wav_header[H_SIZE];
unsigned char buffer1[B_SIZE];
unsigned char buffer2[B_SIZE];
const unsigned char riff_h[4] = { 'R', 'I', 'F', 'F' };
const unsigned char wave_h[4] = { 'W', 'A', 'V', 'E' };
const unsigned char fmt_h[4]  = { 'f', 'm', 't', ' ' };
const unsigned char data_h[4] = { 'd', 'a', 't', 'a' };

unsigned int WavPlayPos = 0;
unsigned char emptyFlag = 0;
unsigned char turn = 0;
int trackStatus = 0;
unsigned int sampleCount = 0;
unsigned int valuePR2;
unsigned int valueSampleCount;




//void TIMER1_IRQHandler(void)
//{
//	sampleCount++;

//    if(WavPlayPos == B_SIZE-1)  // if buffer empty
//    {
//        WavPlayPos = 0;
//        emptyFlag = 1;
//        turn ^= 1;
//    }

//    if (turn == 0)
//       LPC_TIM1->MR0 = buffer1[WavPlayPos]; //COMPARE REGISTER
//    else
//       LPC_TIM1->MR0 = buffer2[WavPlayPos];

//	if(sampleCount > valueSampleCount-1)
//    {
//       sampleCount = 0;
//       WavPlayPos++;
//    }

//		LPC_TIM1->IR = TIMIR_MR0;				 /* Clear the Interrupt Bit */
//}




// returns 0 on successful play,
// returns 1 on no wave file,
// returns 2 on invalid file format 





int playWaveFile(char* WavFileName)
{
	fileConfig_st* filePointer;
	WaveFile currentWaveFile;
	uint8_t WaveReadStatus;
	int WaveCheckStatus,i;

	filePointer = FILE_Open(WavFileName, READ , &WaveReadStatus);

	if(!filePointer)
	{
		return 1;		// no wave file detected
	}
	for( i=0;i<44;i++){
	// Dumping header information on header buffer
	buffer1[i]=FILE_GetCh(filePointer);
	}
	
	WaveReadStatus = ReadWavHeader(&currentWaveFile);

	WaveCheckStatus = checkWavFile(&currentWaveFile);

	if(WaveReadStatus == 0 && WaveCheckStatus == 0)
	{
		

		while(FILE_GetCh(filePointer)!=EOF)
		{
			if(emptyFlag)
			{
				if(turn == 1)
				{
					for( i=0;i<44;i++){
						// Dumping header information on header buffer
						buffer1[i]=FILE_GetCh(filePointer);
						}				
				}
				else
				{
					for( i=0;i<44;i++){
						// Dumping header information on header buffer
						buffer2[i]=FILE_GetCh(filePointer);
						}
				}
				emptyFlag = 0;
			}
		}


		closeWavFile(&currentWaveFile);

		FILE_Close(filePointer);
	


		return 0;

	}
	else
		return 2;		// invalid file format detected!

}		

// Returns 1 on failure, returns 0 on success
int checkWavFile(WaveFile* w_file)
{
	if(w_file->audioFormat != 1)
		return 1;

	if(w_file->numChannels != 1)
		return 1;
	
	if(w_file->bitsPerSample != 8)
		return 1;
	
	switch(w_file->sampleRate)
	{
		case(0x00001F40):	valuePR2 = 416;
							valueSampleCount = 6;
							break;
		case(0x00003E80): 	valuePR2 = 416;
							valueSampleCount = 3;
							break;
		case(0x00005622): 	valuePR2 = 452;
							valueSampleCount = 2;
							break;
		case(0x0000AC44): 	valuePR2 = 452;
							valueSampleCount = 1;
							break;
		default:			return 1;
	}	
	return 0;
}

// Returns 1 on failure, returns 0 on success
int ReadWavHeader(WaveFile* w_file)
{
	int i;

	for(i = 0; i < 4; i++)
	{
     	if(riff_h[i] != wav_header[i])
      		 return 1;
  	}

	for(i = 0; i < 4; i++)
  	{
        if(wave_h[i] != wav_header[i+8])
         	return 1;
    }

	for(i = 0; i < 4; i++)
    {
        if(fmt_h[i] != wav_header[i+12])
     	    return 1;
    }
	
	w_file->audioFormat = wav_header[20];
	w_file->numChannels = wav_header[22];

	w_file->sampleRate = wav_header[24] | (wav_header[25] << 8) |
						(wav_header[26] << 16) | (wav_header[27] << 24);

	w_file->bitsPerSample = wav_header[34];

	return 0;
}
	
//int startWavFile()
//{
//	if(FSInit())
//	{
//		#ifdef DEBUG
//		UART1_puts("SD-Card Mounted.\n");
//		#else
//		PORTBbits.RB2 = 1;
//		#endif
//	}
//	else
//	while(!FSInit()) 
//	{
//		#ifdef DEBUG
//		UART1_puts("Mount error! Exiting...\n");
//		#else
//		PORTBbits.RB2 = 1;
//		#endif
//		return 1;
//	}
//	return 0;
//}

void closeWavFile(WaveFile* w_file)
{
	int i;

	w_file->audioFormat = 0;
	w_file->numChannels = 0;
	w_file->byteRate = 0;
	w_file->sampleRate = 0;
	w_file->bitsPerSample = 0;

	for(i = 0; i < H_SIZE; i++)
		wav_header[i] = 0;
	
	for(i = 0; i < B_SIZE; i++)
		buffer1[i] = 0;

	for(i = 0; i < B_SIZE; i++)
		buffer2[i] = 0;

	WavPlayPos = 0;	
	emptyFlag = 0;
	turn = 0;
	trackStatus = 0;
	sampleCount = 0;
	valuePR2 = 0;
  valueSampleCount = 0;
}

//void printWaveFileInfo(WaveFile* w_file)
//{
//	UART1_puts("\nCompression type: ");
//	if(w_file->audioFormat == 1)
//		UART1_puts("PCM.\n");
//	else
//		UART1_puts("Others.\n");

//	UART1_puts("Channels: ");
//	if(w_file->numChannels == 1)
//		UART1_puts("1\n");
//	else
//		UART1_puts("2\n");

//	UART1_puts("Bits per sample: ");
//	if(w_file->bitsPerSample == 8)
//		UART1_puts("8\n");
//	else
//		UART1_puts("16\n");

//	UART1_puts("Sample Rate: ");
//	switch(w_file->sampleRate)
//	{
//		case(0x00001F40):	UART1_puts("8000hz\n");
//							valuePR2 = 416;
//							valueSampleCount = 6;
//							break;
//		case(0x00003E80): 	UART1_puts("16000hz\n");
//							valuePR2 = 416;
//							valueSampleCount = 3;
//							break;
//		case(0x00005622): 	UART1_puts("22050hz\n");
//							valuePR2 = 452;
//							valueSampleCount = 2;
//							break;
//		case(0x0000AC44): 	UART1_puts("44100hz\n");
//							valuePR2 = 452;
//							valueSampleCount = 1;
//							break;
//		default:			UART1_puts("Invalid.\n");
//							break;
//	}	
//}
//	
	
	

	
	
