/*
 * dmx_transceiver.h
 *
 *  Created on: 09/02/2016
 *      Author: Mariano
 */

#ifndef DMX_TRANSCEIVER_H_
#define DMX_TRANSCEIVER_H_


//---------- Estructura de Paquetes RDM_KIRNO --------//
//typedef struct RDMKirnoPckt
//{
//	unsigned char start_code;
//	unsigned char msg_lenght;
//	unsigned int dest_addr;
//	unsigned int source_addr;
//	unsigned char transc_number;
//	unsigned char * pDUB;
//	unsigned char end_code0;	//0xFE
//	unsigned char end_code1;	//0xFE
//};


//--- ESTADOS DE TRANSMISION DE PAQUETES DMX512
#define PCKT_INIT			0
#define PCKT_END_BREAK		1
#define PCKT_END_MARK		2
#define PCKT_TRANSMITING	3
#define PCKT_END_TX			4


#define PCKT_UPDATE			10

//--- FUNCIONES DEL MODULO ---//
void SendDMXPacket (unsigned char);
void DMX_Ena(void);
void DMX_Disa(void);

void UpdateRDMResponder(void);

#endif /* DMX_TRANSCEIVER_H_ */
