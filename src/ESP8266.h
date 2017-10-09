/*
 * ESP8266.h
 *
 *  Created on: 26/04/2016
 *      Author: Mariano
 */

#ifndef ESP8266_H_
#define ESP8266_H_

#include "hard.h"

#define ESP8266_PRESENT

//--- Configuracion puerto serie -----------------------------//
#define SerialSend(X)		Usart2Send(X)

//--- Timeouts para el ESP -----------------------------------//
#define TT_ESP_AT_MODE		300
#define TT_ESP_RESET		6000
#define TT_ESP_AT_MINI		5		//timeout de esopera de chars luego de una comienzo
									//con 5 esta OK para AT pero puerde SEND OK en tx
#define TT_AT_1SEG			1000
#define TT_AT_3SEG			3000
#define TT_AT_10SEG			10000

//--- Estados para el ESP -----------------------------------//
#define COMM_INIT			0
#define COMM_TO_AT			1
#define COMM_AT_ANSWER		2
#define COMM_WAIT_PARSER	3
#define COMM_WAIT_ANSWER	4
#define COMM_VERIFY_ANSWER	5

#define CMD_RESET	0
#define CMD_PROC	1
#define CMD_ONLY_CHECK	2

//--- Modos del ESP -----------------------------------------//
#define UNKNOW_MODE				0
#define AT_MODE					1
#define TRANSPARENT_MODE		2
#define GOING_AT_MODE			3
#define AT_TRANSMIT				4

#define USART_CALLER	0
#define PROCESS_CALLER	1

enum EspConfigState
{
	CONF_INIT = 0,
	CONF_ASK_AT,
	CONF_AT_CONFIG_0,
	CONF_AT_CONFIG_0B,
	CONF_AT_CONFIG_1,
	CONF_AT_CONFIG_1B,
	CONF_AT_CONFIG_2,
	CONF_AT_CONFIG_2B,
	CONF_AT_CONFIG_3,
	CONF_AT_CONFIG_3B,
	CONF_AT_CONFIG_4,
	CONF_AT_CONFIG_4B,
	CONF_AT_CONFIG_5,
	CONF_AT_CONFIG_5B,
	CONF_AT_CONFIG_6,
	CONF_AT_CONFIG_6B,
	CONF_AT_CONFIG_7,
	CONF_AT_CONFIG_7B,
	CONF_AT_CONFIG_8,
	CONF_AT_CONFIG_8B
};

enum EspSendDataState
{
	SEND_DATA_INIT = 0,
	SEND_DATA_RST,
	SEND_DATA_ASK_CHANNEL,
	SEND_DATA_WAIT_SEND_OK
};

enum EspOpenSocketState
{
	OPEN_SOCKET_INIT = 0,
	OPEN_SOCKET_RST,
	OPEN_SOCKET_ASK_SOCKET,
	OPEN_SOCKET_WAIT_OK
};

enum EspCloseSocketState
{
	CLOSE_SOCKET_INIT = 0,
	CLOSE_SOCKET_RST,
	CLOSE_SOCKET_CMD
};

enum EspGetIPState
{
	GET_IP_INIT = 0,
	GET_IP_RST,
	GET_IP_WAIT_ANS,
	GET_IP_CHECK
};

enum EspEnaState
{
	ENA_INIT = 0,
	ENA_ASK_AT
};

enum EspTranspState
{
	TRANSP_INIT = 0,
	TRANSP_INIT1,
	TRANSP_GOTRANSP,
	TRANSP_GOTRANSP_WAIT
};

//--- Funciones del Modulo -----------------------------------//
void CheckVersion (char *);
//void SendCommandWithAnswer(const char *, void (*pCall) (char *));
void SendCommandWithAnswer(const char *);
void SendDataWithAnswer(unsigned char *, unsigned char);

unsigned char ESP_AskMode(void);
void ESP_SetMode(unsigned char);
void ESP_ATModeRx (unsigned char);
unsigned char SendCommandWaitAnswer (const char *);
void SendCommandWaitAnswerResetSM (void);
void ESP_ATProcess (void);
unsigned char ESPToATMode (unsigned char);
void ESPPreParser(unsigned char *);
void ESPPreParser2 (unsigned char *, unsigned char *);
unsigned char ESPVerifyVersion(unsigned char *);
unsigned char ESP_SendConfigAP (void);
unsigned char ESP_SendConfigClient (void);
void ESP_SendConfigResetSM (void);
unsigned char ESP_EnableNewConn (unsigned char);
unsigned char ESP_SendData (unsigned char, unsigned char *);
void ESP_SendDataResetSM (void);
unsigned char ESP_GetIP (char *);

unsigned char ESP_OpenSocket (void);
void ESP_OpenSocketResetSM (void);
void ESP_CloseSocketResetSM (void);
unsigned char ESP_CloseSocket (void);

#endif /* HLK_RM04_H_ */


