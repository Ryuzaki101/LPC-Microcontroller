//////////////////////////////////////////////////////////////////////////////////////////
//                       LIBRAIRIE DE GESTION DE TEMPORISATIONS                         //
//////////////////////////////////////////////////////////////////////////////////////////
// Auteur                 : Steve Tchuinte                                              //
//                                            																					//
//////////////////////////////////////////////////////////////////////////////////////////


#include <lpc17xx.h>
#include "LPC17xx_Bits.h"
#include "Timing.h"
#include "math.h"


/* If processor works on high frequency delay has to be increased, it can be 
   increased by factor 2^N by this constant                                   */
#define DELAY_2N     0

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                               VARIABLES GLOBALES                                     //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

 LPC_TIM_TypeDef *_timer = NULL;					// pointeur sur timer
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                               FONCTIONS PUBLIQUES                                    //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                             CONFIGURE LE TIMER UTILISÉ                               //
//////////////////////////////////////////////////////////////////////////////////////////
void Timing_InitializeTimer(int numTimer)
{
	switch (numTimer)
	{
		case TIMER0:
			_timer = LPC_TIM0;						// pointer sur TIMER0
			LPC_SC->PCLKSEL0 |= PCLKSEL0_TIM0_4;	// Horloge timer0 = Clock/4 = 25Mhz
			LPC_SC->PCONP |= PCONP_PCTIM0;			// Timer0 sous tension (déjà ok par défaut)
			break;
			
		case TIMER1:
			_timer = LPC_TIM1;						// pointer sur TIMER1
			LPC_SC->PCLKSEL0 |= PCLKSEL0_TIM1_4;	// Horloge timer1 = 25Mhz
			LPC_SC->PCONP |= PCONP_PCTIM1;			// Timer1 sous tension (déjà ok par défaut)
			break;
			
		case TIMER2:
			_timer = LPC_TIM2;						// pointer sur TIMER2
			LPC_SC->PCLKSEL1 |= PCLKSEL1_TIM2_4;	// horloge timer2 = 25Mhz
			LPC_SC->PCONP |= PCONP_PCTIM2;			// Timer2 sous tension
			break;
			
			
		default:
			_timer = LPC_TIM3;						// pointer sur TIMER3
			LPC_SC->PCLKSEL1 |= PCLKSEL1_TIM3_4;	// horloge timer 3 = 25Mhz
			LPC_SC->PCONP |= PCONP_PCTIM3;			// timer3 sous tension
	}
	
	_timer->CTCR = TIMCTCR_TIMERMODE;				// timer en mode timer
	_timer->PR = 24;								// une incrémentation par µs
	_timer->MCR = TIMMCR_MR0I;						// flag d'interruption sur comparaison match0
}





//////////////////////////////////////////////////////////////////////////////////////////
//                       INTRODUIT UN DÉLAI EN MICROSECONDES                            //
//////////////////////////////////////////////////////////////////////////////////////////
void Timing_WaitUs (uint32_t time)
{
	if (time == 0)							// Pour temps très courts
		return;
	
	if (_timer == NULL)						// si aucun timer configuré
		Timing_InitializeTimer(3);			// Prendre timer 0 par défaut
    //MotorPwm_init();								//--------------------------------------------------------->
	_timer->TCR = TIMTCR_RESET;				// Reset timer
	_timer->MR0 = time;						// Temps d'attente en µs
	_timer->IR |= TIMIR_MR0;				// Reset flag Match0
	_timer->TCR = TIMTCR_ENABLE;			// Lancer timer
	while (!(_timer->IR & TIMIR_MR0));		// attendre positionnement du flag
}



void Timing_WaitMs (uint32_t time)
{
	Timing_WaitUs(time * 1000);
}



void Timing_Stop(int numTimer)
{
	switch (numTimer)
	{
		case TIMER0:
			_timer = LPC_TIM0;						// pointer sur TIMER0
      _timer->TCR = 0x0;
			break;
			
		case TIMER1:
			_timer = LPC_TIM1;						// pointer sur TIMER1
      _timer->TCR = 0x0;
			break;
			
		case TIMER2:
			_timer = LPC_TIM2;						// pointer sur TIMER2
      _timer->TCR = 0x0;
			break;
			
		default:
			_timer = LPC_TIM3;						// pointer sur TIMER3
      _timer->TCR = 0x0;
	}
}


void Timing_Start(int numTimer)
{
	switch (numTimer)
	{
		case TIMER0:
			_timer = LPC_TIM0;						// pointer sur TIMER0
			_timer->PR  = 0x00018;
			_timer->TCR = 0x02; 
			_timer->TCR = 0x01;
			break;
			
		case TIMER1:
			_timer = LPC_TIM1;						// pointer sur TIMER1
      _timer->PR  = 0x0001;
			_timer->TCR = 0x02; 
			_timer->TCR = 0x01;
			break;
			
		case TIMER2:
			_timer = LPC_TIM2;						// pointer sur TIMER2
      _timer->PR  = 0x0001;
			_timer->TCR = 0x02; 
			_timer->TCR = 0x01;
			break;
			
		default:
			_timer = LPC_TIM3;						// pointer sur TIMER3
      _timer->PR  = 0x0001;
			_timer->TCR = 0x02; 
			_timer->TCR = 0x01;
	}
}





long get_integer_val(long Hex,char bits)
{
//	uint32_t Hex_2_Int;
	long Hex_2_Int;
	char byte;
	Hex_2_Int=0;

	for(byte=0;byte<bits;byte++)
	{
			if(Hex&(0x0001<<byte)) Hex_2_Int+=1*(pow(2,byte));
			else Hex_2_Int+=0*(pow(2,byte));
	}
	return Hex_2_Int;
}




/*******************************************************************************
* Delay in while loop cycles                                                   *
*   Parameter:    cnt:    number of while cycles to delay                      *
*   Return:                                                                    *
*******************************************************************************/
void delay (int cnt)
{
  cnt <<= DELAY_2N;

  while (cnt--);
}


/*---------------------------------------------------------------------------------
                       void DELAY_us(unsigned int count)
 ----------------------------------------------------------------------------------
 * I/P Arguments: no of ticks (multiple of 1us)
 * Return value	: none

 * description  :
    This function suspends the tasks for specified ticks(in us).

-----------------------------------------------------------------------------------*/

void DELAY_us(unsigned int count)
{
  unsigned int j=0,i=0;

  for(j=0;j<count;j++)
  {
    /* At 100Mhz, the below loop introduces
    DELAY of 1 us */
    for(i=0;i<23;i++);
  }
}



/*---------------------------------------------------------------------------------
                       void DELAY_ms(unsigned int count)
 ----------------------------------------------------------------------------------
 * I/P Arguments: no of ticks (multiple of 1ms)
 * Return value	: none

 * description  :
    This function suspends the tasks for specified ticks(in ms).

-----------------------------------------------------------------------------------*/

void DELAY_ms(unsigned int count)
{
	unsigned int i;
	for (i=0;i<count;i++)
	{
		DELAY_us(1000);
	}
}

 /*---------------------------------------------------------------------------------
                       void DELAY_ms(unsigned int count)
 ----------------------------------------------------------------------------------
 * I/P Arguments: no of ticks (multiple of 1s)
 * Return value	: none

 * description  :
    This function suspends the tasks for specified ticks(in sec).

-----------------------------------------------------------------------------------*/

void DELAY_sec(unsigned int count)
{
	unsigned int i;
	for (i=0;i<count;i++)
	{
		DELAY_ms(1000);
	}
}


unsigned int getPrescalarForUs(uint8_t timerPclkBit)
{
    unsigned int pclk,prescalarForUs;
    pclk = (LPC_SC->PCLKSEL0 >> timerPclkBit) & 0x03;  /* get the pclk info for required timer */

    switch ( pclk )                                    /* Decode the bits to determine the pclk*/
    {
    case 0x00:
        pclk = SystemCoreClock/4;
        break;

    case 0x01:
        pclk = SystemCoreClock;
        break; 

    case 0x02:
        pclk = SystemCoreClock/2;
        break; 

    case 0x03:
        pclk = SystemCoreClock/8;
        break;

    default:
        pclk = SystemCoreClock/4;
        break;  
    }

    prescalarForUs =pclk/1000000 - 1;                    /* Prescalar for 1us (1000000Counts/sec) */

    return prescalarForUs;
}





