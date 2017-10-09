/*
 * grouped.c
 *
 *  Created on: 07/03/2016
 *      Author: Mariano
 */

#include "grouped.h"
#include "lcd.h"
#include "main_menu.h"
#include "hard.h"
#include "stm32f0xx.h"
#include <string.h>
#include <stdio.h>

//#include "standalone.h"
#include "dmx_transceiver.h"
#include "stm32f0x_gpio.h"


/* Externals variables ---------------------------------------------------------*/
//extern volatile unsigned short grouped_timer;
//extern volatile unsigned short grouped_enable_menu_timer;
extern Grouped_Typedef GroupedStruct_constant;

extern volatile unsigned char grouped_master_timeout_timer;
extern volatile unsigned short standalone_timer;
extern volatile unsigned short standalone_enable_menu_timer;


#define grouped_timer standalone_timer
#define grouped_enable_menu_timer standalone_enable_menu_timer

extern const char * s_blank_line [];

extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned short DMX_channel_selected;
extern volatile unsigned char DMX_channel_quantity;

#define SIZEOF_DATA1	512
extern volatile unsigned char data1[];
extern volatile unsigned char data[];

/* Global variables ------------------------------------------------------------*/
unsigned char grouped_state = 0;			//TODO: esto puede compartir RAM con standalone
unsigned char grouped_selections = 0;
unsigned short grouped_ii = 0;

unsigned char grouped_dimming_last_value = 0;
unsigned char grouped_dimming_last_slope = 0;

unsigned char grouped_menu_state = 0;



const unsigned char s_sel_g [] = { 0x02, 0x08, 0x0f };

Grouped_Typedef GroupedStruct_local;
#define grouped_dimming_top GroupedStruct_local.max_dimmer_value_dmx

unsigned char grouped_slave_dim_last = 0;
unsigned char grouped_slave_dim2_last = 0;
//-------- Functions -------------


unsigned char FuncGrouped (void)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;

	switch (grouped_state)
	{
		case GROUPED_INIT:
			//un segundo la pantalla principal
			grouped_timer = 1000;
			grouped_enable_menu_timer = TT_MENU_TIMEOUT;	//30 segs de menu standalone

			//TODO: leer estructura y verificar funcion cargar valore sdefualt o ultimos seleccioneados
			memcpy(&GroupedStruct_local, &GroupedStruct_constant, sizeof(GroupedStruct_local));
			MenuGroupedReset();

		case GROUPED_UPDATE:
			//reviso si es master o slave
			if (GroupedStruct_local.grouped_mode == GROUPED_MODE_SLAVE)
			{
				EXTIOn ();
				grouped_state = GROUPED_SLAVE_INIT;
			}
			else	//es master
			{
				EXTIOff ();
				grouped_dimming_last_slope = DIM_DOWN;
				grouped_dimming_last_value = GroupedStruct_local.max_dimmer_value_dmx;
				grouped_state = GROUPED_OFF;
			}
			break;

		case GROUPED_OFF:
			if (CheckACSw() > S_NO)	//estan prendiendo o queriendo dimerizar
			{
				grouped_timer = 1500;
				grouped_state = GROUPED_OFF_1;
			}
			break;

		case GROUPED_OFF_1:
			if (CheckACSw() == S_NO)	//liberaron el sw
			{
				RELAY_ON;
				if (grouped_selections == MENU_OFF)
				{
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "Switching ON... ");
				}

				if (grouped_dimming_last_value >= ONE_TEN_INITIAL)
				{
					grouped_state = GROUPED_RISING;
					grouped_timer = 0;
				}
				else
				{
					grouped_ii = ONE_TEN_INITIAL;
					Update_TIM3_CH1 (grouped_ii);
					memset((unsigned char *) data1, (unsigned char) grouped_ii, SIZEOF_DATA1);
					SendDMXPacket(PCKT_INIT);
					grouped_master_timeout_timer = TT_MASTER_TIMEOUT;
					grouped_timer = TT_STARTING;	//tarda 940 msegs en arrancar la fuente
					grouped_state = GROUPED_OFF_2;
				}
			}
			break;

		case GROUPED_OFF_2:
			if (!grouped_timer)
			{
				//grouped_timer = TT_RISING_FALLING;
				grouped_timer = TT_RISING_FALLING_FIRST_TIME;
				if (grouped_ii != grouped_dimming_last_value)
				{
					if (grouped_ii < grouped_dimming_last_value)
						grouped_ii++;
					else
						grouped_ii--;

					memset((unsigned char *)data1, (unsigned char) grouped_ii, SIZEOF_DATA1);
					SendDMXPacket(PCKT_INIT);
					grouped_master_timeout_timer = TT_MASTER_TIMEOUT;
					Update_TIM3_CH1 (grouped_ii);
				}
				else
				{
					if (grouped_selections == MENU_OFF)
					{
						LCD_1ER_RENGLON;
						LCDTransmitStr((const char *) "  Lights ON     ");
					}
					grouped_state = GROUPED_ON;
				}
			}
			break;

		case GROUPED_RISING:
			if (!grouped_timer)
			{
				if (GroupedStruct_local.power_up_timer_value > 255)
					grouped_timer = (GroupedStruct_local.power_up_timer_value >> 8);
				else
					grouped_timer = TT_RISING_FALLING_FIRST_TIME;

				if ((grouped_ii < grouped_dimming_last_value) && (grouped_ii < GroupedStruct_local.max_dimmer_value_dmx))
				{
					Update_TIM3_CH1 (grouped_ii);
					memset((unsigned char *)data1, (unsigned char) grouped_ii, SIZEOF_DATA1);
					SendDMXPacket(PCKT_INIT);
					grouped_master_timeout_timer = TT_MASTER_TIMEOUT;
					grouped_ii++;
				}
				else
				{
					if (grouped_selections == MENU_OFF)
					{
						LCD_1ER_RENGLON;
						LCDTransmitStr((const char *) "  Lights ON     ");
					}

					grouped_state = GROUPED_ON;
				}
			}
			break;

		case GROUPED_ON:
			if (CheckACSw() > S_NO)	//estan apagando o queriendo dimerizar
			{
				grouped_timer = 1500;
				grouped_state = GROUPED_ON_1;
			}
			break;

		case GROUPED_ON_1:
			if (CheckACSw() == S_NO)	//liberaron el sw
			{
				if (grouped_timer)	//apagaron
				{
					grouped_dimming_last_value = grouped_ii;
					if (grouped_selections == MENU_OFF)
					{
//						short local;

						LCD_1ER_RENGLON;
						LCDTransmitStr((const char *) "Switching OFF...");

						//me fijo si estoy muy cerca del limite bajo
//						local = GroupedStruct_local.min_dimmer_value_dmx - grouped_ii;
//						if ((local < 10) && (local > -10))
//							grouped_dimming_last_slope = DIM_UP;
					}
					grouped_state = GROUPED_FALLING;
					grouped_timer = TT_RISING_FALLING;		//TODO guarda que me bloque el if de abajo
				}
			}

			if (!grouped_timer)		//estan dimerizando!!!
				grouped_state = GROUPED_DIMMING_LAST;

			break;

		case GROUPED_FALLING:
			if (!grouped_timer)
			{
				if (GroupedStruct_local.power_up_timer_value > 255)
					grouped_timer = (GroupedStruct_local.power_up_timer_value >> 8);
				else
					grouped_timer = TT_RISING_FALLING_FIRST_TIME;

				if (grouped_ii > 0)
				{
					Update_TIM3_CH1 (grouped_ii);
					memset((unsigned char *)data1, (unsigned char) grouped_ii, SIZEOF_DATA1);
					SendDMXPacket(PCKT_INIT);
					grouped_master_timeout_timer = TT_MASTER_TIMEOUT;
					grouped_ii--;
				}
				else
				{
					Update_TIM3_CH1 (0);
					memset((unsigned char *)data1, (unsigned char) grouped_ii, SIZEOF_DATA1);
					SendDMXPacket(PCKT_INIT);
					grouped_master_timeout_timer = TT_MASTER_TIMEOUT;
					grouped_state = GROUPED_OFF;
					if (grouped_selections == MENU_OFF)
					{
						LCD_1ER_RENGLON;
						LCDTransmitStr((const char *) "  Lights OFF    ");
					}
					RELAY_OFF;
				}
			}
			break;

		case GROUPED_DIMMING_LAST:
			if (grouped_dimming_last_slope == DIM_UP)
				grouped_state = GROUPED_DIMMING_UP;
			else
				grouped_state = GROUPED_DIMMING_DOWN;

			break;

		case GROUPED_DIMMING_UP:
			if (!grouped_timer)
			{
				if (GroupedStruct_local.dimming_up_timer_value > 255)
					grouped_timer = (GroupedStruct_local.dimming_up_timer_value >> 8);
				else
					grouped_timer = TT_RISING_FALLING;

				if (CheckACSw() > S_NO)
				{
					if (grouped_ii < GroupedStruct_local.max_dimmer_value_dmx)
					{
						grouped_ii++;
						memset((unsigned char *)data1, (unsigned char) grouped_ii, SIZEOF_DATA1);
						SendDMXPacket(PCKT_INIT);
						grouped_master_timeout_timer = TT_MASTER_TIMEOUT;
						Update_TIM3_CH1 (grouped_ii);
					}
				}
				else	//si liberaron y estoy en maximo lo doy vuelta
				{
					if (grouped_ii >= GroupedStruct_local.max_dimmer_value_dmx)
						grouped_dimming_last_slope = DIM_DOWN;
					grouped_state = GROUPED_ON;
				}
			}
			break;

		case GROUPED_DIMMING_DOWN:
			if (!grouped_timer)
			{
				if (GroupedStruct_local.dimming_up_timer_value > 255)
					grouped_timer = (GroupedStruct_local.dimming_up_timer_value >> 8);
				else
					grouped_timer = TT_RISING_FALLING;

				if (CheckACSw() > S_NO)
				{
					if (grouped_ii > GroupedStruct_local.min_dimmer_value_dmx)
					{
						grouped_ii--;
						memset((unsigned char *)data1, (unsigned char) grouped_ii, SIZEOF_DATA1);
						SendDMXPacket(PCKT_INIT);
						grouped_master_timeout_timer = TT_MASTER_TIMEOUT;
						Update_TIM3_CH1 (grouped_ii);
					}
				}
				else	//si liberaron y estoy en minimo lo doy vuelta
				{
					if (grouped_ii <= GroupedStruct_local.min_dimmer_value_dmx)
						grouped_dimming_last_slope = DIM_UP;
					grouped_state = GROUPED_ON;
				}
			}
			break;

		case GROUPED_SLAVE_INIT:
			DMX_channel_quantity = 1;
			DMX_channel_selected = GroupedStruct_local.grouped_dmx_channel;
			DMX_Ena();

			if (grouped_selections == MENU_OFF)
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "Slave Mode Working");
			}
			grouped_state++;
			break;

		case GROUPED_SLAVE_WORKING:
			//me quedo aca hasta que me saquen por menu

			if (Packet_Detected_Flag)
			{
				//llego un paquete DMX
				Packet_Detected_Flag = 0;

				//en data tengo la info
				Update_TIM3_CH1 (data[1]);
				grouped_ii = data[1];
			}


			if (grouped_ii > 10)
				RELAY_ON;
			else if (grouped_ii < 5)
				RELAY_OFF;

			if (grouped_selections == MENU_OFF)
			{
				if (grouped_slave_dim_last != grouped_ii)
				{
					unsigned char i;
					unsigned short ii;
					char s_lcd [20];

					grouped_slave_dim_last = grouped_ii;

					ii = grouped_slave_dim_last * 100;
					ii = ii / 255;
					if (ii > 100)
						ii = 100;

					LCD_2DO_RENGLON;
					Lcd_SetDDRAM(0x40 + 12);
					sprintf(s_lcd, "%3d", ii);
					LCDTransmitStr(s_lcd);
					LCDTransmitStr("%");

					ii = ii / 10;
					if (grouped_slave_dim2_last != ii)
					{
						grouped_slave_dim2_last = ii;
						LCD_2DO_RENGLON;
						LCDTransmitStr((const char *) "            ");
						LCD_2DO_RENGLON;
						for (i = 0; i < ii; i++)
						{
							LCDStartTransmit(0xff);
						}
					}
				}
			}
			break;

		default:
			grouped_state = GROUPED_INIT;
			break;
	}


	//veo el menu solo si alguien toca los botones o timeout
	switch (grouped_selections)
	{
		case MENU_ON:
			//estado normal
			resp_down = MenuGrouped();

			if (resp_down == RESP_WORKING)	//alguien esta tratando de seleccionar algo, le doy tiempo
				grouped_enable_menu_timer = TT_MENU_TIMEOUT;

			if (resp_down == RESP_SELECTED)	//se selecciono algo
			{
				grouped_enable_menu_timer = TT_MENU_TIMEOUT;
				grouped_selections++;
			}

			if (!grouped_enable_menu_timer)	//ya mostre el menu mucho tiempo, lo apago
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				grouped_selections = MENU_OFF;
			}

			break;

		case MENU_SELECTED:
			//estado algo seleccionado espero update
			resp_down = FuncShowBlink ((const char *) "Something Select", (const char *) "Updating Values", 1, BLINK_NO);

			//if ((resp_down == RESP_FINISH) && (CheckS1() == S_NO))
			if (resp_down == RESP_FINISH)
			{
				grouped_state = GROUPED_UPDATE;
				grouped_selections = MENU_ON;
			}
			break;

		case MENU_OFF:
			//estado menu apagado
			if ((CheckS1() > S_NO) || (CheckS2() > S_NO))
			{
				grouped_enable_menu_timer = TT_MENU_TIMEOUT;			//vuelvo a mostrar
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "wait to free    ");

				grouped_selections++;
			}
			break;

		case MENU_WAIT_FREE:
			if ((CheckS1() == S_NO) && (CheckS2() == S_NO))
			{
				grouped_selections = MENU_ON;
				//voy a activar el Menu
				//me fijo en ue parte del Menu estaba
				//TODO ES UNA CHANCHADA, PERO BUENO...
				if (grouped_menu_state <= GROUPED_MENU_SLAVE_ENABLE)
				{
					//salgo directo
					LCD_2DO_RENGLON;
					LCDTransmitStr((const char *) "Cont.     Select");
				}
				else
				{
					if (grouped_menu_state <= GROUPED_MENU_MOV_SENS_SELECTED_2)
					{
						grouped_menu_state = GROUPED_MENU_MOV_SENS;
					}
					else if (grouped_menu_state <= GROUPED_MENU_LDR_SELECTED_5)
					{
						grouped_menu_state = GROUPED_MENU_LDR;
					}
					else if (grouped_menu_state <= GROUPED_MENU_MAX_DIMMING_SELECTED_1)
					{
						grouped_menu_state = GROUPED_MENU_MAX_DIMMING;
					}
					else if (grouped_menu_state <= GROUPED_MENU_MIN_DIMMING_SELECTED_1)
					{
						grouped_menu_state = GROUPED_MENU_MIN_DIMMING;
					}
					else if (grouped_menu_state <= GROUPED_MENU_RAMP_ON_START_SELECTED_1)
					{
						grouped_menu_state =GROUPED_MENU_RAMP_ON_START;
					}
					else if (grouped_menu_state <= GROUPED_MENU_RAMP_ON_DIMMING_SELECTED_1)
					{
						grouped_menu_state = GROUPED_MENU_RAMP_ON_DIMMING;
					}
					else if (grouped_menu_state <= GROUPED_MENU_MASTER_ENABLE_SELECTED_2)
					{
						grouped_menu_state = GROUPED_MENU_MASTER_ENABLE;
					}
					else if (grouped_menu_state <= GROUPED_MENU_SLAVE_ENABLE_SELECTED_3)
					{
						grouped_menu_state = GROUPED_MENU_SLAVE_ENABLE;
					}
					FuncOptionsReset ();
					FuncShowSelectv2Reset ();
					FuncChangeReset ();
				}
			}
			break;

		default:
			grouped_selections = MENU_ON;
			break;
	}

	if (CheckS1() > S_HALF)
		resp = RESP_CHANGE_ALL_UP;

	//si estoy en modo master y llego timeout mando un paquete
	//OJO mando con la ultima info conocida
	if ((GroupedStruct_local.grouped_mode == GROUPED_MODE_MASTER) && (!grouped_master_timeout_timer))
	{
		SendDMXPacket(PCKT_INIT);
		grouped_master_timeout_timer = TT_MASTER_TIMEOUT;
	}


	return resp;
}


unsigned char MenuGrouped(void)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;
	unsigned char dummy = 0;

	switch (grouped_menu_state)
	{
		case GROUPED_MENU_INIT:
			//empiezo con las selecciones
			resp_down = FuncShowBlink ((const char *) "Starting Grouped", (const char *) "Selections", 1, BLINK_NO);

			if (resp_down == RESP_FINISH)
				grouped_menu_state = GROUPED_MENU_MASTER_ENABLE;
			break;

		case GROUPED_MENU_MASTER_ENABLE:
			resp_down = FuncShowSelectv2 ((const char * ) "Master Enable   ");

			if (resp_down == RESP_CHANGE)	//cambio de menu
				grouped_menu_state = GROUPED_MENU_SLAVE_ENABLE;

			if (resp_down == RESP_SELECTED)	//se eligio el menu
				grouped_menu_state = GROUPED_MENU_MASTER_ENABLE_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_SLAVE_ENABLE:
			resp_down = FuncShowSelectv2 ((const char * ) "Slave Enable   ");

			if (resp_down == RESP_CHANGE)	//cambio de menu
				grouped_menu_state = GROUPED_MENU_MASTER_ENABLE;

			if (resp_down == RESP_SELECTED)	//se eligio el menu
				grouped_menu_state = GROUPED_MENU_SLAVE_ENABLE_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_MOV_SENS:
			resp_down = FuncShowSelectv2 ((const char * ) "Move Sensor Conf");

			if (resp_down == RESP_CHANGE)	//cambio de menu
				grouped_menu_state = GROUPED_MENU_LDR;

			if (resp_down == RESP_SELECTED)	//se eligio el menu
				grouped_menu_state = GROUPED_MENU_MOV_SENS_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_LDR:
			resp_down = FuncShowSelectv2 ((const char * ) "Energy Harv Conf");

			if (resp_down == RESP_CHANGE)	//cambio de menu
				grouped_menu_state = GROUPED_MENU_MAX_DIMMING;


			if (resp_down == RESP_SELECTED)	//se eligio el menu
				grouped_menu_state = GROUPED_MENU_LDR_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_MAX_DIMMING:
			resp_down = FuncShowSelectv2 ((const char * ) "Max Dimming Conf");

			if (resp_down == RESP_CHANGE)	//cambio de menu
				grouped_menu_state = GROUPED_MENU_MIN_DIMMING;

			if (resp_down == RESP_SELECTED)	//se eligio el menu
				grouped_menu_state = GROUPED_MENU_MAX_DIMMING_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_MIN_DIMMING:
			resp_down = FuncShowSelectv2 ((const char * ) "Min Dimming Conf");

			if (resp_down == RESP_CHANGE)	//cambio de menu
				grouped_menu_state = GROUPED_MENU_RAMP_ON_START;

			if (resp_down == RESP_SELECTED)	//se eligio el menu
				grouped_menu_state = GROUPED_MENU_MIN_DIMMING_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_RAMP_ON_START:
			resp_down = FuncShowSelectv2 ((const char * ) "Ramp ON/OFF Conf");

			if (resp_down == RESP_CHANGE)	//cambio de menu
				grouped_menu_state = GROUPED_MENU_RAMP_ON_DIMMING;

			if (resp_down == RESP_SELECTED)	//se eligio el menu
				grouped_menu_state = GROUPED_MENU_RAMP_ON_START_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_RAMP_ON_DIMMING:
			resp_down = FuncShowSelectv2 ((const char * ) "Ramp on Dim Conf");

			if (resp_down == RESP_CHANGE)	//cambio de menu
				grouped_menu_state = GROUPED_MENU_MOV_SENS;

			if (resp_down == RESP_SELECTED)	//se eligio el menu
				grouped_menu_state = GROUPED_MENU_RAMP_ON_DIMMING_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;


		case GROUPED_MENU_MASTER_ENABLE_SELECTED:
			if (GroupedStruct_local.grouped_mode == GROUPED_MODE_MASTER)
				resp_down = 0x80;
			else
				resp_down = 0x81;

			FuncOptions ((const char *) "on   off   back ", (const char *) s_blank_line, (unsigned char *)s_sel_g, 3, resp_down);
			grouped_menu_state++;
			break;

		case GROUPED_MENU_MASTER_ENABLE_SELECTED_1:
			resp_down = FuncOptions ((const char *) "on   off   back ",(const char *)  s_blank_line, (unsigned char *)s_sel_g, 3, 0);

			if ((resp_down & 0x0f) == RESP_SELECTED)
			{
				resp_down = resp_down & 0xf0;
				resp_down >>= 4;
				if (resp_down == 0)
				{
					GroupedStruct_local.grouped_mode = GROUPED_MODE_MASTER;
					resp = RESP_SELECTED;
					grouped_menu_state = GROUPED_MENU_MOV_SENS;
				}

				if (resp_down == 1)
				{
					GroupedStruct_local.grouped_mode = GROUPED_MODE_SLAVE;
					resp = RESP_SELECTED;
					grouped_menu_state = GROUPED_MENU_SLAVE_ENABLE;
				}

				if (resp_down == 2)
				{
					resp = RESP_WORKING;
					grouped_menu_state++;
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "wait to free    ");
				}
			}
			break;

		case GROUPED_MENU_MASTER_ENABLE_SELECTED_2:		//si eligen back vuelvo al menu master slave
			if (CheckS2() == S_NO)
				grouped_menu_state = GROUPED_MENU_MASTER_ENABLE;

			resp = RESP_WORKING;
			break;

		case GROUPED_MENU_SLAVE_ENABLE_SELECTED:
			if (GroupedStruct_local.grouped_mode == GROUPED_MODE_SLAVE)
				resp_down = 0x80;
			else
				resp_down = 0x81;

			FuncOptions ((const char *) "on   off   back ",(const char *)  s_blank_line, (unsigned char *)s_sel_g, 3, resp_down);
			grouped_menu_state++;
			break;

		case GROUPED_MENU_SLAVE_ENABLE_SELECTED_1:
			resp_down = FuncOptions ((const char *) "on   off   back ",(const char *)  s_blank_line, (unsigned char *)s_sel_g, 3, 0);

			if ((resp_down & 0x0f) == RESP_SELECTED)
			{
				resp_down = resp_down & 0xf0;
				resp_down >>= 4;
				if (resp_down == 0)
				{
					GroupedStruct_local.grouped_mode = GROUPED_MODE_SLAVE;
					resp = RESP_SELECTED;
					grouped_menu_state = GROUPED_MENU_SLAVE_ENABLE_SELECTED_3;
				}

				if (resp_down == 1)
				{
					GroupedStruct_local.grouped_mode = GROUPED_MODE_MASTER;
					resp = RESP_SELECTED;
					grouped_menu_state = GROUPED_MENU_MASTER_ENABLE;
				}

				if (resp_down == 2)
				{
					resp = RESP_WORKING;
					grouped_menu_state++;
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "wait to free    ");
				}
			}
			break;

		case GROUPED_MENU_SLAVE_ENABLE_SELECTED_2:		//si eligen back vuelvo al menu master slave
			if (CheckS2() == S_NO)
				grouped_menu_state = GROUPED_MENU_SLAVE_ENABLE;

			resp = RESP_WORKING;
			break;

		case GROUPED_MENU_SLAVE_ENABLE_SELECTED_3:
			resp_down = FuncChangeChannels ((unsigned char *) &GroupedStruct_local.grouped_dmx_channel);

			if (resp_down == RESP_FINISH)
			{
				resp = RESP_SELECTED;
				grouped_menu_state = GROUPED_MENU_SLAVE_ENABLE;
			}
			else if (resp_down == RESP_WORKING)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_MOV_SENS_SELECTED:
			if (GroupedStruct_local.move_sensor_enable)
				resp_down = 0x80;
			else
				resp_down = 0x81;

			FuncOptions ((const char *) "on   off   back ", (const char *) s_blank_line, (unsigned char *)s_sel_g, 3, resp_down);
			grouped_menu_state++;
			break;

		case GROUPED_MENU_MOV_SENS_SELECTED_1:
			resp_down = FuncOptions ((const char *) "on   off   back ", (const char *) s_blank_line, (unsigned char *)s_sel_g, 3, 0);

			if ((resp_down & 0x0f) == RESP_SELECTED)
			{
				resp_down = resp_down & 0xf0;
				resp_down >>= 4;
				if (resp_down == 0)
				{
					GroupedStruct_local.move_sensor_enable = 1;
				}

				if (resp_down == 1)
				{
					GroupedStruct_local.move_sensor_enable = 0;
				}

				if (resp_down == 2)
				{
					resp = RESP_WORKING;
					grouped_menu_state++;
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "wait to free    ");
				}
				else
				{
					resp = RESP_SELECTED;
					grouped_menu_state = GROUPED_MENU_MOV_SENS;
				}
			}
			break;

		case GROUPED_MENU_MOV_SENS_SELECTED_2:
			if (CheckS2() == S_NO)
				grouped_menu_state = GROUPED_MENU_MOV_SENS;

			resp = RESP_WORKING;
			break;

		case GROUPED_MENU_LDR_SELECTED:
			if (GroupedStruct_local.ldr_enable)
				resp_down = 0x80;
			else
				resp_down = 0x81;

			FuncOptions ((const char *) "on   off   back ", (const char *)s_blank_line,(unsigned char *) s_sel_g, 3, resp_down);
			grouped_menu_state++;
			break;

		case GROUPED_MENU_LDR_SELECTED_1:
			resp_down = FuncOptions ((const char *) "on   off   back ", (const char *)s_blank_line, (unsigned char *)s_sel_g, 3, 0);

			if ((resp_down & 0x0f) == RESP_SELECTED)
			{
				resp_down = resp_down & 0xf0;
				resp_down >>= 4;
				if (resp_down == 0)
				{
					GroupedStruct_local.ldr_enable = 1;
					grouped_menu_state++;
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "wait to free    ");
				}

				if (resp_down == 1)
				{
					GroupedStruct_local.ldr_enable = 0;
					resp = RESP_SELECTED;
					grouped_menu_state = GROUPED_MENU_LDR;
				}

				if (resp_down == 2)
				{
					resp = RESP_WORKING;
					grouped_menu_state = GROUPED_MENU_LDR_SELECTED_4;
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "wait to free    ");
				}
			}
			break;

		case GROUPED_MENU_LDR_SELECTED_2:
			if (CheckS2() == S_NO)
				grouped_menu_state++;

			resp = RESP_WORKING;
			break;

		case GROUPED_MENU_LDR_SELECTED_3:
			//resp_down = FuncChange (&GroupedStruct_local.ldr_value);
			resp_down = FuncChangePercent (&GroupedStruct_local.ldr_value);

			if (resp_down == RESP_FINISH)
				grouped_menu_state = GROUPED_MENU_LDR_SELECTED_5;
			else if (resp_down == RESP_WORKING)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_LDR_SELECTED_4:
			if (CheckS2() == S_NO)
				grouped_menu_state = GROUPED_MENU_LDR;

			resp = RESP_WORKING;
			break;

		case GROUPED_MENU_LDR_SELECTED_5:
			resp = RESP_WORKING;
			if (CheckS2() == S_NO)
			{
				grouped_menu_state = GROUPED_MENU_LDR;
				resp = RESP_SELECTED;
			}
			break;

		case GROUPED_MENU_MAX_DIMMING_SELECTED:
			resp_down = FuncChangePercent (&GroupedStruct_local.max_dimmer_value_percent);

			if (resp_down == RESP_FINISH)
			{
				unsigned short local;
				local = GroupedStruct_local.max_dimmer_value_percent * 255;
				local = local / 100;
				GroupedStruct_local.max_dimmer_value_dmx = local;

				grouped_menu_state = GROUPED_MENU_MAX_DIMMING_SELECTED_1;
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "wait to free    ");
			}
			else if (resp_down == RESP_WORKING)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_MAX_DIMMING_SELECTED_1:
			resp = RESP_WORKING;
			if (CheckS2() == S_NO)
			{
				//hago el update si corresponde
				if (grouped_ii > GroupedStruct_local.max_dimmer_value_dmx)
				{
					grouped_ii = GroupedStruct_local.max_dimmer_value_dmx;
					//TODO: esto corresponde hacerlo aca????
					//TODO: no se puede hacer dentro de la funcion en algun UPDATE
					memset((unsigned char *)data1, (unsigned char) grouped_ii, SIZEOF_DATA1);
					SendDMXPacket(PCKT_INIT);
					Update_TIM3_CH1 (grouped_ii);
					grouped_dimming_last_slope = DIM_DOWN;
				}
				grouped_menu_state = GROUPED_MENU_MAX_DIMMING;
				resp = RESP_SELECTED;
			}
			break;

		case GROUPED_MENU_MIN_DIMMING_SELECTED:
			resp_down = FuncChangePercent (&GroupedStruct_local.min_dimmer_value_percent);

			if (resp_down == RESP_FINISH)
			{
				unsigned short local;
				local = GroupedStruct_local.min_dimmer_value_percent * 255;
				local = local / 100;
				GroupedStruct_local.min_dimmer_value_dmx = local;

				grouped_menu_state = GROUPED_MENU_MIN_DIMMING_SELECTED_1;
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "wait to free    ");
			}
			else if (resp_down == RESP_WORKING)
				resp = RESP_WORKING;

			break;

		case GROUPED_MENU_MIN_DIMMING_SELECTED_1:
			resp = RESP_WORKING;
			if (CheckS2() == S_NO)
			{
				//hago el update si corresponde
				if (grouped_ii < GroupedStruct_local.min_dimmer_value_dmx)
				{
					grouped_ii = GroupedStruct_local.min_dimmer_value_dmx;
					//TODO: esto corresponde hacerlo aca????
					//TODO: no se puede hacer dentro de la funcion en algun UPDATE
					memset((unsigned char *)data1, (unsigned char) grouped_ii, SIZEOF_DATA1);
					SendDMXPacket(PCKT_INIT);
					Update_TIM3_CH1 (grouped_ii);
					grouped_dimming_last_slope = DIM_UP;
				}

				grouped_menu_state = GROUPED_MENU_MIN_DIMMING;
				resp = RESP_SELECTED;
			}
			break;

		case GROUPED_MENU_RAMP_ON_START_SELECTED:
			//ajusto el timer
			dummy = GroupedStruct_local.power_up_timer_value / 1000;
			resp_down = FuncChangeSecs (&dummy);

			if (resp_down == RESP_FINISH)
			{
				GroupedStruct_local.power_up_timer_value = dummy * 1000;
				grouped_menu_state = GROUPED_MENU_RAMP_ON_START_SELECTED_1;
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "wait to free    ");
			}

			resp = RESP_WORKING;
			break;

		case GROUPED_MENU_RAMP_ON_START_SELECTED_1:
			resp = RESP_WORKING;
			if (CheckS2() == S_NO)
			{
				grouped_menu_state = GROUPED_MENU_RAMP_ON_START;
				resp = RESP_SELECTED;
			}
			break;

		case GROUPED_MENU_RAMP_ON_DIMMING_SELECTED:
			//ajusto el timer
			dummy = GroupedStruct_local.dimming_up_timer_value / 1000;
			resp_down = FuncChangeSecs (&dummy);

			if (resp_down == RESP_FINISH)
			{
				GroupedStruct_local.dimming_up_timer_value = dummy * 1000;
				grouped_menu_state = GROUPED_MENU_RAMP_ON_DIMMING_SELECTED_1;
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "wait to free    ");
			}

			resp = RESP_WORKING;
			break;

		case GROUPED_MENU_RAMP_ON_DIMMING_SELECTED_1:
			resp = RESP_WORKING;
			if (CheckS2() == S_NO)
			{
				grouped_menu_state = GROUPED_MENU_RAMP_ON_DIMMING;
				resp = RESP_SELECTED;
			}
			break;

		default:
			grouped_menu_state = GROUPED_MENU_INIT;
			break;
	}
	return resp;
}


void FuncGroupedReset (void)
{
	grouped_state = GROUPED_INIT;
}

void MenuGroupedReset(void)
{
	grouped_menu_state = GROUPED_MENU_INIT;
}

