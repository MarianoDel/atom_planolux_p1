   /**
  ******************************************************************************
  * @file    main.c
  * @author  Central LAB
  * @version V1.0.0
  * @date    17-Oct-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  * 
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "IBM_Bluemix_Config.h"

/**
   * @mainpage FP-CLD-BLUEMIX1  firmware package
   * <b>Introduction</b> <br>
   * FP-CLD-BLUEMIX1 provides a software running on STM32 which offers a complete middleware to build applications based on Wi-Fi connectivity (SPW01SA) and to connect STM32 Nucleo boards 
   * with IBM Cloud based services. This firmware package includes Components Device Drivers, Board Support Package and example application for STMicroelectronics X-NUCLEO-IDW01M1
   * X-NUCLEO-NFC01A1, X-NUCLEO-IKS01A1.

   * <b>IBM Bluemix example applications:</b><br>
   * This application reads the sensor data values from the accelerometer, magnetometer and Gyroscope, which are transmitted to the IBM IoT Cloud through WiFi. 
   * The URL of the web page where sensors data can be visualized is also written in NFC tag. 
   * The application is configured by default to run in Quickstart mode for data visualization only, but can be quickly modified in order to register and control the 
   * device in IBM Cloud (the latter mode requires an account on IBM Bluemix Cloud). 
  
   * <b>Toolchain Support</b><br>
   * The library has been developed and tested on following toolchains:
   *        - IAR Embedded Workbench for ARM (EWARM) toolchain V7.20 + ST-Link
   *        - Keil Microcontroller Development Kit (MDK-ARM) toolchain V5.10 + ST-LINK
   *        - System Workbench for STM32 (SW4STM32) V1.2 + ST-LINK
*/


/** @defgroup  FP-CLD-BLUEMIX1 
  * @brief Expample code to connect with IBM Bluemix 
  * @{
  */

/* Extern ()      ------------------------------------------------------------*/
extern UART_HandleTypeDef UartMsgHandle;
extern char print_msg_buff[512];
extern WiFi_Status_t GET_Configuration_Value(char* sVar_name,uint32_t *aValue);

/* Private define ------------------------------------------------------------*/
#define WIFI_SCAN_BUFFER_LIST           15
#define WIFI_SCAN_BUFFER_SIZE           512
#define APPLICATION_DEBUG_MSG            1
/* 
   NFC 0 --> Skip writing dynamic NFC when NFC not used
*/
#define USE_NFC                         1  

/* Private function prototypes -----------------------------------------------*/
static void floatToInt(float in, int32_t *out_int, int32_t *out_dec, int32_t dec_prec);
void SystemClock_Config(void);
WiFi_Status_t wifi_get_AP_settings(void);
void prepare_json_pkt (uint8_t * buffer);
void Get_MAC_Add (uint8_t *macadd);


#ifdef USART_PRINT_MSG
#define printf(arg)    { memset(print_msg_buff, 0x00, sizeof(print_msg_buff)); \
                         sprintf((char*)print_msg_buff,arg);   \
                         HAL_UART_Transmit(&UartMsgHandle, (uint8_t*)print_msg_buff, strlen(print_msg_buff), 1000); }
#endif                     

/* WiFi. Private variables ---------------------------------------------------------*/  
typedef enum {
  wifi_state_reset = 0,
  wifi_state_ready,
  wifi_state_idle,
  wifi_state_connected,
  wifi_state_connecting,
  wifi_state_disconnected,
  wifi_state_error,
  wifi_state_socket_close,
  mqtt_socket_create,
  client_conn,
  mqtt_connect,
  mqtt_sub,
  mqtt_pub,
  mqtt_device_control,
  wifi_undefine_state       = 0xFF,  
} wifi_state_t;

wifi_state_t wifi_state;
wifi_state_t wifi_old_state;
wifi_config config;
wifi_scan net_scan[WIFI_SCAN_BUFFER_LIST];
/* Console SSID/PWD */
uint8_t console_input[1], console_count=0;
char console_ssid[40];
char console_psk[20];
wifi_bool set_AP_config = WIFI_FALSE, SSID_found = WIFI_FALSE;
uint8_t macadd[17];
uint8_t json_buffer[512];
/* Default configuration SSID/PWD */ 
char * ssid = "STM";
char * seckey = "STMdemoPWD";
WiFi_Priv_Mode mode = WPA_Personal; 

/* MQTT. Private variables ---------------------------------------------------------*/  
unsigned char MQTT_read_buf[512];
unsigned char MQTT_write_buf[512];
Network  n;
Client  c;
MQTTMessage  MQTT_msg;
uint8_t url_ibm[80]; 
MQTT_vars mqtt_ibm_setup;
ibm_mode_t ibm_mode;  // EQ. Move this to IBM struct. 
MQTTPacket_connectData options = MQTTPacket_connectData_initializer; 

I2C_HandleTypeDef hi2c1;
#if USE_NFC
/* NFC. I2C handler declaration */
static sURI_Info URI;
#endif

 

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void){

  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  uint32_t i;
  
  /* HAL Init  */ 
  HAL_Init();  
  /* Timer Init */
  SystemClock_Config();  
  /* WiFi Timer config */
  Timer_Config( ); 
  
  /* UART Init */
  UART_Configuration(115200); 
#ifdef USART_PRINT_MSG
  UART_Msg_Gpio_Init();
  USART_PRINT_MSG_Configuration(115200);
#endif     

  printf("\r\n ******************************************************************* \r\n");
  printf("\r\n STMicroelectronics demo to visualize sensors data in IBM IoT cloud. \r\n");
  printf("\r\n ******************************************************************* \r\n");
  
  /********************************/
  /* Sensors Init                 */
  /********************************/
  BSP_IMU_6AXES_Init();
  BSP_HUM_TEMP_Init();
  BSP_PRESSURE_Init();  
  BSP_MAGNETO_Init();
#if APPLICATION_DEBUG_MSG
  printf("\r\n [D]. Initialized Sensors. \r\n");  
#endif  
     
#if USE_NFC
  /*****************************/
  /* NFC Init                 */
  /*****************************/
  while (TT4_Init() != SUCCESS);    
#if APPLICATION_DEBUG_MSG
  printf("\r\n [D]. Initialized NFC. \r\n");  
#endif  
#endif  
  
  /*****************************/
  /* WiFi Init                 */
  /*****************************/
  /* Config WiFi : disable low power mode */
  config.power=active;
  config.power_level=high;
  config.dhcp=on;//use DHCP IP address
  config.web_server=WIFI_TRUE;
  /* Get AP configuration */
  status = wifi_get_AP_settings();
  if(status!=WiFi_MODULE_SUCCESS)
  {
    printf("\r\n[E]. Error in AP Settings\r\n");
    return 0;
  }
  /* Initialize WiFi module */
  wifi_state = wifi_state_idle;
  status = wifi_init(&config);
  if(status!=WiFi_MODULE_SUCCESS)
  {
    printf("\r\n[E].Error in Config\r\n");
    return 0;
  }  
#if APPLICATION_DEBUG_MSG
  printf("\r\n [D]. WiFi initialized. \r\n");  
#endif  
  

  /******************************************/
  /* Initialize MQTT & IBM related parts     */ 
  /******************************************/   
  /* Initialize network interface for MQTT  */
  NewNetwork(&n);
  /* Initialize MQTT timers */
  MQTTtimer_init();
  /* Get MAC address from WiFi */ 
  Get_MAC_Add ( macadd ); 
#if APPLICATION_DEBUG_MSG
  printf("\r\n [D]. WiFi MAC address: \r\n");
  printf((char *)macadd);  
#endif  
  /* Config MQTT pars for IBM */
  Config_MQTT_IBM ( &mqtt_ibm_setup, macadd ); 
  /* Compose Quickstart URL */  
  Compose_Quickstart_URL ( url_ibm, macadd ); 
  printf("\r\n [D] IBM Quickstart URL (https://+)  \r\n");
  printf((char *)url_ibm); 
  printf("\r\n");
  HAL_Delay(200);  

#if USE_NFC
  strcpy(URI.protocol,URI_ID_0x04_STRING);   
  strcpy(URI.URI_Message,(char *)url_ibm);
  strcpy(URI.Information,"\0");
  while (TT4_WriteURI(&URI) != SUCCESS);  
#if APPLICATION_DEBUG_MSG
  printf("\r\n [D] Written URL in dynamic NFC tag  \r\n");
#endif
#endif  
  
  while (1)
  {
    switch (wifi_state) 
    {      
     case wifi_state_reset:
        break;
      
     case wifi_state_ready: 
       HAL_Delay(20);
        status = wifi_network_scan(net_scan, WIFI_SCAN_BUFFER_LIST);
        if(status==WiFi_MODULE_SUCCESS)
        {        
          for ( i=0; i<WIFI_SCAN_BUFFER_LIST; i++ )
            {
                if(( (char *) strstr((const char *)net_scan[i].ssid,(const char *)console_ssid)) !=NULL)
                {    
                   SSID_found = WIFI_TRUE;
                   wifi_connect((uint8_t *)console_ssid, (uint8_t *)console_psk, mode);
                   break;
                 }
            }
            if(!SSID_found) 
            {
              printf("\r\n[E]. Error, given SSID not found! \r\n");
            }
            memset(net_scan, 0x00, sizeof(net_scan));
#if APPLICATION_DEBUG_MSG
            printf("\r\n [D] Connected to network with SSID  \r\n");
            printf((char *)console_ssid);  
#endif  
        }
        else
            printf("\r\n[E]. Error, network AP not found! \r\n");
  
        wifi_state = wifi_state_idle; 
        break;

    case wifi_state_connected:
        // Low power mode not used
        break;
        
    case wifi_state_disconnected:
        wifi_state = wifi_state_reset;
        break;
     
    case mqtt_socket_create:
       
      if(spwf_socket_create (&n,mqtt_ibm_setup.hostname, mqtt_ibm_setup.port, &mqtt_ibm_setup.protocol)<0)
      {
          printf("\r\n [E]. Socket opening with IBM MQTT broker failed. Please check MQTT configuration. Trying reconnection.... \r\n");
          printf((char*)mqtt_ibm_setup.hostname);
          printf("\r\n");
          printf((char*)(&mqtt_ibm_setup.protocol));
          printf("\r\n");
          wifi_state=mqtt_socket_create;
      }
      else
      {   
          /* Initialize MQTT client structure */
          MQTTClient(&c,&n, 4000, MQTT_write_buf, sizeof(MQTT_write_buf), MQTT_read_buf, sizeof(MQTT_read_buf));
          wifi_state=mqtt_connect;
      
          printf("\r\n [D]. Created socket with MQTT broker. \r\n");
      }    
      break;
      
    case mqtt_connect:     
      options.MQTTVersion = 3;
      options.clientID.cstring = (char*)mqtt_ibm_setup.clientid;
      options.username.cstring = (char*)mqtt_ibm_setup.username;
      options.password.cstring = (char*)mqtt_ibm_setup.password;   
      if (MQTTConnect(&c, &options) < 0)
      {  
          printf("\r\n [E]. Client connection with IBM MQTT broker failed. Please check MQTT configuration. Trying reconnection....\r\n");
          wifi_state=mqtt_connect;
      }
      else 
      {  
         if ( mqtt_ibm_setup.ibm_mode == QUICKSTART )
         {
             printf("\r\n [D]. Connected with IBM MQTT broker for Quickstart mode (only MQTT publish supported) \r\n");
             /* Quickstart mode. We only publish data. */
             wifi_state=mqtt_pub;
         } 
         else
         {
             printf("\r\n [D]. Connected with IBM MQTT broker for Registered devices mode (requires account on IBM Bluemix; publish/subscribe supported) \r\n");
             /* Registered device mode. */
             wifi_state=mqtt_sub;
         } 
      }
      HAL_Delay(1500);
     break;
     
    case mqtt_sub: 
       /* Subscribe to topic */ 
       if( MQTTSubscribe(&c, (char*)mqtt_ibm_setup.sub_topic, mqtt_ibm_setup.qos, messageArrived) < 0)
       {
          printf("\r\n [E]. Subscribe with IBM MQTT broker failed. Please check MQTT configuration. \r\n");
       }
       else
       {
         printf("\r\n [D] Subscribed to topic:  \r\n");
         printf((char *)mqtt_ibm_setup.sub_topic);             
       }
       HAL_Delay(1500);     
       wifi_state=mqtt_pub; 
      break;
      
    case mqtt_pub:
      /* Prepare MQTT message */
      prepare_json_pkt(json_buffer);
      MQTT_msg.qos=QOS0;
      MQTT_msg.dup=0;
      MQTT_msg.retained=1;
      MQTT_msg.payload= (char *) json_buffer;
      MQTT_msg.payloadlen=strlen( (char *) json_buffer);  
      
      /* Publish MQTT message */
      if ( MQTTPublish(&c,(char*)mqtt_ibm_setup.pub_topic,&MQTT_msg) < 0)
      {  
          printf("\r\n [E]. Failed to publish data. Reconnecting to MQTT broker.... \r\n");
          wifi_state=mqtt_connect;
      } 
      else
      {
          if (mqtt_ibm_setup.ibm_mode == REGISTERED)
          { 
                printf("\r\n [D]. Wait 2 sec to see if any data is received \r\n"); 
                /* Wait 2 seconds to see if data is received */ 
                MQTTYield(&c, 2000);
          }      
          else
                HAL_Delay(2000);
#if APPLICATION_DEBUG_MSG        
          printf("\r\n [D]. Sensor data are published to IBM cloud \r\n");
          printf(MQTT_msg.payload);
#endif
       } 
       break;
   
    case wifi_state_idle:
        printf(".");
        HAL_Delay(500);
        break;    
        
      default:
      break;
    }     
  }  
}


/**
  * @brief  Query the User for SSID, password, encryption mode and hostname
  * @param  None
  * @retval WiFi_Status_t : Wifi status
  */
WiFi_Status_t wifi_get_AP_settings(void)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  printf("\r\nDo you want to setup SSID?(y/n):");
  printf(" ");
  HAL_Delay(200);
  HAL_UART_Receive(&UartMsgHandle, (uint8_t *)console_input, 1, 100000);
  if(console_input[0]=='y') 
        {
              set_AP_config = WIFI_TRUE;  
              printf("\r\n\nEnter the SSID:");
              printf(" ");
              HAL_Delay(200);

              console_count=0;
              while(1)
              {                
                HAL_UART_Receive(&UartMsgHandle, (uint8_t *)console_input, 1, 100000);
                if(console_input[0]=='\r' || console_input[0]=='\n') 
                    {
                      //do nothing
                    }
                else
                    console_ssid[console_count++]=console_input[0];
                
                if(console_input[0]=='\n') 
                    {
                        break;
                    }
                if(console_count==39) 
                    {
                        printf("Exceeded number of ssid characters permitted");
                        return WiFi_NOT_SUPPORTED;
                    }    
              }              
              
              printf("\r\nEnter the password:");
              printf(" ");
              HAL_Delay(200);
              console_count=0;
              while(1)
              {                
                HAL_UART_Receive(&UartMsgHandle, (uint8_t *)console_input, 1, 100000);
                if(console_input[0]=='\r' || console_input[0]=='\n') 
                    {
                      //do nothing
                    }
                else
                    console_psk[console_count++]=console_input[0];
                
                if(console_input[0]=='\n') 
                    {
                        break;
                    }
                if(console_count==19) 
                    {
                        printf("Exceeded number of psk characters permitted");
                        return WiFi_NOT_SUPPORTED;
                    }    
              }
              
              printf("\r\nEnter the encryption mode(0:Open, 1:WEP, 2:WPA2/WPA2-Personal):");
              printf(" ");
              HAL_Delay(200);
              HAL_UART_Receive(&UartMsgHandle, (uint8_t *)console_input, 1, 100000);
              switch(console_input[0])
              {
                case '0':
                  mode = None;
                  break;
                case '1':
                  mode = WEP;
                  break;
                case '2':
                  mode = WPA_Personal;
                  break;
                default:
                  printf("\r\nWrong Entry. Priv Mode is not compatible\n");
                  return WiFi_NOT_SUPPORTED;              
              }
              
        } else 
            {
                printf("\r\n\nModule will connect with default settings.");
                memcpy(console_ssid, (const char*)ssid, strlen((char*)ssid));
                memcpy(console_psk, (const char*)seckey, strlen((char*)seckey));
            }
  
  printf("\r\n/*************************************************************\r\n");
  printf("\r\n * Configuration Complete                                     \r\n");
  printf("\r\n * Please make sure a server is listening at given hostname   \r\n");
  printf("\r\n *************************************************************\r\n");
  
  return status;
}


/**
  * @brief  Wi-Fi callback activated when Wi-Fi is on 
  * @param  None  
  * @retval None
  */
void ind_wifi_on()
{
  printf("\r\n[D]. Wi-Fi on \r\n");
  wifi_state = wifi_state_ready;
}

/**
  * @brief  Wi-Fi callback activated when Wi-Fi is connected to AP 
  * @param  None  
  * @retval None
  */
void ind_wifi_connected()
{
  wifi_state = mqtt_socket_create;
}

/**
  * @brief  Wi-Fi callback activated Wi-Fi is resuming from sleep mode 
  * @param  None  
  * @retval None
  */
void ind_wifi_resuming()
{
  printf("\r\n [E]. Wifi resuming from sleep user callback... \r\n");
}

/**
  * @brief  Wi-Fi callback activated in case of connection error
  * @param  None  
  * @retval None
  */
void ind_wifi_connection_error(WiFi_Status_t WiFi_DE_AUTH)
{
  printf("\r\n [E]. WiFi connection error. Trying to reconnect to WiFi... \r\n");
  wifi_state=wifi_state_ready;
}

/**
  * @brief  Wi-Fi callback activated when remote server is closed  
  * @param  socket_closed_id : socket identifier  
  * @retval None
  */
void ind_wifi_socket_client_remote_server_closed(uint8_t * socket_closed_id)
{
   printf("\r\n[E]. Remote disconnection from server. Trying to reconnect to MQTT broker... \r\n");
   wifi_state = mqtt_socket_create;
}

 /**
  * @brief  Prepare JSON packet with sensors data
  * @param  buffer : buffer that will contain sensor data in JSON format 
  * @retval None
  */
 void prepare_json_pkt (uint8_t * buffer)
{
      int32_t d1, d2, d3, d4, d5, d6;
      char tempbuff[40];
      volatile AxesRaw_TypeDef ACC_Value;
      volatile AxesRaw_TypeDef GYR_Value;
      volatile float HUMIDITY_Value;
      volatile float TEMPERATURE_Value;
      volatile float PRESSURE_Value;
      volatile AxesRaw_TypeDef MAG_Value;
    
             
      strcpy((char *)buffer,"{\"d\":{\"myName\":\"Nucleo\"");     
      BSP_HUM_TEMP_GetTemperature((float *)&TEMPERATURE_Value);
      floatToInt(TEMPERATURE_Value, &d1, &d2, 2);
      sprintf(tempbuff, ",\"A_Temperature\":%lu.%lu",d1, d2);
      strcat((char *)buffer,tempbuff);
      
      BSP_HUM_TEMP_GetHumidity((float *)&HUMIDITY_Value);
      floatToInt(HUMIDITY_Value, &d3, &d4, 2);
      sprintf(tempbuff, ",\"A_Humidity\":%lu.%lu",d3,d4 );
      strcat(  (char *)buffer,tempbuff);
      
      BSP_PRESSURE_GetPressure((float *)&PRESSURE_Value);
      floatToInt(PRESSURE_Value, &d5, &d6, 2);
      sprintf(tempbuff, ",\"A_Pressure\":%lu.%lu",d5,d6 );
      strcat((char *)buffer,tempbuff);
      
      BSP_IMU_6AXES_X_GetAxesRaw((AxesRaw_TypeDef *)&ACC_Value);
      sprintf(tempbuff, ",\"Acc-X\":%d",ACC_Value.AXIS_X);
      strcat((char *)buffer,tempbuff);
      sprintf(tempbuff, ",\"Acc-Y\":%d",ACC_Value.AXIS_Y);
      strcat((char *)buffer,tempbuff);
      sprintf(tempbuff, ",\"Acc-Z\":%d",ACC_Value.AXIS_Z);
      strcat((char *)buffer,tempbuff);
      
      BSP_IMU_6AXES_G_GetAxesRaw((AxesRaw_TypeDef *)&GYR_Value);
      sprintf(tempbuff, ",\"GYR-X\":%d",GYR_Value.AXIS_X);
      strcat((char *)buffer,tempbuff);
      sprintf(tempbuff, ",\"GYR-Y\":%d",GYR_Value.AXIS_Y);
      strcat((char *)buffer,tempbuff);
      sprintf(tempbuff, ",\"GYR-Z\":%d",GYR_Value.AXIS_Z);
      strcat((char *)buffer,tempbuff);
     
      BSP_MAGNETO_M_GetAxesRaw((AxesRaw_TypeDef *)&MAG_Value);
      sprintf(tempbuff, ",\"MAG-X\":%d",MAG_Value.AXIS_X);
      strcat((char *)buffer,tempbuff);
      sprintf(tempbuff, ",\"MAG-Y\":%d",MAG_Value.AXIS_Y);
      strcat((char *)buffer,tempbuff);
      sprintf(tempbuff, ",\"MAG-Z\":%d",MAG_Value.AXIS_Z);
      strcat((char *)buffer,tempbuff);
      
      strcat((char *)buffer,"}}");
      
      return;
}


/**
  * @brief  GET MAC Address from WiFi
  * @param  macadd : buffer containing MAC address
  * @retval None
  */
void Get_MAC_Add (uint8_t *macadd)
{
   uint8_t mactemp[64];
   uint16_t i,j; 
  
   GET_Configuration_Value("nv_wifi_macaddr",(uint32_t *)mactemp);
   mactemp[17] = '\0';

   for(i=0,j=0;i<strlen((char *)mactemp);i++){
     if(mactemp[i]!=':'){
       macadd[j]=mactemp[i];       
       j++;
     } 
   }
   macadd[j]='\0';
   
   return;
}

/**
  * @brief  MQTT Callback function
  * @param  MessageData : MQTT message received
  * @retval WiFi_Status_t
  */
void messageArrived(MessageData* md)
{
    MQTTMessage* message = md->message;
    
    printf("\r\n [D]. MQTT payload received is: \r\n");
    printf((char*)message->payload);  
}


#if USE_NFC
/*
**
**
*/
void M24SR_I2CInit ( void )
{
  if( hi2c1.Instance == M24SR_I2C)
	  HAL_I2C_DeInit(&hi2c1);
	
  hi2c1.Instance 	     = M24SR_I2C;
  hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
  hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
  hi2c1.Init.OwnAddress1     = 0;
  hi2c1.Init.OwnAddress2     = 0;
#if (defined USE_STM32F4XX_NUCLEO) || (defined USE_STM32L1XX_NUCLEO) || \
	  (defined USE_STM32F1XX_NUCLEO)
  hi2c1.Init.ClockSpeed      = M24SR_I2C_SPEED;
  hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
#elif (defined USE_STM32F0XX_NUCLEO) || (defined USE_STM32L0XX_NUCLEO) || \
      (defined USE_STM32F3XX_NUCLEO)
  hi2c1.Init.Timing          = M24SR_I2C_SPEED;
#endif
   
	if(HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
     while(1); 
  }	
}

void M24SR_GPOInit ( void )
{
  GPIO_InitTypeDef GPIO_InitStruct;
  INIT_CLK_GPO_RFD();
	
#ifndef I2C_GPO_INTERRUPT_ALLOWED
  GPIO_InitStruct.Pin = M24SR_GPO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(M24SR_GPO_PIN_PORT, &GPIO_InitStruct);
#else
  GPIO_InitStruct.Pin = M24SR_GPO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(M24SR_GPO_PIN_PORT, &GPIO_InitStruct);
  /* Enable and set EXTI9_5 Interrupt to the lowest priority */
#if (defined USE_STM32F4XX_NUCLEO) || (defined USE_STM32F3XX_NUCLEO) || \
     (defined USE_STM32L1XX_NUCLEO) || (defined USE_STM32F1XX_NUCLEO) 
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
#elif (defined USE_STM32L0XX_NUCLEO) || (defined USE_STM32F0XX_NUCLEO) 
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);    
#endif
  
#endif
	
  GPIO_InitStruct.Pin = M24SR_RFDIS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(M24SR_RFDIS_PIN_PORT, &GPIO_InitStruct);
}

/**
  * @brief  This function wait the time given in param (in milisecond)
  * @param	time_ms: time value in milisecond
  */
void M24SR_WaitMs(uint32_t time_ms)
{
	HAL_Delay(time_ms);
}

/**
  * @brief  This function retrieve current tick
  * @param	ptickstart: pointer on a variable to store current tick value
  */
void M24SR_GetTick( uint32_t *ptickstart )
{
	*ptickstart = HAL_GetTick();
}

/**
  * @brief  This function read the state of the M24SR GPO
	* @param	none
  * @retval GPIO_PinState : state of the M24SR GPO
  */
void M24SR_GPO_ReadPin( GPIO_PinState * pPinState)
{
	*pPinState = HAL_GPIO_ReadPin(M24SR_GPO_PIN_PORT,M24SR_GPO_PIN);
}

/**
  * @brief  This function set the state of the M24SR RF disable pin
	* @param	PinState: put RF disable pin of M24SR in PinState (1 or 0)
  */
void M24SR_RFDIS_WritePin( GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(M24SR_RFDIS_PIN_PORT,M24SR_RFDIS_PIN,PinState);
}
#endif 

/*
**
**
*/

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
  while (1)
  {
  }
}
#endif

/**
 * @brief  Splits a float into two integer values.
 * @param  in : input floating number
 *         out_int : output integer number
 *         out_dec : output decimal number
 * @retval None
 */
static void floatToInt(float in, int32_t *out_int, int32_t *out_dec, int32_t dec_prec)
{
    *out_int = (int32_t)in;
    in = in - (float)(*out_int);
    *out_dec = (int32_t)trunc(in*pow(10,dec_prec));
}


/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSI)
 *            SYSCLK(Hz)                     = 84000000
 *            HCLK(Hz)                       = 84000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            HSI Frequency(Hz)              = 16000000
 *            PLL_M                          = 16
 *            PLL_N                          = 336
 *            PLL_P                          = 4
 *            PLL_Q                          = 7
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale2 mode
 *            Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 

     clocked below the maximum system frequency, to update the voltage scaling value 

     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  
  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
   
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}


/**
 * @}
 */
 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
