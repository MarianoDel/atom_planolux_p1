/*
 * HLK_RM04.h
 *
 *  Created on: 26/04/2016
 *      Author: Mariano
 */

#ifndef HLK_RM04_H_
#define HLK_RM04_H_

#include "hard.h"

#define HLK_RM04_PRESENT

//--- Timeouts para el HLK -----------------------------------//
#define TT_HLK_AT_MODE		300
#define TT_HLK_RESET		6000
#define TT_HLK_AT_MINI		5		//timeout de esopera de chars luego de una comienzo
#define TT_AT_1SEG			1000

//--- Estados para el HLK -----------------------------------//
#define COMM_INIT			0
#define COMM_TO_AT			1
#define COMM_AT_ANSWER		2
#define COMM_WAIT_PARSER	3
#define COMM_WAIT_ANSWER	4
#define COMM_VERIFY_ANSWER	5

#define CMD_RESET	0
#define CMD_PROC	1
#define CMD_ONLY_CHECK	2

#define HLK_PIN_ON	SW_RX
#define HLK_PIN_OFF	SW_TX

//--- Modos del HLK -----------------------------------------//
#define UNKNOW_MODE				0
#define AT_MODE					1
#define TRANSPARENT_MODE		2
#define GOING_AT_MODE			3

#define USART_CALLER	0
#define PROCESS_CALLER	1

enum HlkConfigState
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

enum HlkEnaState
{
	ENA_INIT = 0,
	ENA_ASK_AT
};

enum HlkTranspState
{
	TRANSP_INIT = 0,
	TRANSP_INIT1,
	TRANSP_GOTRANSP,
	TRANSP_GOTRANSP_WAIT
};

//--- Funciones del Modulo -----------------------------------//
void CheckVersion (char *);
void SendCommandWithAnswer(const char *, void (*pCall) (char *));

unsigned char HLK_Mode(void);
void HLK_ATModeRx (unsigned char);
unsigned char SendCommandWaitAnswer (const char *, unsigned char);
void HLK_TransparentModeRx (unsigned char);
void HLK_ATProcess (void);
unsigned char HLKToATMode (unsigned char);
void HLKPreParser(unsigned char *);
unsigned char HLKVerifyVersion(unsigned char *);
unsigned char HLK_SendConfig (unsigned char);
unsigned char HLK_EnableNewConn (unsigned char);
unsigned char HLK_GoTransparent (unsigned char);

#endif /* HLK_RM04_H_ */


