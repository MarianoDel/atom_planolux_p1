/**
  ******************************************************************************
  * @file    Template_2/main.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Use this template for new projects with stm32f0xx family.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "stm32f0xx_adc.h"
//#include "stm32f0xx_can.h"
//#include "stm32f0xx_cec.h"
//#include "stm32f0xx_comp.h"
//#include "stm32f0xx_crc.h"
//#include "stm32f0xx_crs.h"
//#include "stm32f0xx_dac.h"
//#include "stm32f0xx_dbgmcu.h"
//#include "stm32f0xx_dma.h"
//#include "stm32f0xx_exti.h"
//#include "stm32f0xx_flash.h"
#include "stm32f0xx_gpio.h"
//#include "stm32f0xx_i2c.h"
//#include "stm32f0xx_iwdg.h"
#include "stm32f0xx_misc.h"
//#include "stm32f0xx_pwr.h"
#include "stm32f0xx_rcc.h"
//#include "stm32f0xx_rtc.h"
#include "stm32f0xx_spi.h"
//#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_usart.h"
//#include "stm32f0xx_wwdg.h"
#include "system_stm32f0xx.h"
#include "stm32f0xx_it.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//--- My includes ---//
#include "stm32f0x_gpio.h"
#include "stm32f0x_tim.h"
#include "stm32f0x_uart.h"

#include "hard.h"
//#include "main.h"

#include "lcd.h"

#include "core_cm0.h"
#include "adc.h"
#include "flash_program.h"
#include "main_menu.h"
#include "synchro.h"
#include "dmx_transceiver.h"
#include "standalone.h"
#include "grouped.h"
#include "networked.h"

//--- VARIABLES EXTERNAS ---//
volatile unsigned char timer_1seg = 0;

volatile unsigned short timer_led_comm = 0;
volatile unsigned short timer_for_cat_switch = 0;
volatile unsigned short timer_for_cat_display = 0;
volatile unsigned char buffrx_ready = 0;
volatile unsigned char *pbuffrx;
volatile unsigned short wait_ms_var = 0;

//volatile unsigned char TxBuffer_SPI [TXBUFFERSIZE];
//volatile unsigned char RxBuffer_SPI [RXBUFFERSIZE];
//volatile unsigned char *pspi_tx;
//volatile unsigned char *pspi_rx;
//volatile unsigned char spi_bytes_left = 0;

// ------- Externals del DMX -------
volatile unsigned char Packet_Detected_Flag;
volatile unsigned char DMX_packet_flag;
volatile unsigned char RDM_packet_flag;
volatile unsigned char dmx_receive_flag = 0;
volatile unsigned short DMX_channel_received = 0;
volatile unsigned short DMX_channel_selected = 1;
volatile unsigned char DMX_channel_quantity = 4;

volatile unsigned char data1[512];
//static unsigned char data_back[10];
volatile unsigned char data[256];

// ------- Externals de los timers -------
//volatile unsigned short prog_timer = 0;
//volatile unsigned short mainmenu_timer = 0;
volatile unsigned short show_select_timer = 0;
volatile unsigned char switches_timer = 0;
volatile unsigned char acswitch_timer = 0;

volatile unsigned short scroll1_timer = 0;
volatile unsigned short scroll2_timer = 0;

volatile unsigned short standalone_timer;
volatile unsigned short standalone_enable_menu_timer;
//volatile unsigned short standalone_menu_timer;
volatile unsigned char grouped_master_timeout_timer;

// ------- Externals de los modos -------
StandAlone_Typedef const StandAloneStruct_constant =
//StandAlone_Typedef __attribute__ ((section("memParams"))) const StandAloneStruct_constant =
		{
				.move_sensor_enable = 1,
				.ldr_enable = 0,
				.ldr_value = 100,
				.max_dimmer_value_percent = 100,
				.max_dimmer_value_dmx = 255,
				.min_dimmer_value_percent = 1,
				.min_dimmer_value_dmx = MIN_DIMMING,
				.power_up_timer_value = 3000,
				.dimming_up_timer_value = 3000
		};

Grouped_Typedef const GroupedStruct_constant =
//Grouped_Typedef __attribute__ ((section("memParams1"))) const GroupedStruct_constant =
		{
				//parte master igual a StandAlone
				.move_sensor_enable = 1,
				.ldr_enable = 0,
				.ldr_value = 100,
				.max_dimmer_value_percent = 100,
				.max_dimmer_value_dmx = 255,
				.min_dimmer_value_percent = 1,
				.min_dimmer_value_dmx = MIN_DIMMING,
				.power_up_timer_value = 3000,
				.dimming_up_timer_value = 3000,
				//parte slave
				.grouped_mode = GROUPED_MODE_SLAVE,
				.grouped_dmx_channel = GROUPED_INITIAL_CHANNEL

		};

Networked_Typedef const NetworkedStruct_constant =
//Networked_Typedef __attribute__ ((section("memParams2"))) const NetworkedStruct_constant =
		{
				//parte slave,
				.networked_dmx_channel = NETWORKED_INITIAL_CHANNEL

		};

unsigned char saved_mode;

// ------- para determinar igrid -------
unsigned short max_igrid_last;
unsigned short max_igrid;
unsigned short min_igrid_last;
unsigned short min_igrid;
unsigned short igrid_update_samples;
volatile unsigned char igrid_timer = 0;

// ------- del display LCD -------
const char s_blank_line [] = {"                "};

// ------- Externals de los switches -------
unsigned short s1;
unsigned short s2;
unsigned short sac;
unsigned char sac_aux;


//--- VARIABLES GLOBALES ---//
parameters_typedef param_struct;

// ------- de los timers -------
volatile unsigned short timer_standby;
//volatile unsigned char display_timer;
volatile unsigned char filter_timer;

//volatile unsigned char door_filter;
//volatile unsigned char take_sample;
//volatile unsigned char move_relay;
//volatile unsigned char secs = 0;
//volatile unsigned short minutes = 0;

// ------- del display -------
unsigned char v_opt [10];


// ------- del DMX -------
volatile unsigned char signal_state = 0;
volatile unsigned char dmx_timeout_timer = 0;
//unsigned short tim_counter_65ms = 0;

// ------- de los filtros DMX -------
#define LARGO_F		32
#define DIVISOR_F	5
unsigned char vd0 [LARGO_F + 1];
unsigned char vd1 [LARGO_F + 1];
unsigned char vd2 [LARGO_F + 1];
unsigned char vd3 [LARGO_F + 1];
unsigned char vd4 [LARGO_F + 1];


#define IDLE	0
#define LOOK_FOR_BREAK	1
#define LOOK_FOR_MARK	2
#define LOOK_FOR_START	3

//--- FUNCIONES DEL MODULO ---//
void TimingDelay_Decrement(void);
void Update_PWM (unsigned short);
void UpdatePackets (void);
// ------- del display -------




// ------- del DMX -------
extern void EXTI4_15_IRQHandler(void);
#define DMX_TIMEOUT	20
unsigned char MAFilter (unsigned char, unsigned char *);

//--- FILTROS DE SENSORES ---//
#define LARGO_FILTRO 16
#define DIVISOR      4   //2 elevado al divisor = largo filtro
//#define LARGO_FILTRO 32
//#define DIVISOR      5   //2 elevado al divisor = largo filtro
unsigned short vtemp [LARGO_FILTRO + 1];
unsigned short vpote [LARGO_FILTRO + 1];

//--- FIN DEFINICIONES DE FILTRO ---//


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
	unsigned char i;
	unsigned short ii;
	unsigned char sw_state = 0;
	unsigned char main_state = 0;
	unsigned char last_function;
	unsigned char last_program, last_program_deep;
	unsigned short last_channel;
	unsigned short current_temp = 0;
	char s_lcd [20];
	unsigned char last_percent = 0;
	unsigned char last_spect = 0;
	unsigned char resp = RESP_CONTINUE;
	unsigned char jump_the_menu = RESP_NO_CHANGE;

#ifdef WITH_GRANDMASTER
	unsigned short acc = 0;
	unsigned char dummy = 0;
#endif
#ifdef RGB_FOR_CAT
	unsigned char show_channels_state = 0;
	unsigned char fixed_data[2];		//la eleccion del usaario en los canales de 0 a 100
	unsigned char need_to_save = 0;
#endif
	parameters_typedef * p_mem_init;
	//!< At this stage the microcontroller clock setting is already configured,
    //   this is done through SystemInit() function which is called from startup
    //   file (startup_stm32f0xx.s) before to branch to application main.
    //   To reconfigure the default setting of SystemInit() function, refer to
    //   system_stm32f0xx.c file

	//GPIO Configuration.
	GPIO_Config();


	//ACTIVAR SYSTICK TIMER
	if (SysTick_Config(48000))
	{
		while (1)	/* Capture error */
		{
			if (LED)
				LED_OFF;
			else
				LED_ON;

			for (i = 0; i < 255; i++)
			{
				asm (	"nop \n\t"
						"nop \n\t"
						"nop \n\t" );
			}
		}
	}


	 //PRUEBA LED Y OE
	/*
	 while (1)
	 {
		 if (LED)
		 {
//			 CTRL_BKL_ON;
			 LED_OFF;
		 }
		 else
		 {
			 LED_ON;
//			 CTRL_BKL_OFF;
		 }

		 Wait_ms(150);
	 }
	 */

	 //FIN PRUEBA LED Y OE

	//TIM Configuration.
	TIM_3_Init();
	TIM_14_Init();
	TIM_16_Init();		//para OneShoot() cuando funciona en modo master
	//Timer_4_Init();

	//--- PRUEBA DISPLAY LCD ---
	EXTIOff ();

	LED_ON;
	Wait_ms(1000);
	LED_OFF;
	LCDInit();
	LED_ON;

	//--- Welcome code ---//
	Lcd_Command(CLEAR);
	Wait_ms(500);
	Lcd_Command(CURSOR_OFF);
	Wait_ms(500);
	Lcd_Command(BLINK_OFF);
	Wait_ms(50);
	CTRL_BKL_ON;

	while (FuncShowBlink ((const char *) "  PLANOLUX LLC  ", (const char *) "  Smart Driver  ", 2, BLINK_NO) != RESP_FINISH);
	LED_OFF;

	//DE PRODUCCION Y PARA PRUEBAS EN DMX
	Packet_Detected_Flag = 0;
	DMX_channel_selected = 1;
	DMX_channel_quantity = 4;
	USART1Config();
	//DMX_Disa();
	EXTIOff();

	//--- PRUEBA TX USART
	/*
	while (1)
	{
		if (LED)
			LED_OFF;
		else
			LED_ON;

		USARTSend('M');
		Wait_ms(100);
	}
	*/
	//--- FIN PRUEBA TX USART

	//RELAY_ON;

	//--- PRUEBA USART
	/*
	//EXTIOff();
	USART_Config();
	while (1)
	{
		DMX_channel_received = 0;
		data1[0] = 0;
		data1[1] = 0;
		data1[2] = 0;
		dmx_receive_flag = 1;
		USARTSend('M');
		USARTSend('E');
		USARTSend('D');
		Wait_ms(1);
		if ((data1[0] == 'M') && (data1[1] == 'E') && (data1[2] == 'D'))
			LED_ON;
		else
			LED_OFF;
		Wait_ms(200);
	}
	*/
	//--- FIN PRUEBA USART

	//--- PRUEBA USART + EXTI
	/*
	USART_Config();
	while (1)
	{
		USARTSend('M');
		Wait_ms(10);
	}
	*/
	//--- FIN PRUEBA USART + EXTI

	//--- PRUEBA PWM CH1
	/*
	while (1)
	{
		for (i = 0; i < 255; i++)
		{
			 Update_TIM3_CH1 (i);
			 Wait_ms(10);
			 if (LED)
				 LED_OFF;
			 else
				 LED_ON;
		}
	}

	while (1)
	{
		 Update_TIM3_CH1 (0);
		 Wait_ms(2000);
		 Update_TIM3_CH1 (85);
		 Wait_ms(2000);
		 Update_TIM3_CH1 (170);
		 Wait_ms(2000);
		 Update_TIM3_CH1 (255);
		 Wait_ms(10000);
	}
	*/
	//--- FIN PWM CH1

	//--- PRUEBA ADC con PWM
	/*
	if (ADC_Conf() == 0)
	{
		while (1)
		{
			if (LED)
				LED_OFF;
			else
				LED_ON;

			Wait_ms(150);
		}
	}
	while (1)
	{
		ii = ReadADC1(ADC_Channel_8);
		if (ii > 3722)
			LED_ON;
		else
			LED_OFF;
		Wait_ms(50);
		ii = ii >> 4;
		Update_TIM3_CH1 (ii);
	}
	*/
	//--- FIN ADC con PWM

	//--- PRUEBA S1 y S2 con LCD
	/*
	while (1)
	{
		if (CheckS1() > S_NO)
		{
			if (!(sw_state & 0x01))
			{
				sw_state |= 1;
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "--> S1");
			}
		}
		else
		{
			if ((sw_state & 0x01) != 0)
			{
				sw_state &= 0xfe;
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "                ");
			}
		}

		if (CheckS2() > S_NO)
		{
			if (!(sw_state & 0x10))
			{
				sw_state |= 0x10;
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *) "--> S2");
			}
		}
		else
		{
			if ((sw_state & 0x10) != 0)
			{
				sw_state &= 0xef;
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *) "                ");
			}
		}

		UpdateSwitches ();
	}
	*/
	//--- FIN PRUEBA S1 y S2 con LCD

	//--- PRUEBA SW_AC con LCD
	/*
	while (1)
	{

		if (CheckACSw() > S_HALF)
		{
			if (sw_state != 3)
			{
				sw_state = 3;
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *) "--> AC 3 segs   ");
			}
		}
		else if (CheckACSw() > S_MIN)
		{
			if (sw_state != 2)
			{
				sw_state = 2;
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *) "--> AC 1 seg    ");
			}
		}
		else if (CheckACSw() > S_NO)
		{
			if (sw_state != 1)
			{
				sw_state = 1;
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *) "--> AC < 1 seg  ");
			}
		}
		else
		{
			if (sw_state != 0)
			{
				sw_state = 0;
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *) "                ");
			}
		}

		UpdateACSwitch();

	}
	*/
	//--- FIN PRUEBA SW_AC con LCD

	//--- PRUEBA EXTI PA8 con DMX y sw a TX
	/*
	while (1)
	{
		//cuando tiene DMX mueve el LED
		EXTIOn();
		SW_RX;
		Wait_ms(200);
		EXTIOff();
		SW_TX;
		Wait_ms(200);
	}
	*/
	//--- FIN PRUEBA EXTI PA8 con DMX


	//--- PRUEBA CH0 DMX con switch de display	inicializo mas arriba USART y variables
	/*
//	DMX_Ena();

	if (ADC_Conf() == 0)
	{
		while (1)
		{
			if (LED)
				LED_OFF;
			else
				LED_ON;

			Wait_ms(150);
		}
	}

	while (1)
	{
		if (Packet_Detected_Flag)
		{
			//llego un paquete DMX
			Packet_Detected_Flag = 0;

			//en data tengo la info
			Update_TIM3_CH1 (data[0]);

			ii = data[0] * 100;
			ii = ii / 255;
			if (ii > 100)
				ii = 100;

			if (last_percent != ii)
			{
				last_percent = ii;

				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *) "            ");

				//Lcd_SetDDRAM(0x40 + 12);
				sprintf(s_lcd, "%3d", ii);
				LCDTransmitStr(s_lcd);
				LCDTransmitStr("%");

				LCD_2DO_RENGLON;
				for (i = 0; i < last_percent; i=i+10)
				{
					LCDStartTransmit(0xff);
				}
			}
		}

		UpdateSwitches ();
		UpdateIGrid();

	}
	*/
	//--- FIN PRUEBA CH0 DMX

	//--- PRUEBA CH0 DMX con switch de display	inicializo mas arriba USART y variables
	/*
	if (ADC_Conf() == 0)
	{
		while (1)
		{
			if (LED)
				LED_OFF;
			else
				LED_ON;

			Wait_ms(150);
		}
	}

	//DMX_Disa();
	while (1)
	{
		if (CheckS1() > S_NO)
		{
			sw_state = 1;
		//	RELAY_ON;
		}
		else if (CheckS2() > S_NO)
		{
			sw_state = 0;
		//	RELAY_OFF;
		}

		if (sw_state == 1)		//si tengo que estar prendido
		{
			if (Packet_Detected_Flag)
			{
				//llego un paquete DMX
				Packet_Detected_Flag = 0;

				//en data tengo la info
				Update_TIM3_CH1 (data[0]);
				//Update_TIM3_CH2 (data[1]);
				//Update_TIM3_CH3 (data[2]);
				//Update_TIM3_CH4 (data[3]);
				sprintf(s_lcd, "%03d", data[0]);
				LCD_2DO_RENGLON;
				LCDTransmitStr(s_lcd);
				sprintf(s_lcd, "  %04d", GetIGrid());
				LCDTransmitStr(s_lcd);
			}

			if (!timer_standby)
			{
				timer_standby = 1000;
				sprintf(s_lcd, "%03d", ii);
				LCD_1ER_RENGLON;
				LCDTransmitStr(s_lcd);
				if (ii < 255)
					ii++;
				else
					ii = 0;
			}
		}
		else	//apago los numeros
		{
			if (sw_state == 0)
			{
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *) "                ");
				sw_state = 2;
			}
		}

		UpdateSwitches ();
		UpdateIGrid();

	}
	*/
	//--- FIN PRUEBA CH0 DMX

	//--- PRUEBA DMX cuento paquetes
/*
	LCD_1ER_RENGLON;
	LCDTransmitStr((const char *) "Paquete numero: ");
	LCD_2DO_RENGLON;
	LCDTransmitStr((const char *) "                ");
	ii = 0;
	DMX_Ena();
	//DMX_Disa();
	while (1)
	{
		if (Packet_Detected_Flag)
		{
			Packet_Detected_Flag = 0;
			if (ii < 65532)
				ii++;
			else
				ii = 0;

			sprintf(s_lcd, "%d", ii);
			LCD_2DO_RENGLON;
			LCDTransmitStr(s_lcd);
		}

		UpdateSwitches ();

	}
*/
	//--- FIN PRUEBA DMX cuento paquetes

	//--- PRUEBA CH0 con SW_AC modo STAND_ALONE
	/*
	Wait_ms(3000);
	LCD_1ER_RENGLON;
	LCDTransmitStr((const char *) "  Lights OFF    ");
	LCD_2DO_RENGLON;
	LCDTransmitStr((const char *) "  PLANOLUX LLC  ");

	RELAY_ON;

	if (ADC_Conf() == 0)
	{
		while (1)
		{
			if (LED)
				LED_OFF;
			else
				LED_ON;

			Wait_ms(150);
		}
	}

	Update_TIM3_CH1 (0);
	sw_state = 1;
	while (1)
	{
		switch (sw_state)
		{
			case 1:
				if (CheckACSw() > S_NO)
					sw_state = 2;

				break;

			case 2:
				if (CheckACSw() > S_HALF)
					sw_state = 10;

				if (CheckACSw() == S_NO)
					sw_state = 3;

				break;

			case 3:	//aca lo prendo
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "Switching ON... ");
				ii = 51;
				sw_state++;
				break;

			case 4:	//filtro de subida
				if (!timer_standby)
				{
					timer_standby = 20;
					if (ii < 255)
					{
						Update_TIM3_CH1 (ii);
						ii++;
					}
					else
					{
						Update_TIM3_CH1 (255);
						LCD_1ER_RENGLON;
						LCDTransmitStr((const char *) "  Lights ON     ");
						sw_state = 5;
					}
				}
				break;

			case 5:	//aca me trabo prendido
				if (CheckACSw() > S_NO)
					sw_state = 6;

				break;

			case 6:
				if (CheckACSw() > S_HALF)
					sw_state = 10;

				if (CheckACSw() == S_NO)
					sw_state = 7;

				break;

			case 7:	//aca lo apago
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "Switching OFF...");
				ii = 255;
				sw_state++;
				break;

			case 8:	//filtro de bajada
				if (!timer_standby)
				{
					timer_standby = 20;
					if (ii > 0)
					{
						Update_TIM3_CH1 (ii);
						ii--;
					}
					else
					{
						Update_TIM3_CH1 (0);
						sw_state = 1;
						LCD_1ER_RENGLON;
						LCDTransmitStr((const char *) "  Lights OFF    ");
					}
				}
				break;

			case 10:
				break;

		}
		UpdateSwitches ();
		UpdateIGrid();
		UpdateACSwitch();

	}
	*/
	//--- FIN PRUEBA CH0 con SW_AC modo STAND_ALONE

	//--- PRUEBA ONE SHOOT TIMER 16
	/*
	DMX_TX_PIN_OFF;
	SW_TX;
	TIM_16_Init();
	LED_OFF;
	ii = 0;

	while (1)
	{
		if (!timer_standby)
		{
			timer_standby = 40;	//transmito cada 40ms

			if (ii == 0)
			{
				DMX_TX_PIN_ON;
				OneShootTIM16(1000);
				ii = 1;
			}
			else
			{
				DMX_TX_PIN_ON;
				OneShootTIM16(4000);
				ii = 0;
			}
		}
	}
	*/
	//--- FIN PRUEBA ONE SHOOT TIMER 16

	//--- PRUEBA HARDWARE DE MASTER EN CH0 DMX inicializo mas arriba USART y variables
	/*
	DMX_TX_PIN_OFF;
	SW_TX;
	TIM_16_Init();
	LED_OFF;
	ii = 0;
	data1[0] = 0;

	while (1)
	{
		if (!timer_standby)
		{
			timer_standby = 40;	//transmito cada 40ms


			data1[1] = ii;

			SendDMXPacket(PCKT_INIT);
			if (ii < 256)
				ii++;
			else
				ii = 0;

			//UsartSendDMX();
		}
	}
	*/
	//--- FIN PRUEBA HARDWARE MASTER CH0 DMX

	//--- PRUEBA FUNCION MAIN_MENU
	//leo la memoria, si tengo configuracion de modo
	//entro directo, sino a Main Menu
	if (saved_mode == 0xFF)	//memoria borrada
		main_state = MAIN_INIT;
	else
		jump_the_menu = RESP_YES;

	Update_TIM3_CH2 (255);
	//Wait_ms(2000);
	while (1)
	{
		switch (main_state)
		{
			case MAIN_INIT:
				resp = FuncMainMenu();

				if (resp == MAINMENU_SHOW_STANDALONE_SELECTED)	//TODO deberia forzar init
					main_state = MAIN_STAND_ALONE;

				if (resp == MAINMENU_SHOW_GROUPED_SELECTED)
					main_state = MAIN_GROUPED;

				if (resp == MAINMENU_SHOW_NETWORK_SELECTED)
					main_state = MAIN_NETWORKED;

				jump_the_menu = RESP_NO;
				break;

			case MAIN_STAND_ALONE:
				resp = FuncStandAlone();

				if (resp == RESP_CHANGE_ALL_UP)
				{
					FuncStandAloneReset();
					main_state = MAIN_INIT;
				}

				break;

			case MAIN_GROUPED:
				resp = FuncGrouped();

				if (resp == RESP_CHANGE_ALL_UP)
				{
					FuncGroupedReset();
					main_state = MAIN_INIT;
				}

				break;

			case MAIN_NETWORKED:
				resp = FuncNetworked(jump_the_menu);
				jump_the_menu = RESP_NO_CHANGE;
				main_state++;
				break;

			case MAIN_NETWORKED_1:
				resp = FuncNetworked(jump_the_menu);

				if (resp == RESP_CHANGE_ALL_UP)
					main_state = MAIN_INIT;

				break;

			default:
				main_state = MAIN_INIT;
				break;

		}

		UpdateSwitches();
		UpdateACSwitch();
		UpdatePackets();


	//--- FIN PRUEBA FUNCION MAIN_MENU

	}	//termina while(1)
	return 0;
}

void UpdatePackets (void)
{
	if (Packet_Detected_Flag)
	{
		if (data[0] == 0x00)
			DMX_packet_flag = 1;

		if (data[0] == 0xCC)
			RDM_packet_flag = 1;

		Packet_Detected_Flag = 0;
	}
}
//--- End of Main ---//
void Update_PWM (unsigned short pwm)
{
	Update_TIM3_CH1 (pwm);
	Update_TIM3_CH2 (4095 - pwm);
}


unsigned short Get_Temp (void)
{
	unsigned short total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	vtemp[LARGO_FILTRO] = ReadADC1 (CH_IN_TEMP);
    for (j = 0; j < (LARGO_FILTRO); j++)
    {
    	total_ma += vtemp[j + 1];
    	vtemp[j] = vtemp[j + 1];
    }

    return total_ma >> DIVISOR;
}

unsigned char MAFilter (unsigned char new_sample, unsigned char * vsample)
{
	unsigned short total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	*(vsample + LARGO_F) = new_sample;

    for (j = 0; j < (LARGO_F); j++)
    {
    	total_ma += *(vsample + j + 1);
    	*(vsample + j) = *(vsample + j + 1);
    }

    return total_ma >> DIVISOR_F;
}

unsigned short MAFilter16 (unsigned char new_sample, unsigned char * vsample)
{
	unsigned short total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	*(vsample + LARGO_F) = new_sample;

    for (j = 0; j < (LARGO_F); j++)
    {
    	total_ma += *(vsample + j + 1);
    	*(vsample + j) = *(vsample + j + 1);
    }

    return total_ma >> DIVISOR_F;
}





void EXTI4_15_IRQHandler(void)
{
	unsigned short aux;


	if(EXTI->PR & 0x0100)	//Line8
	{

		//si no esta con el USART detecta el flanco	PONER TIMEOUT ACA?????
		if ((dmx_receive_flag == 0) || (dmx_timeout_timer == 0))
		//if (dmx_receive_flag == 0)
		{
			switch (signal_state)
			{
				case IDLE:
					if (!(DMX_INPUT))
					{
						//Activo timer en Falling.
						TIM14->CNT = 0;
						TIM14->CR1 |= 0x0001;
						signal_state++;
					}
					break;

				case LOOK_FOR_BREAK:
					if (DMX_INPUT)
					{
						//Desactivo timer en Rising.
						aux = TIM14->CNT;

						//reviso BREAK
						//if (((tim_counter_65ms) || (aux > 88)) && (tim_counter_65ms <= 20))
						if ((aux > 87) && (aux < 210))	//Consola STARLET 6
						//if ((aux > 87) && (aux < 2000))		//Consola marca CODE tiene break 1.88ms
						{
							LED_ON;
							//Activo timer para ver MARK.
							//TIM2->CNT = 0;
							//TIM2->CR1 |= 0x0001;

							signal_state++;
							//tengo el break, activo el puerto serie
							DMX_channel_received = 0;
							//dmx_receive_flag = 1;

							dmx_timeout_timer = DMX_TIMEOUT;		//activo el timer cuando prendo el puerto serie
							//USARTx_RX_ENA;
						}
						else	//falso disparo
							signal_state = IDLE;
					}
					else	//falso disparo
						signal_state = IDLE;

					TIM14->CR1 &= 0xFFFE;
					break;

				case LOOK_FOR_MARK:
					if ((!(DMX_INPUT)) && (dmx_timeout_timer))	//termino Mark after break
					{
						//ya tenia el serie habilitado
						//if ((aux > 7) && (aux < 12))
						dmx_receive_flag = 1;
					}
					else	//falso disparo
					{
						//termine por timeout
						dmx_receive_flag = 0;
						//USARTx_RX_DISA;
					}
					signal_state = IDLE;
					LED_OFF;
					break;

				default:
					signal_state = IDLE;
					break;
			}
		}

		EXTI->PR |= 0x0100;
	}
}

void TimingDelay_Decrement(void)
{
	if (wait_ms_var)
		wait_ms_var--;

//	if (display_timer)
//		display_timer--;

	if (timer_standby)
		timer_standby--;

	if (switches_timer)
		switches_timer--;

	if (acswitch_timer)
		acswitch_timer--;

	if (dmx_timeout_timer)
		dmx_timeout_timer--;

//	if (prog_timer)
//		prog_timer--;

//	if (take_sample)
//		take_sample--;

	if (filter_timer)
		filter_timer--;

	if (igrid_timer)
		igrid_timer--;

	if (grouped_master_timeout_timer)
		grouped_master_timeout_timer--;

	if (show_select_timer)
		show_select_timer--;

	if (scroll1_timer)
		scroll1_timer--;

	if (scroll2_timer)
		scroll2_timer--;

	if (standalone_timer)
		standalone_timer--;

//	if (standalone_menu_timer)
//		standalone_menu_timer--;

	if (standalone_enable_menu_timer)
		standalone_enable_menu_timer--;

	/*
	//cuenta 1 segundo
	if (button_timer_internal)
		button_timer_internal--;
	else
	{
		if (button_timer)
		{
			button_timer--;
			button_timer_internal = 1000;
		}
	}
	*/
}





