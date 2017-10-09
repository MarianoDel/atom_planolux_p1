/*
 * HLK_RM04.c
 *
 *  Created on: 26/04/2016
 *      Author: Mariano
 */

#include "HLK_RM04.h"
#include "uart.h"
#include "main_menu.h"
#include "stm32f0xx.h"

#include <string.h>



//--- Externals -----------------------------------//
extern unsigned short hlk_timeout;
extern unsigned char hlk_mini_timeout;
extern unsigned char hlk_answer;
extern unsigned char hlk_transparent_finish;

extern volatile unsigned char data1[];
extern volatile unsigned char data[];

#define data512		data1
#define data256		data


//--- Globals -------------------------------------//

void (* pCallBack) (char *);

unsigned char hlk_command_state = 0;
unsigned char hlk_timeout_cnt = 0;
unsigned char hlk_mode = UNKNOW_MODE;
volatile unsigned char * prx;
volatile unsigned char at_start = 0;
volatile unsigned char at_finish = 0;
enum HlkConfigState hlk_config_state = CONF_INIT;

unsigned char HLK_EnableNewConn (unsigned char p)
{
	unsigned char resp = RESP_CONTINUE;

	if (p == CMD_RESET)
	{
		hlk_config_state = ENA_INIT;
		return resp;
	}

	switch (hlk_config_state)
	{
		case ENA_INIT:
			hlk_config_state++;
			resp = SendCommandWaitAnswer((const char *) "at+reconn=1\r\n", CMD_RESET);
			break;

		case ENA_ASK_AT:
			resp = SendCommandWaitAnswer((const char *) "at+reconn=1\r\n", CMD_PROC);;
			break;

		default:
			hlk_config_state = ENA_INIT;
			break;
	}
	return resp;
}

//con CMD_RESET hace reset de la maquina
//con CMD_PROC busca primero ir a AT Mode y luego va a transparente
//con CMD_ONLY_CHECK revisa si esta en AT y pasa a transparente o constesta que ya esta en transparente
unsigned char HLK_GoTransparent (unsigned char p)
{
	unsigned char resp = RESP_CONTINUE;

	if (p == CMD_RESET)
	{
		hlk_config_state = TRANSP_INIT;
		return resp;
	}

	if (p == CMD_PROC)
	{
		switch (hlk_config_state)
		{
			case TRANSP_INIT:
				hlk_config_state++;
				resp = HLKToATMode(CMD_RESET);
				break;

			case TRANSP_INIT1:
				resp = HLKToATMode(CMD_PROC);

				if (resp == RESP_OK)
				{
					resp = RESP_CONTINUE;
					hlk_config_state = TRANSP_GOTRANSP;
				}
				break;

			case TRANSP_GOTRANSP:
				USARTSend((char *) (const char *) "at+out_trans=0\r\n");
				resp = RESP_CONTINUE;
				hlk_config_state = TRANSP_GOTRANSP_WAIT;
				hlk_timeout = 500;
				hlk_mode = TRANSPARENT_MODE;
				break;

			case TRANSP_GOTRANSP_WAIT:
				if (!hlk_timeout)
					resp = RESP_OK;
				break;

			default:
				hlk_config_state = TRANSP_INIT;
				break;
		}
	}

	if (p == CMD_ONLY_CHECK)
	{
		if (HLK_Mode() != TRANSPARENT_MODE)
		{
			USARTSend((char *) (const char *) "at+out_trans=0\r\n");
			hlk_mode = TRANSPARENT_MODE;
		}
		resp = RESP_OK;
	}
	return resp;
}

unsigned char HLK_SendConfig (unsigned char p)
{
	unsigned char resp = RESP_CONTINUE;

	if (p == CMD_RESET)
	{
		hlk_config_state = CONF_INIT;
		return resp;
	}

	switch (hlk_config_state)
	{
		case CONF_INIT:
			hlk_config_state++;
			HLKToATMode(CMD_RESET);
			break;

		case CONF_ASK_AT:
			resp = HLKToATMode(CMD_PROC);

			if (resp == RESP_OK)
			{
				hlk_config_state = CONF_AT_CONFIG_0;
				resp = RESP_CONTINUE;
			}
			break;

		case CONF_AT_CONFIG_0:
			resp = SendCommandWaitAnswer((const char *) "at+netmode=3\r\n", CMD_RESET);
			hlk_config_state = CONF_AT_CONFIG_0B;
			break;

		case CONF_AT_CONFIG_0B:
			resp = SendCommandWaitAnswer((const char *) "at+netmode=3\r\n", CMD_PROC);

			if (resp == RESP_OK)
			{
				hlk_config_state = CONF_AT_CONFIG_1;
				resp = RESP_CONTINUE;
			}
			break;

		case CONF_AT_CONFIG_1:
			resp = SendCommandWaitAnswer((const char *) "at+wifi_conf=KIRNO_WIFI,wpa2_aes,12345678\r\n", CMD_RESET);
			hlk_config_state = CONF_AT_CONFIG_1B;
			break;

		case CONF_AT_CONFIG_1B:
			resp = SendCommandWaitAnswer((const char *) "at+wifi_conf=KIRNO_WIFI,wpa2_aes,12345678\r\n", CMD_PROC);

			if (resp == RESP_OK)
			{
				hlk_config_state = CONF_AT_CONFIG_2;
				resp = RESP_CONTINUE;
			}
			break;

		case CONF_AT_CONFIG_2:
			resp = SendCommandWaitAnswer((const char *) "at+wifi_conf=KIRNO_WIFI,wpa2_aes,12345678\r\n", CMD_RESET);
			hlk_config_state = CONF_AT_CONFIG_2B;
			break;

		case CONF_AT_CONFIG_2B:
			resp = SendCommandWaitAnswer((const char *) "at+wifi_conf=KIRNO_WIFI,wpa2_aes,12345678\r\n", CMD_PROC);

			if (resp == RESP_OK)
			{
				hlk_config_state = CONF_AT_CONFIG_3;
				resp = RESP_CONTINUE;
			}
			break;

		case CONF_AT_CONFIG_3:
			resp = SendCommandWaitAnswer((const char *) "at+net_ip=192.168.1.254,255.255.255.0,192.168.1.1\r\n", CMD_RESET);
			hlk_config_state = CONF_AT_CONFIG_3B;
			break;

		case CONF_AT_CONFIG_3B:
			resp = SendCommandWaitAnswer((const char *) "at+net_ip=192.168.1.254,255.255.255.0,192.168.1.1\r\n", CMD_PROC);

			if (resp == RESP_OK)
			{
				hlk_config_state = CONF_AT_CONFIG_4;
				resp = RESP_CONTINUE;
			}
			break;

		case CONF_AT_CONFIG_4:
			resp = SendCommandWaitAnswer((const char *) "at+dhcpd_ip=192.168.1.100,192.168.1.200,255.255.255.0,192.168.1.254\r\n", CMD_RESET);
			hlk_config_state = CONF_AT_CONFIG_4B;
			break;

		case CONF_AT_CONFIG_4B:
			resp = SendCommandWaitAnswer((const char *) "at+dhcpd_ip=192.168.1.100,192.168.1.200,255.255.255.0,192.168.1.254\r\n", CMD_PROC);

			if (resp == RESP_OK)
			{
				hlk_config_state = CONF_AT_CONFIG_5;
				resp = RESP_CONTINUE;
			}
			break;

		case CONF_AT_CONFIG_5:
			resp = SendCommandWaitAnswer((const char *) "at+dhcpd=1\r\n", CMD_RESET);
			hlk_config_state = CONF_AT_CONFIG_5B;
			break;

		case CONF_AT_CONFIG_5B:
			resp = SendCommandWaitAnswer((const char *) "at+dhcpd=1\r\n", CMD_PROC);

			if (resp == RESP_OK)
			{
				hlk_config_state = CONF_AT_CONFIG_6;
				resp = RESP_CONTINUE;
			}
			break;

		case CONF_AT_CONFIG_6:
			resp = SendCommandWaitAnswer((const char *) "at+remotepro=tcp\r\n", CMD_RESET);
			hlk_config_state = CONF_AT_CONFIG_6B;
			break;

		case CONF_AT_CONFIG_6B:
			resp = SendCommandWaitAnswer((const char *) "at+remotepro=tcp\r\n", CMD_PROC);

			if (resp == RESP_OK)
			{
				hlk_config_state = CONF_AT_CONFIG_7;
				resp = RESP_CONTINUE;
			}
			break;

		case CONF_AT_CONFIG_7:
			resp = SendCommandWaitAnswer((const char *) "at+remoteport=10002\r\n", CMD_RESET);
			hlk_config_state = CONF_AT_CONFIG_7B;
			break;

		case CONF_AT_CONFIG_7B:
			resp = SendCommandWaitAnswer((const char *) "at+remoteport=10002\r\n", CMD_PROC);

			if (resp == RESP_OK)
			{
				hlk_config_state = CONF_AT_CONFIG_8;
				resp = RESP_CONTINUE;
			}
			break;

		case CONF_AT_CONFIG_8:
			USARTSend((char *) (const char *) "at+net_commit=1\r\n");
			resp = RESP_OK;
			break;

		default:
			hlk_config_state = CONF_INIT;
			break;
	}

	return resp;
}

//con CMD_RESET hace reset de la maquina, con CMD_PROC recorre la rutina
//contesta RESP_CONTINUE, RESP_TIMEOUT, RESP_NOK o RESP_OK
unsigned char HLKToATMode (unsigned char p)
{
	unsigned char resp = RESP_CONTINUE;

	if (p == CMD_RESET)
	{
		hlk_command_state = COMM_INIT;
		return resp;
	}

	//hago polling hasta que este con modo AT
	switch (hlk_command_state)
	{
		case COMM_INIT:
			HLK_PIN_ON;
			hlk_timeout = TT_HLK_AT_MODE;
			hlk_timeout_cnt = 0;
			hlk_command_state = COMM_TO_AT;
			//hlk_mode = UNKNOW_MODE;
			hlk_mode = GOING_AT_MODE;
			break;

		case COMM_TO_AT:
			if (!hlk_timeout)
			{
				HLK_PIN_OFF;
				hlk_command_state = COMM_AT_ANSWER;
				SendCommandWithAnswer((const char *) "at+ver=?\r\n", CheckVersion);	//blanquea hlk_answer
				hlk_timeout = 3000;
			}
			break;

		case COMM_AT_ANSWER:
			if ((hlk_answer == RESP_TIMEOUT) || (!hlk_timeout))
			{
				if (hlk_timeout_cnt >= 3)
					resp = RESP_TIMEOUT;
				else
				{
					hlk_timeout_cnt++;
					hlk_command_state = COMM_TO_AT;
				}
			}

			if (hlk_answer == RESP_READY)
			{
				//hlk_command_state = COMM_WAIT_PARSER;
				HLKPreParser((unsigned char *)data256);

				resp = HLKVerifyVersion((unsigned char *)data256);

				if (resp == RESP_OK)
					hlk_mode = AT_MODE;
				else
					hlk_mode = UNKNOW_MODE;
			}
			break;

		default:
			hlk_command_state = COMM_INIT;
			break;
	}

	return resp;
}

unsigned char SendCommandWaitAnswer (const char * comm, unsigned char p)	//blanquea hlk_answer
{
	unsigned char i, length = 0;
	unsigned char resp = RESP_CONTINUE;
	char s_comm [80];

	if (p == CMD_RESET)
	{
		hlk_command_state = COMM_INIT;
		return resp;
	}

	switch (hlk_command_state)
	{
		case COMM_INIT:
			hlk_command_state = COMM_WAIT_ANSWER;
			SendCommandWithAnswer(comm, CheckVersion);	//blanquea hlk_answer
			hlk_timeout = TT_AT_1SEG;
			break;

		case COMM_WAIT_ANSWER:
			if ((hlk_answer == RESP_TIMEOUT) || (!hlk_timeout))
			{
				resp = RESP_TIMEOUT;
			}

			if (hlk_answer == RESP_READY)
				hlk_command_state = COMM_VERIFY_ANSWER;

			break;

		case COMM_VERIFY_ANSWER:
			for (i = 0; i < sizeof(s_comm); i++)	//copio hasta el primer \r
			{
				if (comm[i] != '\r')
					s_comm[i] = comm[i];
				else
				{
					length = i;
					s_comm[i] = '\0';
					i = sizeof(s_comm);
				}
			}

			HLKPreParser((unsigned char *)data256);
			if (strncmp(s_comm, (char *)data256, length) == 0)
			{
				if ((*(data256 + length) == 'o') && (*(data256 + length + 1) == 'k'))
					resp = RESP_OK;
			}
			else
				resp = RESP_NOK;
			break;

		default:
			hlk_command_state = COMM_INIT;
			break;
	}

	return resp;
}

//Manda un comando al HLK y espera respuesta
//avisa de la respuesta al callback elegido con *pCall
//blanquea hlk_answer
//
void SendCommandWithAnswer(const char * str, void (*pCall) (char * answer))
{
	hlk_answer = RESP_NO_ANSWER;

	pCallBack = pCall;

	USARTSend((char *) str);
}

unsigned char HLK_Mode(void)
{
	return hlk_mode;	//TRANSPARENT_MODE, AT_MODE, UNKNOW_MODE
}


//me llaman desde el proceso principal para update
void HLK_ATProcess (void)
{
	//reviso timeouts para dar aviso del fin de mensajes at
	if ((at_start) && (at_finish) && (!hlk_mini_timeout))
	{
		at_start = 0;
		at_finish = 0;
		hlk_answer = RESP_READY;	//aviso que tengo una respuesta para revisar
	}
}

//me llaman desde usart rx si estoy en modo AT
void HLK_ATModeRx (unsigned char d)
{
	//tengo que ver en que parte del AT estoy
	if (!at_start)
	{
		if ((d == 'A') || (d == 'a'))
		{
			prx = (unsigned char *) data256;
			*prx = d;
			prx++;
			at_start = 1;
		}
	}
	else if (at_start)
	{
		if (d == '\n')		//no se cuantos finales de linea voy a tener en la misma respuesta
			at_finish = 1;

		*prx = d;
		if (prx < &data256[SIZEOF_DATA256])
			prx++;
		else
		{
			//recibi demasiados bytes juntos
			//salgo por error
			prx = (unsigned char *) data256;
			hlk_answer = RESP_NOK;
		}
	}

	//mientras reciba bytes hago update del timer
	hlk_mini_timeout = TT_HLK_AT_MINI;
}

//me llaman desde usart rx si estoy en modo TRANSPARENT
void HLK_TransparentModeRx (unsigned char d)
{
	if (!hlk_transparent_finish)	//si llego un byte cuando todavia estoy analizando, lo pierdo
	{
		if (d != '\n')
		{
			*prx = d;
			if (prx < &data256[SIZEOF_DATA256])
				prx++;
			else
			{
				//recibi demasiados bytes juntos sin final de linea
				prx = (unsigned char *) data256;
			}
		}
		else	//cuando veo final de linea aviso
		{
			*prx = '\0';
			prx = (unsigned char *) data256;
			hlk_transparent_finish = 1;
		}
	}
}

void HLKPreParser(unsigned char * d)
{
	unsigned char i;
	unsigned char * l;

	l = d;
	for (i = 0; i < SIZEOF_DATA256; i++)
	{
		if (*d != '\0')
		{
			if ((*d > 31) && (*d < 127))		//todos los codigos numeros y letras
			{
				*l = *d;
				l++;
			}
			d++;
		}
		else
		{
			*l = '\0';
			break;
		}
	}
}

unsigned char HLKVerifyVersion(unsigned char * d)
{
	char comp;

	//primero reviso el echo del at
	comp = strncmp ((char *) d, (const char *) "at+ver=?", (sizeof ("at+ver=?")) - 1);
	if (comp == 0)
	{
		//ahora reviso solo algunos valores
		if ((*(d+8) == 'V') && (*(d+10) == '.') && (*(d+13) == '('))
			return RESP_OK;
	}
	return RESP_NOK;
}

void CheckVersion (char * answer)
{
	unsigned char comp = 0;

	comp = strncmp (answer, (const char *) "VER 1.8", (sizeof ("VER 1.8")) - 1);

	if (comp == 0)
		hlk_answer = RESP_OK;
	else
		hlk_answer = RESP_NOK;
}

