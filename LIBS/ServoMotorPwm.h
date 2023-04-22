#ifndef __PWM_H 
#define __PWM_H

#include "type.h"
#include "GLCD.h"
#include <stdint.h>


/*************************************************************************************************
                           PWM Config Bit Positions
*************************************************************************************************/
#define PWM_1   1u   /* ((uint32_t)1<<0) */
#define PWM_2   2u   /* ((uint32_t)1<<1) */
#define PWM_3   4u   /* ((uint32_t)1<<2) */
#define PWM_4   8u   /* ((uint32_t)1<<5) */
#define PWM_5  16u   /* ((uint32_t)1<<4) */
#define PWM_6  32u   /* ((uint32_t)1<<5) */

#define PWM_CHANNEL_MASK  0x3Fu /* 00111111- All 6pwm channels selected */
/*************************************************************************************************/



/*************************************************************************************************
                           PWM Config Bit Positions
*************************************************************************************************/
#define TCR_CNT_EN  0x00000001
#define TCR_RESET	  0x00000002
#define TCR_PWM_EN	0x00000008


#define PWMSEL2			(1 << 2)
#define PWMSEL3			(1 << 3)
#define PWMSEL4			(1 << 4)
#define PWMSEL5			(1 << 5)
#define PWMSEL6			(1 << 6)
#define PWMENA1			(1 << 9)
#define PWMENA2			(1 << 10)
#define PWMENA3			(1 << 11)
#define PWMENA4			(1 << 12)
#define PWMENA5			(1 << 13)
#define PWMENA6			(1 << 14)

#define LER0_EN			(1 << 0)
#define LER1_EN			(1 << 1)
#define LER2_EN			(1 << 2)
#define LER3_EN			(1 << 3)
#define LER4_EN			(1 << 4)
#define LER5_EN			(1 << 5)
#define LER6_EN			(1 << 6)

#define PWM_CYCLE						255
/***************************************************************************************************
                             Function prototypes
***************************************************************************************************/
extern int  gripper_close(void);
extern int gripper_open(void);
extern void PWM_Init(uint32_t cycleTime);
extern void PWM_Set( uint32_t channelNum, uint32_t cycle, uint32_t offset );
extern void PWM_SetDutyCycle( uint32_t pin, uint32_t dutyCycle );
extern void PWM_Start( uint32_t channelNum );
extern void PWM_Stop( uint32_t channelNum );
extern uint32_t PWM_Init1( uint32_t ChannelNum, uint32_t cycleTime);

#endif /* end __PWM_H */
