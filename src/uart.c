/**
  ******************************************************************************
  * @file    Template_2/stm32f0_uart.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   UART functions.
  ******************************************************************************
  * @attention
  *
  * Use this functions to configure serial comunication interface (UART).
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hard.h"
#include "stm32f0xx.h"
#include "uart.h"
#include "dmx_transceiver.h"

#ifdef USE_HLK_WIFI
#include "HLK_RM04.h"
#endif
#ifdef USE_ESP_WIFI
#include "ESP8266.h"
#endif

#include <string.h>




//--- Private typedef ---//
//--- Private define ---//
//--- Private macro ---//

//#define USE_USARTx_TIMEOUT



//--- VARIABLES EXTERNAS ---//

extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned char dmx_receive_flag;
extern volatile unsigned short DMX_channel_received;
extern volatile unsigned short DMX_channel_selected;
extern volatile unsigned char DMX_channel_quantity;
extern volatile unsigned char data1[];
//static unsigned char data_back[10];
extern volatile unsigned char data[];


#define data512		data1		//en rx es la trama recibida; en tx es la trama a enviar
#define data256		data		//en rx son los valores del channel elegido
volatile unsigned char * pdmx;

extern volatile unsigned char tx2buff[];
extern volatile unsigned char rx2buff[];

//--- Private variables ---//
volatile unsigned short rdm_bytes_left = 0;

volatile unsigned char * ptx2;
volatile unsigned char * ptx2_pckt_index;

//Reception buffer.

//Transmission buffer.

//--- Private function prototypes ---//
//--- Private functions ---//


void USART1_IRQHandler(void)
{
	unsigned short i;
	unsigned char dummy;

	/* USART in mode Receiver --------------------------------------------------*/
	if (USART1->ISR & USART_ISR_RXNE)
	{

		//RX DMX
		dummy = USART1->RDR & 0x0FF;

#ifdef USE_DMX
		if (dmx_receive_flag != PCKT_NOT_READY)
		{
			if (DMX_channel_received == 0)		//empieza paquete me fijo si es DMX o RDM
			{
				LED_ON;
				if (dummy == 0xCC)	//es RDM
				{
					dmx_receive_flag = PCKT_RDM;
					rdm_bytes_left = 0;
				}
				else if (dummy == 0x00)	//es DMX
				{
					dmx_receive_flag = PCKT_DMX;
				}
				else
				{
					LED_OFF;
					return;		//no se que es vuelvo
				}
				data1[0] = dummy;
				DMX_channel_received++;
			}
			else
			{
				//estoy recibiendo un paquete, segun cual sea me fijo el final
				if (dmx_receive_flag == PCKT_DMX)
				{
					if (DMX_channel_received < 512)
					{
						data1[DMX_channel_received] = dummy;
						DMX_channel_received++;

						if (DMX_channel_received >= (DMX_channel_selected + DMX_channel_quantity))
						{
							//en data[0] siempre copio el ch0, depues los elegidos
							data[0] = data1[0];
							for (i=0; i<DMX_channel_quantity; i++)
							{
								data[i+1] = data1[(DMX_channel_selected) + i];
							}
							//--- Reception end ---//
							DMX_channel_received = 0;
							dmx_receive_flag = PCKT_NOT_READY;
							Packet_Detected_Flag = 1;
							LED_OFF;	//termina paquete
						}
					}
					else
					{
						//debe ser algun error
						DMX_channel_received = 0;
						dmx_receive_flag = PCKT_NOT_READY;
						LED_OFF;	//termina paquete
					}
				}	//fin if PCKT_DMX

				if (dmx_receive_flag == PCKT_RDM)	//estoy recibiendo paquete RDM
				{
					if (DMX_channel_received == 1)		//el segundo byte es el largo de paquete
					{
						rdm_bytes_left = dummy;
						data1[DMX_channel_received] = dummy;
						DMX_channel_received++;
					}
					else if (DMX_channel_received < rdm_bytes_left)	//bytes sucesivos
					{
						data1[DMX_channel_received] = dummy;
						DMX_channel_received++;
					}
					else	//termina paquete RDM
					{
						for (i = 0; i < rdm_bytes_left; i++)	//backup info
						{
							data[i] = data1[i];
						}
						//--- Reception end ---//
						DMX_channel_received = 0;
						dmx_receive_flag = PCKT_NOT_READY;
						Packet_Detected_Flag = 1;
						LED_OFF;	//termina paquete
					}
				}	//fin if PCKT_RDM

			}	//fin else dmx_channel_received
		}
		else
			USART1->RQR |= 0x08;	//hace un flush de los datos sin leerlos
#endif
	}

	/* USART in mode Transmitter -------------------------------------------------*/
	//if (USART_GetITStatus(USARTx, USART_IT_TXE) == SET)


	if (USART1->CR1 & USART_CR1_TXEIE)
	{
		if (USART1->ISR & USART_ISR_TXE)
		{
	//		USARTx->CR1 &= ~0x00000088;	//bajo TXEIE bajo TE
			//USART1->CR1 &= ~USART_CR1_TXEIE;
			//USARTx->TDR = 0x00;
#ifdef USE_DMX
			if (pdmx < &data1[512])
			{
				USART1->TDR = *pdmx;
				pdmx++;
			}
			else
			{
				USART1->CR1 &= ~USART_CR1_TXEIE;
				SendDMXPacket(PCKT_UPDATE);
			}

/*
			switch (transmit_mode)
			{
				case TRANSMIT_DMX:
					//activo interrupt

					//envio start code

					break;

				case TRANSMITING_DMX:
					if (pdmx < &data1[512])

					break;

				case TRANSMIT_RDM:

					break;

				default:
					transmit_mode = TRANSMIT_DMX;
					break;

			}
*/
#endif
		}

	}

	if ((USART1->ISR & USART_ISR_ORE) || (USART1->ISR & USART_ISR_NE) || (USART1->ISR & USART_ISR_FE))
	{
		USART1->ICR |= 0x0e;
		dummy = USART1->RDR;
	}
}

void USART2_IRQHandler(void)
{
	unsigned short i;
	unsigned char dummy;
	unsigned char mode = 0;

	/* USART in mode Receiver --------------------------------------------------*/
	if (USART2->ISR & USART_ISR_RXNE)
	{
		//RX WIFI
		dummy = USART2->RDR & 0x0FF;

#ifdef USE_ESP_WIFI
		mode = ESP_AskMode();	//TODO: despues sacar el modo y tratar de resolver siempre en AT_MODE
		if ((mode == AT_MODE) || (mode == GOING_AT_MODE))
		{
			ESP_ATModeRx(dummy);
		}
		else
			USART2->RQR |= 0x08;	//hace un flush de los datos sin leerlos
#endif
#ifdef USE_HLK_WIFI
		mode = HLK_Mode();
		if ((mode == AT_MODE) || (mode == GOING_AT_MODE))
		{
			HLK_ATModeRx(dummy);
		}
		else if (mode == TRANSPARENT_MODE)
		{
			HLK_TransparentModeRx(dummy);
		}
		else
			USART2->RQR |= 0x08;	//hace un flush de los datos sin leerlos
#endif

	}
	/* USART in mode Transmitter -------------------------------------------------*/

	if (USART2->CR1 & USART_CR1_TXEIE)
	{
		if (USART2->ISR & USART_ISR_TXE)
		{
			if ((ptx2 < &tx2buff[SIZEOF_DATA]) && (ptx2 < ptx2_pckt_index))
			{
				USART2->TDR = *ptx2;
				ptx2++;
			}
			else
			{
				ptx2 = tx2buff;
				ptx2_pckt_index = tx2buff;
				USART2->CR1 &= ~USART_CR1_TXEIE;
			}
		}
	}

	if ((USART2->ISR & USART_ISR_ORE) || (USART2->ISR & USART_ISR_NE) || (USART2->ISR & USART_ISR_FE))
	{
		USART2->ICR |= 0x0e;
		dummy = USART2->RDR;
	}
}

#ifdef USE_DMX
void UsartSendDMX (void)
{
	//data1[0] = 0x00;
	pdmx = &data1[0];
	//USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
	USART1->CR1 |= USART_CR1_TXEIE;
}
#endif

void Usart2Send (char * send)
{
	unsigned char i;

	i = strlen(send);
	Usart2SendUnsigned(send, i);
}

void Usart2SendUnsigned(unsigned char * send, unsigned char size)
{
	if ((ptx2_pckt_index + size) < &tx2buff[SIZEOF_DATA])
	{
		memcpy((unsigned char *)ptx2_pckt_index, send, size);
		ptx2_pckt_index += size;
		USART2->CR1 |= USART_CR1_TXEIE;
	}
}


void USARTSend(char * send)
{
	unsigned char i;

	i = strlen(send);

	if ((pdmx + i) < &data512[512])
	{
		//TODO: revisar si estoy enviando y apagar int para actualizar pdmx
		strcpy((char*)pdmx, (const char *)send);
		//TODO: no muevo el puntero, me lo mueve la interrupcion
		//pdmx += i;


		USART1->CR1 |= USART_CR1_TXEIE;
	}
}

//TODO: OJO hasta ahora no puede enviar 0x00 por la INT!!!
void USARTSendUnsigned(unsigned char * send, unsigned char size)
{
	if ((pdmx + size) < &data512[512])
	{
		//TODO: revisar si estoy enviando y apagar int para actualizar pdmx
		memcpy((unsigned char *)pdmx, send, size);
		pdmx += size;

		USART1->CR1 |= USART_CR1_TXEIE;
	}
}

#ifdef VER_1_3
void USART2Config(void)
{
	if (!USART2_CLK)
		USART2_CLK_ON;

	GPIOA->AFR[0] |= 0x0001100;	//PA2 -> AF1 PA3 -> AF1

	USART2->BRR = USART_115200;
	USART2->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

	ptx2 = tx2buff;
	ptx2_pckt_index = tx2buff;
	//prx2 = rx2buff;
	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn, 7);
}
#endif

void USART1Config(void)
{
	if (!USART1_CLK)
		USART1_CLK_ON;

#ifdef VER_1_3
	GPIOB->AFR[0] |= 0x00000000;	//PB7 -> AF0 PB6 -> AF0
#endif
#ifdef VER_1_2
	GPIOA->AFR[1] |= 0x00000110;	//PA10 -> AF1 PA9 -> AF1
#endif

	USART1->BRR = USART_250000;
	USART1->CR2 |= USART_CR2_STOP_1;	//2 bits stop
//	USART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
//	USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE;	//SIN TX
	USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;	//para pruebas TX

	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 5);
}


void USARTSendSingle(unsigned char value)
{
	while ((USART1->ISR & 0x00000080) == 0);
	USART1->TDR = value;
}

//--- end of file ---//
