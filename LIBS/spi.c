/***************************************************************************************************
                                   ExploreEmbedded    
 ****************************************************************************************************
 * File:   spi.h
 * Version: 16.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: File contains the functions to read and write data from SPI

The libraries have been tested on ExploreEmbedded development boards. We strongly believe that the 
library works on any of development boards for respective controllers. However, ExploreEmbedded 
disclaims any kind of hardware failure resulting out of usage of libraries, directly or indirectly.
Files may be subject to change without prior notice. The revision history contains the information 
related to updates. 


GNU GENERAL PUBLIC LICENSE: 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 

Errors and omissions should be reported to codelibraries@exploreembedded.com
 **************************************************************************************************/

#include <lpc17xx.h>
#include "spi.h"
#include "gpio.h"


uint8_t dummy_u8;





/***************************************************************************************************
                           void SPI_Init(void)
 ****************************************************************************************************
 * I/P Arguments : none

 * Return value  : none

 * description :This function initializes the SPI module of LPC1768.
                 MOSI -- P0.18
                 MISO -- P0.17
                 SCK  -- P0.15
                 SSEL -- P0.16               
 ****************************************************************************************************/
void SPI_Init(void)
{    
    
	
  /* SSEL is GPIO, output set to high. */
  LPC_GPIO1->FIODIR   |=  (1<<21);            /* P1.21 is output             */
  LPC_GPIO1->FIOPIN   |=  (1<<21);            /* set P1.21 high (SSEL inact.)*/
 	LPC_PINCON->PINSEL3 |= (1<<10)|(1<<11)|(1<<9)|(1<<8);// NO CLK NO POWER ON P1.21 SSEL0(SD CARD CS) P1.20 SCK0 (SD CARD CLOCK) 

  /* P3.26 is SD Card Power Supply Enable Pin */
  LPC_GPIO3->FIODIR   |=  (1<<26);            /* P3.26 is output             */
  LPC_GPIO3->FIOPIN   &= ~(1<<26);            /* set P3.26 low(enable power) */

  /* SCK, MISO, MOSI are SSP pins. */
  LPC_PINCON->PINSEL3 &= ~(3UL<<8);          /* P1.20 cleared               */
  LPC_PINCON->PINSEL3 |=  (3UL<<8);          /* P1.20 SCK0                  */
  LPC_PINCON->PINSEL3 &= ~((3<<14) | (3<<16));  /* P1.23, P1.24 cleared        */
  LPC_PINCON->PINSEL3 |=  ((3<<14) | (3<<16));  /* P1.23 MISO0, P1.24 MOSI0    */

  LPC_SC->PCLKSEL1 &= ~(3<<10);               /* PCLKSP0 = CCLK/4 ( 25MHz)   */
  //LPC_SC->PCLKSEL1 |=  (1<<10);               /* PCLKSP0 = CCLK   (100MHz)   */
  SPI_DisableChipSelect();                  /* Disable the Slave Select */

  LPC_SSP0->CPSR = 250;                       /* 100MHz / 250 = 400kBit      */
                                              /* maximum of 18MHz is possible*/    
  LPC_SSP0->CR0  = 0x0007;                    /* 8Bit, CPOL=0, CPHA=0        */
  LPC_SSP0->CR1  = 0x0002;                    /* SSP0 enable, master         */
  LPC_SC->PCONP       |= (1 << 21);           /* Enable power to SSPI0 block */
}





/***************************************************************************************************
                          uint8_t SPI_Write (uint8_t spiData_u8)
 ****************************************************************************************************
 * I/P Arguments : 
                 uint8_t : Byte of data to be send on SPI.

 * Return value  : 
                 uint8_t : Returns back the data send on SPI, this is used in case of SD card.

 * description :
                 This function is used to send a byte of data through SPI.              
 ****************************************************************************************************/
uint8_t SPI_Write (uint8_t spiData_u8)
{
    LPC_SSP0->DR = spiData_u8;
    while(util_GetBitStatus(LPC_SSP0->SR,SBIT_BSY) == 1); /* wait until data is sent */
    dummy_u8 = LPC_SSP0->SR;
    dummy_u8 = LPC_SSP0->DR;

    return spiData_u8;
}






/***************************************************************************************************
                          uint8_t SPI_Read(void)
 ****************************************************************************************************
 * I/P Arguments : none

 * Return value  : 
                  uint8_t : Returns back a byte of data read from SPI.

 * description :
                 This function is used to read a byte of data from SPI.              
 ****************************************************************************************************/
uint8_t SPI_Read(void)
{
    uint8_t spiData_u8;

    LPC_SSP0->DR = 0xff;

    while(util_GetBitStatus(LPC_SSP0->SR,SBIT_BSY) == 1); /* wait until data is received */
    dummy_u8 = LPC_SSP0->SR;                               /* Dummy read to clear the flags */
    spiData_u8 = (uint8_t)LPC_SSP0->DR; 

    return spiData_u8;
}






/***************************************************************************************************
                    static uint32_t spi_GetPclk(void)
 ****************************************************************************************************
 * I/P Arguments : none

 * Return value  : 
                  uint32_t : SPI clock frequency set in PCLKSEL register.

 * description :
                 This function is used to determine the pclk value for SPI.              
 ****************************************************************************************************/


