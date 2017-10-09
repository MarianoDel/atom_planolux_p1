/*
 * dmx_transceiver.c
 *
 *  Created on: 09/02/2016
 *      Author: Mariano
 */
#include "dmx_transceiver.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "stm32f0x_tim.h"
#include "uart.h"
#include "stm32f0x_gpio.h"

#include "rdm_util.h"

//--- VARIABLES EXTERNAS ---//
extern volatile unsigned char RDM_packet_flag;
extern volatile unsigned char data1[];
extern volatile unsigned char data[];


//--- VARIABLES GLOBALES ---//
volatile unsigned char dmx_state = 0;


//--- FUNCIONES DEL MODULO ---//
void DMX_Ena(void)
{
	//habilito la interrupción
	EXTIOn ();
	USART1->CR1 |= USART_CR1_UE;
}

void DMX_Disa(void)
{
	//deshabilito la interrupción
	EXTIOff ();
	USART1->CR1 &= ~USART_CR1_UE;
}

#ifdef USE_DMX
//revisa si existe paquete RDM y que hacer con el mismo
//
void UpdateRDMResponder(void)
{
	RDMKirnoHeader * p_header;

	p_header = (RDMKirnoHeader *) data;
	if (RDM_packet_flag)
	{
		//voy a revisar si el paquete tiene buen checksum
		if (RDMUtil_VerifyChecksumK((unsigned char *)data, data[1]) == true)
		{
			LED_ON;
			//reviso si es unicast
			if (RDMUtil_IsUnicast(p_header->dest_uid) == true)
				LED_OFF;
		}
		else
			LED_OFF;
		RDM_packet_flag = 0;
	}
}

//funcion para enviar el buffer data1[512] al DMX
//recibe PCKT_INIT por el usuario
//recibe PCKT_UPDATE desde su propia maquina de estados
void SendDMXPacket (unsigned char new_func)
{
	if ((new_func == PCKT_INIT) &&
			((dmx_state == PCKT_INIT) || (dmx_state == PCKT_END_TX)))
	{
		//empiezo la maquina de estados
		dmx_state = PCKT_INIT;
	}
	else if (new_func != PCKT_UPDATE)	//update de la maquina de estados
		return;

	switch (dmx_state)
	{
		case PCKT_INIT:
			SW_TX;
			LED_ON;
			dmx_state++;
			DMX_TX_PIN_ON;	//mando break
			OneShootTIM16(88);
			break;

		case PCKT_END_BREAK:
			dmx_state++;
			DMX_TX_PIN_OFF;	//espero mark after break
			OneShootTIM16(8);
			break;

		case PCKT_END_MARK:
			dmx_state++;
			UsartSendDMX();
			break;

		case PCKT_TRANSMITING:	//se deben haber transmitido el start code + los 512 canales
			dmx_state = PCKT_END_TX;	//se a llama al terminar de transmitir con la USART con UPDATE
			DMX_TX_PIN_OFF;
			LED_OFF;
			break;

		case PCKT_END_TX:	//estado de espera luego de transmitir
			break;

		default:
			dmx_state = PCKT_END_TX;
			DMX_TX_PIN_OFF;
			break;
	}
}

#endif
