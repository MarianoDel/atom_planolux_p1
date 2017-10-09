
#include "lcd.h"
#include "stm32f0xx.h"



#ifdef WITH_STATE_MACHINE
#include "string.h"
#define LCD_BUFF_TX 128
volatile char * pStr;
volatile char lcdbufftx [LCD_BUFF_TX];
#endif

volatile unsigned char lcd_addr_cmd = 0;
volatile unsigned char lcd_state = 0;
volatile unsigned char lcd_state_cmd = 0;


#ifdef lcd4bit
  #define OUTNUMBER(character)  (character<<lcdDataPins) /* Shift the data   */
                        /*    bits until match the 4 consecutive output      */
                        /*    pins                                           */
  #define PORTMASK              (~(0x0F<<lcdDataPins))   /* To locate the 4  */
                        /*    consecutive output pins of the port, which     */
                        /*    will transmit the data                         */
#endif


#define cActive     1
#define cInactive   0

/* Variables used with the main program */
UINT8   timerLCD;       /* Variable for delay times                          */

/* Local variables */
UINT8    LCDInternalStatus = 0; /* Variable for internal status              */
UINT8   howmany;        /* Temporal variable that have the number of         */
                        /*      characters pending to print                  */
UINT8   *nextchr;       /* Pointer to next character to print                */

//UINT8 memstr[40];

/* Local functions */
void   LCDPrintNext(void); /* Function that print the next character and     */
                        /*      increment the pointer "nextchr" and          */
                        /*      decrement the counter "howmany"              */

/*+++++++++++++++++++++++++++++++ LCD Driver ++++++++++++++++++++++++++++++++*/

//TRANSMITIR PRIMERO LOS MSB Y LUEGO LOS LSB EN 4BITS
//float frecuencia = 0.0;
//#define LIBDEF_PRINTF_FLOATING   1  
//#define __NO_FLOAT__
/*
const char coseno[91][6] = {"1,000", "1,000", "0,999", "0,999", "0,998", "0,996", "0,995", "0,993", "0,990", "0,988",
                            "0,985", "0,982", "0,978", "0,974", "0,970", "0,966", "0,961", "0,956", "0,951", "0,946",
                            "0,940", "0,934", "0,927", "0,921", "0,914", "0,906", "0,899", "0,891", "0,883", "0,875",
                            "0,866", "0,857", "0,848", "0,839", "0,829", "0,819", "0,809", "0,799", "0,788", "0,777",
                            "0,766", "0,755", "0,743", "0,731", "0,719", "0,707", "0,695", "0,682", "0,669", "0,656",
                            "0,643", "0,629", "0,616", "0,602", "0,588", "0,574", "0,559", "0,545", "0,530", "0,515",
                            "0,500", "0,485", "0,469", "0,454", "0,438", "0,423", "0,407", "0,391", "0,375", "0,358",
                            "0,342", "0,326", "0,309", "0,292", "0,276", "0,259", "0,242", "0,225", "0,208", "0,191",
                            "0,174", "0,156", "0,139", "0,122", "0,105", "0,087", "0,070", "0,052", "0,035", "0,017",
                            "0,000"};
  */


/* Indicate to LCD "read data" through fallflank */
void LCDSend(void)
{
	LCD_E_ON;
	Lcd_Delay(1);
	LCD_E_OFF;
}

  // Initialize LCD 4 bits
void LCDInit(void)
{
#ifndef PINS_DEFINED_IN_HARD
	//habilito clocks
	if (!RCC_GPIOA_clk)
		RCC_GPIOB_clkEnable;

	if (!RCC_GPIOC_clk)
		RCC_GPIOC_clkEnable;

	pStr = lcdbufftx;

	//GPIOA A1 E A2 RS A4 BKL output 10MHz push pull
	GPIOA->CRL &= 0xFFF0F00F;
	GPIOA->CRL |= 0x00010110;
	LCD_E_OFF;
	LCD_RS_OFF;
	//LCD_BKL_OFF;

	//GPIOC de 0 a 3 output 10MHz open drain
	GPIOC->CRL &= 0xFFFF0000;
	GPIOC->CRL |= 0x00005555;
#endif
      
  //lcdPortDD |= 0x0F;   //PTB0 - 3 salidas

  //LCDClearData;
  //lcdPort |= 0x00000003;
  Lcd_WritePort (0x03);
  LCDSend();

  Wait_ms(20);

  //lcdPort &= PMASK;
  //lcdPort += 0x03;
  LCDSend();
  Wait_ms(20);

  //lcdPort &= PMASK;
  //lcdPort += 0x03;
  LCDSend();
  Wait_ms(20);

  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= 0x02;    //4 bits
  Lcd_WritePort (0x02);
  LCDSend();
  Lcd_Delay(60);

   //lcdPort &= PMASK;
  //lcdPort += 0x02;  //seteo interfase num de lineas y tamaño caracter
  LCDSend();
  Lcd_Delay(60);
  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= 0x08;
  Lcd_WritePort (0x08);
  LCDSend();
  Lcd_Delay(60);


  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= 0x00;    //Display OFF Cursor OFF Parpadeo OFF
  Lcd_WritePort (0x00);
  LCDSend();
  Lcd_Delay(60);
  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= 0x08;
  Lcd_WritePort (0x08);
  LCDSend();
  Lcd_Delay(60);


  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= 0x00;    //Reseteo Cursor y DDRAM
  Lcd_WritePort (0x00);
  LCDSend();
  Lcd_Delay(60);
  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= 0x01;
  Lcd_WritePort (0x01);
  LCDSend();
  Wait_ms(30);



  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= 0x00;    //Movimiento del cursor y desplazamiento
  Lcd_WritePort (0x00);
  LCDSend();
  Lcd_Delay(60);
  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= 0x06;
  Lcd_WritePort (0x06);
  LCDSend();
  Wait_ms(30);
}


  // Initialize LCD 8 bits
/*
void LCDInit(void)
{
  lcdEDD =  1;                   // LCD Enable pin as output             
  lcdE =    0;                   // LCD Enable pin is clear              
  lcdRSDD = 1;                   // Data/Instruction pin as output       
  lcdRS =   0;                   // Data/Instruction is clear            
//  lcdRWDD = 1;                 // Data/Instruction pin as output       
//  lcdRW =   0;                 // Data/Instruction is clear            
  //lcdFlagDD = 0;  //entrada
      
  lcdPortDD = 0xFF;
  lcdPort   = 0x00;
        
  lcdPort = 0x30;
  LCDSend();
  Wait_ms(20);
        
  lcdPort = 0x30;            
  LCDSend();
  Wait_ms(20);
                 
  lcdPort = 0x30;
  LCDSend();
  Wait_ms(20);

  lcdPort = 0x38;           //probar 0x3a  
  LCDSend();
  Lcd_Delay(30);        
              
  lcdPort = 0x01;       //Reseteo cursor y DDRAM
  LCDSend();
  Wait_ms(3);

  lcdPort = 0x06; 
  LCDSend();
  Wait_ms(3);        
}
*/

//Comandos LCD 4 bits
void Lcd_Command (unsigned char cmd)
{
   switch (cmd)
   {
    case CLEAR:
      //lcdPort &= PMASK;                   //Reseteo Cursor y DDRAM
      LCDClearData;
      LCDSend();
      Lcd_Delay(30);
      //lcdPort &= PMASK;
      //LCDClearData;
      //lcdPort |= 0x01;
      Lcd_WritePort (0x01);
      LCDSend();    
      Wait_ms(30);
      break;

    case RET_HOME:
      //lcdPort &= PMASK;                   //Reseteo cursor a primer posicion
      LCDClearData;
      LCDSend();
      Lcd_Delay(30);
      //lcdPort &= PMASK;
      //LCDClearData;
      //lcdPort |= 0x02;
      Lcd_WritePort (0x02);
      LCDSend();    
      Wait_ms(30);
      break;

    case DISPLAY_ON:
      //lcdPort &= PMASK;
      LCDClearData;
      LCDSend();
      Lcd_Delay(30);
      //lcdPort &= PMASK;
      //LCDClearData;
      //lcdPort |= 0x0c;
      Lcd_WritePort (0x0c);
      LCDSend();
      Wait_ms(30);
      break;

    case DISPLAY_OFF:
      //lcdPort &= PMASK;
      LCDClearData;
      LCDSend();
      Lcd_Delay(30);
      //lcdPort &= PMASK;
      //LCDClearData;
      //lcdPort |= 0x08;
      Lcd_WritePort (0x08);
      LCDSend();
      Wait_ms(30);
      break;
      
    case CURSOR_ON:
      //lcdPort &= PMASK;
      LCDClearData;
      LCDSend();
      Lcd_Delay(30);
      //lcdPort &= PMASK;
      //LCDClearData;
      //lcdPort |= 0x0e;
      Lcd_WritePort (0x0e);
      LCDSend();
      Wait_ms(30);
      break;

    case CURSOR_OFF:
      //lcdPort &= PMASK;
      LCDClearData;
      LCDSend();
      Lcd_Delay(30);
      //lcdPort &= PMASK;
      //LCDClearData;
      //lcdPort |= 0x0c;                     //siempre con display on
      Lcd_WritePort (0x0c);
      LCDSend();
      Wait_ms(30);
      break;

    case BLINK_ON:
      //lcdPort &= PMASK;
      LCDClearData;
      LCDSend();
      Lcd_Delay(30);
      //lcdPort &= PMASK;
      //LCDClearData;
      //lcdPort |= 0x0f;                     //siempre con display on cursor on
      Lcd_WritePort (0x0f);
      LCDSend();
      Wait_ms(30);
      break;

    case BLINK_OFF:
      //lcdPort &= PMASK;
    	LCDClearData;
      LCDSend();
      Lcd_Delay(30);
      //lcdPort &= PMASK;
      //LCDClearData;
      //lcdPort |= 0x0c;                     //siempre con display on cursor on
      Lcd_WritePort (0x0c);
      LCDSend();
      Wait_ms(30);
      break;
      
    default:
      break;
   }
}


//Comandos LCD 8 bits
/*
void Lcd_Command (unsigned char cmd)
{
   switch (cmd)
   {
    case CLEAR:
      lcdPort = 0x01;       //Reseteo cursor y DDRAM
      LCDSend();
      Wait_ms(3);
      break;

    case RET_HOME:
      lcdPort = 0x02;       //Reseteo cursor a primer posicion
      LCDSend();
      Wait_ms(3);
      break;

    case DISPLAY_ON:
      lcdPort = 0x0c;       //
      LCDSend();
      Lcd_Delay(30);
      break;

    case DISPLAY_OFF:
      lcdPort = 0x08;       //
      LCDSend();
      Lcd_Delay(30);
      break;
      
    case CURSOR_ON:
      lcdPort = 0x0e;       //siempre con display on
      LCDSend();
      Lcd_Delay(30);
      break;

    case CURSOR_OFF:
      lcdPort = 0x0c;       //siempre con display on
      LCDSend();
      Lcd_Delay(30);
      break;

    case BLINK_ON:
      lcdPort = 0x0f;       //siempre con display on cursor on
      LCDSend();
      Lcd_Delay(30);
      break;

    case BLINK_OFF:
      lcdPort = 0x0e;       //siempre con display on cursor on
      LCDSend();
      Lcd_Delay(30);
      break;
      
    default:
      break;
   }
}
*/

//Set RAM 4 bits
void Lcd_SetDDRAM (unsigned char addr)   //datos validos 100ns despues de que E sube
{
  unsigned char b;
  addr |= 0x80;
  b = addr;
  b >>= 4;
  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= b;
  Lcd_WritePort (b);
  LCDSend();
  Lcd_Delay(30);
  b = addr;
  b &= 0x0F;
  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= b;
  Lcd_WritePort (b);
  LCDSend();
  Lcd_Delay(30);       //con 30 67us; con 10 29us
}


//Set RAM 8 bits
/*
void Lcd_SetDDRAM (unsigned char addr)   //datos validos 100ns despues de que E sube
{
  addr |= 0x80;
  lcdPort = addr;         //seteo DDRAM en el origen
  LCDSend();
  Lcd_Delay(30);
}
*/    
  
void LCDTransmitArray(UINT8 *pStr, UINT8 length)
{
    UINT8 i;
    for (i=0; i<length; i++)
    {
        LCDStartTransmit(pStr[i]);
    }
}

//Transmision 4 bits
void LCDStartTransmit(UINT8 cData)
{
  unsigned char b;
  //mando el caracter
  //lcdRS = 1;
  LCD_RS_ON;
  b = cData;
  b >>= 4;
  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= b;
  Lcd_WritePort (b);
  LCDSend();
  Lcd_Delay(30);
  b = cData;
  b &= 0x0F;  
  //lcdPort &= PMASK;
  //LCDClearData;
  //lcdPort |= b;
  Lcd_WritePort (b);
  LCDSend();
  Lcd_Delay(30);       //con 30 67us; con 10 29us
  //lcdRS = 0;
  LCD_RS_OFF;
}

//Transmision 8 bits
/*
void LCDStartTransmit(UINT8 cData)
{
  //mando el caracter
  lcdPort = cData;
  lcdRS = 1;        
  LCDSend();
  Lcd_Delay(30);       //con 30 67us; con 10 29us
  lcdRS = 0;  
}
*/

void LCDTransmitStr(const char * pStr)
{
  while (*pStr != 0)
  {
    LCDStartTransmit(*pStr);
    pStr++;
  }
}

  /* Clear display */
  void LCDClear(void) {

    //lcdRS = 0;
	  LCD_RS_OFF;
    #ifdef lcd4bit
      //lcdPort &= PORTMASK;
	  //LCDClearData;
      //lcdPort |= OUTNUMBER(0x00);
	  Lcd_WritePort (0x00);
      LCDSend();
      //lcdPort &= PORTMASK;
      //LCDClearData;
      //lcdPort |= OUTNUMBER(0x01);
      Lcd_WritePort (0x01);
      LCDSend();
    #else
      //lcdPort |= 0x01;
      Lcd_WritePort (0x01);
      LCDSend();
    #endif
    timerLCD = gTimeBaseInterruptperms * 2;
    LCDInternalStatus = lcdStatusWaiting;

  }
  
  /* Set the cursor on the second line */
  void LCD2L(void) {

    LCDCursor(0x40);

  }

  /* Print the next character and increment the pointer "nextchr" and        */
  /*      decrement the counter "howmany"                                    */
  void LCDPrintNext(void) {
  
    if (howmany-- > 0) {            
      //lcdRS = 1;
    	LCD_RS_ON;
      #ifdef lcd4bit
        //lcdPort &= PORTMASK;
    	//LCDClearData;
        //lcdPort |= OUTNUMBER(((*nextchr)>>0x04));
    	Lcd_WritePort (((*nextchr)>>0x04));
        LCDSend();
        //lcdPort &= PORTMASK;
        //LCDClearData;
        //lcdPort |= OUTNUMBER(((*nextchr) & 0x0F));
        Lcd_WritePort (((*nextchr) & 0x0F));
        LCDSend();          
      #else
        //lcdPort |= (*nextchr);
        Lcd_WritePort (*nextchr);
        LCDSend();
      #endif
      nextchr++;
      timerLCD = gTimeBaseInterruptperms;
      LCDInternalStatus = lcdStatusPrinting;
    }
    else {
      LCDInternalStatus = lcdStatusReady;
    }
    
  }
  
  /* Display a string with a specific length */
  void LCDPrint(UINT8 *where, UINT8 length) {

    if (LCDInternalStatus == lcdStatusReady) {
      if (length > 0) {
        howmany = length;
        nextchr = where;
        LCDPrintNext();
      }
      else { // ignore
      }
    }
    else {
      LCDInternalStatus = lcdStatusError;
    }
    
  }

  /* This must be called each time that timerLCD is equal to 0 */
  void LCDTimeBase(void) {

    if (LCDInternalStatus == lcdStatusInit) {
      LCDInit();
    } else if (LCDInternalStatus == lcdStatusPrinting) {
      LCDPrintNext();
    } else if (LCDInternalStatus == lcdStatusWaiting) {
      LCDInternalStatus = lcdStatusReady;
    }
    
  }
  
  /* Return the status of LCD Drive */
  UINT8 LCDStatus(void) {
  
    return LCDInternalStatus;
    
  }
  
  /* Sets the cursor in the display ddramAddress */
  void LCDCursor(UINT8 ddramAddress) {
  
    if (LCDInternalStatus == lcdStatusReady) {
      //lcdRS = 0;
    	LCD_RS_OFF;
      ddramAddress |= 0x80;
      #ifdef lcd4bit        
        //lcdPort &= PORTMASK;
      	//LCDClearData;
        //lcdPort |= OUTNUMBER(ddramAddress>>4);
      	Lcd_WritePort (ddramAddress>>4);
        LCDSend();
        //lcdPort &= PORTMASK;
        //LCDClearData;
        //lcdPort |= OUTNUMBER(ddramAddress & 0x0F);
        Lcd_WritePort (ddramAddress & 0x0F);
        LCDSend();
      #else
        //lcdPort |= ddramAddress;
        Lcd_WritePort (ddramAddress);
        LCDSend();
      #endif
      timerLCD = gTimeBaseInterruptperms;
      LCDInternalStatus = lcdStatusWaiting;
    }
    else {
      LCDInternalStatus = lcdStatusError;
    }
    
  }
  
void Lcd_Delay (unsigned short i)        //con 30 67us; con 10 29us
{
	unsigned char a, dummy = 0;
  while (i)
  {
	  //for (a = 0; a < 100; a++)
	  for (a = 0; a < 30; a++)
	  {
		  dummy++;
	  }

	  i--;
  }
}


void Lcd_WritePort (unsigned char data_w)
{
#ifdef PORT_INVERTED
	lcdPort2->BRR = PMASK;
	lcdPort2->BSRR |= (SwapNibble(data_w) << lcdDataPinOffset);
#else
	//lcdPort2->BRR = PMASK;
	LCDClearData;
	lcdPort2->BSRR |= (data_w << lcdDataPinOffset);
#endif
}

unsigned char SwapNibble (unsigned char a)
{
	unsigned char result = 0;

	if (a & 0x01)
		result |= 0x08;
	if (a & 0x02)
		result |= 0x04;
	if (a & 0x04)
		result |= 0x02;
	if (a & 0x08)
		result |= 0x01;

	return result;
}
/*
void LCDTransmitSM(UINT8 *pStr)
{
    //necesito global p2Str y lcd_state
    //maquina de estado para enviar strings
    unsigned char b = 0;

    switch (lcd_state)
    {
        case 0:
            //preparo el string
            if (*pStr != 0)
            {
                p2Str = pStr;
                lcd_state++;
            }
            break;

        case 1:
            if (*p2Str != 0)
            {
            	 //ACTIVO RS
            	 LCD_RS_ON;
            	 //PRIMER NIBBLE
            	 b = *p2Str;
            	 b >>= 4; //lcdPort &= PMASK;
            	 LCDClearData;
            	 lcdPort |= b;


            	 //ACTIVO PULSO PRIMER NIBBLE
            	 LCD_E_ON;
            	 lcd_state++;
            }
            else
            {
                lcd_state = 0;
            }
            break;

        case 2:
            //TERMINO PULSO DEL PRIMER NIBBLE
            LCD_E_OFF;
            lcd_state++;
            break;

        case 3:

        	//SEGUNDO NIBBLE
        	b = *p2Str;
        	b &= 0x0F;
        	LCDClearData;
        	lcdPort |= b;

            //ACTIVO PULSO SEGUNDO NIBBLE
            LCD_E_ON;
            lcd_state++;
            break;

        case 4:
            //TERMINO PULSO DEL SEGUNDO NIBBLE
            LCD_E_OFF;
            lcd_state++;
            break;

        case 5:
            //DESACTIVO RS
            LCD_RS_OFF;
            p2Str++;
            lcd_state = 1;
            break;

        default:
            lcd_state = 0;
            break;
    }
}
*/

//---- RUTINAS STATE MACHINE ---//
#ifdef WITH_STATE_MACHINE
void LCD_callback (void)
{
    //funcion callback que la llama el TIM7 cada 1mseg

    LCDTransmitSM();
}
/*
void LCD_callback (void)
{
    //funcion callback que la llama el TIM7 cada 1mseg
    //usa lcd_state!!!
    unsigned char b = 1;
    unsigned char * dummyptr;

    dummyptr = &b;

    //me fijo si hay alguna ejecutandose
    if (lcd_state_cmd > 1)
    	LcdSetDDRAMSM (b);
    else if (lcd_state > 1)
    	LCDTransmitSM (dummyptr);
    else if (lcd_state_cmd != 0)
            LcdSetDDRAMSM (b);
    //sino comienzo con los renglones
    else if (lcd_state != 0)
        LCDTransmitSM (dummyptr);

}
*/
void LcdSetDDRAMSM (unsigned char addr)
{
  unsigned char b;

    switch (lcd_state_cmd)
    {
        case 0:
            //preparo la direccion
        	if (addr != 0xFF)
        	{
        		lcd_addr_cmd = addr;
        		lcd_addr_cmd |= 0x80;
				lcd_state_cmd++;
        	}
            break;

        case 1:
            //PRIMER NIBBLE
            b = lcd_addr_cmd;
            b >>= 4;
            b &= 0x0F;
            LCDClearData;
            //lcdPort |= b;
            Lcd_WritePort (b);

            //ACTIVO PULSO PRIMER NIBBLE
            LCD_E_ON;
            lcd_state_cmd++;
            break;

        case 2:
            //TERMINO PULSO DEL PRIMER NIBBLE
            LCD_E_OFF;
            lcd_state_cmd++;
            break;

        case 3:
            //SEGUNDO NIBBLE
            b = lcd_addr_cmd;
            b &= 0x0F;
            LCDClearData;
            //lcdPort |= b;
            Lcd_WritePort (b);

            //ACTIVO PULSO SEGUNDO NIBBLE
            LCD_E_ON;
            lcd_state_cmd++;
            break;

        case 4:
            //TERMINO PULSO DEL SEGUNDO NIBBLE
            LCD_E_OFF;
            lcd_state_cmd = 0;
            addr = 0xFF;
            break;

        default:
        	addr = 0xFF;
            lcd_state_cmd = 0;
            break;
    }
}

//Comandos:
//    /n    segunda linea
//    /r    primera linea
//    /t    CLEAR
//    /b    RET_HOME
//    /a    DISPLAY_ON
//    /f    DISPLAY_OFF
//    /v    CURSOR_ON
//    /'    CURSOR_OFF
//    //    BLINK_ON

void LCDTransmitSMStr(char * send)
{
    unsigned char len, lenbuff;
    len = strlen(send);
    lenbuff = strlen((char *)lcdbufftx);

    //me fijo si entra
    if ((len + lenbuff) < LCD_BUFF_TX)
    {
        //puedo concatenar
        strcat((char *)lcdbufftx, send);

        //reviso si estaba habilitado el callback
        //if (lcd_state == 0)
        //    lcd_state = 1;
    }
}

void LCDTransmitSM(void)
{
    //necesito global p2Str y lcd_state
    //maquina de estado para enviar strings
    unsigned char b = 0;

    switch (lcd_state)
    {
        case 0:
            //preparo el string y me fijo si no es un comando
            if (*pStr != 0)
            {
                //reviso que comando es:
                switch (*pStr)
                {
                    case '\n':
                        //avanzo a la proxima linea
                        lcd_state = 10;
                        lcd_addr_cmd = 0x40;
                        lcd_addr_cmd |= 0x80;
                        pStr++;
                        break;

                    case '\r':
                        //vuelvo a primera linea CLEAR:
                        lcd_state = 10;
                        lcd_addr_cmd = 0x00;
                        lcd_addr_cmd |= 0x80;
                        pStr++;
                        break;

                    case '\t':
                        //CMD CLEAR
                        lcd_state = 20;
                        lcd_addr_cmd = 0x01;    //igual al CLEAR
                        pStr++;
                        break;

                    case '\b':
                        //CMD RET_HOME
                        lcd_state = 20;
                        lcd_addr_cmd = 0x02;
                        pStr++;
                        break;

                    case '\a':
                        //CMD DISPLAY_ON
                        lcd_state = 20;
                        lcd_addr_cmd = 0x0c;
                        pStr++;
                        break;

                    case '\f':
                        //CMD DISPLAY_OFF
                        lcd_state = 20;
                        lcd_addr_cmd = 0x08;
                        pStr++;
                        break;

                    case '\v':
                        //CMD CURSOR_ON
                        lcd_state = 20;
                        lcd_addr_cmd = 0x0e;
                        pStr++;
                        break;

                    case '\'':
                        //CMD CURSOR_OFF
                        lcd_state = 20;
                        lcd_addr_cmd = 0x0c;
                        pStr++;
                        break;

                    case '\\':
                        //CMD BLINK_ON
                        lcd_state = 20;
                        lcd_addr_cmd = 0x0f;
                        pStr++;
                        break;

                    default:
                        //debe ser caracter
                        lcd_state++;
                        break;
                }
            }
            else
            {
                //llegue al final del buffer
                pStr = lcdbufftx;
                *pStr = '\0';
            }
            break;

        case 1:
            //ACTIVO RS
            LCD_RS_ON;
            //PRIMER NIBBLE
            b = *pStr;
            b >>= 4;
            LCDClearData;
            //lcdPort |= b;
            Lcd_WritePort (b);

            //ACTIVO PULSO PRIMER NIBBLE
            LCD_E_ON;
            lcd_state++;
            break;

        case 2:
            //TERMINO PULSO DEL PRIMER NIBBLE
            LCD_E_OFF;
            lcd_state++;
            break;

        case 3:
            //SEGUNDO NIBBLE
            b = *pStr;
            b &= 0x0F;
            LCDClearData;
            //lcdPort |= b;
            Lcd_WritePort (b);

            //ACTIVO PULSO SEGUNDO NIBBLE
            LCD_E_ON;
            lcd_state++;
            break;

        case 4:
            //TERMINO PULSO DEL SEGUNDO NIBBLE
            LCD_E_OFF;
            lcd_state++;
            break;

        case 5:
            //DESACTIVO RS
            LCD_RS_OFF;
            pStr++;
            lcd_state = 0;
            break;

        //CASES PARA DDRAM
        case 10:
            //PRIMER NIBBLE
            b = lcd_addr_cmd;
            b >>= 4;
            LCDClearData;
            //lcdPort |= b;
            Lcd_WritePort (b);

            //ACTIVO PULSO PRIMER NIBBLE
            LCD_E_ON;
            lcd_state++;
            break;

        case 11:
            //TERMINO PULSO DEL PRIMER NIBBLE
            LCD_E_OFF;
            lcd_state++;
            break;

        case 12:
            //SEGUNDO NIBBLE
            b = lcd_addr_cmd;
            b &= 0x0F;
            LCDClearData;
            //lcdPort |= b;
            Lcd_WritePort (b);

            //ACTIVO PULSO SEGUNDO NIBBLE
            LCD_E_ON;
            lcd_state++;
            break;

        case 13:
            //TERMINO PULSO DEL SEGUNDO NIBBLE
            LCD_E_OFF;
            lcd_state = 0;
            break;

            //CASES PARA COMMANDS
        case 20:
            //PRIMER NIBBLE
            LCDClearData;

            //ACTIVO PULSO PRIMER NIBBLE
            LCD_E_ON;
            lcd_state++;
            break;

        case 21:
            //TERMINO PULSO DEL PRIMER NIBBLE
            LCD_E_OFF;
            lcd_state++;
            break;

        case 22:
            //SEGUNDO NIBBLE
            LCDClearData;
            //lcdPort |= lcd_addr_cmd;
            Lcd_WritePort (lcd_addr_cmd);

            //ACTIVO PULSO SEGUNDO NIBBLE
            LCD_E_ON;
            lcd_state++;
            break;

        case 23:
            //TERMINO PULSO DEL SEGUNDO NIBBLE
            LCD_E_OFF;
            lcd_state = 0;
            break;

        default:
            pStr = lcdbufftx;
            *pStr = 0;
            lcd_state = 0;
            break;
    }
}
#endif	//WITH_STATE_MACHINE


/*++++++++++++++++++++++++++++++ END LCD Driver +++++++++++++++++++++++++++++*/
