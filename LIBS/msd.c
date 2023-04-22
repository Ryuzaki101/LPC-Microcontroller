/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : msd.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : MSD card driver source file.
*                      Pin assignment:
*             ----------------------------------------------
*             |  EM-LPC1700   |     MSD          Pin        |
*             ----------------------------------------------
*             | P0.16         |   ChipSelect      1         |
*             | P0.18 / MOSI  |   DataIn          2         |
*             |               |   GND             3 (0 V)   |
*             |               |   VDD             4 (3.3 V) |
*             | P0.15 / SCLK  |   Clock           5         |
*             |               |   GND             6 (0 V)   |
*             | P0.17 / MISO  |   DataOut         7         |
*             -----------------------------------------------
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <LPC17xx.h>
#include "File_Config.h"
#include "msd.h"
#include "GLCD.h"

#include <stdio.h>				   
#include <ctype.h>				    
#include <stdlib.h>
#include <setjmp.h>
#include <rt_misc.h>
#include <string.h>    
#include "Serial.h"
#include "typedef.h"
#include "SD_File.h"

// cris: these two are for some rudimentay debugging;
#define COUNTER_TIMEOUT 32
U8 received_byte; 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Global Variable ------------------------------------------------------------*/
u32 Mass_Block_Count;
u32 Mass_Block_Size;
u32 Mass_Memory_Size;
sMSD_CSD MSD_csd;

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : MSD_Init
* Description    : Initializes the MSD/SD communication.
* Input          : None
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed 
*******************************************************************************/

u8 MSD_Init(void)
{
  u32 i = 0;
  /* Initialize SPI1 */
  spi_init();
	
  /* MSD chip select high */
  spi_ss(1);
  /* Send dummy byte 0xFF, 10 times with CS high*/
  /* rise CS and MOSI for 80 clocks cycles */
  for (i = 0; i <= 9; i++)
  {
    /* Send dummy byte 0xFF */
		received_byte = spi_send(DUMMY);
		//printf("received: %d\n",received_byte);
  }
	
  /*------------Put MSD in SPI mode--------------*/
  /* MSD initialized and set to SPI mode properly */
  return (MSD_GoIdleState());
}

/*******************************************************************************
* Function Name  : MSD_WriteBlock
* Description    : Writes a block on the MSD
* Input          : - pBuffer : pointer to the buffer containing the data to be
*                    written on the MSD.
*                  - WriteAddr : address to write on.
*                  - NumByteToWrite: number of data to write
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed 
*******************************************************************************/
u8 MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u32 i = 0;
  u8 rvalue = MSD_RESPONSE_FAILURE;

  /* MSD chip select low */
  spi_ss(0);
  /* Send CMD24 (MSD_WRITE_BLOCK) to write multiple block */
  MSD_SendCmd(MSD_WRITE_BLOCK, WriteAddr, 0xFF);

  /* Check if the MSD acknowledged the write block command: R1 response (0x00: no errors) */
  if (!MSD_GetResponse(MSD_RESPONSE_NO_ERROR))
  {
    /* Send a dummy byte */
		spi_send(DUMMY);
    /* Send the data token to signify the start of the data */
		spi_send(0xFE);
    /* Write the block data to MSD : write count data by block */
    for (i = 0; i < NumByteToWrite; i++)
    {
      /* Send the pointed byte */
			spi_send(*pBuffer);
      /* Point to the next location where the byte read will be saved */
      pBuffer++;
    }
    /* Put CRC bytes (not really needed by us, but required by MSD) */
    spi_send(DUMMY);
		spi_send(DUMMY);
    /* Read data response */
    if (MSD_GetDataResponse() == MSD_DATA_OK)
    {
      rvalue = MSD_RESPONSE_NO_ERROR;
    }
  }

  /* MSD chip select high */
  spi_ss(1);
  /* Send dummy byte: 8 Clock pulses of delay */
  spi_send(DUMMY);
  /* Returns the reponse */
  return rvalue;
}

/*******************************************************************************
* Function Name  : MSD_ReadBlock
* Description    : Reads a block of data from the MSD.
* Input          : - pBuffer : pointer to the buffer that receives the data read
*                    from the MSD.
*                  - ReadAddr : MSD's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the MSD.
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed 
*******************************************************************************/
u8 MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  u32 Count_timeout = COUNTER_TIMEOUT;
  u32 i = 0;
  u8 rvalue = MSD_RESPONSE_FAILURE;

  spi_ss(1); // MSD chip select high
  spi_send(DUMMY); // Send dummy byte: 8 Clock pulses of delay

// 		spi_ss(0); // MSD chip select low
// 		// Send CMD17 (MSD_READ_SINGLE_BLOCK) to read one block 
// 		MSD_SendCmd(MSD_READ_SINGLE_BLOCK, ReadAddr, 0xFF);
// 		// Check if the MSD acknowledged the read block command:
// 		// Wait for Response (R1 Format) equal to 0x00 
// 		if (MSD_GetResponse(MSD_RESPONSE_NO_ERROR) == MSD_RESPONSE_FAILURE) {
// 				printf("Error MSD_ReadBlock(): CMD17 rejected\n");
// 				return MSD_RESPONSE_FAILURE;
// 		} else {
// 				printf("MSD_ReadBlock(): CMD17 accepted\n");
// 		}
  
  // CMD17
  Count_timeout = COUNTER_TIMEOUT;
  do {
    spi_ss(1); // MSD chip select high					 
    spi_send(DUMMY); // Send Dummy byte 0xFF						
    spi_ss(0); // MSD chip select low 					 
    MSD_SendCmd(MSD_READ_SINGLE_BLOCK, ReadAddr, 0xFF); // send CMD17						
    // Wait for no error Response (R1 Format) equal to 0x00 
    received_byte = MSD_GetResponse(MSD_RESPONSE_NO_ERROR);
    if (received_byte == MSD_RESPONSE_FAILURE) Count_timeout--;
    //printf("received: %d\n",received_byte);
  }
  while(received_byte == MSD_RESPONSE_FAILURE && Count_timeout > 0);	
  
  if (received_byte == MSD_RESPONSE_FAILURE) {
    printf("Error MSD_ReadBlock(): CMD17 rejected and timeout\n");
    return MSD_RESPONSE_FAILURE;				
  }else {
    //printf("MSD_ReadBlock(): CMD17 accepted\n");
  }				

  // Now look for the data token to signify the start of the data 
  if (!MSD_GetResponse(MSD_START_DATA_SINGLE_BLOCK_READ))
  {
    // Read the MSD block data : read NumByteToRead data 
    for (i = 0; i < NumByteToRead; i++)
    {
      *pBuffer = spi_send(DUMMY); // Save the received data 
      // Point to the next location where the byte read will be saved 
      pBuffer++;
    }
    // Get CRC bytes (not really needed by us, but required by MSD) 
    spi_send(DUMMY);
    spi_send(DUMMY);
    // Set response value to success 
    rvalue = MSD_RESPONSE_NO_ERROR;
  }
  
  return rvalue;
}

/*******************************************************************************
* Function Name  : MSD_WriteBuffer
* Description    : Writes many blocks on the MSD
* Input          : - pBuffer : pointer to the buffer containing the data to be
*                    written on the MSD.
*                  - WriteAddr : address to write on.
*                  - NumByteToWrite: number of data to write
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed 
*******************************************************************************/

u8 MSD_WriteBuffer(u8* pBuffer, u32 WriteAddr, u32 NumByteToWrite)
{
  u32 i = 0, NbrOfBlock = 0, Offset = 0;
  u8 rvalue = MSD_RESPONSE_FAILURE;

  /* Calculate number of blocks to write */
  NbrOfBlock = NumByteToWrite / BLOCK_SIZE;
  /* MSD chip select low */
  spi_ss(0);

  /* Data transfer */
  while (NbrOfBlock --)
  {
    /* Send CMD24 (MSD_WRITE_BLOCK) to write blocks */
    MSD_SendCmd(MSD_WRITE_BLOCK, WriteAddr + Offset, 0xFF);

    /* Check if the MSD acknowledged the write block command: R1 response (0x00: no errors) */
    if (MSD_GetResponse(MSD_RESPONSE_NO_ERROR))
    {
      return MSD_RESPONSE_FAILURE;
    }
    /* Send dummy byte */
    spi_send(DUMMY);
    /* Send the data token to signify the start of the data */
    spi_send(MSD_START_DATA_SINGLE_BLOCK_WRITE);
    /* Write the block data to MSD : write count data by block */
    for (i = 0; i < BLOCK_SIZE; i++)
    {
      /* Send the pointed byte */
      spi_send(*pBuffer);
      /* Point to the next location where the byte read will be saved */
      pBuffer++;
    }
    /* Set next write address */
    Offset += 512;
    /* Put CRC bytes (not really needed by us, but required by MSD) */
    spi_send(DUMMY);
    spi_send(DUMMY);
    /* Read data response */
    if (MSD_GetDataResponse() == MSD_DATA_OK)
    {
      /* Set response value to success */
      rvalue = MSD_RESPONSE_NO_ERROR;
    }
    else
    {
      /* Set response value to failure */
      rvalue = MSD_RESPONSE_FAILURE;
    }
  }

  /* MSD chip select high */
  spi_ss(1);
  /* Send dummy byte: 8 Clock pulses of delay */
  spi_send(DUMMY);
  /* Returns the reponse */
  return rvalue;
}

/*******************************************************************************
* Function Name  : MSD_ReadBuffer
* Description    : Reads multiple block of data from the MSD.
* Input          : - pBuffer : pointer to the buffer that receives the data read
*                    from the MSD.
*                  - ReadAddr : MSD's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the MSD.
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed 
*******************************************************************************/
u8 MSD_ReadBuffer(u8* pBuffer, u32 ReadAddr, u32 NumByteToRead)
{
  u32 i = 0, NbrOfBlock = 0, Offset = 0;
  u8 rvalue = MSD_RESPONSE_FAILURE;

  /* Calculate number of blocks to read */
  NbrOfBlock = NumByteToRead / BLOCK_SIZE;
  /* MSD chip select low */
  spi_ss(0);

  /* Data transfer */
  while (NbrOfBlock --)
  {
    /* Send CMD17 (MSD_READ_SINGLE_BLOCK) to read one block */
    MSD_SendCmd(MSD_READ_SINGLE_BLOCK, ReadAddr + Offset, 0xFF);
    /* Check if the MSD acknowledged the read block command: R1 response (0x00: no errors) */
    if (MSD_GetResponse(MSD_RESPONSE_NO_ERROR))
    {
      return  MSD_RESPONSE_FAILURE;
    }
    /* Now look for the data token to signify the start of the data */
    if (!MSD_GetResponse(MSD_START_DATA_SINGLE_BLOCK_READ))
    {
      /* Read the MSD block data : read NumByteToRead data */
      for (i = 0; i < BLOCK_SIZE; i++)
      {
        /* Read the pointed data */
        *pBuffer = spi_send(DUMMY);
        /* Point to the next location where the byte read will be saved */
        pBuffer++;
      }
      /* Set next read address*/
      Offset += 512;
      /* get CRC bytes (not really needed by us, but required by MSD) */
      spi_send(DUMMY);
      spi_send(DUMMY);
      /* Set response value to success */
      rvalue = MSD_RESPONSE_NO_ERROR;
    }
    else
    {
      /* Set response value to failure */
      rvalue = MSD_RESPONSE_FAILURE;
    }
  }

  /* MSD chip select high */
  spi_ss(1);
  /* Send dummy byte: 8 Clock pulses of delay */
  spi_send(DUMMY);
  /* Returns the reponse */
  return rvalue;
}


/*******************************************************************************
* Function Name  : MSD_GetCSDRegister
* Description    : Read the CSD card register.
*                  Reading the contents of the CSD register in SPI mode
*                  is a simple read-block transaction.
* Input          : - MSD_csd: pointer on an SCD register structure
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed 
*******************************************************************************/
u8 MSD_GetCSDRegister(sMSD_CSD* MSD_csd)
{
  u32 i = 0;
  u8 rvalue = MSD_RESPONSE_FAILURE;
  u8 CSD_Tab[16];

  /* MSD chip select low */
  spi_ss(0);
  /* Send CMD9 (CSD register) or CMD10(CSD register) */
  MSD_SendCmd(MSD_SEND_CSD, 0, 0xFF);

  /* Wait for response in the R1 format (0x00 is no errors) */
  if (!MSD_GetResponse(MSD_RESPONSE_NO_ERROR))
  {
    if (!MSD_GetResponse(MSD_START_DATA_SINGLE_BLOCK_READ))
    {
      for (i = 0; i < 16; i++)
      {
        /* Store CSD register value on CSD_Tab */
        CSD_Tab[i] = spi_send(DUMMY);
      }
    }
    /* Get CRC bytes (not really needed by us, but required by MSD) */
		spi_send(DUMMY);
		spi_send(DUMMY);
    /* Set response value to success */
    rvalue = MSD_RESPONSE_NO_ERROR;
  }

  /* MSD chip select high */
  spi_ss(1);
  /* Send dummy byte: 8 Clock pulses of delay */
  spi_send(DUMMY);
  /* Byte 0 */
  MSD_csd->CSDStruct = (CSD_Tab[0] & 0xC0) >> 6;
  MSD_csd->SysSpecVersion = (CSD_Tab[0] & 0x3C) >> 2;
  MSD_csd->Reserved1 = CSD_Tab[0] & 0x03;
  /* Byte 1 */
  MSD_csd->TAAC = CSD_Tab[1] ;
  /* Byte 2 */
  MSD_csd->NSAC = CSD_Tab[2];
  /* Byte 3 */
  MSD_csd->MaxBusClkFrec = CSD_Tab[3];
  /* Byte 4 */
  MSD_csd->CardComdClasses = CSD_Tab[4] << 4;
  /* Byte 5 */
  MSD_csd->CardComdClasses |= (CSD_Tab[5] & 0xF0) >> 4;
  MSD_csd->RdBlockLen = CSD_Tab[5] & 0x0F;
  /* Byte 6 */
  MSD_csd->PartBlockRead = (CSD_Tab[6] & 0x80) >> 7;
  MSD_csd->WrBlockMisalign = (CSD_Tab[6] & 0x40) >> 6;
  MSD_csd->RdBlockMisalign = (CSD_Tab[6] & 0x20) >> 5;
  MSD_csd->DSRImpl = (CSD_Tab[6] & 0x10) >> 4;
  MSD_csd->Reserved2 = 0; /* Reserved */
  MSD_csd->DeviceSize = (CSD_Tab[6] & 0x03) << 10;
  /* Byte 7 */
  MSD_csd->DeviceSize |= (CSD_Tab[7]) << 2;
  /* Byte 8 */
  MSD_csd->DeviceSize |= (CSD_Tab[8] & 0xC0) >> 6;
  MSD_csd->MaxRdCurrentVDDMin = (CSD_Tab[8] & 0x38) >> 3;
  MSD_csd->MaxRdCurrentVDDMax = (CSD_Tab[8] & 0x07);
  /* Byte 9 */
  MSD_csd->MaxWrCurrentVDDMin = (CSD_Tab[9] & 0xE0) >> 5;
  MSD_csd->MaxWrCurrentVDDMax = (CSD_Tab[9] & 0x1C) >> 2;
  MSD_csd->DeviceSizeMul = (CSD_Tab[9] & 0x03) << 1;
  /* Byte 10 */
  MSD_csd->DeviceSizeMul |= (CSD_Tab[10] & 0x80) >> 7;
  MSD_csd->EraseGrSize = (CSD_Tab[10] & 0x7C) >> 2;
  MSD_csd->EraseGrMul = (CSD_Tab[10] & 0x03) << 3;
  /* Byte 11 */
  MSD_csd->EraseGrMul |= (CSD_Tab[11] & 0xE0) >> 5;
  MSD_csd->WrProtectGrSize = (CSD_Tab[11] & 0x1F);
  /* Byte 12 */
  MSD_csd->WrProtectGrEnable = (CSD_Tab[12] & 0x80) >> 7;
  MSD_csd->ManDeflECC = (CSD_Tab[12] & 0x60) >> 5;
  MSD_csd->WrSpeedFact = (CSD_Tab[12] & 0x1C) >> 2;
  MSD_csd->MaxWrBlockLen = (CSD_Tab[12] & 0x03) << 2;
  /* Byte 13 */
  MSD_csd->MaxWrBlockLen |= (CSD_Tab[13] & 0xc0) >> 6;
  MSD_csd->WriteBlockPaPartial = (CSD_Tab[13] & 0x20) >> 5;
  MSD_csd->Reserved3 = 0;
  MSD_csd->ContentProtectAppli = (CSD_Tab[13] & 0x01);
  /* Byte 14 */
  MSD_csd->FileFormatGrouop = (CSD_Tab[14] & 0x80) >> 7;
  MSD_csd->CopyFlag = (CSD_Tab[14] & 0x40) >> 6;
  MSD_csd->PermWrProtect = (CSD_Tab[14] & 0x20) >> 5;
  MSD_csd->TempWrProtect = (CSD_Tab[14] & 0x10) >> 4;
  MSD_csd->FileFormat = (CSD_Tab[14] & 0x0C) >> 2;
  MSD_csd->ECC = (CSD_Tab[14] & 0x03);
  /* Byte 15 */
  MSD_csd->CRC = (CSD_Tab[15] & 0xFE) >> 1;
  MSD_csd->Reserved4 = 1;

  /* Return the reponse */
  return rvalue;
}

/*******************************************************************************
* Function Name  : MSD_GetCIDRegister
* Description    : Read the CID card register.
*                  Reading the contents of the CID register in SPI mode
*                  is a simple read-block transaction.
* Input          : - MSD_cid: pointer on an CID register structure
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed 
*******************************************************************************/
u8 MSD_GetCIDRegister(sMSD_CID* MSD_cid)
{
  u32 i = 0;
  u8 rvalue = MSD_RESPONSE_FAILURE;
  u8 CID_Tab[16];

  /* MSD chip select low */
  spi_ss(0);
  /* Send CMD10 (CID register) */
  MSD_SendCmd(MSD_SEND_CID, 0, 0xFF);

  /* Wait for response in the R1 format (0x00 is no errors) */
  if (!MSD_GetResponse(MSD_RESPONSE_NO_ERROR))
  {
    if (!MSD_GetResponse(MSD_START_DATA_SINGLE_BLOCK_READ))
    {
      /* Store CID register value on CID_Tab */
      for (i = 0; i < 16; i++)
      {
        CID_Tab[i] = spi_send(DUMMY);
      }
    }
    /* Get CRC bytes (not really needed by us, but required by MSD) */
    spi_send(DUMMY);
    spi_send(DUMMY);
    /* Set response value to success */
    rvalue = MSD_RESPONSE_NO_ERROR;
  }

  /* MSD chip select high */
  spi_ss(1);
  /* Send dummy byte: 8 Clock pulses of delay */
  spi_send(DUMMY);

  /* Byte 0 */
  MSD_cid->ManufacturerID = CID_Tab[0];
  /* Byte 1 */
  MSD_cid->OEM_AppliID = CID_Tab[1] << 8;
  /* Byte 2 */
  MSD_cid->OEM_AppliID |= CID_Tab[2];
  /* Byte 3 */
  MSD_cid->ProdName1 = CID_Tab[3] << 24;
  /* Byte 4 */
  MSD_cid->ProdName1 |= CID_Tab[4] << 16;
  /* Byte 5 */
  MSD_cid->ProdName1 |= CID_Tab[5] << 8;
  /* Byte 6 */
  MSD_cid->ProdName1 |= CID_Tab[6];
  /* Byte 7 */
  MSD_cid->ProdName2 = CID_Tab[7];
  /* Byte 8 */
  MSD_cid->ProdRev = CID_Tab[8];
  /* Byte 9 */
  MSD_cid->ProdSN = CID_Tab[9] << 24;
  /* Byte 10 */
  MSD_cid->ProdSN |= CID_Tab[10] << 16;
  /* Byte 11 */
  MSD_cid->ProdSN |= CID_Tab[11] << 8;
  /* Byte 12 */
  MSD_cid->ProdSN |= CID_Tab[12];
  /* Byte 13 */
  MSD_cid->Reserved1 |= (CID_Tab[13] & 0xF0) >> 4;
  /* Byte 14 */
  MSD_cid->ManufactDate = (CID_Tab[13] & 0x0F) << 8;
  /* Byte 15 */
  MSD_cid->ManufactDate |= CID_Tab[14];
  /* Byte 16 */
  MSD_cid->CRC = (CID_Tab[15] & 0xFE) >> 1;
  MSD_cid->Reserved2 = 1;

  /* Return the reponse */
  return rvalue;
}

/*******************************************************************************
* Function Name  : MSD_SendCmd
* Description    : Send 5 bytes command to the MSD card.
* Input          : - Cmd: the user expected command to send to MSD card
*                  - Arg: the command argument
*                  - Crc: the CRC
* Output         : None
* Return         : None
*******************************************************************************/
void MSD_SendCmd(u8 Cmd, u32 Arg, u8 Crc)
{
  u32 i = 0x00;
  u8 Frame[6];

  /* Construct byte1 */
  Frame[0] = (Cmd | 0x40);
  /* Construct byte2 */
  Frame[1] = (u8)(Arg >> 24);
  /* Construct byte3 */
  Frame[2] = (u8)(Arg >> 16);
  /* Construct byte4 */
  Frame[3] = (u8)(Arg >> 8);
  /* Construct byte5 */
  Frame[4] = (u8)(Arg);
  /* Construct CRC: byte6 */
  Frame[5] = (Crc);

  /* Send the Cmd bytes */
  for (i = 0; i < 6; i++)
  {
		spi_send(Frame[i]);
  }
}

/*******************************************************************************
* Function Name  : MSD_GetDataResponse
* Description    : Get MSD card data response.
* Input          : None
* Output         : None
* Return         : The MSD status: Read data response xxx0<status>1
*                   - status 010: Data accecpted
*                   - status 101: Data rejected due to a crc error
*                   - status 110: Data rejected due to a Write error.
*                   - status 111: Data rejected due to other error.
*******************************************************************************/
u8 MSD_GetDataResponse(void)
{
  u32 i = 0;
  u8 response, rvalue;

  while (i <= 64)
  {
    /* Read resonse */
    response = spi_send(DUMMY);
    /* Mask unused bits */
    response &= 0x1F;

    switch (response)
    {
      case MSD_DATA_OK:
      {
        rvalue = MSD_DATA_OK;
        break;
      }

      case MSD_DATA_CRC_ERROR:
        return MSD_DATA_CRC_ERROR;

      case MSD_DATA_WRITE_ERROR:
        return MSD_DATA_WRITE_ERROR;

      default:
      {
        rvalue = MSD_DATA_OTHER_ERROR;
        break;
      }
    }
    /* Exit loop in case of data ok */
    if (rvalue == MSD_DATA_OK)
      break;
    /* Increment loop counter */
    i++;
  }
  /* Wait null data */
  while ((spi_send(DUMMY)) == 0);
  
  /* Return response */
  return response;
}

/*******************************************************************************
* Function Name  : MSD_GetStatus
* Description    : Returns the MSD status.
* Input          : None
* Output         : None
* Return         : The MSD status.
*******************************************************************************/
u16 MSD_GetStatus(void)
{
  u16 Status = 0;

  /* MSD chip select low */
  spi_ss(0);
  /* Send CMD13 (MSD_SEND_STATUS) to get MSD status */
  MSD_SendCmd(MSD_SEND_STATUS, 0, 0xFF);

  Status = spi_send(DUMMY);
  Status |= (u16)(spi_send(DUMMY) << 8);

  /* MSD chip select high */
  spi_ss(1);
  /* Send dummy byte 0xFF */
  spi_send(DUMMY);

  return Status;
}
/*******************************************************************************
* Function Name  : MSD_GetResponse
* Description    : Returns the MSD response.
* Input          : None
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed 
*******************************************************************************/
u8 MSD_GetResponse(u8 Response)
{
	u32 Count = 0xFFF;
	u8 this_response;

	while (((this_response=spi_send(DUMMY)) != Response) && Count) {
		//printf("Got response: %d\n",this_response);
		Count--;
	}
		
	if (Count == 0) {
		// After time out 
		return MSD_RESPONSE_FAILURE;
	} else {
		// Right response got 
		//printf("Got response: %d\n",Response);
		return MSD_RESPONSE_NO_ERROR;
	}
}
/*******************************************************************************
* Function Name  : MSD_GoIdleState
* Description    : Put MSD in Idle state.
* Input          : None
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed 
*******************************************************************************/
// cris: this is the original Embest version, which works only with
// SD cards that are less than 2GB;

u8 MSD_GoIdleState(void)
{
  /* MSD chip select low */
  spi_ss(0);
  /* Send CMD0 (GO_IDLE_STATE) to put MSD in SPI mode */
  MSD_SendCmd(MSD_GO_IDLE_STATE, 0, 0x95);
	
  /* Wait for In Idle State Response (R1 Format) equal to 0x01 */
  if (MSD_GetResponse(MSD_IN_IDLE_STATE))
  {
    /* No Idle State Response: return response failue */
    return MSD_RESPONSE_FAILURE;
  }
  /*----------Activates the card initialization process-----------*/
  do {
    /* MSD chip select high */
		spi_ss(1);
    /* Send Dummy byte 0xFF */
		spi_send(DUMMY);
    /* MSD chip select low */
		spi_ss(0);
    /* Send CMD1 (Activates the card process) until response equal to 0x0 */
    MSD_SendCmd(MSD_SEND_OP_COND, 0, 0xFF);
    /* Wait for no error Response (R1 Format) equal to 0x00 */
  }
  while (MSD_GetResponse(MSD_RESPONSE_NO_ERROR));
  /* MSD chip select high */
  spi_ss(1);
  /* Send dummy byte 0xFF */
  spi_send(DUMMY);
  return MSD_RESPONSE_NO_ERROR;
}

// cris: this is the version, which I am trying to make to work
// with SDHC (high capacity) cards with size > 2GB;
// I am basically implementing the flow-graph from:
// http://elm-chan.org/docs/mmc/sdinit.png
// and discussed here: http://elm-chan.org/docs/mmc/mmc_e.html
// but it does not work yet; I tried it with two different 8GB cards; 
// it needs more investigation;

// u8 MSD_GoIdleState(void)
// {
// 		u32 Count_timeout = COUNTER_TIMEOUT;
// 		
// 		// (1) send CMD0	
// 		spi_ss(0); // MSD chip select low 
// 		// Send CMD0 (GO_IDLE_STATE) to put MSD in SPI mode 
// 		MSD_SendCmd(MSD_GO_IDLE_STATE, 0, 0x95);
// 		// Wait for In Idle State Response (R1 Format) equal to 0x01 
// 		if (MSD_GetResponse(MSD_IN_IDLE_STATE) == MSD_RESPONSE_FAILURE) {
// 				printf("Error in MSD_GoIdleState(): CMD0 rejected\n");
// 				return MSD_RESPONSE_FAILURE;
// 		} else {
// 				printf("MSD_GoIdleState(): CMD0 accepted\n");
// 		}
// 		
// 		
// 		//----------Activates the card initialization process-----------

// 		// (2) try CMD8 until:
// 		// --the card accepts the command and returns Response (R3 format) 
// 		// equal to 0x01
// 		// --the card rejects the command and returns illegal 
// 		// command error (0x05)
// 		// --the card rejects the command with failure
// 		
// 		spi_ss(1); // MSD chip select high 	
// 		spi_send(DUMMY); // Send Dummy byte 0xFF 	
// 		spi_ss(0); // MSD chip select low 	
// 		MSD_SendCmd(MSD_SEND_IF_COND, 0x000001AA, 0x87); // Send CMD8 


// 		// Wait for In Idle State Response (R7 Format) equal to 0x01 
// 		// and trailing 0x1AA
//  		if (MSD_GetResponse(MSD_IN_IDLE_STATE) == MSD_RESPONSE_FAILURE) {
// 				// (A) this means that 0x01 is not received
// 				printf("Error in MSD_GoIdleState(): CMD8 is illegal\n");
// 				// ACMD41
// 				Count_timeout = COUNTER_TIMEOUT;
// 				do {
// 					  spi_ss(1); // MSD chip select high					 
// 						spi_send(DUMMY); // Send Dummy byte 0xFF						
// 						spi_ss(0); // MSD chip select low 					 
// 						MSD_SendCmd(MSD_APP_CMD, 0, 0x65); // send CMD55, CRC=0x65					
// 						spi_ss(1); // MSD chip select high 						 
// 						spi_send(DUMMY); // Send Dummy byte 0xFF					 
// 						spi_ss(0); // MSD chip select low					 
// 						MSD_SendCmd(MSD_APP_SEND_OP_COND, 0x40000000, 0xFF); // Send CMD41							
// 						// Wait for no error Response (R1 Format) equal to 0x00 
// 						received_byte = MSD_GetResponse(MSD_RESPONSE_NO_ERROR);
// 						if (received_byte == MSD_RESPONSE_FAILURE) Count_timeout--;
// 						printf("1 received: %d\n",received_byte);
// 				}
// 				while(received_byte == MSD_RESPONSE_FAILURE && Count_timeout > 0);
// 				
// 				if (received_byte == MSD_RESPONSE_NO_ERROR) {
// 						printf("MSD_GoIdleState(): SD Ver.1\n");
// 						// this is SD Ver.1
// 						// () MSD chip select high 
// 						spi_ss(1);
// 						// Send dummy byte 0xFF 
// 						spi_send(DUMMY);	
// 						return MSD_RESPONSE_NO_ERROR;					
// 				}
// 				// else means timeout or received Error
// 				printf("MSD_GoIdleState(): ACMD41 timeout\n");
// 				// CMD1 until the card accepts the command and returns
// 				// Response (R1 format) equal to 0x00	
// 				Count_timeout = COUNTER_TIMEOUT;
// 				do {
// 					  spi_ss(1); // MSD chip select high					 
// 						spi_send(DUMMY); // Send Dummy byte 0xFF						
// 						spi_ss(0); // MSD chip select low 										  
// 						MSD_SendCmd(MSD_SEND_OP_COND, 0x40000000, 0xFF); // send CMD1
// 						// Wait for no error Response (R1 Format) equal to 0x00 
// 						received_byte = MSD_GetResponse(MSD_RESPONSE_NO_ERROR);
// 						if (received_byte == MSD_RESPONSE_FAILURE) Count_timeout--;
// 						printf("2 received: %d\n",received_byte);
// 				}
// 				while(received_byte == MSD_RESPONSE_FAILURE && Count_timeout > 0);

// 				if (received_byte == MSD_RESPONSE_NO_ERROR) {
// 						printf("MSD_GoIdleState(): MMC Ver.3\n");
// 						// this is MMC Ver.3
// 					  spi_ss(1); // MSD chip select high					 
// 						spi_send(DUMMY); // Send Dummy byte 0xFF					
// 						return MSD_RESPONSE_NO_ERROR;					
// 				}	
// 				// else means timeout or Unknown card
// 				printf("MSD_GoIdleState(): CMD1 timeout\n");
// 				return MSD_RESPONSE_FAILURE;
// 								
//  		} 
//  		else 
// 		{
// 				// (B) this means that 0x01 is received
// 				printf("MSD_GoIdleState(): CMD8 accepted\n");
// 				// ACMD41
// 				Count_timeout = COUNTER_TIMEOUT;
// 				do {				 
// 						spi_ss(1); // MSD chip select high					 
// 						spi_send(DUMMY); // Send Dummy byte 0xFF						
// 						spi_ss(0); // MSD chip select low 					 
// 						MSD_SendCmd(MSD_APP_CMD, 0, 0x65); // send CMD55, CRC=0x65
// 						if (MSD_GetResponse(MSD_IN_IDLE_STATE) == MSD_RESPONSE_FAILURE) {
// 								printf("Error in MSD_GoIdleState(): CMD55 rejected\n");
// 								return MSD_RESPONSE_FAILURE;
// 						} else {
// 								printf("MSD_GoIdleState(): CMD55 accepted\n");
// 						}					
// 						spi_ss(1); // MSD chip select high 						 
// 						spi_send(DUMMY); // Send Dummy byte 0xFF					 
// 						spi_ss(0); // MSD chip select low					 
// 						MSD_SendCmd(MSD_APP_SEND_OP_COND, 0x40000000, 0xE5); // send CMD41						
// 						// Wait for no error Response (R1 Format) equal to 0x00 
// 						received_byte = MSD_GetResponse(MSD_RESPONSE_NO_ERROR);
// 						if (received_byte == MSD_RESPONSE_FAILURE) Count_timeout--;
// 						printf("2 received: %d\n",received_byte);
// 				}
// 				while(received_byte == MSD_RESPONSE_FAILURE && Count_timeout > 0);

// 				if (received_byte == MSD_RESPONSE_NO_ERROR) {
// 						printf("MSD_GoIdleState(): SD Ver.2\n");
// 						// this is SD Ver.2
// 						// () MSD chip select high 
// 						spi_ss(1);
// 						// Send dummy byte 0xFF 
// 						spi_send(DUMMY);	
// 						return MSD_RESPONSE_NO_ERROR;					
// 				}	
// 				// else means timeout or Unknown card
// 				printf("MSD_GoIdleState(): ACMD41 timeout\n");
// 				return MSD_RESPONSE_FAILURE;
// 				
// 		}

// }

/*******************************************************************************
* Function Name  : MSD_WriteByte
* Description    : Write a byte on the MSD.
* Input          : Data: byte to send.
* Output         : None
* Return         : None.
*******************************************************************************/
void MSD_WriteByte(u8 Data)
{
  spi_send(Data);	 
}

/*******************************************************************************
* Function Name  : MSD_ReadByte
* Description    : Read a byte from the MSD.
* Input          : None.
* Output         : None
* Return         : The received byte.
*******************************************************************************/
u8 MSD_ReadByte(void)
{
  return spi_send(DUMMY);
}

/*******************************************************************************
* Function Name  : SPI_Config
* Description    : Initializes the SPI1 and CS pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Config(u16 BaudRatePrescaler)
{
  spi_init();
}

/*******************************************************************************
* Function Name  : Get_Medium_Characteristics.
* Description    : Get the microSD card size.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
// read discussion from the supplemental lab material for lab#5 for more info;
void Get_Medium_Characteristics(void)
{
  u32 temp1 = 0;
  u32 temp2 = 0;

  MSD_GetCSDRegister(&MSD_csd);

  temp1 = MSD_csd.DeviceSize + 1;
  temp2 = 1 << (MSD_csd.DeviceSizeMul + 2);

  Mass_Block_Count = temp1 * temp2;

  Mass_Block_Size =  1 << MSD_csd.RdBlockLen;

  Mass_Memory_Size = (Mass_Block_Count * Mass_Block_Size);

}

/**********************************END OF FILE**********************************/
