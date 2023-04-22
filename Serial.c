/*----------------------------------------------------------------------------
 * Name:    Serial.c
 * Purpose: MCB1700 Low level serial functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <LPC17xx.H> 
#include "Serial.h"

/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
 *----------------------------------------------------------------------------*/
void SER_init (int uart_port, int baudrate) 
{
  uint32_t Fdiv;
  uint32_t pclkdiv, pclk;
  LPC_UART_TypeDef *pUart;

  // (1)
  if (uart_port == 0) {                          // UART0 
    LPC_PINCON->PINSEL0 |= (1 << 4);             // Pin P0.2 used as TXD0 (Com0) 
    LPC_PINCON->PINSEL0 |= (1 << 6);             // Pin P0.3 used as RXD0 (Com0) 
    pclkdiv = (LPC_SC->PCLKSEL0 >> 6) & 0x03;    // Bits 7:6 are for UART0; see page 57 of the user manual!   
    pUart = (LPC_UART_TypeDef *)LPC_UART0;
  } else {                                       // UART2 
    // power up the UART2 peripheral;
    // Note: this is the only difference compared to the other branch of
    // this if, i.e., the case of UART0; here we need to power-up the UART2
    // because (as described on page 307 of the user manual) only UART0 is
    // powered by default! so, to be able to use UART2, we need to set to
    // "1" the bit index 24 of the PCONP register, as discussed on page
    // 64 of the user manual!
    LPC_SC->PCONP |= ( 1 << 24 );    
    LPC_PINCON->PINSEL0 |= (1 << 20);            // Pin P0.10 used as TXD1 (Com2) 
    LPC_PINCON->PINSEL0 |= (1 << 22);            // Pin P0.11 used as RXD1 (Com2) 
    pclkdiv = (LPC_SC->PCLKSEL1 >> 16) & 0x03;   // Bits 17:16 of PCLKSEL1 are for UART2; see page 58 of the user manual!
    pUart = (LPC_UART_TypeDef *)LPC_UART2;
  }

  // (2)
  switch ( pclkdiv )
  {
    case 0x00:
    default:
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
  }
  
  // (3)
  pUart->LCR    = 0x83;                          // 8 bits, no Parity, 1 Stop bit
  Fdiv = ( pclk / 16 ) / baudrate;               // baud rate 
  pUart->DLM = Fdiv / 256;
  pUart->DLL = Fdiv % 256;
  pUart->LCR = 0x03;                             // DLAB = 0
  pUart->FCR = 0x07;                             // Enable and reset TX and RX FIFO
}


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int SER_putChar (int uart, int c) {
  LPC_UART_TypeDef *pUart;

  pUart = (uart == 0) ? (LPC_UART_TypeDef *)LPC_UART0 : (LPC_UART_TypeDef *)LPC_UART2;
  while (!(pUart->LSR & 0x20));
  return (pUart->THR = c);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
int SER_getChar (int uart) {
  LPC_UART_TypeDef *pUart;

  pUart = (uart == 0) ? (LPC_UART_TypeDef *)LPC_UART0 : (LPC_UART_TypeDef *)LPC_UART2;
  while (!(pUart->LSR & 0x01));
  return (pUart->RBR);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (non blocking read)
 *----------------------------------------------------------------------------*/
int SER_getChar_nb (int uart) {
  LPC_UART_TypeDef *pUart;

  pUart = (uart == 0) ? (LPC_UART_TypeDef *)LPC_UART0 : (LPC_UART_TypeDef *)LPC_UART2;
  if (pUart->LSR & 0x01)
    return (pUart->RBR);
  else
    return 0;
}


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
void SER_putString (int uart, char *s) {

  while (*s != 0) {
   SER_putChar(uart, *s++);
  }
}
