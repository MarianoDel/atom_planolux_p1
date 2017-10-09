/*
 * tcp_transceiver.c
 *
 *  Created on: 13/06/2016
 *      Author: Mariano
 */

#include "tcp_transceiver.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main_menu.h"
#include "ESP8266.h"
#include "uart.h"
#include "lcd.h"



//--- Externals -----------------------------------//
extern volatile unsigned short tcp_send_timeout;
extern unsigned char esp_answer;
extern unsigned char esp_unsolicited_pckt;
extern volatile unsigned char bufftcp[];

//--- Globals -------------------------------------//
unsigned char bufftcpsend [MAX_BUFF_INDEX] [SIZEOF_BUFFTCP_SEND];
//unsigned char bport_index_send [5] [5];
unsigned char * ptcp;
unsigned char tcp_tx_state = 0;

unsigned char bufftcprecv [MAX_BUFF_INDEX] [SIZEOF_BUFFTCP_SEND];
unsigned char bport_index_receiv [MAX_BUFF_INDEX];

extern unsigned char esp_mini_timeout;
extern volatile unsigned char rx2buff[];


//--- Module Functions ----------------------------//

enum TcpMessages CheckTCPMessage(char * d, unsigned char * new_room_bright, unsigned char * new_lamp_bright, unsigned char * bytes)
{
	//reviso que tipo de mensaje tengo en data
	if (strncmp((char *) (const char *) "kAlive;", (char *)d, (sizeof("kAlive;") - 1)) == 0)
	{
		strcpy ((char *) d, (char *) (d + 7));
		*bytes = 7;
		return KEEP_ALIVE;
	}

	if (strncmp((char *) (const char *) "geta;", (char *)d, (sizeof("geta;") - 1)) == 0)
	{
		strcpy ((char *) d, (char *) (d + 5));
		*bytes = 5;
		return GET_A;
	}

	if ((*d == 'r') && (*(d + 2) == ','))
	{
		if (ReadPcktR(d, 1, new_room_bright, bytes) == RESP_OK)
		{
			strcpy ((char *) d, (char *) (d + *bytes));
			return ROOM_BRIGHT;
		}
		else
			return NONE_MSG;

	}

	if ((*d == 's') && (*(d + 2) == ','))
	{
		strcpy ((char *) d, (char *) (d + 7));
		*bytes = 7;
		*new_room_bright = 20;
		return LAMP_BRIGHT;
	}

	if (strncmp((char *) (const char *) "o0,0;", (char *)d, sizeof("o0,0;") - 1) == 0)
	{
		strcpy ((char *) d, (char *) (d + 5));
		*bytes = 5;
		*new_room_bright = 0;
		return LIGHTS_OFF;
	}

	if (strncmp((char *) (const char *) "o0,1;", (char *)d, sizeof("o0,1;") - 1) == 0)
	{
		strcpy ((char *) d, (char *) (d + 5));
		*bytes = 5;
		return LIGHTS_ON;
	}

	return NONE_MSG;
}

void TCPProcessInit (void)
{
	unsigned char i;

	for (i = 0; i < MAX_BUFF_INDEX; i++)
	{
		ptcp = &bufftcpsend[i] [0];
		*(ptcp+1) = 0;
	}

}

//me llaman continuamente para avanzar las maquinas de estado
//mayormente en transmision
void TCPProcess (void)
{
	//char * ptcp; a global
	unsigned char i, rxlen;
	unsigned char resp = RESP_CONTINUE;

	//--- En transmision ---//
	switch (tcp_tx_state)
	{
		case TCP_TX_IDLE:
			//reviso si tengo algo que enviar
			for (i = 0; i < MAX_BUFF_INDEX; i++)
			{
				ptcp = &bufftcpsend[i] [0];
				if (*(ptcp+1) != 0)
				{
					tcp_tx_state++;
					i = 10;
				}
			}
			break;

		case TCP_TX_READY_TO_SEND:
			//tengo el puntero al buffer que quiero enviar
			//pido la transmision al ESP, me contesta RESP_OK, RESP_CONTINUE, RESP_TIMEOUT
			//veo despues que hago en cada caso
			ESP_SendDataResetSM();
			tcp_tx_state++;
			break;

		case TCP_TX_SENDING:
			//me quedo esperando el signo de envio o timeout
			resp = ESP_SendData(0, ptcp);

			if (resp == RESP_OK)
			{
				//termino de enviar limpio buffer
				*(ptcp+1) = 0;
				LCD_2DO_RENGLON;
				LCDTransmitStr((char *) (const char *) "Sended tcp ok   ");
				tcp_tx_state = TCP_TX_IDLE;
			}

			if (resp == RESP_NOK)
			{
				//no encontro el SEND OK  limpio buffer
				//TODO: ver despues si no debo intentar un par de veces mas antes de descartar
				*(ptcp+1) = 0;
				LCD_2DO_RENGLON;
				LCDTransmitStr((char *) (const char *) "Error on tcp tx ");
				tcp_tx_state = TCP_TX_IDLE;
			}

			if (resp == RESP_TIMEOUT)
			{
				//no pudo enviar  limpio buffer
				//TODO: ver despues si no debo intentar un par de veces mas antes de descartar
				*(ptcp+1) = 0;
				LCD_2DO_RENGLON;
				LCDTransmitStr((char *) (const char *) "Timeout tcp tx  ");
				tcp_tx_state = TCP_TX_IDLE;
			}
			break;

		default:
			tcp_tx_state = TCP_TX_IDLE;
			break;
	}

	//--- En recepcion ---//
	if (esp_unsolicited_pckt == RESP_READY)
	{
		esp_unsolicited_pckt = RESP_CONTINUE;

		//guardo en el proximo buffer vacio o vuelvo
		for (i = 0; i < MAX_BUFF_INDEX; i++)
		{
			if (bport_index_receiv[i] == 0)	//buffer vacio, lo uso
			{
				if (TCPReadDataSocket((unsigned char*) bufftcp, &bufftcprecv[i] [0], &rxlen) != 0xFF)
				{
					if (rxlen > 0)
						bport_index_receiv[i] = rxlen;

					i = MAX_BUFF_INDEX;
				}
			}
		}
	}
}

//revisa el rxbuff del puerto serie buscando alguna respuesta que no se haya levantado o reconocido
void CycleAnswers (void)
{
	if ((CheckTxEmptyBuffer() == 0) && (!esp_mini_timeout))
	{
		unsigned char bindex, lookplus, rxlen = 0;

		//parseo el rx2buff buscando respuestas
		bindex = FirstRxEmptyBuffer();

		if (bindex < MAX_BUFF_INDEX)	//tengo lugar
		{
			//busco en el buffer un +IPD
			for (lookplus = 0; lookplus < (SIZEOF_BUFFTCP_SEND - 1); lookplus++)
			{
				if ((rx2buff[lookplus] == '+') && (rx2buff[lookplus + 1] == 'I'))
				{
					if (TCPReadDataSocket((unsigned char *)&rx2buff[lookplus], &bufftcprecv[bindex] [0], &rxlen) != 0xFF)
					{
						if (rxlen > 0)
							bport_index_receiv[bindex] = rxlen;
					}

					//ya lo procese, lo marco
					rx2buff[lookplus] = '-';
					lookplus = SIZEOF_BUFFTCP_SEND;
				}
			}
		}
	}
}


//Revisa tipo de mensaje y puerto (0 a 4)
//procesa el buffer en crudo y lo copia en un buffer de salida con solo numeros y letras
//devuelve length
unsigned char TCPPreProcess(unsigned char * d, unsigned char * output, unsigned char * length)
{
	unsigned char port = 0xFF;
	unsigned char len_index = 0;
	unsigned char i;
	unsigned char max_len = 0;
	unsigned char * d_offset;
	unsigned char bytes_cnt = 0;

	//reviso que tipo de mensaje tengo en data
	//primero reviso estados de conexiones
//	if ((*d >= '0') && (*d <= '4') && (*(d+1) == ','))
//	{
//		if (strncmp((char *) (const char *) "CONNECT\r", (char *) (d + 2), sizeof("CONNECT\r") - 1) == 0)
//			return KEEP_ALIVE;
//
//		if (strncmp((char *) (const char *) "CLOSED\r", (char *) (d + 2), sizeof("CLOSED\r") - 1) == 0)
//			return KEEP_ALIVE;
//	}

	//llega:
	//+IPD,0,6:geta;\n
	if (strncmp((char *) (const char *) "+IPD,", (char *) d, sizeof("+IPD,") - 1) == 0)
	{
		if ((*(d+5) >= '0') && (*(d+5) <= '4'))
		{
			port = *(d+5) - '0';
			for (i = 0; i < 4; i++)	//busco length
			{
				if (*(d+7+i) == ':')
					i = 4;

				len_index++;
			}

			max_len = (unsigned char) atoi ((char *) (d+7));

			if (max_len > 0)
			{
				d_offset = (d+7+len_index);

				for (i = 0; i < max_len; i++)	//copio todos los codigos numeros y letras
				{
					if (*d_offset != '\0')
					{
						if ((*d_offset > 31) && (*d_offset < 127))		//todos los codigos numeros y letras
						{
							*output = *d_offset;
							output++;
							bytes_cnt++;
						}
						d_offset++;
					}
					else
					{
						*output = '\0';
						break;
					}
				}
				*length = bytes_cnt;
			}
			else
				port = 0xFF;
		}
	}
	return port;
}

//el bufftcpsend de transmision es port,lenght,data
unsigned char TCPSendData (unsigned char port, char * data)
{
	unsigned char * p;
	unsigned char length = 0;
	unsigned char i;
	unsigned char resp = RESP_NOK;

	//aca reviso si el puerto esta conectado
	if ((port >= 0) && (port <= 4))
	{
		length = strlen(data);

		//busco buffer tcp vacio
		for (i = 0; i < MAX_BUFF_INDEX; i++)
		{
			p = &bufftcpsend [i] [0];
			if (*(p+1) == 0)
				i = 10;				//buffer vacio, lo uso
		}

		if ((i >= 10) && (length < (SIZEOF_BUFFTCP_SEND - 2)))
		{
			*p = port;
			*(p+1) = length;
			strcpy ((p+2), data);
			resp = RESP_OK;
		}
	}

	return resp;
}

unsigned char FirstTxEmptyBuffer (void)
{
	unsigned char i;
	//busco buffer tcp vacio
	for (i = 0; i < MAX_BUFF_INDEX; i++)
	{
		if (bufftcpsend [i] [1] == 0)
			return i;
	}
	return i;
}

unsigned char CheckTxEmptyBuffer (void)
{
	unsigned char i, res = 0;
	//busco buffer tcp vacio
	for (i = 0; i < MAX_BUFF_INDEX; i++)
	{
		res |= bufftcpsend [i] [1];
	}

	return res;
}

unsigned char FirstRxEmptyBuffer (void)
{
	unsigned char i;
	//busco buffer tcp vacio
	for (i = 0; i < MAX_BUFF_INDEX; i++)
	{
		if (bport_index_receiv[i] == 0)
			return i;
	}
	return i;
}

//el bufftcpsend de transmision es port,lenght,data
unsigned char TCPSendDataSocket (unsigned char length, unsigned char * data)
{
	unsigned char * p;
	unsigned char i;
	unsigned char resp = RESP_NOK;

	unsigned char port = 0;
	//aca reviso si el puerto esta conectado
	if ((port >= 0) && (port <= 4))
	{
		//busco buffer tcp vacio
		i = FirstTxEmptyBuffer();

		if ((i < MAX_BUFF_INDEX) && (length < (SIZEOF_BUFFTCP_SEND - 2)))
		{
			p = &bufftcpsend [i] [0];
			*p = port;
			*(p+1) = length;

//			for (i = 0; i < length; i++)
//				*(p + 2 + i) = *(data + i);
			memcpy((p + 2), data, length);

			resp = RESP_OK;
		}
	}
	return resp;
}

//Revisa tipo de mensaje y puerto (0 a 4)
//procesa el buffer en crudo y lo copia en un buffer de salida con solo numeros y letras
//devuelve length
unsigned char TCPReadDataSocket(unsigned char * d, unsigned char * output, unsigned char * length)
{
	unsigned char port = 0xFF;
	unsigned char len_index = 0;
	unsigned char i;
	unsigned char max_len = 0;
	unsigned char * d_offset;

	//llega:
	//+IPD,0,6:geta;\n
	if (strncmp((char *) (const char *) "+IPD,", (char *) d, sizeof("+IPD,") - 1) == 0)
	{
		if ((*(d+5) >= '0') && (*(d+5) <= '4'))
		{
			port = *(d+5) - '0';
			for (i = 0; i < 4; i++)	//busco length
			{
				if (*(d+7+i) == ':')
					i = 4;

				len_index++;
			}

			max_len = (unsigned char) atoi ((char *) (d+7));

			if (max_len > 0)
			{
				d_offset = (d+7+len_index);
				memcpy (output, d_offset, max_len);
				*length = max_len;
			}
			else
				port = 0xFF;
		}
	}
	return port;
}

unsigned char ReadSocket (unsigned char * buff, unsigned char maxlen)
{
	unsigned char i, len = 0;

	//levanto los buffers utilizados
	for (i = 0; i < MAX_BUFF_INDEX; i++)
	{
		if (bport_index_receiv[i] != 0)	//buffer con data, lo mando
		{
			if (bport_index_receiv[i] < maxlen)	//tengo lugar
			{
				memcpy (buff, &bufftcprecv[i] [0], bport_index_receiv[i]);
				len = bport_index_receiv[i];
				bport_index_receiv[i] = 0;
			}
			else
			{
				memcpy (buff, &bufftcprecv[i] [0], maxlen);
				len = maxlen;
				bport_index_receiv[i] -= maxlen;
				//ajusto el buffer
				memcpy (&bufftcprecv[i] [0], &bufftcprecv[i] [maxlen], bport_index_receiv[i]);

			}

			i = MAX_BUFF_INDEX;
		}
	}

	return len;

}
unsigned char ReadPcktR(unsigned char * p, unsigned short own_addr, unsigned char * new_r, unsigned char * len)
{
	unsigned char new_shine;

	if (*(p+2) != ',')
		return RESP_NOK;

	if (GetValue(p + 3) == 0xffff)
		return RESP_NOK;

	new_shine = GetValue(p + 3);

	switch (*(p+1))
	{
		case '0':
			if ((own_addr >= 1) && (own_addr < 31))
			{
				*new_r = new_shine;
			}
			break;

		case '1':
			if ((own_addr >= 31) && (own_addr < 61))
			{
				*new_r = new_shine;
			}
			break;

		case '2':
			if ((own_addr >= 61) && (own_addr < 91))
			{
				*new_r = new_shine;
			}
			break;

		case '3':
			if ((own_addr >= 91) && (own_addr < 121))
			{
				*new_r = new_shine;
			}
			break;

		default:
			break;
	}

	if (new_shine < 10)
		*len = 5;
	else if (new_shine < 100)
		*len = 6;
	else
		*len = 7;

	return RESP_OK;
}

//en S me llega un parametro particular
//del brillo de esa lampara pero indicando cada habitacion
//s0,0,255;\r\n
void ReadPcktS(unsigned char * p)
{
	/*
	unsigned char room;
	unsigned char slider;
	unsigned char new_shine;
	unsigned short ii;

	if (*(p+2) != ',')
		return;

	room = *(p+1) - '0';

	if ((room < 0) || (room > 3))
		return;

	ii = GetValue(p + 3);
	if (ii == 0xffff)
		return;
	else
		slider = (unsigned char) ii;

	if (slider < 10)
	{
		ii = GetValue(p + 5);
		if (ii == 0xffff)
			return;
		else
			new_shine = (unsigned char) ii;
	}
	else if (slider < 30)
	{
		ii = GetValue(p + 6);
		if (ii == 0xffff)
			return;
		else
			new_shine = (unsigned char) ii;
	}
	else
		return;

	ii = (room * 30) + slider + 1;
	orig_shine_slider[ii] = new_shine;
	data1[ii] = ((new_shine + 1) * orig_shine_room[room]) >> 8;
	*/
}

unsigned short GetValue (unsigned char * pn)
{
	unsigned char i;
	unsigned char colon = 0;
	unsigned short new_val = 0xffff;

	//me fijo la posiciones de la , o ;
	for (i = 0; i < 6; i++)
	{
		if ((*(pn + i) == ',') || ((*(pn + i) == ';')))
		{
			colon = i;
			i = 6;
		}
	}

	if ((colon == 0) || (colon >= 5))
		return 0;

	switch (colon)
	{
		case 1:
			new_val = *pn - '0';
			break;

		case 2:
			new_val = (*pn - '0') * 10 + (*(pn + 1) - '0');
			break;

		case 3:
			new_val = (*pn - '0') * 100 + (*(pn + 1) - '0') * 10 + (*(pn + 2) - '0');
			break;

		case 4:
			new_val = (*pn - '0') * 1000 + (*(pn + 1) - '0') * 100 + (*(pn + 2) - '0') * 10 + (*(pn + 2) - '0');
			break;

	}
	return new_val;
}

unsigned char IpIsValid (char * ip)
{
	unsigned char i;
	unsigned char dot1 = 0;
	unsigned char dot2 = 0;
	unsigned char dot3 = 0;
	unsigned char dot4 = 0;
	unsigned char size1 = 0;
	unsigned char size2 = 0;
	unsigned char size3 = 0;
	unsigned char size4 = 0;

	unsigned char new_val = 0;
	char s_octet [4];

	//me fijo las posiciones del .
	for (i = 0; i < 15; i++)
	{
		if (*(ip + i) == '\0')
		{
			dot4 = i;
			i = 15;
		}
		else
		{
			if (*(ip + i) == '.')
			{
				if (!dot1)
					dot1 = i;
				else if (!dot2)
					dot2 = i;
				else if (!dot3)
					dot3 = i;
				else
					return RESP_NOK;
			}
		}
	}

	if (!dot4)
		dot4 = 15;

	//me fijo si tiene los tres .
	if ((dot1 == 0) || (dot2 == 0) || (dot3 == 0))
		return RESP_NOK;

	size1 = dot1;
	size2 = dot2 - dot1 - 1;
	size3 = dot3 - dot2 - 1;
	size4 = dot4 - dot3 - 1;

	//me fijo que no tengan mucho espacio
	if ((size1 > 3) || (size2 > 3) || (size3 > 3) || (size4 > 3))
		return RESP_NOK;

	//los puntos son validos, ahora reviso los numeros
	memset (s_octet, '\0', sizeof(s_octet));
	strncpy (s_octet, ip, size1);
	if (OctetIsValid (s_octet, &new_val) == RESP_NOK)
		return RESP_NOK;

	memset (s_octet, '\0', sizeof(s_octet));
	strncpy (s_octet, ip + dot1 + 1, size2);
	if (OctetIsValid (s_octet, &new_val) == RESP_NOK)
		return RESP_NOK;

	memset (s_octet, '\0', sizeof(s_octet));
	strncpy (s_octet, ip + dot2 + 1, size3);
	if (OctetIsValid (s_octet, &new_val) == RESP_NOK)
		return RESP_NOK;

	memset (s_octet, '\0', sizeof(s_octet));
	strncpy (s_octet, ip + dot3 + 1, size4);
	if (OctetIsValid (s_octet, &new_val) == RESP_NOK)
		return RESP_NOK;

	return RESP_OK;
}

unsigned char OctetIsValid (char * ip, unsigned char * octet_val)
{
	unsigned char i;
	unsigned char dot = 0;
	short new_val = 0;

	//me fijo la posiciones del . o null
	for (i = 0; i < 4; i++)
	{
		if ((*(ip + i) == '.') || (*(ip + i) == '\0'))
		{
			dot = i;
			i = 10;
		}
	}

	//si tiene . me fijo el numero
	if (dot == 0)
	{
		*octet_val = 0;
		return RESP_NOK;
	}


	switch (dot)
	{
		case 1:
			new_val = *ip - '0';
			break;

		case 2:
			new_val = (*ip - '0') * 10 + (*(ip + 1) - '0');
			break;

		case 3:
			new_val = (*ip - '0') * 100 + (*(ip + 1) - '0') * 10 + (*(ip + 2) - '0');
			break;
	}

	if ((new_val < 0) || (new_val > 255))
	{
		*octet_val = 0;
		return RESP_NOK;
	}

	*octet_val = new_val;
	return RESP_OK;
}
