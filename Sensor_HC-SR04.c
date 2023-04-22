//#include <lpc17xx.h>
//#include <stdlib.h>
//#include "RTL.h"
//#include "File_Config.h"
//#include "Net_Config.h"
//#include "LPC17xx_Bits.h"
//#include "Sensor_HC-SR04.h"
//#include "Timing.h"
//#include "GLCD.h"			   // accès à la librairie graphique

//LPC_PWM_TypeDef *_pwm1 = NULL;
//LPC_TIM_TypeDef *_timer1 = NULL;

//uint32_t pulse_start;  //--> "pulse_start" for another approach distance calculation
//uint32_t pulse_end;   //--> "pulse_end" for another approach distance calculation
//uint32_t pulse_duration;
//uint32_t distance;

//#define OBJ_DETECT  (LPC_GPIO0->FIODIR0 = 1 << 11)

//void HC_SR04_init(uint8_t TP, uint8_t EP)
//void HC_SR04_init1(void)
//{
//	LPC_GPIO0->FIODIR1   |= 1UL <<10;				//Trigger Pin Input
//	LPC_PINCON->PINMODE0 |= 0<<20;				 //Trigger  Pin Pull-Up
//	Timing_WaitUs(10);
//	LPC_PINCON->PINMODE0 |= 3<<20;   		 //Trigger  Pin Pull-Down
//	
//	LPC_GPIO0->FIODIR1   |= 0UL <<11;	 //Echo Pin Output
//	LPC_PINCON->PINMODE3 |= 1<<22;
//	
// //--> "pulse_start" for another approach distance calculation
//	while(!(LPC_GPIO0->FIODIR1));//Waiting for ECHO
//	pulse_start = _timer1->TCR = TIMTCR_ENABLE;//Start Timer 
//	
//--> "pulse_end" for another approach distance calculation
//	while((LPC_GPIO0->FIODIR0));//Waiting for ECHO goes LOW
//	pulse_end = _timer1->TCR = TIMTCR_RESET;//Timer Stops
//  
//  pulse_duration =  pulse_end - pulse_start;
//  distance = pulse_duration * 17150;
//  distance = round(distance,2);
//	
//	a = ((_timer1->TC)<<8)/58.82 + 1;
//	
//	if(a > 2 && a <= 400)//Check whether the result is valid or not
//	{
//		GLCD_Clear(CYAN);	
//		GLCD_SetTextColors(YELLOW,BLUE);		
//		GLCD_Print(76,55,"Distance =  \n");
//		GLCD_PrintChar(106,55,(a%10+48));	
//		
//		a = a/10;
//		GLCD_Print_Val8(106,54,(a%10+48),3);	
//		
//		a = a/10;
//		GLCD_PrintChar(106,53,(a%10+48));
//    GLCD_Print(76,65," cm");
//	}
//	
//	else
//	{
//		GLCD_Clear(CYAN);		
//		GLCD_Print(76,55,"Out of Range");
//	}
//		Timing_WaitMs(400);
//}

//void SensorSetup(void){
//	LPC_GPIO1->FIODIR1   |= 1UL << 10;		       // P1.10 en sortie
//	LPC_GPIO1->FIODIR1 	 |= 0UL << 9;		        //  P1.9 en entrée
//	LPC_PINCON->PINMODE3 |= 0<<3;              //   pull down
//	GLCD_Print(40,170,"~~ HC-SR04 ~~"); 
//}


//void loop(void){
//	LPC_PINCON->PINMODE3 |= 1<<3;                //   pull down
//	Timing_WaitMs(10);
//	LPC_PINCON->PINMODE3 |= 0<<3;              //   pull down
//	long measure = 0;
//	long mm = measure / (2*0.34);
//	char cm;
//	char m;
//	
//	//Print the Result in mm, cm et m
//	GLCD_Print(40,170,"Abstand : "); 
//	GLCD_PrintChar(40,170,mm);              //Précison de l'unité
//	
//	GLCD_Print(40,170,"mm");
//	GLCD_PrintChar(0,mm/10.0,2);          // Calcul de l'unité "centimètre".Cette Fonction devrait prend que 2 arguments
//	
//	GLCD_PrintChar(40,170,cm);
//	GLCD_PrintChar(0,mm/1000.0,2);      // Calcul de l'unité "centimètre".Cette Fonction devrait prend que 2 arguments
//	GLCD_PrintChar(40,170,m); 
//	
//	Timing_WaitMs(60);
//}


//long HC_SR04_ranging(int sys)
//{
//	distance_cm  = duration /29/2;
//	distance_inc = duration /74/2;
//	if(sys)
//		return distance_cm;
//	else
//		return distance_inc;
//}
