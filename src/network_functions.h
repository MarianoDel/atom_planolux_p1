/*
 * network_functions.h
 *
 *  Created on: 11/10/2016
 *      Author: Mariano
 */

#ifndef NETWORK_FUNCTIONS_H_
#define NETWORK_FUNCTIONS_H_

#include "MQTTClient.h"

//--- WIFI Function States ---//
typedef enum {
  wifi_state_reset = 0,
  wifi_state_ready,
  wifi_state_sending_conf,
  wifi_state_wait_ip,
  wifi_state_wait_ip1,
  wifi_state_idle,
  wifi_state_connected,
  wifi_state_connecting,
  wifi_state_disconnected,
  wifi_state_error,
  wifi_state_socket_close,
  wifi_undefine_state       = 0xFF,
} wifi_state_t;

//--- MQTT Function States ---//
typedef enum {
	mqtt_init = 0,
	mqtt_sending_connect,
	mqtt_waiting_connack_load,
	mqtt_waiting_connack,
	mqtt_connect_failed,
	mqtt_connect,
	mqtt_pub_prepare,
	mqtt_pub,
	mqtt_pub_failed,
	mqtt_waiting_pubcomp,
	mqtt_waiting_puback,
	mqtt_sending_subscribe,
	mqtt_waiting_suback,
	mqtt_subscribe_failed,
	mqtt_device_control,
	mqtt_undefine_state       = 0xFF,
} mqtt_state_t;


void WIFIFunctionResetSM (void);
unsigned char WIFIFunction (void);

void MQTTFunctionResetSM (void);
unsigned char MQTTFunction (void);
void SubsCallBack(MessageData*);

#endif /* NETWORK_FUNCTIONS_H_ */
