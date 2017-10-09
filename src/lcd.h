#include "hard.h"

#include "stm32f0x_tim.h"	//para Wait_ms

//--- LCD Configurations ---//
#ifdef VER_1_3
#define PINS_DEFINED_IN_HARD
//#define WITH_STATE_MACHINE		//rutinas SM hay que ir llamando a un callback
									//y se envia desde un buffer con interrupcion


//#define PORT_INVERTED				//bit bajo con alto PA0->LCD_D7
#define PORT_DIRECT					//bit bajo con pin bajo PA8->LCD_D4


typedef unsigned char UINT8;

#define PMASK       0x0F00

//#define LCDClearData GPIOA->BSRR = 0x000F0000	//reset PA3..PA0
//#define LCDClearData GPIOA->BSRR = 0x0F000000	//reset PA8..PA11
#define LCDClearData	GPIOA->BRR = PMASK;

/***  Declarations of ports of a specified MCU  ***/
/* Choose the output control pins accord your MCU */
#define lcd4bit		1		/* 4 bit interface; comment this line if  */
							/*        is 8 bit interface              */
#define lcdPort2	GPIOA	/* Port of 4 data bits to lcd connection  */
#define lcdDataPinOffset	8	//offset al primer pin de los datos
								//PA0 = 0; PA4 = 4; PA8 = 8

#endif

#ifdef VER_1_2
#define PINS_DEFINED_IN_HARD
//#define WITH_STATE_MACHINE		//rutinas SM hay que ir llamando a un callback
									//y se envia desde un buffer con interrupcion


#define PORT_INVERTED				//bit bajo con alto PA0->LCD_D7
//#define PORT_DIRECT					//bit bajo con pin bajo PA8->LCD_D4


typedef unsigned char UINT8;

#define PMASK       0x000F

#define LCDClearData GPIOA->BSRR = 0x000F0000	//reset PA3..PA0
//#define LCDClearData GPIOA->BSRR = 0x0F000000	//reset PA8..PA11

/***  Declarations of ports of a specified MCU  ***/
/* Choose the output control pins accord your MCU */
#define lcd4bit		1		/* 4 bit interface; comment this line if  */
							/*        is 8 bit interface              */
#define lcdPort2	GPIOA	/* Port of 4 data bits to lcd connection  */
#define lcdDataPinOffset	0	//offset al primer pin de los datos
								//PA0 = 0; PA4 = 4; PA8 = 8

#endif

#define CLEAR         0
#define RET_HOME      1
#define DISPLAY_ON    2
#define DISPLAY_OFF   3
#define CURSOR_ON     4
#define CURSOR_OFF    5
#define BLINK_ON      6
#define BLINK_OFF     7

#ifndef PINS_DEFINED_IN_HARD
#define LCD_E ((GPIOA->ODR & 0x0002) == 1)
#define LCD_E_ON GPIOA->BSRR = 0x00000002
#define LCD_E_OFF GPIOA->BSRR = 0x00020000

#define LCD_RS ((GPIOA->ODR & 0x0004) == 1)
#define LCD_RS_ON	GPIOA->BSRR = 0x00000004
#define LCD_RS_OFF GPIOA->BSRR = 0x00040000

#define LCD_BKL ((GPIOA->ODR & 0x0010) == 1)
#define LCD_BKL_ON	GPIOA->BSRR = 0x00000010
#define LCD_BKL_OFF GPIOA->BSRR = 0x00100000
#endif

#define gTimeBaseInterruptperms  1

void Lcd_WritePort (unsigned char);

void Lcd_Delay(unsigned short);
void Lcd_Command (unsigned char);
void LCDTransmitStr(const char * pStr);
void LCDStartTransmit(UINT8 cData);
void LCDTransmitArray(UINT8 *pStr, UINT8 length);
void Lcd_SetDDRAM (unsigned char addr);

#ifdef WITH_STATE_MACHINE
void LCDTransmitSM(void);
void LCDTransmitSMStr(char * send);
void LCD_callback (void);
void LcdSetDDRAMSM (unsigned char addr);
#endif

#ifdef PORT_INVERTED
unsigned char SwapNibble (unsigned char);
#endif

#define LCD_1ER_RENGLON Lcd_SetDDRAM(0x00)
#define LCD_2DO_RENGLON Lcd_SetDDRAM(0x40)
#define LCD_3ER_RENGLON Lcd_SetDDRAM(0x14)
#define LCD_4TO_RENGLON Lcd_SetDDRAM(0x54)

/*++++++++++++++++++++++++++++++ DON´T MODIFY +++++++++++++++++++++++++++++++*/

/* Possible status of LCD */
#define lcdStatusReady       1 /* LCD is ready to use                        */
#define lcdStatusError       2 /* Error ocurred when you are using the LCD   */
                               /*      and you try to execute other          */
                               /*      operation. You can change the state   */
                               /*      using LCDClear() function             */
#define lcdStatusPrinting    3 /* LCD is printing string                     */
#define lcdStatusInit        4 /* LCD is initializing                        */
#define lcdStatusWaiting     5 /* LCD status are waiting for ready mode      */
#define lcdStatusWaitingInit 0 /* LCD is waiting for initialization          */

/* Function declaration */
void  LCDInit(void);           /* Initialize the LCD                         */
void  LCDClear(void);          /* Clear the LCD                              */
void  LCD2L(void);             /* Go to second line of LCD                   */
void  LCDPrint(UINT8 *where, UINT8 length); /* Print from [where] memory     */
                               /*      address [length] characters           */
void  LCDTimeBase(void);       /* Time Base of LCD. This must be called      */
                               /*     each time that timerLCD is equal to 0  */
UINT8 LCDStatus(void);         /* Return the status of the LCD               */
void  LCDCursor(UINT8 ddramAddress); /* Send the address that you can put    */
                                     /*        the cursor                    */
void LCDSend(void);                                     
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
