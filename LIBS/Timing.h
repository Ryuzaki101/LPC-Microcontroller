//////////////////////////////////////////////////////////////////////////////////////////
//                          DÉCLARATIONS POUR LIBRAIRIE TIMING.C                        //
//////////////////////////////////////////////////////////////////////////////////////////
// Auteur                 : Steve Romaric Tchuinte                                     //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// Objet : Déclarations pour la librairie Timing.c                                      //
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                                 DEFINES                                              //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>



// Numéro des timers
// ------------------
#define TIMER0	0
#define TIMER1	1
#define TIMER2 	2
#define TIMER3	3

//////////////////////////////////////////////////////////////////////////////////////////
//                                IR : LPC_TIMx->IR                                     //
//////////////////////////////////////////////////////////////////////////////////////////

// Interrupt Register : flags interruptions des timers : écrire 1 pour resetter
// ---------------------------------------------------------------------------

#define TIMIR_MR0 	0x01 		// Interrupt Interrupt flag for match channel 0
#define TIMIR_MR1 	0x02		// Interrupt Interrupt flag for match channel 1
#define TIMIR_MR2		0x04		// Interrupt Interrupt flag for match channel 2
#define TIMIR_MR3		0x08		// Interrupt Interrupt flag for match channel 3
#define TIMIR_CR0		0x10		// Interrupt Interrupt flag for capture channel 0 event
#define TIMIR_CR1		0x20		// Interrupt Interrupt flag for capture channel 1 event

//////////////////////////////////////////////////////////////////////////////////////////
//                                TCR : LPC_TIMx->TCR                                   //
//////////////////////////////////////////////////////////////////////////////////////////

// Timer Control Register: Mise en service et reset des timers
// ------------------------------------------------------------

#define TIMTCR_ENABLE	0x01		// 1 = en service
#define TIMTCR_RESET	0x02		// 1 = reset timer et prédiviseur tant que 1

//////////////////////////////////////////////////////////////////////////////////////////
//                            CTCR : LPC_TIMx->CTCR                                     //
//////////////////////////////////////////////////////////////////////////////////////////

// Count/Timer Control Register : Sélection timer/compteur
// --------------------------------------------------------

// mode (un seul choix)
#define TIMCTCR_TIMERMODE	0x00		// Mode timer
#define TIMCTCR_COUNTRISE	0x01		// Mode compteur flanc montant
#define TIMCTCR_COUNTFALL	0x02		// Mode compteur flanc descendant
#define TIMCTCR_COUNTBOTH	0x03		// Mode compteur flancs montant + descendant

// pin (si mode compteur, sinon laisser à 0)
#define TIMCTCR_CAP0		0x00		// Pin CAP0 utilisée (si mode compteur)
#define TIMCTCR_CAP1		0x04		// Pin CAP1 utilisée (si mode compteur)

//////////////////////////////////////////////////////////////////////////////////////////
//                              MCR : LPC_TIMx->MCR                                     //
//////////////////////////////////////////////////////////////////////////////////////////

// Match Control Register : Sélection de l'action à effectuer sur égalités
// -----------------------------------------------------------------------

// Timer Counter Register = Match Register 0 (cumulatives)
#define TIMMCR_MR0I		0x01		// Interruption
#define	TIMMCR_MR0R 	0x02		// Reset
#define TIMMCR_MR0S		0x04		// Stop

// Timer Counter Register = Match Register 1 (cumulatives)
#define TIMMCR_MR1I		0x08		// Interruption
#define TIMMCR_MR1R		0x10		// Reset
#define TIMMCR_MR1S		0x20		// Stop

// Timer Counter Register = Match Register 2 (cumulatives)
#define TIMMCR_MR2I		0x40		// Interruption
#define TIMMCR_MR2R		0x80		// Reset
#define TIMMCR_MR2S		0x100		// Stop

// Timer Counter Register = Match Register 3 (cumulatives)
#define TIMMCR_MR3I		0x200		// Interruption
#define TIMMCR_MR3R		0x400		// Reset
#define TIMMCR_MR3S		0x800		// Stop

//////////////////////////////////////////////////////////////////////////////////////////
//                                 CCR : LPC_TIMx->CCR                                  //
//////////////////////////////////////////////////////////////////////////////////////////

// Capture Control Register : Évènements de capture
// -------------------------------------------------

// Déclenchement par pin CAP0
#define TIMCCR_CAP0RE	0x01		// Capture sur flanc montant
#define TIMCCR_CAP0FE	0x02		// capture sur flanc descendant
#define TIMCCR_CAP00I	0x04		// Déclenchement d'une interruption si capture

// Déclenchement par pin CAP1
#define TIMCCR_CAP1RE	0x08		// Capture sur flanc montant
#define TIMCCR_CAP1FE	0x10		// capture sur flanc descendant
#define TIMCCR_CAP10I	0x20		// Déclenchement d'une interruption si capture

//////////////////////////////////////////////////////////////////////////////////////////
//                                EMR : LPC_TIMx->EMR                                   //
//////////////////////////////////////////////////////////////////////////////////////////

// External Match Register : Action sur pîns lors des correspondances
// -------------------------------------------------------------------

// Statut des pins de sortie
#define TIMEMR_EM0		0x01		// correspondance Match0 vers pin MATx.0
#define TIMEMR_EM1		0x02		// correspondance Match0 vers pin MATx.1
#define TIMEMR_EM2		0x04		// correspondance Match0 vers pin MATx.2
#define TIMEMR_EM3		0x08		// correspondance Match0 vers pin MATx.3

// Actions sur correspondance Match 0
#define TIMEMR_EMC0CLR	0x10		// EM0 = 0 si correspondance
#define TIMEMR_EMC0SET	0x20		// EM0 = 1: Si EMC0CLR et EMC0SET = 1 -> Toggle

// Actions sur correspondance Match 1
#define TIMEMR_EMC1CLR	0x40		// EM1 = 0 si correspondance
#define TIMEMR_EMC1SET	0x80		// EM1 = 1: Si EMC1CLR et EMC1SET = 1 -> Toggle

// Actions sur correspondance Match 2
#define TIMEMR_EMC2CLR	0x100		// EM2 = 0 si correspondance
#define TIMEMR_EMC2SET	0x200		// EM2 = 1: Si EMC2CLR et EMC2SET = 1 -> Toggle

// Actions sur correspondance Match 3
#define TIMEMR_EMC3CLR	0x400		// EM3 = 0 si correspondance
#define TIMEMR_EMC3SET	0x800		// EM3 = 1: Si EMC3CLR et EMC3SET = 1 -> Toggle


//Nouveaux Define Embedded World
#define SBIT_TIMER0 1 
#define SBIT_TIMER1 2 

#define SBIT_MR0I 0 
#define SBIT_MR0R 1 

#define SBIT_CNTEN  0 

#define PCLK_TIMER0 2 
#define PCLK_TIMER1 4

#define MiliToMicroSec(x)  (x*1000)  			/* ms is multiplied by 1000 to get us*/

//////////////////////////////////////////////////////////////////////////////////////////
//                                  PROTOTYPES                                          //
//////////////////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
// SÉLECTIONNE LE TIMER DES TEMPORISATIONS ET LE CONFIGURE
// Si cette fonction n'est pas appelée avant l'utilisation des temporisations,
// le TIMER0 sera configuré par défaut
// numTimer: numéro du timer, de TIMER0 à TIMER3
//----------------------------------------------------------------------------
extern void Timing_InitializeTimer(int numTimer);


//----------------------------------------------------------------------------------------
// ATTEND LA DURÉE PRÉCISÉE EN µs
// time : nombre minimum de µs d'attente, de 0 à 0xFFFFFFFF
//----------------------------------------------------------------------------------------
extern void Timing_WaitUs (uint32_t time);

//----------------------------------------------------------------------------------------
// ATTEND LA DURÉE PRÉCISÉE EN ms
// time : nombre minimum de ms d'attente, de 0 à 400.000.000
//----------------------------------------------------------------------------------------
extern void Timing_WaitMs (uint32_t time);


//----------------------------------------------------------------------------
// SÉLECTIONNE LE TIMER DES TEMPORISATIONS ET LE STOP
// Si cette fonction n'est pas appelée avant l'utilisation des temporisations,
// le TIMER0 sera configuré par défaut
// numTimer: numéro du timer, de TIMER0 à TIMER3
//----------------------------------------------------------------------------
extern void Timing_Stop(int numTimer);

extern void Timing_Start(int numTimer);

extern void delay (int cnt);
extern void DELAY_sec(unsigned int count);
extern void DELAY_us (unsigned int count);
extern void DELAY_ms (unsigned int count);


//extern uint32_t get_integer_val(uint32_t Hex,char bits);
extern long get_integer_val(long Hex,char bits);

extern unsigned int getPrescalarForUs(uint8_t timerPclkBit);







