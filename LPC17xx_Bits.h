//////////////////////////////////////////////////////////////////////////////////////////
//                     DÉCLARATIONS SUPPLÉMENTAIRES (BITS) POUR LCP17xx                 //
//////////////////////////////////////////////////////////////////////////////////////////
// Auteur                 : Steve Romaric Tchuinte                                                    //
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                               DEFINES BITS SYSTÈME                                   //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                                PCONP : LPC_SC->PCONP                                 //
//////////////////////////////////////////////////////////////////////////////////////////


// DÉFINITIONS DES BITS DE PCONP(valeur après SystemInit = 0x042887DE)
// -------------------------------------------------------------------

#define PCONP_PCTIM0 	0x02		// Timer/Counter 0 power/clock control bit (1)
#define PCONP_PCTIM1 	0x04		// Timer/Counter 1 power/clock control bit (1)
#define PCONP_PCUART0	0x08		// UART0 power/clock control bit (1)

#define PCONP_PCUART1	0x10		// UART1 power/clock control bit (1)
#define PCONP_PWM1 		0x40		// PWM1 power/clock control bit (1)
#define PCONP_PCI2C0 	0x80		// I2C0 interface power/clock control bit (1)

#define PCONP_PCSPI 	0x100		// The SPI interface power/clock control bit (1)
#define PCONP_PCRTC 	0x200		// The RTC power/clock control bit (1)
#define PCONP_PCSSP1 	0x400		// The SSP 1 interface power/clock control bit (1)

#define PCONP_PCAD 		0x1000		// A/D converter (ADC) power/clock control bit (0)
									// Note: Clear the PDN bit in the AD0CR before clearing this bit, and set
									// this bit before setting PDN.
#define PCONP_PCCAN1	0x2000		// CAN Controller 1 power/clock control bit (0)
#define PCONP_PCCAN2	0x4000		// CAN Controller 2 power/clock control bit (0)
#define PCONP_PCGPIO	0x8000		// GPIO (1)

#define PCONP_PCRIT		0x10000		// Repetitive Interrupt Timer power/clock control bit (0)
#define PCONP_PCMC		0x20000		// Motor Control PWM (0)
#define PCONP_PCQEI		0x40000		// Quadrature Encoder Interface power/clock control bit (0)
#define PCONP_PCI2C1	0x80000		// The I2C1 interface power/clock control bit (1)

#define PCONP_PCSSP0	0x200000	// The SSP0 interface power/clock control bit (1)
#define PCONP_PCTIM2	0x400000	// Timer 2 power/clock control bit (0)
#define PCONP_PCTIM3	0x800000	// Timer 3 power/clock control bit (0)

#define PCONP_PCUART2	0x1000000	// UART 2 power/clock control bit (0)
#define PCONP_PCUART3	0x2000000	// UART 3 power/clock control bit (0)
#define PCONP_PCI2C2	0x4000000	// I2C interface 2 power/clock control bit (1)
#define PCONP_PCI2S		0x8000000	// I2S interface power/clock control bit (0)

#define PCONP_PCGPDMA	0x20000000 	// GPDMA function power/clock control bit (0)
#define PCONP_PCENET	0x40000000	// Ethernet block power/clock control bit (0)
#define PCONP_PCUSB		0x80000000	// USB interface power/clock control bit (0)

//////////////////////////////////////////////////////////////////////////////////////////
//                           PCLKSEL0 : LPC_SC->PCLKSEL0                                //
//////////////////////////////////////////////////////////////////////////////////////////

// Diviseur d'horloge pour les périphériques - Part 1
// --------------------------------------------------

#define PCLKSEL0_WDT_4		0x00000000	// Watchdog sur CCLK/4
#define PCLKSEL0_WDT_1		0x00000001	// CCLK 
#define PCLKSEL0_WDT_2		0x00000002	// CCLK / 2
#define PCLKSEL0_WDT_8		0x00000003	// CCLK / 8
#define PCLKSEL0_WDT_MASK	0xFFFFFFFC	// Masque pour annulation bits

#define PCLKSEL0_TIM0_4		0x00000000	// Idem Timer 0
#define PCLKSEL0_TIM0_1		0x00000004
#define PCLKSEL0_TIM0_2		0x00000008
#define PCLKSEL0_TIM0_8		0x0000000C
#define PCLKSEL0_TIM0_MASK	0xFFFFFFF3

#define PCLKSEL0_TIM1_4		0x00000000	// Timer 1
#define PCLKSEL0_TIM1_1		0x00000010
#define PCLKSEL0_TIM1_2		0x00000020
#define PCLKSEL0_TIM1_8		0x00000030
#define PCLKSEL0_TIM1_MASK	0xFFFFFFCF

#define PCLKSEL0_UART0_4	0x00000000	// UART 0
#define PCLKSEL0_UART0_1	0x00000040
#define PCLKSEL0_UART0_2	0x00000080
#define PCLKSEL0_UART0_8	0x000000C0
#define PCLKSEL0_UART0_MASK 0xFFFFFF3F

#define PCLKSEL0_UART1_4	0x00000000	// UART 1
#define PCLKSEL0_UART1_1	0x00000100
#define PCLKSEL0_UART1_2	0x00000200
#define PCLKSEL0_UART1_8	0x00000300
#define PCLKSEL0_UART1_MASK 0xFFFFFCFF

#define PCLKSEL0_PWM1_4		0x00000000	// PWM1
#define PCLKSEL0_PWM1_1		0x00001000
#define PCLKSEL0_PWM1_2		0x00002000
#define PCLKSEL0_PWM_8		0x00003000
#define PCLKSEL0_PWM1_MASK	0xFFFFCFFF

#define PCLKSEL0_I2C0_4		0x00000000	// I2C0
#define PCLKSEL0_I2C0_1		0x00004000
#define PCLKSEL0_I2C0_2		0x00008000
#define PCLKSEL0_I2C0_8		0x0000C000
#define PCLKSEL0_I2C0_MASK	0xFFFF3FFF

#define PCLKSEL0_SPI_4		0x00000000	// SPI
#define PCLKSEL0_SPI_1		0x00010000
#define PCLKSEL0_SPI_2		0x00020000
#define PCLKSEL0_SPI_8		0x00030000
#define PCLKSEL0_SPI_MASK	0xFFFCFFFF

#define PCLKSEL0_SSP1_4		0x00000000	// SSP1
#define PCLKSEL0_SSP1_1		0x00100000
#define PCLKSEL0_SSP1_2		0x00200000
#define PCLKSEL0_SSP1_8		0x00300000
#define PCLKSEL0_SSP1_MASK	0xFFCFFFFF

#define PCLKSEL0_DAC_4		0x00000000	// DAC
#define PCLKSEL0_DAC_1		0x00400000
#define PCLKSEL0_DAC_2		0x00800000
#define PCLKSEL0_DAC_8		0x00C00000
#define PCLKSEL0_DAC_MASK	0xFF3FFFFF

#define PCLKSEL0_ADC_4		0x00000000	// ADC
#define PCLKSEL0_ADC_1		0x01000000
#define PCLKSEL0_ADC_2		0x02000000
#define PCLKSEL0_ADC_8		0x03000000
#define PCLKSEL0_ADC_MASK	0xFCFFFFFF

#define PCLKSEL0_CAN1_4		0x00000000	//CAN1
#define PCLKSEL0_CAN1_1		0x04000000
#define PCLKSEL0_CAN1_2		0x08000000
#define PCLKSEL0_CAN1_6		0x0C000000	// ! CCLK/6
#define PCLKSEL0_CAN1_MASK	0xF3FFFFFF

#define PCLKSEL0_CAN2_4		0x00000000	// CAN2
#define PCLKSEL0_CAN2_1		0x10000000
#define PCLKSEL0_CAN2_2		0x20000000
#define PCLKSEL0_CAN2_6		0x30000000	// /6
#define PCLKSEL0_CAN2_MASK	0xCFFFFFFF

#define PCLKSEL0_ACF_4		0x00000000	// CAN FILTER
#define PCLKSEL0_ACF_1		0x40000000
#define PCLKSEL0_ACF_2		0x80000000
#define PCLKSEL0_ACF_6		0xC0000000	// /6
#define PCLKSEL0_ACF_MASK	0x3FFFFFFF // masque pour annulation bits

//////////////////////////////////////////////////////////////////////////////////////////
//                           PCLKSEL1 : LPC_SC->PCLKSEL1                                //
//////////////////////////////////////////////////////////////////////////////////////////

// Diviseur d'horloge pour les périphériques - Part 2
// --------------------------------------------------

#define PCLKSEL1_QEI_4		0x00000000	// Quadrature Encoder = CCLK / 4
#define PCLKSEL1_QEI_1		0x00000001	// CCLK 
#define PCLKSEL1_QEI_2		0x00000002	// CCLK / 2
#define PCLKSEL1_QEI_8		0x00000003	// CCLK / 8
#define PCLKSEL1_QEI_MASK	0xFFFFFFFC	// Masque pour annulation bits

#define PCLKSEL1_GPIOINT_4	0x00000000	// Idem pour interruptions GPIO
#define PCLKSEL1_GPIOINT_1	0x00000004
#define PCLKSEL1_GPIOINT_2	0x00000008
#define PCLKSEL1_GPIOINT_8	0x0000000C
#define PCLKSEL1_GPIOINT_MASK 0xFFFFFFF3

#define PCLKSEL1_PCB_4		0x00000000	// Pin connect Block
#define PCLKSEL1_PCB_1		0x00000010
#define PCLKSEL1_PCB_2		0x00000020
#define PCLKSEL1_PCB_8		0x00000030
#define PCLKSEL1_PCB_MASK	0xFFFFFFCF

#define PCLKSEL1_I2C1_4		0x00000000	// I2C 1
#define PCLKSEL1_I2C1_1		0x00000040
#define PCLKSEL1_I2C1_2		0x00000080
#define PCLKSEL1_I2C1_8		0x000000C0
#define PCLKSEL1_I2C_MASK	0xFFFFFF3F

#define PCLKSEL1_SSP0_4		0x00000000	// SSP0
#define PCLKSEL1_SSP0_1		0x00000400
#define PCLKSEL1_SSP0_2		0x00000800
#define PCLKSEL1_SSP0_8		0x00000C00
#define PCLKSEL1_SSP0_MASK	0xFFFFF3FF

#define PCLKSEL1_TIM2_4		0x00000000	// TIMER 2
#define PCLKSEL1_TIM2_1		0x00001000
#define PCLKSEL1_TIM2_2		0x00002000
#define PCLKSEL1_TIM2_8		0x00003000
#define PCLKSEL1_TIM2_MASK	0xFFFFCFFF

#define PCLKSEL1_TIM3_4		0x00000000	// TIMER 3
#define PCLKSEL1_TIM3_1		0x00004000
#define PCLKSEL1_TIM3_2		0x00008000
#define PCLKSEL1_TIM3_8		0x0000C000
#define PCLKSEL1_TIM3_MASK	0xFFFF3FFF

#define PCLKSEL1_UART2_4	0x00000000	// UART 2
#define PCLKSEL1_UART2_1	0x00010000
#define PCLKSEL1_UART2_2	0x00020000
#define PCLKSEL1_UART2_8	0x00030000
#define PCLKSEL1_UART2_MASK	0xFFFCFFFF

#define PCLKSEL1_UART3_4	0x00000000	// UART3
#define PCLKSEL1_UART3_1	0x00040000
#define PCLKSEL1_UART3_2	0x00080000
#define PCLKSEL1_UART3_8	0x000C0000
#define PCLKSEL1_UART3_MASK	0xFFF3FFFF

#define PCLKSEL1_I2C2_4		0x00000000	// I2C 2
#define PCLKSEL1_I2C2_1		0x00100000
#define PCLKSEL1_I2C2_2		0x00200000
#define PCLKSEL1_I2C2_8		0x00300000
#define PCLKSEL1_I2C2_MASK	0xFFCFFFFF

#define PCLKSEL1_I2S_4		0x00000000	// I2S
#define PCLKSEL1_I2S_1		0x00400000
#define PCLKSEL1_I2S_2		0x00800000
#define PCLKSEL1_I2S_8		0x00C00000
#define PCLKSEL1_I2S_MASK	0xFF3FFFFF

#define PCLKSEL1_RIT_4		0x00000000	// REPETITIVE INTERRUPT TIMER
#define PCLKSEL1_RIT_1		0x04000000
#define PCLKSEL1_RIT_2		0x08000000
#define PCLKSEL1_RIT_8		0x0C000000
#define PCLKSEL1_RIT_MASK	0xF3FFFFFF

#define PCLKSEL1_SYSCON_4	0x00000000	// SYSTEM CONTROL BLOCK
#define PCLKSEL1_SYSCON_1	0x10000000
#define PCLKSEL1_SYSCON_2	0x20000000
#define PCLKSEL1_SYSCON_8	0x30000000
#define PCLKSEL1_SYSCON_MASK 0xCFFFFFFF

#define PCLKSEL1_MC_4		0x00000000	// MOTOR CONTROL PWM
#define PCLKSEL1_MC_1		0x40000000
#define PCLKSEL1_MC_2		0x80000000
#define PCLKSEL1_MC_8		0xC0000000
#define PCLKSEL1_MC_MASK	0x3FFFFFFF


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                                   DEFINES                                            //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

// POINTEUR NULL
#ifndef NULL
	#define NULL ((void*) 0)
#endif

#ifndef null
	#define null NULL
#endif

// BOOLÉEN
#ifndef Boolean
	enum { false, true };
	typedef unsigned char Boolean;
#endif

// GESTION DES INTERRUPTIONS
#define	DISABLE_INTERRUPT __set_PRIMASK (1)
#define ENABLE_INTERRUPT __set_PRIMASK(0)

// DIRECTIVES
#define ATTRIBUTE	__attribute__
#define ALIGN4		((aligned (4)))


