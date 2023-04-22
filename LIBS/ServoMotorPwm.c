#include <lpc17xx.h>
#include "LPC17xx_Bits.h"
#include "Timing.h"
#include "gpio.h"
#include "GLCD2.h"
#include "ServoMotorPwm.h"


static uint32_t pwmInitDoneFlag_u32 = 0;
volatile uint32_t match_counter0, match_counter1;


/***************************************************************************************************
                        void PWM_Init(uint32_t cycleTime)
 ****************************************************************************************************
** Author				: 	Steve Tchuinte
 * I/P Arguments   : uint32_t- PWM cycle time(Ton+Toff).
 * Return value    : none

 * description :This function initializes the internal PWM1 module of lpc1768.
                By default the pwm  cycle(Ton+Toff) is set 255 to match the arduino style.
 ***************************************************************************************************/
void PWM_Init(uint32_t cycleTime)
{

    LPC_PWM1->TCR = TCR_RESET;    /* Counter Reset */ 
    LPC_PWM1->PR = 0x0;        /* count frequency:Fpclk */
    LPC_PWM1->MCR = 3;    /* interrupt on PWMMR0, reset on PWMMR0, reset TC if PWM matches */                
    LPC_PWM1->MR0 = 100; //	PWM_SetDutyCycle(PWM_2,dutyCycle);  //P2_1       /* set PWM cycle(Ton+Toff) */
    LPC_PWM1->LER = LER0_EN; 
    LPC_PWM1->TCR = TCR_CNT_EN | TCR_PWM_EN;    /* counter enable, PWM enable */
}







uint32_t PWM_Init1( uint32_t ChannelNum, uint32_t cycle )
{
	LPC_PINCON->PINSEL4 = 0x00001555;	/* set GPIOs for all PWM pins on PWM0 */
	LPC_PWM1->MR1 = 0;
	LPC_PWM1->LER = LER1_EN;
  return (1);
}

/******************************************************************************
** Author				: 	Steve Tchuinte
** Function name:		PWM_Set
**
** Descriptions:		PWM cycle setup
**
** parameters:			Channel number, PWM cycle, and offset
** Returned value:		None
** 
******************************************************************************/
void PWM_Set( uint32_t ChannelNum, uint32_t cycle, uint32_t offset )
{			
  if ( ChannelNum == 1 )
  {
	LPC_PWM1->MR0 = cycle;		/* set PWM cycle */
	LPC_PWM1->MR1 = cycle * 5/6 + offset;
	LPC_PWM1->MR2 = cycle * 2/3 + offset;
	LPC_PWM1->MR3 = cycle * 1/2 + offset;
	LPC_PWM1->MR4 = cycle * 1/3 + offset;
	LPC_PWM1->MR5 = cycle * 1/6 + offset;
	LPC_PWM1->MR6 = offset;

	/* The LER will be cleared when the Match 0 takes place, in order to
	load and execute the new value of match registers, all the PWMLERs need to
	reloaded. all PWM latch enabled */
	LPC_PWM1->LER = LER0_EN | LER1_EN | LER2_EN | LER3_EN | LER4_EN | LER5_EN | LER6_EN;
  }
  return;
}



/******************************************************************************
** Author				: 	Steve Tchuinte
** Function name:		PWM_Start
**
** Descriptions:		Enable PWM by setting the PCR, PTCR registers
**
** parameters:			channel number
** Returned value:		None
** 
******************************************************************************/
void PWM_Start( uint32_t channelNum )
{
  if ( channelNum == 1 )
  {
	/* All single edge, all enable */
	LPC_PWM1->PCR = PWMENA1 | PWMENA2 | PWMENA3 | PWMENA4 | PWMENA5 | PWMENA6;
	LPC_PWM1->TCR = TCR_CNT_EN | TCR_PWM_EN;	/* counter enable, PWM enable */
  }
  return;
}



/******************************************************************************
** Author				: 	Steve Tchuinte
** Function name:		PWM_Stop
**
** Descriptions:		Stop all PWM channels
**
** parameters:			channel number
** Returned value:		None
** 
******************************************************************************/
void PWM_Stop( uint32_t channelNum )
{
  if ( channelNum == 1 )
  {
	LPC_PWM1->PCR = 0;
	LPC_PWM1->TCR = 0x00;		/* Stop all PWMs */
  }
  return;
}



/***************************************************************************************************
                         void PWM_SetDutyCycle( uint32_t pin, uint32_t dutyCycle )
 ****************************************************************************************************
** Author				: 	Steve Tchuinte
 * I/P Arguments: uint32_t: Pin number at which PWM needs to be generated.(PWM_1 - PWM_5).
                  uint32_t : required dutyCycle at the pwm pin.
 * Return value    : none

 * description :This function initializes the internal PWM1 module of lpc1768.
                By default the pwm  cycle(Ton+Toff) is set 255 to match the arduino style.
 ***************************************************************************************************/
void PWM_SetDutyCycle( uint32_t pin, uint32_t dutyCycle )
{  
    uint32_t pinSelect=0;
    if(pwmInitDoneFlag_u32 == 0)
    {
        PWM_Init(PWM_CYCLE);   // Set the PWM_CYCLE to 255 if user has not called the PWM_Init function.
        pwmInitDoneFlag_u32 = 1;
    }  
    pin = pin & PWM_CHANNEL_MASK; /* Ensure only supported channel bits are set */

    while(pin)
    {
        pinSelect = ((~pin)+1) & pin; /* Extract the first bit  from left side */
        pin = pin & (~pinSelect);     /* Clear the Bit after extracting, as it is being processed now*/

        switch(pinSelect)
        {
        case PWM_1:
            GPIO_PinFunction(P2_0,PINSEL_FUNC_1);
            LPC_PWM1->MR1 = dutyCycle;
            LPC_PWM1->LER|= LER1_EN; 
            break;
        default :
            break;            
        }
    }
}  



//Fonction pour ouvrir le gripper
int gripper_open(void)
{   
		uint32_t duty;
	
		GLCD_SetTextColors(CYAN,RED);					   		
		GLCD_Print(XCMPT1,YCMPT1,"Winkel = 180 Grd");			   
	
		for(duty=0;duty<2000;duty++) 
    {
			PWM_SetDutyCycle(PWM_1,duty); 
			DELAY_ms(20);
    }
		return 1;
}


//Fonction pour fermer le gripper
int gripper_close(void)
{	
	if ( PWM_Init1(1,0) != 1 )
  {
		PWM_Set(1,2000,0);
		PWM_Start(1);
		GLCD_SetTextColors(CYAN,RED);					   		
		GLCD_Print(XCMPT1,YCMPT1,"Winkel = 0 Grd");			   
		return 0;
	}
}



