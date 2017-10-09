/*
 * mqtt_wifi_interface.c
 *
 *  Created on: 03/10/2016
 *      Author: Mariano
 */

#include "mqtt_wifi_interface.h"
#include "ESP8266.h"
#include "main_menu.h"


#include <stdint.h>

//--- Externals ----------------------//
extern unsigned long MilliTimer;

//--- Globals ------------------------//


//--- Function Definitions -----------//

void Config_MQTT_Mosquitto ( MQTT_vars *mqtt_broker_setup)
{
    strcpy((char*)mqtt_broker_setup->pub_topic, "iot/json");
    strcpy((char*)mqtt_broker_setup->sub_topic, "");
    strcpy((char*)mqtt_broker_setup->clientid,"plano:p1:0001");
    //strcat((char*)mqtt_broker_setup->clientid,(char *)macadd);
    mqtt_broker_setup->qos = QOS0;
    strcpy((char*)mqtt_broker_setup->username,"test");
    strcpy((char*)mqtt_broker_setup->password,"test");
    strcpy((char*)mqtt_broker_setup->hostname,"quickstart.messaging.internetofthings.ibmcloud.com");
    strcpy((char*)mqtt_broker_setup->device_type,"");
    strcpy((char*)mqtt_broker_setup->org_id,"");
    mqtt_broker_setup->port = 1883; //TLS
    mqtt_broker_setup->protocol = 't'; // TLS no certificates

}

void MQTTtimer_init(void)
{
	MilliTimer = 0;
}

int CheckForPubs (Client * c, unsigned short timeout_ms)
{
    int rc = OK;
    Timer timer;

    InitTimer(&timer);
    countdown_ms(&timer, timeout_ms);

    rc = cycle(c, &timer);

    return rc;
}

/**
* @brief  wifi_socket_client_open
*         Open a network socket
* @param  Hostname hostname to connect to
*         portnumber portnumber of the Host to connect to
*         protocol tcp or udp protocol
*         sock_id socket id of the opened socket returned to the user
* @retval WiFi_Status_t : return status of socket open request
*/
unsigned char wifi_socket_client_open(uint8_t * hostname, uint32_t port_number, uint8_t * protocol, uint8_t * sock_id)
{

  unsigned char resp = RESP_CONTINUE;

//  /* AT+S.SOCKON=myserver,1234,t<cr> */
//  sprintf((char*)(char*)WiFi_AT_Cmd_Buff,AT_SOCKET_OPEN,hostname,(int)port_number,protocol);
//  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
//  if(status == WiFi_MODULE_SUCCESS)
//  {
//    status = USART_Receive_AT_Resp(Receive_SockON_ID);
//  }
//
//  *sock_id = SocketId; //return the socket id to the user

  return resp;
}

/**
* @brief  wifi_socket_client_write
*         Write len bytes of data to socket
* @param  sock_id : socket ID of the socket to write to
*         DataLength : data length to send
*         pData : pointer of data buffer to be written
* @retval WiFi_Status_t : return status of socket write request
*/
unsigned char wifi_socket_client_write(uint8_t sock_id, uint16_t DataLength,char * pData)
{
	unsigned char resp = RESP_CONTINUE;
  /* AT+S.SOCKW=00,11<cr> */
  //WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  //Check if sock_id is open
//  if(!open_sockets[sock_id])
//    return WiFi_NOT_READY;
//
//  if(DataLength>=1024)
//    return WiFi_NOT_SUPPORTED;
//
//  /* AT+S.SOCKW=00,11<cr> */
//  sprintf((char*)(char*)WiFi_AT_Cmd_Buff,AT_SOCKET_WRITE,sock_id,DataLength);
//  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
//  if(status == WiFi_MODULE_SUCCESS)
//  {
//    memset(WiFi_AT_Cmd_Buff, 0x00, sizeof WiFi_AT_Cmd_Buff);
//    memcpy((char*)(char*)WiFi_AT_Cmd_Buff, (char*) pData,DataLength);
//    WiFi_AT_Cmd_Buff[DataLength+1]='\r';
//    status = USART_Transmit_AT_Cmd(DataLength+2);
//    if(status == WiFi_MODULE_SUCCESS)
//      {
//        status = USART_Receive_AT_Resp(Receive_AT_Cmd_Response);
//      }
//  }

  return resp;

}

/**
* @brief  wifi_socket_client_close
*         The SOCKC command allows to close socket
* @param  the socket ID of the socket which needs to be closed.
* @retval WiFi_Status_t : return status of socket close request
*/
unsigned char wifi_socket_client_close(uint8_t sock_close_id)
{
	unsigned char resp = RESP_CONTINUE;
//  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
//  Reset_AT_CMD_Buffer();
//
//   /* AT+S.SOCKC=00<cr> */
//  sprintf((char*)(char*)WiFi_AT_Cmd_Buff,AT_SOCKET_CLOSE,sock_close_id);
//  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
//  //Reset the count and socket array
//  if(no_of_open_client_sockets>0)
//    no_of_open_client_sockets--;
//  open_sockets[sock_close_id] = WIFI_FALSE;
//
//  Set_AT_Cmd_Response_False = WIFI_TRUE;
//  return status;
	return resp;
}
