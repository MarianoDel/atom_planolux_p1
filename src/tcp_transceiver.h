/*
 * tcp_transceiver.h
 *
 *  Created on: 13/06/2016
 *      Author: Mariano
 */

#ifndef TCP_TRANSCEIVER_H_
#define TCP_TRANSCEIVER_H_


//-------- Module Defines -------------------------//
enum TcpMessages
{
	NONE_MSG = 0,
	KEEP_ALIVE,
	ROOM_BRIGHT,
	LAMP_BRIGHT,
	GET_A,
	LIGHTS_OFF,
	LIGHTS_ON
};

//--- SIZE OF TCP BUFFERS ---------//
#define SIZEOF_BUFFTCP_SEND		128
#define MAX_BUFF_INDEX			5

//--- TIMEOUTS TIMERS FOR TCP ---------//
#define TT_KALIVE	8000	//8 segundos de keep alive para el tcp
#define TT_TCP_SEND		1000

//--- ESTADOS DEL TCP SEND ---------//
#define TCP_TX_IDLE					0
#define TCP_TX_READY_TO_SEND		1
#define TCP_TX_SENDING				2

//--- Module Functions ----------------------------//
enum TcpMessages CheckTCPMessage(char *, unsigned char *, unsigned char *, unsigned char *);
unsigned char TCPPreProcess(unsigned char *, unsigned char *, unsigned char *);
unsigned char ReadPcktR(unsigned char *, unsigned short, unsigned char *, unsigned char *);
void ReadPcktS(unsigned char *);
unsigned short GetValue (unsigned char *);
unsigned char TCPSendData (unsigned char , char *);
unsigned char TCPSendDataSocket (unsigned char, unsigned char *);
unsigned char TCPReadDataSocket (unsigned char *, unsigned char *, unsigned char *);
void TCPProcess (void);
void TCPProcessInit (void);

unsigned char ReadSocket (unsigned char *, unsigned char);
unsigned char FirstTxEmptyBuffer (void);
unsigned char FirstRxEmptyBuffer (void);
unsigned char CheckTxEmptyBuffer (void);
void CycleAnswers (void);

unsigned char IpIsValid (char *);
unsigned char OctetIsValid (char *, unsigned char *);

#endif /* TCP_TRANSCEIVER_H_ */
