#ifndef WAVEPLAYER_H
#define WAVEPLAYER_H

#define DEBUG

#define B_SIZE 2048
#define H_SIZE 45 

typedef struct {
unsigned short audioFormat;
unsigned short numChannels;
unsigned short byteRate;
unsigned long  sampleRate;
unsigned long  bitsPerSample;
} WaveFile;

int checkWavFile(WaveFile*);
int ReadWavHeader(WaveFile*);
int playWavFile(char*);
int startWavFile();
void closeWavFile(WaveFile*);
void printWaveFileInfo(WaveFile*);


#endif
