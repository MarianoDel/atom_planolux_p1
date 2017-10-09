/*
 * mqtt_wifi_interface.h
 *
 *  Created on: 03/10/2016
 *      Author: Mariano
 */

#ifndef MQTT_WIFI_INTERFACE_H_
#define MQTT_WIFI_INTERFACE_H_

#include "MQTTClient.h"
#include <stdint.h>



typedef enum
{
  WiFi_TIME_OUT_ERROR           = 0,
  WiFi_MODULE_SUCCESS           = 1,
  WiFi_HAL_OK,
  WiFi_NOT_SUPPORTED,
  WiFi_NOT_READY,
  WiFi_SCAN_FAILED,
  WiFi_AT_CMD_BUSY,
  WiFi_SSID_ERROR,
  WiFi_CONFIG_ERROR,
  WiFi_STA_MODE_ERROR,
  WiFi_AP_MODE_ERROR,
  WiFi_AT_CMD_RESP_ERROR,
  WiFi_AT_FILE_LENGTH_ERROR,
  WiFi_HAL_UART_ERROR,
  WiFi_IN_LOW_POWER_ERROR,
  WiFi_HW_FAILURE_ERROR,
  WiFi_HEAP_TOO_SMALL_WARNING,
  WiFi_STACK_OVERFLOW_ERROR,
  WiFi_HARD_FAULT_ERROR,
  WiFi_MALLOC_FAILED_ERROR,
  WiFi_INIT_ERROR,
  WiFi_POWER_SAVE_WARNING,
  WiFi_SIGNAL_LOW_WARNING,
  WiFi_JOIN_FAILED,
  WiFi_SCAN_BLEWUP,
  WiFi_START_FAILED_ERROR,
  WiFi_EXCEPTION_ERROR,
  WiFi_DE_AUTH,
  WiFi_DISASSOCIATION,
  WiFi_UNHANDLED_IND_ERROR,
  WiFi_RX_MGMT,
  WiFi_RX_DATA,
  WiFi_RX_UNK
} WiFi_Status_t;

typedef enum {
  QUICKSTART = 0,
  REGISTERED = 1,
} ibm_mode_t;


typedef struct mqtt_vars
{
	uint8_t pub_topic[48];
	uint8_t sub_topic[48];
	uint8_t clientid[64];
	enum QoS qos;
	uint8_t username[32];
	uint8_t password[32]; // feedid?
	uint8_t hostname[64];
	uint8_t device_type[32];
	uint8_t org_id[32];
	uint32_t port;
	uint8_t protocol; // //t -> tcp , s-> secure tcp, c-> secure tcp + certificates
	ibm_mode_t ibm_mode; // QUICKSTART, REGISTERED
} MQTT_vars;


void MQTTtimer_init(void);

void Config_MQTT_Mosquitto ( MQTT_vars *);
void TIM4_IRQHandler(void);
void Error_Handler(void);
void messageArrived(MessageData* md);
int CheckForPubs (Client * c, unsigned short timeout_ms);


unsigned char wifi_socket_client_open(uint8_t * hostname, uint32_t port_number, uint8_t * protocol, uint8_t * sock_id);
unsigned char wifi_socket_client_write(uint8_t sock_id, uint16_t DataLength,char * pData);
unsigned char wifi_socket_client_close(uint8_t sock_close_id);

#endif /* MQTT_WIFI_INTERFACE_H_ */
