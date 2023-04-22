//-----------------------------------------------------------------------------
//
// Steve Tchuinte:
// this is a somewhat cleaned up version of the original GLCD.c file
// that comes inside the Example/GLCD.rar archive on the DVD shipped
// with the LandTiger 2.0 board;
// cleaning consists mainly in commenting out portions that turned
// out to be unecessary for my particular board instance; initial version
// would not be linked by the MDK lite edition...
//
// for my own reference, here is what the header of the original file contained:
//
//                                 http://www.powermcu.com
// File name:      GLCD.c
// Descriptions:  Has been tested SSD1289¡ILI9320¡R61505U¡SSD1298¡ST7781¡SPFD5408B¡ILI9325¡ILI9328¡
//                HX8346A¡HX8347A
// Created by:    AVRman
// Created date:  2012-3-10
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes 
#include <stdint.h>
#include "lpc17xx.h"
#include "GLCD1.h" 
#include "AsciiLib.h"

//-----------------------------------------------------------------------------
// Private variables 
static uint8_t LCD_Code;

//-----------------------------------------------------------------------------
// Private define 
#define  ILI9320    0  // 0x9320 
#define  ILI9325    1  // 0x9325 
#define  ILI9328    2  // 0x9328 
#define  ILI9331    3  // 0x9331 
#define  SSD1298    4  // 0x8999 
#define  SSD1289    5  // 0x8989 
#define  ST7781    6  // 0x7783 
#define  LGDP4531  7  // 0x4531 
#define  SPFD5408B  8  // 0x5408 
#define  R61505U    9  // 0x1505 0x0505 
#define  HX8346A    10 // 0x0046 
#define  HX8347D    11 // 0x0047 
#define  HX8347A    12 // 0x0047 
#define  LGDP4535  13 // 0x4535 
#define  SSD2119    14 // 3.5 LCD 0x9919


//-----------------------------------------------------------------------------
// Function Name  : Lcd_Configuration
// Description    : Configures LCD Control lines
static void LCD_Configuration(void)
{
  // Configure the LCD Control pins
  
  // EN = P0.19 , LE = P0.20 , DIR = P0.21 , CS = P0.22 , RS = P0.23 , RS = P0.23 
  // RS = P0.23 , WR = P0.24 , RD = P0.25 , DB[0.7] = P2.0...P2.7 , DB[8.15]= P2.0...P2.7  
  LPC_GPIO0->FIODIR   |= 0x03f80000;
  LPC_GPIO0->FIOSET    = 0x03f80000;
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_Send
// Description    : 
// Input          : - byte: byte to be sent
static __attribute__((always_inline)) void LCD_Send (uint16_t byte) 
{
  LPC_GPIO2->FIODIR |= 0xFF;       // P2.0...P2.7 Output 
  LCD_DIR(1)                        // Interface A->B 
  LCD_EN(0)                         // Enable 2A->2B 
  LPC_GPIO2->FIOPIN = byte;         // Write D0..D7 
  LCD_LE(1)                         
  LCD_LE(0)                         // latch D0..D7  
  LPC_GPIO2->FIOPIN = byte >> 8;   // Write D8..D15 
}


//-----------------------------------------------------------------------------
// Function Name  : wait_delay
// Description    : Delay Time
// Input          : - nCount: Delay Time
static void wait_delay(int count)
{
  while(count--);
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_Read
// Description    : 
// Input          : - byte: byte to be read
static __attribute__((always_inline)) uint16_t LCD_Read (void) 
{
  uint16_t value;
  
  LPC_GPIO2->FIODIR &= ~(0xFF);               // P2.0...P2.7 Input 
  LCD_DIR(0);                                  // Interface B->A 
  LCD_EN(0);                                 // Enable 2B->2A 
  wait_delay(30);                             // delay some times 
  value = LPC_GPIO2->FIOPIN0;                 // Read D8..D15 
  LCD_EN(1);                                 // Enable 1B->1A 
  wait_delay(30);                             // delay some times 
  value = (value << 8) | LPC_GPIO2->FIOPIN0; // Read D0..D7 
  LCD_DIR(1);
  return  value;
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_WriteIndex
// Description    : 
// Input          : - index:
static __attribute__((always_inline)) void LCD_WriteIndex(uint16_t index)
{
  LCD_CS(0);
  LCD_RS(0);
  LCD_RD(1);
  LCD_Send( index ); 
  wait_delay(25);  
  LCD_WR(0);  
  wait_delay(1);
  LCD_WR(1);
  LCD_CS(1);
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_WriteData
// Description    : 
// Input          : - index: 
static __attribute__((always_inline)) void LCD_WriteData(uint16_t data)
{        
  LCD_CS(0);
  LCD_RS(1);   
  LCD_Send( data );
  LCD_WR(0);     
  wait_delay(1);
  LCD_WR(1);
  LCD_CS(1);
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_ReadData
static __attribute__((always_inline)) uint16_t LCD_ReadData(void)
{ 
  uint16_t value;
  
  LCD_CS(0);
  LCD_RS(1);
  LCD_WR(1);
  LCD_RD(0);
  value = LCD_Read();
  LCD_RD(1);
  LCD_CS(1);
  
  return value;
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_WriteReg
// Description    : Writes to the selected LCD register.
// Input          : - LCD_Reg: address of the selected register.
//                  - LCD_RegValue: value to write to the selected register.
static __attribute__((always_inline)) void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{ 
  // Write 16-bit Index, then Write Reg   
  LCD_WriteIndex(LCD_Reg);         
  // Write 16-bit Reg 
  LCD_WriteData(LCD_RegValue);  
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_WriteReg
// Description    : Reads the selected LCD Register.
// Return         : LCD Register Value.
static __attribute__((always_inline)) uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
  uint16_t LCD_RAM;
  
  // Write 16-bit Index (then Read Reg) 
  LCD_WriteIndex(LCD_Reg);
  // Read 16-bit Reg 
  LCD_RAM = LCD_ReadData();        
  return LCD_RAM;
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_SetCursor
// Description    : Sets the cursor position.
// Input          : - Xpos: specifies the X position.
//                  - Ypos: specifies the Y position. 
static void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
  #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
    uint16_t temp = Xpos;
    Xpos = Ypos;
    Ypos = ( MAX_X - 1 ) - temp;  
  #elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )  
  #endif

  switch( LCD_Code )
  {
     default: // 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 
       LCD_WriteReg(0x0020, Xpos );     
       LCD_WriteReg(0x0021, Ypos );     
       break; 
     case SSD1298: // 0x8999       
     case SSD1289: // 0x8989 
       LCD_WriteReg(0x004e, Xpos );      
       LCD_WriteReg(0x004f, Ypos );          
       break;  
     case HX8346A:   // 0x0046 
     case HX8347A:   // 0x0047 
     case HX8347D:   // 0x0047 
       LCD_WriteReg(0x02, Xpos>>8 );                                                  
       LCD_WriteReg(0x03, Xpos );  
       LCD_WriteReg(0x06, Ypos>>8 );                           
       LCD_WriteReg(0x07, Ypos );    
       break;     
     case SSD2119: // 3.5 LCD 0x9919
       break; 
  }
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_Delay
// Description    : Delay Time
// Input          : - nCount: Delay Time
static void delay_ms(uint16_t ms)    
{ 
  uint16_t i,j; 
  for( i = 0; i < ms; i++ )
  { 
    for( j = 0; j < 1141; j++ );
  }
} 


//-----------------------------------------------------------------------------
// Function Name  : LCD_Initialization
// Description    : Initialize TFT Controller
void LCD_Initialization(void)
{
  uint16_t DeviceCode;
  
  LCD_Configuration();
  delay_ms(100);
  DeviceCode = LCD_ReadReg(0x0000);  

  // Note: Here, I commented out what appeared, thru trial and error, to be
  // unnecessary; needed to do this to get this whole thing linkable with
  // the lite edition of uVision;
  // if it does not work for you, uncomment out the "if" that works for you;
  /*---
  if( DeviceCode == 0x9325 || DeviceCode == 0x9328 )  
  {
    LCD_Code = ILI9325;
    LCD_WriteReg(0x00e7,0x0010);      
    LCD_WriteReg(0x0000,0x0001); // start internal osc
    LCD_WriteReg(0x0001,0x0100);     
    LCD_WriteReg(0x0002,0x0700); // power on sequence
    LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) ); // importance
    LCD_WriteReg(0x0004,0x0000);                                   
    LCD_WriteReg(0x0008,0x0207);             
    LCD_WriteReg(0x0009,0x0000);         
    LCD_WriteReg(0x000a,0x0000);   //display setting        
    LCD_WriteReg(0x000c,0x0001);  //display setting        
    LCD_WriteReg(0x000d,0x0000);               
    LCD_WriteReg(0x000f,0x0000);
    // Power On sequence
    LCD_WriteReg(0x0010,0x0000);   
    LCD_WriteReg(0x0011,0x0007);
    LCD_WriteReg(0x0012,0x0000);                                                                 
    LCD_WriteReg(0x0013,0x0000);                 
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0010,0x1590);   
    LCD_WriteReg(0x0011,0x0227);
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0012,0x009c);                  
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0013,0x1900);   
    LCD_WriteReg(0x0029,0x0023);
    LCD_WriteReg(0x002b,0x000e);
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0020,0x0000);                                                            
    LCD_WriteReg(0x0021,0x0000);           
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0030,0x0007); 
    LCD_WriteReg(0x0031,0x0707);   
    LCD_WriteReg(0x0032,0x0006);
    LCD_WriteReg(0x0035,0x0704);
    LCD_WriteReg(0x0036,0x1f04); 
    LCD_WriteReg(0x0037,0x0004);
    LCD_WriteReg(0x0038,0x0000);        
    LCD_WriteReg(0x0039,0x0706);     
    LCD_WriteReg(0x003c,0x0701);
    LCD_WriteReg(0x003d,0x000f);
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0050,0x0000);        
    LCD_WriteReg(0x0051,0x00ef);   
    LCD_WriteReg(0x0052,0x0000);     
    LCD_WriteReg(0x0053,0x013f);
    LCD_WriteReg(0x0060,0xa700);        
    LCD_WriteReg(0x0061,0x0001); 
    LCD_WriteReg(0x006a,0x0000);
    LCD_WriteReg(0x0080,0x0000);
    LCD_WriteReg(0x0081,0x0000);
    LCD_WriteReg(0x0082,0x0000);
    LCD_WriteReg(0x0083,0x0000);
    LCD_WriteReg(0x0084,0x0000);
    LCD_WriteReg(0x0085,0x0000);
      
    LCD_WriteReg(0x0090,0x0010);     
    LCD_WriteReg(0x0092,0x0000);  
    LCD_WriteReg(0x0093,0x0003);
    LCD_WriteReg(0x0095,0x0110);
    LCD_WriteReg(0x0097,0x0000);        
    LCD_WriteReg(0x0098,0x0000);  
    //display on sequence    
    LCD_WriteReg(0x0007,0x0133);
    
    LCD_WriteReg(0x0020,0x0000);                                                         
    LCD_WriteReg(0x0021,0x0000);    
  }
  ---*/
  /*---
  if( DeviceCode == 0x9320 || DeviceCode == 0x9300 )
  {
    LCD_Code = ILI9320;
    LCD_WriteReg(0x00,0x0000);
    LCD_WriteReg(0x01,0x0100);  //Driver Output Contral
    LCD_WriteReg(0x02,0x0700);  //LCD Driver Waveform Contral
    LCD_WriteReg(0x03,0x1018);  //Entry Mode Set
    
    LCD_WriteReg(0x04,0x0000);  //Scalling Contral
     LCD_WriteReg(0x08,0x0202);  //Display Contral
    LCD_WriteReg(0x09,0x0000);  //Display Contral 3.(0x0000)
    LCD_WriteReg(0x0a,0x0000);  //Frame Cycle Contal.(0x0000)
    LCD_WriteReg(0x0c,(1<<0));  //Extern Display Interface Contral
    LCD_WriteReg(0x0d,0x0000);  //Frame Maker Position
    LCD_WriteReg(0x0f,0x0000);  //Extern Display Interface Contral 2.
    
    delay_ms(100);  //delay 100 ms    
    LCD_WriteReg(0x07,0x0101);  //Display Contral
    delay_ms(100);  //delay 100 ms    
  
    LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));  //Power Control 1.(0x16b0)  
    LCD_WriteReg(0x11,0x0007);                //Power Control 2
    LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));  //Power Control 3.(0x0138)  
    LCD_WriteReg(0x13,0x0b00);                //Power Control 4
    LCD_WriteReg(0x29,0x0000);                //Power Control 7
    
    LCD_WriteReg(0x2b,(1<<14)|(1<<4));
      
    LCD_WriteReg(0x50,0);       //Set X Start
    LCD_WriteReg(0x51,239);      //Set X End
    LCD_WriteReg(0x52,0);        //Set Y Start
    LCD_WriteReg(0x53,319);      //Set Y End
    
    LCD_WriteReg(0x60,0x2700);  //Driver Output Control
    LCD_WriteReg(0x61,0x0001);  //Driver Output Control
    LCD_WriteReg(0x6a,0x0000);  //Vertical Srcoll Control
    
    LCD_WriteReg(0x80,0x0000);  //Display Position? Partial Display 1
    LCD_WriteReg(0x81,0x0000);  //RAM Address Start? Partial Display 1
    LCD_WriteReg(0x82,0x0000);  //RAM Address End-Partial Display 1
    LCD_WriteReg(0x83,0x0000);  //Displsy Position? Partial Display 2
    LCD_WriteReg(0x84,0x0000);  //RAM Address Start? Partial Display 2
    LCD_WriteReg(0x85,0x0000);  //RAM Address End? Partial Display 2
    
    LCD_WriteReg(0x90,(0<<7)|(16<<0));  //Frame Cycle Contral.(0x0013)  
    LCD_WriteReg(0x92,0x0000);  //Panel Interface Contral 2.(0x0000)
    LCD_WriteReg(0x93,0x0001);  //Panel Interface Contral 3.
    LCD_WriteReg(0x95,0x0110);  //Frame Cycle Contral.(0x0110)  
    LCD_WriteReg(0x97,(0<<8));  
    LCD_WriteReg(0x98,0x0000);  //Frame Cycle Contral
  
    LCD_WriteReg(0x07,0x0173);
  }
  ---*/
  /*---
  if( DeviceCode == 0x9331 )
  {
    LCD_Code = ILI9331;
    LCD_WriteReg(0x00E7, 0x1014);
    LCD_WriteReg(0x0001, 0x0100);   //set SS and SM bit
    LCD_WriteReg(0x0002, 0x0200);   //set 1 line inversion
    LCD_WriteReg(0x0003, 0x1030);   //set GRAM write direction and BGR=1
    LCD_WriteReg(0x0008, 0x0202);   //set the back porch and front porch
    LCD_WriteReg(0x0009, 0x0000);   //set non-display area refresh cycle ISC[3:0]
    LCD_WriteReg(0x000A, 0x0000);   //FMARK function
    LCD_WriteReg(0x000C, 0x0000);   //RGB interface setting
    LCD_WriteReg(0x000D, 0x0000);   //Frame marker Position
    LCD_WriteReg(0x000F, 0x0000);   //RGB interface polarity
    //Power On sequence
    LCD_WriteReg(0x0010, 0x0000);   //SAP, BT[3:0], AP, DSTB, SLP, STB  
    LCD_WriteReg(0x0011, 0x0007);   //DC1[2:0], DC0[2:0], VC[2:0]
    LCD_WriteReg(0x0012, 0x0000);   //VREG1OUT voltage  
    LCD_WriteReg(0x0013, 0x0000);   //VDV[4:0] for VCOM amplitude
    delay_ms(200);  //delay 200 ms    
    LCD_WriteReg(0x0010, 0x1690);   //SAP, BT[3:0], AP, DSTB, SLP, STB  
    LCD_WriteReg(0x0011, 0x0227);   //DC1[2:0], DC0[2:0], VC[2:0]
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0012, 0x000C);   //Internal reference voltage= Vci  
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0013, 0x0800);   //Set VDV[4:0] for VCOM amplitude
    LCD_WriteReg(0x0029, 0x0011);   //Set VCM[5:0] for VCOMH
    LCD_WriteReg(0x002B, 0x000B);   //Set Frame Rate
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0020, 0x0000);   //GRAM horizontal Address
    LCD_WriteReg(0x0021, 0x0000);   //GRAM Vertical Address
    //Adjust the Gamma Curve
    LCD_WriteReg(0x0030, 0x0000);
    LCD_WriteReg(0x0031, 0x0106);
    LCD_WriteReg(0x0032, 0x0000);
    LCD_WriteReg(0x0035, 0x0204);
    LCD_WriteReg(0x0036, 0x160A);
    LCD_WriteReg(0x0037, 0x0707);
    LCD_WriteReg(0x0038, 0x0106);
    LCD_WriteReg(0x0039, 0x0707);
    LCD_WriteReg(0x003C, 0x0402);
    LCD_WriteReg(0x003D, 0x0C0F);
    //Set GRAM area
    LCD_WriteReg(0x0050, 0x0000);   //Horizontal GRAM Start Address
    LCD_WriteReg(0x0051, 0x00EF);   //Horizontal GRAM End Address
    LCD_WriteReg(0x0052, 0x0000);   //Vertical GRAM Start Address
    LCD_WriteReg(0x0053, 0x013F);   //Vertical GRAM Start Address
    LCD_WriteReg(0x0060, 0x2700);   //Gate Scan Line
    LCD_WriteReg(0x0061, 0x0001);   // NDL,VLE, REV
    LCD_WriteReg(0x006A, 0x0000);   //set scrolling line
    //Partial Display Control
    LCD_WriteReg(0x0080, 0x0000);
    LCD_WriteReg(0x0081, 0x0000);
    LCD_WriteReg(0x0082, 0x0000);
    LCD_WriteReg(0x0083, 0x0000);
    LCD_WriteReg(0x0084, 0x0000);
    LCD_WriteReg(0x0085, 0x0000);
    //Panel Control
    LCD_WriteReg(0x0090, 0x0010);
    LCD_WriteReg(0x0092, 0x0600);
    LCD_WriteReg(0x0007,0x0021);    
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0007,0x0061);
    delay_ms(50);  //delay 50 ms    
    LCD_WriteReg(0x0007,0x0133);    //262K color and display ON
  }
  ---*/
  /*---
  if( DeviceCode == 0x9919 )
  {
    LCD_Code = SSD2119;
    //POWER ON &RESET DISPLAY OFF
    LCD_WriteReg(0x28,0x0006);
    LCD_WriteReg(0x00,0x0001);    
    LCD_WriteReg(0x10,0x0000);    
    LCD_WriteReg(0x01,0x72ef);
    LCD_WriteReg(0x02,0x0600);
    LCD_WriteReg(0x03,0x6a38);  
    LCD_WriteReg(0x11,0x6874);
    LCD_WriteReg(0x0f,0x0000);    //RAM WRITE DATA MASK
    LCD_WriteReg(0x0b,0x5308);    //RAM WRITE DATA MASK
    LCD_WriteReg(0x0c,0x0003);
    LCD_WriteReg(0x0d,0x000a);
    LCD_WriteReg(0x0e,0x2e00);  
    LCD_WriteReg(0x1e,0x00be);
    LCD_WriteReg(0x25,0x8000);
    LCD_WriteReg(0x26,0x7800);
    LCD_WriteReg(0x27,0x0078);
    LCD_WriteReg(0x4e,0x0000);
    LCD_WriteReg(0x4f,0x0000);
    LCD_WriteReg(0x12,0x08d9);
    //Adjust the Gamma Curve
    LCD_WriteReg(0x30,0x0000);
    LCD_WriteReg(0x31,0x0104);   
    LCD_WriteReg(0x32,0x0100);  
    LCD_WriteReg(0x33,0x0305);  
    LCD_WriteReg(0x34,0x0505);   
    LCD_WriteReg(0x35,0x0305);  
    LCD_WriteReg(0x36,0x0707);  
    LCD_WriteReg(0x37,0x0300);  
    LCD_WriteReg(0x3a,0x1200);  
    LCD_WriteReg(0x3b,0x0800);     
    LCD_WriteReg(0x07,0x0033);
  }
  ---*/
  /*---
  if( DeviceCode == 0x1505 || DeviceCode == 0x0505 )
  {
    LCD_Code = R61505U;
    //initializing funciton   
    LCD_WriteReg(0xe5,0x8000);  //Set the internal vcore voltage   
    LCD_WriteReg(0x00,0x0001);  //start OSC   
    LCD_WriteReg(0x2b,0x0010);  //Set the frame rate as 80 when the internal resistor is used for oscillator circuit   
    LCD_WriteReg(0x01,0x0100);  //s720  to  s1 ; G1 to G320   
    LCD_WriteReg(0x02,0x0700);  //set the line inversion   
    LCD_WriteReg(0x03,0x1018);  //65536 colors    
    LCD_WriteReg(0x04,0x0000);   
    LCD_WriteReg(0x08,0x0202);  //specify the line number of front and back porch periods respectively   
    LCD_WriteReg(0x09,0x0000);   
    LCD_WriteReg(0x0a,0x0000);   
    LCD_WriteReg(0x0c,0x0000);  //select  internal system clock  
    LCD_WriteReg(0x0d,0x0000);   
    LCD_WriteReg(0x0f,0x0000);    
    LCD_WriteReg(0x50,0x0000);  //set windows adress   
    LCD_WriteReg(0x51,0x00ef);   
    LCD_WriteReg(0x52,0x0000);   
    LCD_WriteReg(0x53,0x013f);   
    LCD_WriteReg(0x60,0x2700);   
    LCD_WriteReg(0x61,0x0001);   
    LCD_WriteReg(0x6a,0x0000);   
    LCD_WriteReg(0x80,0x0000);   
    LCD_WriteReg(0x81,0x0000);   
    LCD_WriteReg(0x82,0x0000);   
    LCD_WriteReg(0x83,0x0000);   
    LCD_WriteReg(0x84,0x0000);   
    LCD_WriteReg(0x85,0x0000);   
    LCD_WriteReg(0x90,0x0010);   
    LCD_WriteReg(0x92,0x0000);   
    LCD_WriteReg(0x93,0x0003);   
    LCD_WriteReg(0x95,0x0110);   
    LCD_WriteReg(0x97,0x0000);   
    LCD_WriteReg(0x98,0x0000);    
    //power setting function   
    LCD_WriteReg(0x10,0x0000);   
    LCD_WriteReg(0x11,0x0000);   
    LCD_WriteReg(0x12,0x0000);   
    LCD_WriteReg(0x13,0x0000);   
    delay_ms(100);   
    LCD_WriteReg(0x10,0x17b0);   
    LCD_WriteReg(0x11,0x0004);   
    delay_ms(50);   
    LCD_WriteReg(0x12,0x013e);   
    delay_ms(50);   
    LCD_WriteReg(0x13,0x1f00);   
    LCD_WriteReg(0x29,0x000f);   
    delay_ms(50);   
    LCD_WriteReg(0x20,0x0000);   
    LCD_WriteReg(0x21,0x0000);   
    
    //initializing function    
    LCD_WriteReg(0x30,0x0204);   
    LCD_WriteReg(0x31,0x0001);   
    LCD_WriteReg(0x32,0x0000);   
    LCD_WriteReg(0x35,0x0206);   
    LCD_WriteReg(0x36,0x0600);   
    LCD_WriteReg(0x37,0x0500);   
    LCD_WriteReg(0x38,0x0505);   
    LCD_WriteReg(0x39,0x0407);   
    LCD_WriteReg(0x3c,0x0500);   
    LCD_WriteReg(0x3d,0x0503);   
    
    //display on  
    LCD_WriteReg(0x07,0x0173);  
  }  
  ---*/
  if( DeviceCode == 0x8989 )
  {
      LCD_Code = SSD1289;
      LCD_WriteReg(0x0000,0x0001);    delay_ms(50); 
      LCD_WriteReg(0x0003,0xA8A4);    delay_ms(50);   
      LCD_WriteReg(0x000C,0x0000);    delay_ms(50);   
      LCD_WriteReg(0x000D,0x080C);    delay_ms(50);   
      LCD_WriteReg(0x000E,0x2B00);    delay_ms(50);   
      LCD_WriteReg(0x001E,0x00B0);    delay_ms(50);   
      LCD_WriteReg(0x0001,0x2B3F);    delay_ms(50);   //320*240 0x2B3F
      LCD_WriteReg(0x0002,0x0600);    delay_ms(50);
      LCD_WriteReg(0x0010,0x0000);    delay_ms(50);
      LCD_WriteReg(0x0011,0x6070);    delay_ms(50);   //0x6070
      LCD_WriteReg(0x0005,0x0000);    delay_ms(50);
      LCD_WriteReg(0x0006,0x0000);    delay_ms(50);
      LCD_WriteReg(0x0016,0xEF1C);    delay_ms(50);
      LCD_WriteReg(0x0017,0x0003);    delay_ms(50);
      LCD_WriteReg(0x0007,0x0133);    delay_ms(50);         
      LCD_WriteReg(0x000B,0x0000);    delay_ms(50);
      LCD_WriteReg(0x000F,0x0000);    delay_ms(50);   
      LCD_WriteReg(0x0041,0x0000);    delay_ms(50);
      LCD_WriteReg(0x0042,0x0000);    delay_ms(50);
      LCD_WriteReg(0x0048,0x0000);    delay_ms(50);
      LCD_WriteReg(0x0049,0x013F);    delay_ms(50);
      LCD_WriteReg(0x004A,0x0000);    delay_ms(50);
      LCD_WriteReg(0x004B,0x0000);    delay_ms(50);
      LCD_WriteReg(0x0044,0xEF00);    delay_ms(50);
      LCD_WriteReg(0x0045,0x0000);    delay_ms(50);
      LCD_WriteReg(0x0046,0x013F);    delay_ms(50);
      LCD_WriteReg(0x0030,0x0707);    delay_ms(50);
      LCD_WriteReg(0x0031,0x0204);    delay_ms(50);
      LCD_WriteReg(0x0032,0x0204);    delay_ms(50);
      LCD_WriteReg(0x0033,0x0502);    delay_ms(50);
      LCD_WriteReg(0x0034,0x0507);    delay_ms(50);
      LCD_WriteReg(0x0035,0x0204);    delay_ms(50);
      LCD_WriteReg(0x0036,0x0204);    delay_ms(50);
      LCD_WriteReg(0x0037,0x0502);    delay_ms(50);
      LCD_WriteReg(0x003A,0x0302);    delay_ms(50);
      LCD_WriteReg(0x003B,0x0302);    delay_ms(50);
      LCD_WriteReg(0x0023,0x0000);    delay_ms(50);
      LCD_WriteReg(0x0024,0x0000);    delay_ms(50);
      LCD_WriteReg(0x0025,0x8000);    delay_ms(50);
      LCD_WriteReg(0x004f,0);     
      LCD_WriteReg(0x004e,0);      
  }
  /*---
  if( DeviceCode == 0x8999 )
  {
    LCD_Code = SSD1298;    
    LCD_WriteReg(0x0028,0x0006);
    LCD_WriteReg(0x0000,0x0001);
    LCD_WriteReg(0x0003,0xaea4);    //power control 1---line frequency and VHG,VGL voltage
    LCD_WriteReg(0x000c,0x0004);    //power control 2---VCIX2 output voltage
    LCD_WriteReg(0x000d,0x000c);    //power control 3---Vlcd63 voltage
    LCD_WriteReg(0x000e,0x2800);    //power control 4---VCOMA voltage VCOML=VCOMH*0.9475-VCOMA
    LCD_WriteReg(0x001e,0x00b5);    //POWER CONTROL 5---VCOMH voltage   
    LCD_WriteReg(0x0001,0x3b3f);     
    LCD_WriteReg(0x0002,0x0600);
    LCD_WriteReg(0x0010,0x0000);
    LCD_WriteReg(0x0011,0x6830);
    LCD_WriteReg(0x0005,0x0000);
    LCD_WriteReg(0x0006,0x0000);
    LCD_WriteReg(0x0016,0xef1c);  
    LCD_WriteReg(0x0007,0x0033);    //Display control 1
    //when GON=1 and DTE=0,all gate outputs become VGL
    //when GON=1 and DTE=0,all gate outputs become VGH
    //non-selected gate wires become VGL     
    LCD_WriteReg(0x000b,0x0000);
    LCD_WriteReg(0x000f,0x0000);
    LCD_WriteReg(0x0041,0x0000);
    LCD_WriteReg(0x0042,0x0000);
    LCD_WriteReg(0x0048,0x0000);
    LCD_WriteReg(0x0049,0x013f);
    LCD_WriteReg(0x004a,0x0000);
    LCD_WriteReg(0x004b,0x0000); 
    LCD_WriteReg(0x0044,0xef00);  //Horizontal RAM start and end address
    LCD_WriteReg(0x0045,0x0000);  //Vretical RAM start address
    LCD_WriteReg(0x0046,0x013f);  //Vretical RAM end address 
    LCD_WriteReg(0x004e,0x0000);  //set GDDRAM x address counter
    LCD_WriteReg(0x004f,0x0000);  //set GDDRAM y address counter   
    //y control
    LCD_WriteReg(0x0030,0x0707);
    LCD_WriteReg(0x0031,0x0202);
    LCD_WriteReg(0x0032,0x0204);
    LCD_WriteReg(0x0033,0x0502);
    LCD_WriteReg(0x0034,0x0507);
    LCD_WriteReg(0x0035,0x0204);
    LCD_WriteReg(0x0036,0x0204);
    LCD_WriteReg(0x0037,0x0502);
    LCD_WriteReg(0x003a,0x0302);
    LCD_WriteReg(0x003b,0x0302); 
    LCD_WriteReg(0x0023,0x0000);
    LCD_WriteReg(0x0024,0x0000);
    LCD_WriteReg(0x0025,0x8000);
    LCD_WriteReg(0x0026,0x7000);
    LCD_WriteReg(0x0020,0xb0eb);
    LCD_WriteReg(0x0027,0x007c);
  }
  if( DeviceCode == 0x5408 )
  {
    LCD_Code = SPFD5408B;
    
    LCD_WriteReg(0x0001,0x0100);    //Driver Output Contral Register 
    LCD_WriteReg(0x0002,0x0700);    //LCD Driving Waveform Contral
    LCD_WriteReg(0x0003,0x1030);    //Entry Mode
    
    LCD_WriteReg(0x0004,0x0000);    //Scalling Control register
    LCD_WriteReg(0x0008,0x0207);    //Display Control 2
    LCD_WriteReg(0x0009,0x0000);    //Display Control 3
    LCD_WriteReg(0x000A,0x0000);    //Frame Cycle Control
    LCD_WriteReg(0x000C,0x0000);    //External Display Interface Control 1
    LCD_WriteReg(0x000D,0x0000);    //Frame Maker Position
    LCD_WriteReg(0x000F,0x0000);    //External Display Interface Control 2
    delay_ms(50);
    LCD_WriteReg(0x0007,0x0101);    //Display Control
    delay_ms(50);
    LCD_WriteReg(0x0010,0x16B0);      //Power Control 1
    LCD_WriteReg(0x0011,0x0001);      //Power Control 2
    LCD_WriteReg(0x0017,0x0001);      //Power Control 3
    LCD_WriteReg(0x0012,0x0138);      //Power Control 4
    LCD_WriteReg(0x0013,0x0800);      //Power Control 5
    LCD_WriteReg(0x0029,0x0009);    //NVM read data 2
    LCD_WriteReg(0x002a,0x0009);    //NVM read data 3
    LCD_WriteReg(0x00a4,0x0000);  
    LCD_WriteReg(0x0050,0x0000);    
    LCD_WriteReg(0x0051,0x00EF);   
    LCD_WriteReg(0x0052,0x0000);    
    LCD_WriteReg(0x0053,0x013F);  
       
    LCD_WriteReg(0x0060,0x2700);    //Driver Output Control
              
    LCD_WriteReg(0x0061,0x0003);    //Driver Output Control
    LCD_WriteReg(0x006A,0x0000);    //Vertical Scroll Control
    
    LCD_WriteReg(0x0080,0x0000);    //Display Position ¨C Partial Display 1
    LCD_WriteReg(0x0081,0x0000);    //RAM Address Start ¨C Partial Display 1
    LCD_WriteReg(0x0082,0x0000);    //RAM address End - Partial Display 1
    LCD_WriteReg(0x0083,0x0000);    //Display Position ¨C Partial Display 2
    LCD_WriteReg(0x0084,0x0000);    //RAM Address Start ¨C Partial Display 2
    LCD_WriteReg(0x0085,0x0000);    //RAM address End ¨C Partail Display2
    LCD_WriteReg(0x0090,0x0013);    //Frame Cycle Control
    LCD_WriteReg(0x0092,0x0000);     //Panel Interface Control 2
    LCD_WriteReg(0x0093,0x0003);    //Panel Interface control 3
    LCD_WriteReg(0x0095,0x0110);    //Frame Cycle Control
    LCD_WriteReg(0x0007,0x0173);
  }
  if( DeviceCode == 0x4531 )
  {  
    LCD_Code = LGDP4531;
    //Setup display
    LCD_WriteReg(0x00,0x0001);
    LCD_WriteReg(0x10,0x0628);
    LCD_WriteReg(0x12,0x0006);
    LCD_WriteReg(0x13,0x0A32);
    LCD_WriteReg(0x11,0x0040);
    LCD_WriteReg(0x15,0x0050);
    LCD_WriteReg(0x12,0x0016);
    delay_ms(50);
    LCD_WriteReg(0x10,0x5660);
    delay_ms(50);
    LCD_WriteReg(0x13,0x2A4E);
    LCD_WriteReg(0x01,0x0100);
    LCD_WriteReg(0x02,0x0300);  
    LCD_WriteReg(0x03,0x1030);    
    LCD_WriteReg(0x08,0x0202);
    LCD_WriteReg(0x0A,0x0000);
    LCD_WriteReg(0x30,0x0000);
    LCD_WriteReg(0x31,0x0402);
    LCD_WriteReg(0x32,0x0106);
    LCD_WriteReg(0x33,0x0700);
    LCD_WriteReg(0x34,0x0104);
    LCD_WriteReg(0x35,0x0301);
    LCD_WriteReg(0x36,0x0707);
    LCD_WriteReg(0x37,0x0305);
    LCD_WriteReg(0x38,0x0208);
    LCD_WriteReg(0x39,0x0F0B);
    delay_ms(50);
    LCD_WriteReg(0x41,0x0002);
    LCD_WriteReg(0x60,0x2700);
    LCD_WriteReg(0x61,0x0001);
    LCD_WriteReg(0x90,0x0119);
    LCD_WriteReg(0x92,0x010A);
    LCD_WriteReg(0x93,0x0004);
    LCD_WriteReg(0xA0,0x0100);
    delay_ms(50);
    LCD_WriteReg(0x07,0x0133);
    delay_ms(50);
    LCD_WriteReg(0xA0,0x0000);
  }
  if( DeviceCode == 0x4535 )
  {  
    LCD_Code = LGDP4535;  
    LCD_WriteReg(0x15, 0x0030);      //Set the internal vcore voltage                                              
    LCD_WriteReg(0x9A, 0x0010);      //Start internal OSC
    LCD_WriteReg(0x11, 0x0020);       //set SS and SM bit
    LCD_WriteReg(0x10, 0x3428);       //set 1 line inversion
    LCD_WriteReg(0x12, 0x0002);       //set GRAM write direction and BGR=1 
    LCD_WriteReg(0x13, 0x1038);       //Resize register
    delay_ms(40); 
    LCD_WriteReg(0x12, 0x0012);       //set the back porch and front porch
    delay_ms(40); 
    LCD_WriteReg(0x10, 0x3420);       //set non-display area refresh cycle ISC[3:0]
    LCD_WriteReg(0x13, 0x3045);       //FMARK function
    delay_ms(70); 
    LCD_WriteReg(0x30, 0x0000);      //RGB interface setting
    LCD_WriteReg(0x31, 0x0402);       //Frame marker Position
    LCD_WriteReg(0x32, 0x0307);      //RGB interface polarity
    LCD_WriteReg(0x33, 0x0304);      //SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WriteReg(0x34, 0x0004);      //DC1[2:0], DC0[2:0], VC[2:0]
    LCD_WriteReg(0x35, 0x0401);      //VREG1OUT voltage
    LCD_WriteReg(0x36, 0x0707);      //VDV[4:0] for VCOM amplitude
    LCD_WriteReg(0x37, 0x0305);      //SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WriteReg(0x38, 0x0610);      //DC1[2:0], DC0[2:0], VC[2:0]
    LCD_WriteReg(0x39, 0x0610);      //VREG1OUT voltage
    LCD_WriteReg(0x01, 0x0100);      //VDV[4:0] for VCOM amplitude
    LCD_WriteReg(0x02, 0x0300);      //VCM[4:0] for VCOMH
    LCD_WriteReg(0x03, 0x1030);      //GRAM horizontal Address
    LCD_WriteReg(0x08, 0x0808);      //GRAM Vertical Address
    LCD_WriteReg(0x0A, 0x0008);    
    LCD_WriteReg(0x60, 0x2700);       //Gate Scan Line
    LCD_WriteReg(0x61, 0x0001);       //NDL,VLE, REV
    LCD_WriteReg(0x90, 0x013E);
    LCD_WriteReg(0x92, 0x0100);
    LCD_WriteReg(0x93, 0x0100);
    LCD_WriteReg(0xA0, 0x3000);
    LCD_WriteReg(0xA3, 0x0010);
    LCD_WriteReg(0x07, 0x0001);
    LCD_WriteReg(0x07, 0x0021);
    LCD_WriteReg(0x07, 0x0023);
    LCD_WriteReg(0x07, 0x0033);
    LCD_WriteReg(0x07, 0x0133);
  }          
  else if( DeviceCode == 0x0047 )
  {
    LCD_Code = HX8347D;
    //Start Initial Sequence
    LCD_WriteReg(0xEA,0x00);                          
    LCD_WriteReg(0xEB,0x20);                                                     
    LCD_WriteReg(0xEC,0x0C);                                                   
    LCD_WriteReg(0xED,0xC4);                                                    
    LCD_WriteReg(0xE8,0x40);                                                     
    LCD_WriteReg(0xE9,0x38);                                                    
    LCD_WriteReg(0xF1,0x01);                                                    
    LCD_WriteReg(0xF2,0x10);                                                    
    LCD_WriteReg(0x27,0xA3);                                                    
    //GAMMA SETTING
    LCD_WriteReg(0x40,0x01);                           
    LCD_WriteReg(0x41,0x00);                                                   
    LCD_WriteReg(0x42,0x00);                                                   
    LCD_WriteReg(0x43,0x10);                                                    
    LCD_WriteReg(0x44,0x0E);                                                   
    LCD_WriteReg(0x45,0x24);                                                  
    LCD_WriteReg(0x46,0x04);                                                  
    LCD_WriteReg(0x47,0x50);                                                   
    LCD_WriteReg(0x48,0x02);                                                    
    LCD_WriteReg(0x49,0x13);                                                  
    LCD_WriteReg(0x4A,0x19);                                                  
    LCD_WriteReg(0x4B,0x19);                                                 
    LCD_WriteReg(0x4C,0x16);                                                 
    LCD_WriteReg(0x50,0x1B);                                                   
    LCD_WriteReg(0x51,0x31);                                                    
    LCD_WriteReg(0x52,0x2F);                                                     
    LCD_WriteReg(0x53,0x3F);                                                    
    LCD_WriteReg(0x54,0x3F);                                                     
    LCD_WriteReg(0x55,0x3E);                                                     
    LCD_WriteReg(0x56,0x2F);                                                   
    LCD_WriteReg(0x57,0x7B);                                                     
    LCD_WriteReg(0x58,0x09);                                                  
    LCD_WriteReg(0x59,0x06);                                                 
    LCD_WriteReg(0x5A,0x06);                                                   
    LCD_WriteReg(0x5B,0x0C);                                                   
    LCD_WriteReg(0x5C,0x1D);                                                   
    LCD_WriteReg(0x5D,0xCC);                                                   
    //Power Voltage Setting
    LCD_WriteReg(0x1B,0x18);                                                    
    LCD_WriteReg(0x1A,0x01);                                                    
    LCD_WriteReg(0x24,0x15);                                                    
    LCD_WriteReg(0x25,0x50);                                                    
    LCD_WriteReg(0x23,0x8B);                                                   
    LCD_WriteReg(0x18,0x36);                           
    LCD_WriteReg(0x19,0x01);                                                    
    LCD_WriteReg(0x01,0x00);                                                   
    LCD_WriteReg(0x1F,0x88);                                                    
    delay_ms(50);
    LCD_WriteReg(0x1F,0x80);                                                  
    delay_ms(50);
    LCD_WriteReg(0x1F,0x90);                                                   
    delay_ms(50);
    LCD_WriteReg(0x1F,0xD0);                                                   
    delay_ms(50);
    LCD_WriteReg(0x17,0x05);                                                    
    LCD_WriteReg(0x36,0x00);                                                    
    LCD_WriteReg(0x28,0x38);                                                 
    delay_ms(50);
    LCD_WriteReg(0x28,0x3C);                                                
  }
  if( DeviceCode == 0x7783 )
  {
    LCD_Code = ST7781;
    //Start Initial Sequence
    LCD_WriteReg(0x00FF,0x0001);
    LCD_WriteReg(0x00F3,0x0008);
    LCD_WriteReg(0x0001,0x0100);
    LCD_WriteReg(0x0002,0x0700);
    LCD_WriteReg(0x0003,0x1030);  
    LCD_WriteReg(0x0008,0x0302);
    LCD_WriteReg(0x0008,0x0207);
    LCD_WriteReg(0x0009,0x0000);
    LCD_WriteReg(0x000A,0x0000);
    LCD_WriteReg(0x0010,0x0000);  
    LCD_WriteReg(0x0011,0x0005);
    LCD_WriteReg(0x0012,0x0000);
    LCD_WriteReg(0x0013,0x0000);
    delay_ms(50);
    LCD_WriteReg(0x0010,0x12B0);
    delay_ms(50);
    LCD_WriteReg(0x0011,0x0007);
    delay_ms(50);
    LCD_WriteReg(0x0012,0x008B);
    delay_ms(50);  
    LCD_WriteReg(0x0013,0x1700);
    delay_ms(50);  
    LCD_WriteReg(0x0029,0x0022);    
    LCD_WriteReg(0x0030,0x0000);
    LCD_WriteReg(0x0031,0x0707);
    LCD_WriteReg(0x0032,0x0505);
    LCD_WriteReg(0x0035,0x0107);
    LCD_WriteReg(0x0036,0x0008);
    LCD_WriteReg(0x0037,0x0000);
    LCD_WriteReg(0x0038,0x0202);
    LCD_WriteReg(0x0039,0x0106);
    LCD_WriteReg(0x003C,0x0202);
    LCD_WriteReg(0x003D,0x0408);
    delay_ms(50);        
    LCD_WriteReg(0x0050,0x0000);    
    LCD_WriteReg(0x0051,0x00EF);    
    LCD_WriteReg(0x0052,0x0000);    
    LCD_WriteReg(0x0053,0x013F);    
    LCD_WriteReg(0x0060,0xA700);    
    LCD_WriteReg(0x0061,0x0001);
    LCD_WriteReg(0x0090,0x0033);        
    LCD_WriteReg(0x002B,0x000B);    
    LCD_WriteReg(0x0007,0x0133);
  }
  else // special ID
  {
    DeviceCode = LCD_ReadReg(0x67);
    if( DeviceCode == 0x0046 )
    {
      LCD_Code = HX8346A;
      //Gamma for CMO 3.2
      LCD_WriteReg(0x46,0x94);
      LCD_WriteReg(0x47,0x41);
      LCD_WriteReg(0x48,0x00);
      LCD_WriteReg(0x49,0x33);
      LCD_WriteReg(0x4a,0x23);
      LCD_WriteReg(0x4b,0x45);
      LCD_WriteReg(0x4c,0x44);
      LCD_WriteReg(0x4d,0x77);
      LCD_WriteReg(0x4e,0x12);
      LCD_WriteReg(0x4f,0xcc);
      LCD_WriteReg(0x50,0x46);
      LCD_WriteReg(0x51,0x82);
      //240x320 window setting
      LCD_WriteReg(0x02,0x00);
      LCD_WriteReg(0x03,0x00);
      LCD_WriteReg(0x04,0x01);
      LCD_WriteReg(0x05,0x3f);
      LCD_WriteReg(0x06,0x00);
      LCD_WriteReg(0x07,0x00);
      LCD_WriteReg(0x08,0x00); 
      LCD_WriteReg(0x09,0xef);     
      //Display Setting
      LCD_WriteReg(0x01,0x06);            
      LCD_WriteReg(0x16,0xC8);  //MY(1) MX(1) MV(0)
      LCD_WriteReg(0x23,0x95);
      LCD_WriteReg(0x24,0x95);
      LCD_WriteReg(0x25,0xff);    
      LCD_WriteReg(0x27,0x02);
      LCD_WriteReg(0x28,0x02);
      LCD_WriteReg(0x29,0x02);
      LCD_WriteReg(0x2a,0x02);
      LCD_WriteReg(0x2c,0x02);
      LCD_WriteReg(0x2d,0x02);                
      LCD_WriteReg(0x3a,0x01);
      LCD_WriteReg(0x3b,0x01);
      LCD_WriteReg(0x3c,0xf0);
      LCD_WriteReg(0x3d,0x00);
      delay_ms(2);
      LCD_WriteReg(0x35,0x38);
      LCD_WriteReg(0x36,0x78);  
      LCD_WriteReg(0x3e,0x38);
      LCD_WriteReg(0x40,0x0f);
      LCD_WriteReg(0x41,0xf0);
      //Power Supply Setting
      LCD_WriteReg(0x19,0x49);
      LCD_WriteReg(0x93,0x0f);
      delay_ms(1);
      LCD_WriteReg(0x20,0x30);
      LCD_WriteReg(0x1d,0x07);
      LCD_WriteReg(0x1e,0x00);
      LCD_WriteReg(0x1f,0x07);
      //VCOM Setting for CMO 3.2¡± Panel
      LCD_WriteReg(0x44,0x4d);
      LCD_WriteReg(0x45,0x13);
      delay_ms(1);
      LCD_WriteReg(0x1c,0x04);
      delay_ms(2);
      LCD_WriteReg(0x43,0x80);
      delay_ms(5);
      LCD_WriteReg(0x1b,0x08);
      delay_ms(4);
      LCD_WriteReg(0x1b,0x10);      
      delay_ms(4);    
      //Display ON Setting
      LCD_WriteReg(0x90,0x7f);
      LCD_WriteReg(0x26,0x04);
      delay_ms(4);
      LCD_WriteReg(0x26,0x24);
      LCD_WriteReg(0x26,0x2c);
      delay_ms(4);
      LCD_WriteReg(0x26,0x3c);  
      //Set internal VDDD voltage
      LCD_WriteReg(0x57,0x02);
      LCD_WriteReg(0x55,0x00);
      LCD_WriteReg(0x57,0x00);
    }
    if( DeviceCode == 0x0047 )
    {
      LCD_Code = HX8347A;
      LCD_WriteReg(0x0042,0x0008);   
      //Gamma setting  
      LCD_WriteReg(0x0046,0x00B4); 
       LCD_WriteReg(0x0047,0x0043); 
       LCD_WriteReg(0x0048,0x0013); 
      LCD_WriteReg(0x0049,0x0047); 
      LCD_WriteReg(0x004A,0x0014); 
      LCD_WriteReg(0x004B,0x0036); 
      LCD_WriteReg(0x004C,0x0003); 
      LCD_WriteReg(0x004D,0x0046); 
      LCD_WriteReg(0x004E,0x0005);  
      LCD_WriteReg(0x004F,0x0010);  
      LCD_WriteReg(0x0050,0x0008);  
      LCD_WriteReg(0x0051,0x000a);  
      //Window Setting
       LCD_WriteReg(0x0002,0x0000); 
      LCD_WriteReg(0x0003,0x0000); 
      LCD_WriteReg(0x0004,0x0000); 
      LCD_WriteReg(0x0005,0x00EF); 
      LCD_WriteReg(0x0006,0x0000); 
      LCD_WriteReg(0x0007,0x0000); 
      LCD_WriteReg(0x0008,0x0001); 
      LCD_WriteReg(0x0009,0x003F); 
      delay_ms(10); 
      LCD_WriteReg(0x0001,0x0006); 
      LCD_WriteReg(0x0016,0x00C8);   
      LCD_WriteReg(0x0023,0x0095); 
      LCD_WriteReg(0x0024,0x0095); 
      LCD_WriteReg(0x0025,0x00FF); 
      LCD_WriteReg(0x0027,0x0002); 
      LCD_WriteReg(0x0028,0x0002); 
      LCD_WriteReg(0x0029,0x0002); 
      LCD_WriteReg(0x002A,0x0002); 
      LCD_WriteReg(0x002C,0x0002); 
      LCD_WriteReg(0x002D,0x0002); 
      LCD_WriteReg(0x003A,0x0001);  
      LCD_WriteReg(0x003B,0x0001);  
      LCD_WriteReg(0x003C,0x00F0);    
      LCD_WriteReg(0x003D,0x0000); 
      delay_ms(20); 
      LCD_WriteReg(0x0035,0x0038); 
      LCD_WriteReg(0x0036,0x0078); 
      LCD_WriteReg(0x003E,0x0038); 
      LCD_WriteReg(0x0040,0x000F); 
      LCD_WriteReg(0x0041,0x00F0);  
      LCD_WriteReg(0x0038,0x0000); 
      //Power Setting 
      LCD_WriteReg(0x0019,0x0049);  
      LCD_WriteReg(0x0093,0x000A); 
      delay_ms(10); 
      LCD_WriteReg(0x0020,0x0020);   
      LCD_WriteReg(0x001D,0x0003);   
      LCD_WriteReg(0x001E,0x0000);  
      LCD_WriteReg(0x001F,0x0009);   
      LCD_WriteReg(0x0044,0x0053);  
      LCD_WriteReg(0x0045,0x0010);   
      delay_ms(10);  
      LCD_WriteReg(0x001C,0x0004);  
      delay_ms(20); 
      LCD_WriteReg(0x0043,0x0080);    
      delay_ms(5); 
      LCD_WriteReg(0x001B,0x000a);    
      delay_ms(40);  
      LCD_WriteReg(0x001B,0x0012);    
      delay_ms(40);   
      //Display On Setting 
      LCD_WriteReg(0x0090,0x007F); 
      LCD_WriteReg(0x0026,0x0004); 
      delay_ms(40);  
      LCD_WriteReg(0x0026,0x0024); 
      LCD_WriteReg(0x0026,0x002C); 
      delay_ms(40);   
      LCD_WriteReg(0x0070,0x0008); 
      LCD_WriteReg(0x0026,0x003C);  
      LCD_WriteReg(0x0057,0x0002); 
      LCD_WriteReg(0x0055,0x0000); 
      LCD_WriteReg(0x0057,0x0000); 
    }   
  }              
  ---*/

  delay_ms(50); // delay 50 ms  
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_Clear
// Description    : 0xffff
// Input          : - Color: Screen Color
void LCD_Clear(uint16_t Color)
{
  uint32_t index;
  
  if( LCD_Code == HX8347D || LCD_Code == HX8347A )
  {
    LCD_WriteReg(0x02,0x00);                                                  
    LCD_WriteReg(0x03,0x00);  
                    
    LCD_WriteReg(0x04,0x00);                           
    LCD_WriteReg(0x05,0xEF);  
                     
    LCD_WriteReg(0x06,0x00);                           
    LCD_WriteReg(0x07,0x00);    
                   
    LCD_WriteReg(0x08,0x01);                           
    LCD_WriteReg(0x09,0x3F);     
  }
  else
  {  
    LCD_SetCursor(0,0); 
  }  

  LCD_WriteIndex(0x0022);
  for( index = 0; index < MAX_X * MAX_Y; index++ )
  {
    LCD_WriteData(Color);
  }
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_BGR2RGB
// Description    : RRRRRGGGGGGBBBBB  BBBBBGGGGGGRRRRR 
// Input          : - color: BRG
static uint16_t LCD_BGR2RGB(uint16_t color)
{
  uint16_t  r, g, b, rgb;
  
  b = ( color>>0 )  & 0x1f;
  g = ( color>>5 )  & 0x3f;
  r = ( color>>11 ) & 0x1f;
  
  rgb =  (b<<11) + (g<<5) + (r<<0);
  
  return( rgb );
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_GetPoint
// Description    :
// Input          : - Xpos: Row Coordinate
//                  - Xpos: Line Coordinate 
// Return          : Screen Color
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
  uint16_t dummy;
  
  LCD_SetCursor(Xpos,Ypos);
  LCD_WriteIndex(0x0022);  
  
  switch( LCD_Code )
  {
    case ST7781:
    case LGDP4531:
    case LGDP4535:
    case SSD1289:
    case SSD1298:
      dummy = LCD_ReadData(); // Empty read 
      dummy = LCD_ReadData();   
       return  dummy;        
    case HX8347A:
    case HX8347D:
    {
      uint8_t red,green,blue;
        
      dummy = LCD_ReadData(); // Empty read 

      red = LCD_ReadData() >> 3; 
      green = LCD_ReadData() >> 2; 
      blue = LCD_ReadData() >> 3; 
      dummy = (uint16_t) ( ( red<<11 ) | ( green << 5 ) | blue ); 
    }  
      return  dummy;

    default: // 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x9919 
      dummy = LCD_ReadData(); // Empty read 
      dummy = LCD_ReadData();   
      return  LCD_BGR2RGB( dummy );
  }
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_SetPoint
// Description    : 
// Input          : - Xpos: Row Coordinate
//                  - Ypos: Line Coordinate 
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
  if( Xpos >= MAX_X || Ypos >= MAX_Y )
  {
    return;
  }
  LCD_SetCursor(Xpos,Ypos);
  LCD_WriteReg(0x0022,point);
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_DrawLine
// Description    : Bresenham's line algorithm
// Input          : - x1: 
//                  - y1: 
//                  - x2: 
//                  - y2:
//                   - color: 
void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color)
{
  short dx,dy; 
  short temp;  

  if( x0 > x1 )   
  {
    temp = x1;
    x1 = x0;
    x0 = temp;   
  }
  if( y0 > y1 )   
  {
    temp = y1;
    y1 = y0;
    y0 = temp;   
  }
  
  dx = x1-x0; 
  dy = y1-y0;   

  if( dx == 0 ) 
  {
    do
    { 
      LCD_SetPoint(x0, y0, color);
      y0++;
    }
    while( y1 >= y0 ); 
    return; 
  }
  if( dy == 0 )   
  {
    do
    {
      LCD_SetPoint(x0, y0, color);  
      x0++;
    }
    while( x1 >= x0 ); 
    return;
  }
  
  // (Bresenham)
  if( dx > dy )     
  {
    temp = 2 * dy - dx;          
    while( x0 != x1 )
    {
      LCD_SetPoint(x0,y0,color);  
      x0++;                     
      if( temp > 0 )    
      {
        y0++;          
        temp += 2 * dy - 2 * dx; 
      }
      else         
      {
        temp += 2 * dy; 
      }       
    }
    LCD_SetPoint(x0,y0,color);
  }  
  else
  {
    temp = 2 * dx - dy;    
    while( y0 != y1 )
    {
      LCD_SetPoint(x0,y0,color);     
      y0++;                 
      if( temp > 0 )           
      {
        x0++;               
        temp+=2*dy-2*dx; 
      }
      else
      {
        temp += 2 * dy;
      }
    } 
    LCD_SetPoint(x0,y0,color);
  }
} 


//-----------------------------------------------------------------------------
// Function Name  : LCD_PutChar
// Description    :
// Input          : - Xpos: 
//                  - Ypos: 
//                  - ASCI: 
//                  - charColor:  
//                  - bkColor:
void LCD_PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor, uint16_t bkColor)
{
  uint16_t i, j;
  uint8_t buffer[16], tmp_char;
  GetASCIICode(buffer,ASCI); 
  for( i=0; i<16; i++ )
  {
    tmp_char = buffer[i];
    for( j=0; j<8; j++ )
    {
      if( (tmp_char >> 7 - j) & 0x01 == 0x01 )
      {
        LCD_SetPoint( Xpos + j, Ypos + i, charColor ); 
      }
      else
      {
        LCD_SetPoint( Xpos + j, Ypos + i, bkColor ); 
      }
    }
  }
}


//-----------------------------------------------------------------------------
// Function Name  : LCD_PutText
// Description    : 
// Input          : - Xpos:
//                  - Ypos:  
//                  - str: 
//                  - charColor:  
//                  - bkColor: 
void LCD_PutText(uint16_t Xpos, uint16_t Ypos, uint8_t *str, uint16_t Color, uint16_t bkColor)
{
  uint8_t TempChar;
  do
  {
    TempChar = *str++;  
    LCD_PutChar( Xpos, Ypos, TempChar, Color, bkColor );    
    if( Xpos < MAX_X - 8 )
    {
      Xpos += 8;
    } 
    else if ( Ypos < MAX_Y - 16 )
    {
      Xpos = 0;
      Ypos += 16;
    }   
    else
    {
      Xpos = 0;
      Ypos = 0;
    }    
  }
  while ( *str != 0 );
}

/*******************************************************************************
* Draw Bargraph                                                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        maximum width of bargraph (in pixels)            *
*                   h:        bargraph height                                  *
*                   val:      value of active bargraph (in 1/1024)             *
*   Return:                                                                    *
*******************************************************************************/

void LCD_DrawBargraph (unsigned int x, unsigned int y, unsigned int w, unsigned int h, 
  unsigned int val, uint16_t barColor, uint16_t bkColor) 
{
  // Note: this is taken from the GLCD file of the MCB1700 board;
  int i,j;

  val = (val * w) >> 10; // scale value;
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (j >= val) {
        LCD_SetPoint( x+j, y+i, bkColor); 
      } else {
        LCD_SetPoint( x+j, y+i, barColor); 
      }
    }
  }
}
