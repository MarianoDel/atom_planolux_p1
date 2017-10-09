/*
 * main_menu.c
 *
 *  Created on: 11/02/2016
 *      Author: Mariano
 */

#include "main_menu.h"
#include "hard.h"
#include "lcd.h"
#include "standalone.h"
#include <stdio.h>
#include <string.h>

/* Externals variables ---------------------------------------------------------*/
//extern volatile unsigned short mainmenu_timer;
extern volatile unsigned short show_select_timer;

extern volatile unsigned short scroll1_timer;
extern volatile unsigned short scroll2_timer;

/* Global variables ------------------------------------------------------------*/
unsigned char mainmenu_state = 0;
unsigned char mainmenu_repeat = 0;
unsigned char show_select_state = 0;
unsigned char options_state = 0;
unsigned char options_curr_sel = 0;

unsigned char scroll1_state = 0;
unsigned char scroll1_last_window = 0;
unsigned char scroll1_current_window = 0;

unsigned char scroll2_state = 0;
unsigned char scroll2_last_window = 0;
unsigned char scroll2_current_window = 0;

unsigned char blinking_state = 0;
unsigned char blinking_how_many = 0;

unsigned char change_state = 0;
unsigned char change_current_val = 0;
unsigned char change_last_option = 0;

extern const char * s_blank_line [];

const unsigned char s_sel_up_down [] = { 0x02, 0x08, 0x0f };
//-------- Functions -------------

//funcion de seleccion del menu principal
//devuelve nueva selección o estado anterior
unsigned char FuncMainMenu (void)
{
	unsigned char resp = MAINMENU_CONTINUE;
	unsigned char resp_down = 0;

	switch (mainmenu_state)
	{
		case MAINMENU_INIT:
			resp_down = FuncShowBlink ((const char *) "    Entering    ", (const char *) "   Main Menu    ", 3, BLINK_DIRECT);

			if (resp_down == RESP_FINISH)
				mainmenu_state = MAINMENU_SHOW_STANDALONE;
			break;

		case MAINMENU_INIT_1:
			/*
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) "    Entering    ");
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "   Main Menu    ");
			mainmenu_timer = 500;
			mainmenu_state++;
			*/
			break;

		case MAINMENU_INIT_2:
			/*
			if (!mainmenu_timer)
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr(s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr(s_blank_line);
				mainmenu_timer = 500;
				mainmenu_state++;
			}
			*/
			break;

		case MAINMENU_INIT_3:
			/*
			if (!mainmenu_timer)
			{
				if (mainmenu_repeat)
				{
					mainmenu_repeat--;
					mainmenu_state = MAINMENU_INIT_1;
				}
				else
					mainmenu_state = MAINMENU_SHOW_STANDALONE;
			}
			*/
			break;

		case MAINMENU_SHOW_STANDALONE:
			resp_down = FuncShowSelectv2((const char *) "Stand Alone     ");

			if (resp_down == RESP_CHANGE)
				mainmenu_state = MAINMENU_SHOW_GROUPED;

			if (resp_down == RESP_SELECTED)
				mainmenu_state = MAINMENU_SHOW_STANDALONE_SELECTED;

			if (resp_down == RESP_CHANGE_ALL_UP)
				mainmenu_state = MAINMENU_INIT;

			break;

		case MAINMENU_SHOW_GROUPED:
			resp_down = FuncShowSelectv2((const char *) "Grouped         ");

			if (resp_down == RESP_CHANGE)
				mainmenu_state = MAINMENU_SHOW_NETWORK;

			if (resp_down == RESP_SELECTED)
				mainmenu_state = MAINMENU_SHOW_GROUPED_SELECTED;

			if (resp_down == RESP_CHANGE_ALL_UP)
				mainmenu_state = MAINMENU_INIT;

			break;

		case MAINMENU_SHOW_NETWORK:
			resp_down = FuncShowSelectv2((const char *) "Network         ");

			if (resp_down == RESP_CHANGE)
				mainmenu_state = MAINMENU_SHOW_STANDALONE;

			if (resp_down == RESP_SELECTED)
				mainmenu_state = MAINMENU_SHOW_NETWORK_SELECTED;

			if (resp_down == RESP_CHANGE_ALL_UP)
				mainmenu_state = MAINMENU_INIT;

			break;


		case MAINMENU_SHOW_STANDALONE_SELECTED:
			FuncShowBlink ((const char *) "Stand Alone     ", (const char *) "Selected...     ", 0, BLINK_NO);
			/*
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) "Stand Alone     ");
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Selected...     ");
			*/
			mainmenu_state++;
			break;

		case MAINMENU_SHOW_STANDALONE_SELECTED_1:
			if (CheckS2() == S_NO)
			{
				resp = MAINMENU_SHOW_STANDALONE_SELECTED;
				mainmenu_state = MAINMENU_INIT;
			}
			break;

		case MAINMENU_SHOW_GROUPED_SELECTED:
			FuncShowBlink ((const char *) "Grouped         ", (const char *) "Selected...     ", 0, BLINK_NO);
			/*
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) "Grouped         ");
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Selected...     ");
			*/
			mainmenu_state++;
			break;

		case MAINMENU_SHOW_GROUPED_SELECTED_1:
			if (CheckS2() == S_NO)
			{
				resp = MAINMENU_SHOW_GROUPED_SELECTED;
				mainmenu_state = MAINMENU_INIT;
			}
			break;

		case MAINMENU_SHOW_NETWORK_SELECTED:
			FuncShowBlink ((const char *) "Networked       ", (const char *) "Selected...     ", 0, BLINK_NO);
			/*
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) "Networked       ");
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Selected...     ");
			*/
			mainmenu_state++;
			break;

		case MAINMENU_SHOW_NETWORK_SELECTED_1:
			if (CheckS2() == S_NO)
			{
				resp = MAINMENU_SHOW_NETWORK_SELECTED;
				mainmenu_state = MAINMENU_INIT;
			}
			break;

		default:
			mainmenu_state = MAINMENU_INIT;
			break;
	}
	return resp;
}

//funcion que muestra los string de renglon 1 y renglon 2
//recibe puntero a primer renglon
//puntero a segundo renglon
//how many cantidad de pantallas en blinking
//modo BLINK_DIRECT o BLINK_CROSS
//si modo es BLINK_NO en how_many me pasan los segundos antes de terminar
unsigned char FuncShowBlink (const char * p_text1, const char * p_text2, unsigned char how_many, unsigned char mode)
{
    unsigned char resp = RESP_CONTINUE;

    switch (blinking_state)
    {
        case BLINKING_INIT:
            if (mode == BLINK_DIRECT)
                blinking_state = BLINKING_MARK_D;
            else if (mode == BLINK_CROSS)
                blinking_state = BLINKING_MARK_C;
            else
                blinking_state = BLINKING_MARK_N;

//            if (!how_many)
//                how_many = 1;

            blinking_how_many = how_many;
            show_select_timer = 0;
            break;

        case BLINKING_MARK_D:
            if (!show_select_timer)
            {
                if (blinking_how_many)
                {
                	blinking_how_many--;
                    LCD_1ER_RENGLON;
                    LCDTransmitStr(p_text1);
                    LCD_2DO_RENGLON;
                    LCDTransmitStr(p_text2);
                    show_select_timer = 1000;
                    blinking_state = BLINKING_SPACE_D;
                }
                else
                {
                	blinking_state = BLINKING_INIT;
                	resp = RESP_FINISH;
                }
            }
            break;

        case BLINKING_SPACE_D:
            if (!show_select_timer)
            {
                LCD_1ER_RENGLON;
                LCDTransmitStr((const char *)s_blank_line);
                LCD_2DO_RENGLON;
                LCDTransmitStr((const char *)s_blank_line);
                show_select_timer = 600;
                blinking_state = BLINKING_MARK_D;
            }
            break;

        case BLINKING_MARK_C:
            if (!show_select_timer)
            {
                if (blinking_how_many)
                {
                	blinking_how_many--;
                    LCD_1ER_RENGLON;
                    LCDTransmitStr(p_text1);
                    LCD_2DO_RENGLON;
                    LCDTransmitStr((const char *)s_blank_line);
                    show_select_timer = 1000;
                    blinking_state = BLINKING_SPACE_C;
                }
                else
                {
                    LCD_1ER_RENGLON;
                    LCDTransmitStr((const char *) s_blank_line);
                    blinking_state = BLINKING_INIT;
                    resp = RESP_FINISH;
                }
            }
            break;

        case BLINKING_SPACE_C:
            if (!show_select_timer)
            {
                LCD_1ER_RENGLON;
                LCDTransmitStr((const char *) s_blank_line);
                LCD_2DO_RENGLON;
                LCDTransmitStr(p_text2);
                show_select_timer = 1000;
                blinking_state = BLINKING_MARK_C;
            }
            break;

        case BLINKING_MARK_N:
            LCD_1ER_RENGLON;
            LCDTransmitStr(p_text1);
            LCD_2DO_RENGLON;
            LCDTransmitStr(p_text2);
            show_select_timer = 1000 * how_many;
            blinking_state = BLINKING_SPACE_N;
            break;

        case BLINKING_SPACE_N:
            if (!show_select_timer)
            {
            	blinking_state = BLINKING_INIT;
                resp = RESP_FINISH;
            }
            break;

        default:
        	blinking_state = BLINKING_INIT;
            resp = RESP_FINISH;
            break;
    }
    return resp;
}

//funcion que muestra el string enviado en formato de menu
unsigned char FuncShowSelect (const char * p_text)
{
	unsigned char resp = RESP_CONTINUE;

	switch (show_select_state)
	{
		case SHOW_SELECT_INIT:
			show_select_state++;
			break;

		case SHOW_SELECT_1:
			LCD_1ER_RENGLON;
			LCDTransmitStr(p_text);
			show_select_timer = 1000;
			show_select_state++;
			break;

		case SHOW_SELECT_2:
			if (!show_select_timer)
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				show_select_timer = 500;
				show_select_state++;
			}

			//check s1 y s2
			if (CheckS1() > S_NO)
				show_select_state = SHOW_SELECT_CHANGE;

			if (CheckS2() > S_NO)
				show_select_state = SHOW_SELECT_SELECTED;

			break;

		case SHOW_SELECT_3:
			if (!show_select_timer)
			{
				show_select_state = SHOW_SELECT_1;
			}

			//check s1 y s2
			if (CheckS1() > S_NO)
				show_select_state = SHOW_SELECT_CHANGE;

			if (CheckS2() > S_NO)
				show_select_state = SHOW_SELECT_SELECTED;

			break;

		case SHOW_SELECT_SELECTED:
			LCD_1ER_RENGLON;
			LCDTransmitStr(p_text);
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Selected...     ");
			show_select_state++;
			break;

		case SHOW_SELECT_SELECTED_1:
			if (CheckS2() == S_NO)
			{
				resp = RESP_SELECTED;
				show_select_state = SHOW_SELECT_INIT;
			}
			break;

		case SHOW_SELECT_CHANGE:
			LCD_1ER_RENGLON;
			LCDTransmitStr(p_text);
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Changing...     ");
			show_select_state++;
			break;

		case SHOW_SELECT_CHANGE_1:
			if (CheckS1() == S_NO)
			{
				resp = RESP_CHANGE;
				show_select_state = SHOW_SELECT_INIT;
			}

			if (CheckS1() > S_HALF)
			{
				resp = RESP_CHANGE_ALL_UP;
				show_select_state = SHOW_SELECT_INIT;
			}
			break;

		default:
			show_select_state = SHOW_SELECT_INIT;
			break;
	}

	return resp;
}

void FuncShowSelectv2Reset (void)
{
	show_select_state = SHOW_SELECT_INIT;
}

//funcion que muestra el string enviado en formato de menu
//ademas v2 agrega el cont o select en el segundo renglon
unsigned char FuncShowSelectv2 (const char * p_text)
{
	unsigned char resp = RESP_CONTINUE;

	switch (show_select_state)
	{
		case SHOW_SELECT_INIT:
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Cont.     Select");
			show_select_state++;
			break;

		case SHOW_SELECT_1:
			LCD_1ER_RENGLON;
			LCDTransmitStr(p_text);
			show_select_timer = 1000;
			show_select_state++;
			break;

		case SHOW_SELECT_2:
			if (!show_select_timer)
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) s_blank_line);
				show_select_timer = 500;
				show_select_state++;
			}

			//check s1 y s2
			if (CheckS1() > S_NO)
				show_select_state = SHOW_SELECT_CHANGE;

			if (CheckS2() > S_NO)
				show_select_state = SHOW_SELECT_SELECTED;

			break;

		case SHOW_SELECT_3:
			if (!show_select_timer)
			{
				show_select_state = SHOW_SELECT_1;
			}

			//check s1 y s2
			if (CheckS1() > S_NO)
				show_select_state = SHOW_SELECT_CHANGE;

			if (CheckS2() > S_NO)
				show_select_state = SHOW_SELECT_SELECTED;

			break;

		case SHOW_SELECT_SELECTED:
			LCD_1ER_RENGLON;
			LCDTransmitStr(p_text);
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Selected...     ");
			show_select_state++;
			break;

		case SHOW_SELECT_SELECTED_1:
			if (CheckS2() == S_NO)
			{
				resp = RESP_SELECTED;
				show_select_state = SHOW_SELECT_INIT;
			}
			break;

		case SHOW_SELECT_CHANGE:
			LCD_1ER_RENGLON;
			LCDTransmitStr(p_text);
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Changing...     ");
			show_select_state++;
			break;

		case SHOW_SELECT_CHANGE_1:
			if (CheckS1() == S_NO)
			{
				resp = RESP_CHANGE;
				show_select_state = SHOW_SELECT_INIT;
			}

			if (CheckS1() > S_HALF)
			{
				resp = RESP_CHANGE_ALL_UP;
				show_select_state = SHOW_SELECT_INIT;
			}
			break;

		default:
			show_select_state = SHOW_SELECT_INIT;
			break;
	}

	return resp;
}

//recibe el primer renglon y el segundo
//recibe un puntero a las posiciones de memoria de los asteriscos
//recibe una variable de catidad de opciones y otra variable con la primera opcion a la que apunta
//devuelve RESP_CONTINUE o (RESP_SELECTED | opcion_elegida << 4)
unsigned char FuncOptions (const char * p_text1, const char * p_text2, unsigned char * p_sel, unsigned char opts, unsigned char first_option)
{
	unsigned char resp = RESP_CONTINUE;

	if (first_option & 0x80)	//me piden que elija una opcion especial
	{
		if ((first_option & 0x7F) < opts)	//no esta fuera de escala
		{
			if (options_state == OPTIONS_INIT)
				options_curr_sel = (first_option & 0x7F);
			else
			{
				if (options_curr_sel != (first_option & 0x7F)) //fuerzo el cambio, ya estaba mostrando la pantalla
					options_state = OPTIONS_CHANGE_SELECT;
			}
		}
		else
			options_curr_sel = 0;
	}

	switch (options_state)
	{
		case OPTIONS_INIT:
			LCD_1ER_RENGLON;
			LCDTransmitStr(p_text1);
			LCD_2DO_RENGLON;
			LCDTransmitStr(p_text2);
			show_select_timer = 0;
			options_state++;
			break;

		case OPTIONS_WAIT_SELECT:
			if (!show_select_timer)
			{
				Lcd_SetDDRAM(*(p_sel + options_curr_sel));
				LCDTransmitStr("*");
				show_select_timer = 1000;
				options_state++;
			}
			break;

		case OPTIONS_WAIT_SELECT_1:
			if (CheckS1() > S_NO)
			{
				options_state = OPTIONS_CHANGE_SELECT;
			}

			if (CheckS2() > S_NO)
			{
				resp = (RESP_SELECTED | (options_curr_sel << 4));
				options_state = OPTIONS_INIT;
			}

			if (!show_select_timer)
			{
				Lcd_SetDDRAM(*(p_sel + options_curr_sel));
				LCDTransmitStr(" ");
				show_select_timer = 500;
				options_state = OPTIONS_WAIT_SELECT_3;
			}
			break;

		case OPTIONS_WAIT_SELECT_2:
			if (CheckS1() == S_NO)
			{
				options_state = OPTIONS_WAIT_SELECT_1;
			}
			break;

		case OPTIONS_WAIT_SELECT_3:
			if (CheckS1() > S_NO)
			{
				options_state = OPTIONS_CHANGE_SELECT;
			}

			if (CheckS2() > S_NO)
			{
				resp = (RESP_SELECTED | (options_curr_sel << 4));
				options_state = OPTIONS_INIT;
			}

			if (!show_select_timer)
			{
				Lcd_SetDDRAM(*(p_sel + options_curr_sel));
				LCDTransmitStr("*");
				show_select_timer = 1000;
				options_state = OPTIONS_WAIT_SELECT_1;
			}
			break;

		case OPTIONS_CHANGE_SELECT:
			Lcd_SetDDRAM(*(p_sel + options_curr_sel));
			LCDTransmitStr(" ");

			if (first_option & 0x80)	//me piden que elija una opcion especial
				options_curr_sel = (first_option & 0x7F);
			else
			{
				if (options_curr_sel < (opts - 1))
					options_curr_sel++;
				else
					options_curr_sel = 0;
			}

			Lcd_SetDDRAM(*(p_sel + options_curr_sel));
			LCDTransmitStr("*");

			options_state = OPTIONS_WAIT_SELECT_2;
			break;

		default:
			options_state = OPTIONS_INIT;
			break;
	}

	return resp;
}

void FuncOptionsReset (void)
{
	options_state = OPTIONS_INIT;
}

//recibe el valor original para arrancar seleccion
//recibe el modo CHANGE_PERCENT, CHANGE_SECS o CHANGE_CHANNELS Ademas puede tener |CHANGE_RESET
//recibe min val permitido, MAX val permitido
//devuelve RESP_CONTINUE o RESP_FINISH si termino la seleccion
unsigned char FuncChange (unsigned char * p_orig_value, unsigned char mode, unsigned char min_val, unsigned char max_val)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;
	char s_current [20];

	switch (change_state)
	{
		case CHANGE_INIT:
			change_current_val = *p_orig_value;
			change_last_option = 0;
			FuncOptionsReset();
			change_state++;
			break;

		case CHANGE_WAIT_SELECT:
			memset(s_current, ' ', sizeof(s_current));
			if (mode == CHANGE_PERCENT)
			{
				sprintf(s_current, "chg  %3d", change_current_val);
				strcat(s_current, (const char*)"%   sel");
			}
			else if (mode == CHANGE_SECS)
			{
				sprintf(s_current, "chg %2d", change_current_val);
				strcat(s_current, (const char*)" secs sel");
			}
			else	//debe ser CHANNELS
			{
				sprintf(s_current, "chg   %3d", change_current_val);
				strcat(s_current, (const char*)"ch  sel");
			}

			resp_down = FuncOptions ((const char *) "up  down   done ", s_current,(unsigned char *) s_sel_up_down, 3, change_last_option);
			change_last_option = 0;

			if ((resp_down & 0x0f) == RESP_SELECTED)
			{
				resp_down = resp_down & 0xf0;
				resp_down >>= 4;
				if (resp_down == 0)
				{
					if (change_current_val < max_val)
						change_current_val++;

					resp = RESP_WORKING;
				}

				if (resp_down == 1)
				{
					if (change_current_val > min_val)
						change_current_val--;

					change_last_option = (1 | 0x80);	//fuerzo update de la opcion
					resp = RESP_WORKING;
				}

				if (resp_down == 2)
				{
					change_state = CHANGE_INIT;
					resp = RESP_FINISH;
					*p_orig_value = change_current_val;
				}
			}
			break;

		default:
			change_state = CHANGE_INIT;
			break;
	}

	return resp;
}

void FuncChangeReset (void)
{
	change_state = CHANGE_INIT;
}


//hace un scroll en el primer renglon del lcd
//recibe un puntero al string
//devuelve RESP_CONTINUE o RESP_FINISH
unsigned char FuncScroll1 (const char * p_text)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char last_window;
	unsigned char i;

	switch (scroll1_state)
	{
		case SCROLL_INIT:
			scroll1_last_window = strlen(p_text) + 32;
			scroll1_current_window = 1;
			scroll1_state++;
			break;

		case SCROLL_SENDING:
			if (!scroll1_timer)
			{
				last_window = scroll1_current_window + 16;
				LCD_1ER_RENGLON;

				for (i = scroll1_current_window; i < last_window; i++)
				{
					if (i < 16)
						LCDStartTransmit(' ');
					else if (i < (scroll1_last_window - 16))
						LCDStartTransmit(*(p_text + (i - 16)));
					else if (i < scroll1_last_window)
						LCDStartTransmit(' ');
					else
					{
						//termine de enviar
						i = last_window;
						scroll1_state++;
					}
				}
				scroll1_current_window++;
				scroll1_timer = 550;
			}
			break;

		case SCROLL_FINISH:
			resp = RESP_FINISH;
			scroll1_state = SCROLL_INIT;
			break;

		default:
			resp = RESP_FINISH;
			scroll1_state = SCROLL_INIT;
			break;
	}

	return resp;
}

//hace un scroll en el segundo renglon del lcd
//recibe un puntero al string
//devuelve RESP_CONTINUE o RESP_FINISH
unsigned char FuncScroll2 (const char * p_text)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char last_window;
	unsigned char i;

	switch (scroll2_state)
	{
		case SCROLL_INIT:
			scroll2_last_window = strlen(p_text) + 32;
			scroll2_current_window = 1;
			scroll2_state++;
			break;

		case SCROLL_SENDING:
			if (!scroll2_timer)
			{
				last_window = scroll2_current_window + 16;
				LCD_2DO_RENGLON;

				for (i = scroll2_current_window; i < last_window; i++)
				{
					if (i < 16)
						LCDStartTransmit(' ');
					else if (i < (scroll2_last_window - 16))
						LCDStartTransmit(*(p_text + (i - 16)));
					else if (i < scroll2_last_window)
						LCDStartTransmit(' ');
					else
					{
						//termine de enviar
						i = last_window;
						scroll2_state++;
					}
				}
				scroll2_current_window++;
				scroll2_timer = 550;
			}
			break;

		case SCROLL_FINISH:
			resp = RESP_FINISH;
			scroll2_state = SCROLL_INIT;
			break;

		default:
			resp = RESP_FINISH;
			scroll2_state = SCROLL_INIT;
			break;
	}

	return resp;
}

