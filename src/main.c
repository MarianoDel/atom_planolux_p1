/**
  ******************************************************************************
  * @file    Template_2/main.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Use this template for new projects with stm32f0xx family.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_adc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//--- My includes ---//
#include "stm32f0x_gpio.h"
#include "stm32f0x_tim.h"
#include "uart.h"
#include "hard.h"
#include "lcd.h"

#include "core_cm0.h"
#include "adc.h"
#include "flash_program.h"
#include "main_menu.h"
#include "synchro.h"
#include "dmx_transceiver.h"
#include "standalone.h"
#include "grouped.h"
#include "networked.h"

#ifdef USE_RM04_WIFI
#include "HLK_RM04.h"
#endif
#ifdef USE_ESP_WIFI
#include "ESP8266.h"
#endif
#include "tcp_transceiver.h"

//para MQTT
//#include "MQTT_SPWF_interface.h"
#include "MQTTClient.h"
#include "rdm_util.h"
#include "mqtt_wifi_interface.h"
#include "network_functions.h"

//--- VARIABLES EXTERNAS ---//
volatile unsigned char timer_1seg = 0;

volatile unsigned short timer_led_comm = 0;
volatile unsigned short timer_for_cat_switch = 0;
volatile unsigned short timer_for_cat_display = 0;

volatile unsigned short wait_ms_var = 0;

unsigned char new_ldr_sample;

// ------- Externals del Puerto serie  -------
volatile unsigned char tx2buff[SIZEOF_DATA];
volatile unsigned char rx2buff[SIZEOF_DATA];

//volatile unsigned char TxBuffer_SPI [TXBUFFERSIZE];
//volatile unsigned char RxBuffer_SPI [RXBUFFERSIZE];
//volatile unsigned char *pspi_tx;
//volatile unsigned char *pspi_rx;
//volatile unsigned char spi_bytes_left = 0;

// ------- Externals del DMX -------
#ifdef USE_DMX
volatile unsigned char Packet_Detected_Flag;
volatile unsigned char DMX_packet_flag;
volatile unsigned char RDM_packet_flag;
volatile unsigned char dmx_receive_flag = 0;
volatile unsigned short DMX_channel_received = 0;
volatile unsigned short DMX_channel_selected = 1;
volatile unsigned char DMX_channel_quantity = 4;

volatile unsigned char data1[SIZEOF_DATA1];
//static unsigned char data_back[10];
volatile unsigned char data[SIZEOF_DATA];
#endif
// ------- Externals de los timers -------
//volatile unsigned short prog_timer = 0;
//volatile unsigned short mainmenu_timer = 0;
volatile unsigned short show_select_timer = 0;
volatile unsigned char switches_timer = 0;
volatile unsigned char acswitch_timer = 0;

volatile unsigned short scroll1_timer = 0;
volatile unsigned short scroll2_timer = 0;

volatile unsigned short standalone_timer;
volatile unsigned short standalone_enable_menu_timer;
//volatile unsigned short standalone_menu_timer;
volatile unsigned char grouped_master_timeout_timer;
volatile unsigned short take_temp_sample = 0;
volatile unsigned short take_ldr_sample = 0;
volatile unsigned short minutes = 0;
volatile unsigned char timer_wifi_bright = 0;

// ------- Externals de los modos -------
StandAlone_Typedef const StandAloneStruct_constant =
//StandAlone_Typedef __attribute__ ((section("memParams"))) const StandAloneStruct_constant =
		{
				.move_sensor_enable = 1,
				.ldr_enable = 0,
				.ldr_value = 100,
				.max_dimmer_value_percent = 100,
				.max_dimmer_value_dmx = 255,
//				.min_dimmer_value_percent = 10,
//				.min_dimmer_value_dmx = MIN_DIMMING,
				.min_dimmer_value_percent = 15,
				.min_dimmer_value_dmx = 37,
				.power_up_timer_value = 3000,
				.dimming_up_timer_value = 3000
		};

Grouped_Typedef const GroupedStruct_constant =
//Grouped_Typedef __attribute__ ((section("memParams1"))) const GroupedStruct_constant =
		{
				//parte master igual a StandAlone
				.move_sensor_enable = 1,
				.ldr_enable = 0,
				.ldr_value = 100,
				.max_dimmer_value_percent = 100,
				.max_dimmer_value_dmx = 255,
				.min_dimmer_value_percent = 1,
				.min_dimmer_value_dmx = MIN_DIMMING,
				.power_up_timer_value = 3000,
				.dimming_up_timer_value = 3000,
				//parte slave
				.grouped_mode = GROUPED_MODE_SLAVE,
				.grouped_dmx_channel = GROUPED_INITIAL_CHANNEL

		};

Networked_Typedef const NetworkedStruct_constant =
//Networked_Typedef __attribute__ ((section("memParams2"))) const NetworkedStruct_constant =
		{
				//parte slave,
				.networked_dmx_channel = NETWORKED_INITIAL_CHANNEL

		};

unsigned char saved_mode;

// ------- para determinar igrid -------
volatile unsigned char igrid_timer = 0;
volatile unsigned char vgrid_timer = 0;

// ------- del display LCD -------
const char s_blank_line [] = {"                "};

// ------- Externals de los switches -------
unsigned short s1;
unsigned short s2;
unsigned short sac;
unsigned char sac_aux;

// ------- Externals del HLK_RM04 -------
#ifdef USE_HLK_WIFI
unsigned short hlk_timeout = 0;
unsigned char hlk_mini_timeout = 0;
unsigned char hlk_answer = 0;
unsigned char hlk_transparent_finish = 0;
#endif

// ------- Externals del ESP8266 -------
#ifdef USE_ESP_WIFI
unsigned short esp_timeout = 0;
unsigned char esp_mini_timeout = 0;
unsigned char esp_answer = 0;
unsigned char esp_unsolicited_pckt = 0;
volatile unsigned char bufftcp[SIZEOF_BUFFTCP];
char bufftcp_transp[SIZEOF_BUFFTCP - 5];
volatile unsigned short tcp_send_timeout;
#endif

//#ifdef WIFI_TO_CEL_PHONE_PROGRAM
unsigned char messages [100];
//#endif
//--- VARIABLES GLOBALES ---//
parameters_typedef param_struct;

// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned short tcp_kalive_timer;
//volatile unsigned char display_timer;
volatile unsigned char filter_timer;

//volatile unsigned char door_filter;
//volatile unsigned char take_sample;
//volatile unsigned char move_relay;
volatile unsigned short secs = 0;
volatile unsigned short mqtt_func_timer = 0;
volatile unsigned short wifi_func_timer = 0;


// ------- del display -------
unsigned char v_opt [10];


// ------- del DMX -------
#ifdef USE_DMX
volatile unsigned char signal_state = 0;
volatile unsigned char dmx_timeout_timer = 0;
//unsigned short tim_counter_65ms = 0;
#endif

// ------- de los filtros DMX -------
#define LARGO_F		32
#define DIVISOR_F	5
unsigned char vd0 [LARGO_F + 1];
unsigned char vd1 [LARGO_F + 1];
unsigned char vd2 [LARGO_F + 1];
unsigned char vd3 [LARGO_F + 1];
unsigned char vd4 [LARGO_F + 1];


#define IDLE	0
#define LOOK_FOR_BREAK	1
#define LOOK_FOR_MARK	2
#define LOOK_FOR_START	3

/* MQTT. Private variables ---------------------------------------------------------*/
unsigned char MQTT_read_buf[SIZEOF_BUFFTCP_SEND];
unsigned char MQTT_write_buf[SIZEOF_BUFFTCP_SEND];
Network  n;
Client  c;
MQTTMessage  MQTT_msg;
//uint8_t url_ibm[80];
MQTT_vars mqtt_ibm_setup;
//ibm_mode_t ibm_mode;  // EQ. Move this to IBM struct.
MQTTPacket_connectData options = MQTTPacket_connectData_initializer;
uint8_t json_buffer[SIZEOF_BUFFTCP_SEND];
void prepare_json_pkt (uint8_t * buffer);



//--- FUNCIONES DEL MODULO ---//
void TimingDelay_Decrement(void);
void Update_PWM (unsigned short);
void UpdatePackets (void);

// ------- del display -------


// ------- del DMX -------
extern void EXTI4_15_IRQHandler(void);
#define DMX_TIMEOUT	20

//--- FILTROS DE SENSORES ---//
#define LARGO_FILTRO 16
#define DIVISOR      4   //2 elevado al divisor = largo filtro
//#define LARGO_FILTRO 32
//#define DIVISOR      5   //2 elevado al divisor = largo filtro
unsigned short vtemp [LARGO_FILTRO + 1];
unsigned short vpote [LARGO_FILTRO + 1];

//--- FIN DEFINICIONES DE FILTRO ---//


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
	unsigned char i,ii;
	unsigned char bytes_remain, bytes_read, need_ack = 0;
	unsigned char resp = RESP_CONTINUE;
	unsigned short local_meas, local_meas_last;
	unsigned char main_state = 0;
	char s_lcd [20];
	enum TcpMessages tcp_msg = NONE_MSG;
	unsigned char new_room = 0;
	unsigned char new_lamp = 0;
	unsigned char last_bright = 0;
	unsigned char show_ldr = 0;
	int dummy_resp = 0;

#ifdef USE_PROD_PROGRAM
	unsigned char jump_the_menu = 0;
#endif
	parameters_typedef * p_mem_init;
	//!< At this stage the microcontroller clock setting is already configured,
    //   this is done through SystemInit() function which is called from startup
    //   file (startup_stm32f0xx.s) before to branch to application main.
    //   To reconfigure the default setting of SystemInit() function, refer to
    //   system_stm32f0xx.c file

	//GPIO Configuration.
	GPIO_Config();


	//ACTIVAR SYSTICK TIMER
	if (SysTick_Config(48000))
	{
		while (1)	/* Capture error */
		{
			if (LED)
				LED_OFF;
			else
				LED_ON;

			for (i = 0; i < 255; i++)
			{
				asm (	"nop \n\t"
						"nop \n\t"
						"nop \n\t" );
			}
		}
	}


#if ((defined USE_ESP_WIFI) || (defined USE_HLK_WIFI))
	WRST_OFF;
	Wait_ms(2);
	WRST_ON;
#endif

//#ifdef WIFI_TO_CEL_PHONE_PROGRAM
//	RELAY_ON;
//#endif

	//ADC Configuration
	AdcConfig();

	//TIM Configuration.
	TIM_3_Init();
	TIM_14_Init();		//lo uso en la interrupcion de RX DMX
	TIM_15_Init();		//lo uso para SysTick en MQTT
	TIM_16_Init();		//para OneShoot() cuando funciona en modo master en envio de DMX
	TIM_17_Init();		//lo uso para el ADC de Igrid

	//--- PRUEBA DISPLAY LCD ---
	EXTIOff ();

	LCDInit();
	LED_ON;

	//--- Welcome code ---//
	Lcd_Command(CLEAR);
	Wait_ms(100);
	Lcd_Command(CURSOR_OFF);
	Wait_ms(100);
	Lcd_Command(BLINK_OFF);
	Wait_ms(100);
	CTRL_BKL_ON;

	//while (FuncShowBlink ((const char *) "Kirno Technology", (const char *) " WiFi Test V1.0 ", 2, BLINK_NO) != RESP_FINISH);
	while (FuncShowBlink ((const char *) "  PLANOLUX LLC  ", (const char *) "  Smart Driver  ", 2, BLINK_NO) != RESP_FINISH);
	LED_OFF;

	//----- Prueba de Timers MQTT ---------//
//    Timer tim;
//    InitTimer(&tim);
//	countdown_ms(&tim, 50);
//	while (1)
//	{
//
//
//		//Wait_ms(50);
//		if (expired(&tim))
//		{
//			countdown_ms(&tim, 50);
//			if (LED)
//				LED_OFF;
//			else
//				LED_ON;
//		}
//	}
	//----- Fin Prueba de Timers MQTT ---------//
	//----- Prueba switches ---------//
//	while (1)
//	{
//		if (CheckS1())
//		{
//			LCD_1ER_RENGLON;
//			LCDTransmitStr((const char *) "S1 -> ON        ");
//			i = 1;
//		}
//		else if (i == 1)
//		{
//			i = 0;
//			LCD_1ER_RENGLON;
//			LCDTransmitStr((const char *) "S1 -> OFF       ");
//		}
//
//		if (CheckS2())
//		{
//			LCD_2DO_RENGLON;
//			LCDTransmitStr((const char *) "S2 -> ON        ");
//			ii = 1;
//		}
//		else if (ii == 1)
//		{
//			ii = 0;
//			LCD_2DO_RENGLON;
//			LCDTransmitStr((const char *) "S2 -> OFF       ");
//		}
//
//		UpdateSwitches();
//	}

	//DE PRODUCCION Y PARA PRUEBAS EN DMX

	//Packet_Detected_Flag = 0;
	//DMX_channel_selected = 1;
	//DMX_channel_quantity = 4;
	USART1Config();
#ifdef VER_1_3
	USART2Config();
#endif
	EXTIOff();

#ifdef VER_1_2
	Update_TIM3_CH2 (255);
#endif
	DMX_TX_PIN_OFF;

	//---------- Prueba DMX_INPUT con y sin INT --------//
//	EXTIOn();
//	while (1)
//	{
////		if (DMX_INPUT)
////			LED_ON;
////		else
////			LED_OFF;
//	}
	//---------- Fin Prueba DMX_INPUT con y sin INT ----//

	//---------- Prueba USART2 --------//

//    while( 1 )
//    {
////    	Usart2SendUnsigned((unsigned char *) (const char *) "Primero\r\n", sizeof("Primero\r\n") - 1);
////    	Usart2SendUnsigned((unsigned char *) (const char *) "Segundo\r\n", sizeof("Segundo\r\n") - 1);
////    	Usart2SendUnsigned((unsigned char *) (const char *) "Tercero\r\n", sizeof("Tercero\r\n") - 1);
////    	Usart2SendUnsigned((unsigned char *) (const char *) "Cuarto\r\n", sizeof("Cuarto\r\n") - 1);
////      Wait_ms(3000);
//
////    	Usart2SendUnsigned((unsigned char *) (const char *) "PrimeroPrimero\r\n", sizeof("PrimeroPrimero\r\n") - 1);
////        Wait_ms(1);
////    	Usart2SendUnsigned((unsigned char *) (const char *) "SegundoSegundo\r\n", sizeof("SegundoSegundo\r\n") - 1);
////        Wait_ms(1);
////        Usart2SendUnsigned((unsigned char *) (const char *) "TerceroTercero\r\n", sizeof("TerceroTercero\r\n") - 1);
////        Wait_ms(1);
////        Usart2SendUnsigned((unsigned char *) (const char *) "CuartoCuarto\r\n", sizeof("CuartoCuarto\r\n") - 1);
////        Wait_ms(3000);
//
////    	for (i = 0; i < 255; i++)
////    	{
////        	Usart2SendUnsigned((unsigned char *) (const char *) "Primero\r\n", sizeof("Primero\r\n") - 1);
////        	Usart2SendUnsigned((unsigned char *) (const char *) "Segundo\r\n", sizeof("Segundo\r\n") - 1);
////        	Usart2SendUnsigned((unsigned char *) (const char *) "Tercero\r\n", sizeof("Tercero\r\n") - 1);
////        	Usart2SendUnsigned((unsigned char *) (const char *) "Cuarto\r\n", sizeof("Cuarto\r\n") - 1);
////    	}
////    	Wait_ms(3000);
//
//    	Usart2Send((char *) (const char *) "Primero\r\n");
//    	Usart2Send((char *) (const char *) "Segundo\r\n");
//    	Usart2Send((char *) (const char *) "Tercero\r\n");
//    	Usart2Send((char *) (const char *) "Cuarto\r\n");
//    	Wait_ms(3000);
//
//    }

    //---------- Fin Prueba USART2 --------//

	//---------- Prueba USART --------//

//    while( 1 )
//    {
//
//    	USARTSendSingle('M');
//        Wait_ms(500);
//
//        if (CTRL_BKL)
//        {
//        	LED_OFF;
//        	CTRL_BKL_OFF;
//        }
//        else
//        {
//        	LED_ON;
//        	CTRL_BKL_ON;
//        }
//
//    }

    //---------- Fin Prueba USART --------//

    //---------- Prueba Recibir DMX Pckts --------//
//	EXTIOn ();
//	DMX_channel_quantity = 1;
//	DMX_channel_selected = 6;
//	DMX_Ena();
//
//	while (1)
//	{
//		if (DMX_packet_flag)
//		{
//			//llego un paquete DMX
//			DMX_packet_flag = 0;
//
//			//en data tengo la info
//			Update_TIM3_CH1 (data[1]);
//
//			//prendo relay
//			if (data[1] > 20)
//			{
//				if (!RELAY)
//					RELAY_ON;
//			}
//			else if (data[1] < 10)
//			{
//				if (RELAY)
//					RELAY_OFF;
//			}
//
//		}
//		UpdatePackets();
//
//	}
	//---------- Fin Prueba Recibir DMX Pckts --------//

    //---------- Prueba Enviar DMX Pckts --------//
//	EXTIOn ();
//	DMX_channel_quantity = 1;
//	DMX_channel_selected = 1;
//	DMX_Ena();
//	i = 0;
//
//	while (1)
//	{
//		if (!timer_standby)
//		{
//			timer_standby = 100;
//			data1[1] = i;
//			data1[2] = i;
//			data1[3] = i;
//			data1[4] = i;
//			data1[5] = i;
//			data1[6] = i;
//			if (i < 255)
//				i++;
//			else
//				i = 0;
//
//			SendDMXPacket (PCKT_INIT);
//			show_ldr++;
//		}
//
//		if (DMX_packet_flag)
//		{
//			//llego un paquete DMX
//			DMX_packet_flag = 0;
//
//			//en data tengo la info
//			Update_TIM3_CH1 (data[1]);
//		}
//
//		if (show_ldr > 3)
//		{
//			show_ldr = 0;
//			LCD_1ER_RENGLON;
//			LCDTransmitStr(s_blank_line);
//			sprintf (s_lcd, "LDR: %04i", GetLDR());
//			LCD_1ER_RENGLON;
//			LCDTransmitStr(s_lcd);
//		}
//		UpdatePackets();
//	}
	//---------- Fin Prueba Recibir DMX Pckts --------//

#if defined (MQTT_MEM_ONLY) || defined (WIFI_TO_MQTT_BROKER)
#ifdef MQTT_MEM_ONLY
	main_state = mqtt_init;
	MQTTFunctionResetSM();
#endif

#ifdef WIFI_TO_MQTT_BROKER
	main_state = wifi_state_reset;
	WIFIFunctionResetSM();
#endif

    while( 1 )
    {
#ifdef MQTT_MEM_ONLY
    	//---------- Prueba Conexiones Prueba MQTT_MEM_ONLY ---------//
    	resp = MQTTFunction();
#endif

#ifdef WIFI_TO_MQTT_BROKER
    	//---------- Prueba Conexiones ESP8266 to MQTT BROKER (Mosquitto) ---------//
    	resp = WIFIFunction();
#endif

		if (CheckS2())
		{
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "S2 -> ON        ");
			ii = 1;
		}

    	//Procesos continuos
    	ESP_ATProcess ();
    	TCPProcess();
		UpdateSwitches();

    	if (!timer_wifi_bright)
    	{
    		timer_wifi_bright = 5;	//muevo un punto cada 5ms
    		if (new_room > last_bright)		//TODO: en vez de new_room deberia utilizar un filtro de los ultimos valores recibidos
    		{
    			last_bright++;
    			Update_TIM3_CH1 (last_bright);
    		}
    		else if (new_room < last_bright)
    		{
    			last_bright--;
    			Update_TIM3_CH1 (last_bright);
    		}

    		//prendo relay
//    		if (last_bright > 20)
//    		{
//    			if (!RELAY)
//    				RELAY_ON;
//    		}
//    		else if (last_bright < 10)
//    		{
//    			if (RELAY)
//    				RELAY_OFF;
//    		}
    	}
    }
#endif
	//---------- Prueba Conexiones ESP8266 & HLK_RM04  --------//
#ifdef WIFI_TO_CEL_PHONE_PROGRAM
    main_state = MAIN_INIT;
	RELAY_ON;

    while( 1 )
    {

#ifdef USE_ESP_WIFI
    	switch (main_state)
    	{
			case MAIN_INIT:
				//USARTSend("ESP8266 Test...\r\n");
				Usart2Send("ESP8266 Test...\r\n");
				TCPProcessInit ();
				timer_standby = 6000;
				main_state++;
				WRST_OFF;
				Wait_ms(20);
				WRST_ON;
				break;

			case MAIN_INIT_1:
				if (!timer_standby)
				{
					main_state++;

					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "Send to ESP Conf");
					ESP_SendConfigResetSM ();
				}
				break;

			case MAIN_SENDING_CONF:
				resp = ESP_SendConfigAP ();

    			if ((resp == RESP_TIMEOUT) || (resp == RESP_NOK))
    			{
					LCD_2DO_RENGLON;
					if (resp == RESP_TIMEOUT)
						LCDTransmitStr((const char *) "ESP: Timeout    ");
					else
						LCDTransmitStr((const char *) "ESP: Error      ");
					main_state = MAIN_ERROR;
					timer_standby = 20000;	//20 segundos de error
				}

    			if (resp == RESP_OK)
    			{
					LCD_2DO_RENGLON;
					LCDTransmitStr((const char *) "ESP: Configured ");
					timer_standby = 1000;
					main_state = MAIN_WAIT_CONNECT_0;
				}
				break;

			case MAIN_WAIT_CONNECT_0:
				if (!timer_standby)
				{
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "Enable connect  ");
					LCD_2DO_RENGLON;
					LCDTransmitStr(s_blank_line);

					main_state = MAIN_WAIT_CONNECT_1;
				}
				break;

			case MAIN_WAIT_CONNECT_1:
					main_state = MAIN_WAIT_CONNECT_2;
    			break;

			case MAIN_WAIT_CONNECT_2:
//				if (esp_unsolicited_pckt == RESP_READY)
//				{
//					esp_unsolicited_pckt = RESP_CONTINUE;
//					//TODO: quitar lenght desde TCPPreProcess y pasarlo a CheckTCPMessages para quedarme con lo ultimo
//					if (TCPPreProcess((unsigned char *) bufftcp, bufftcp_transp, &bytes_remain) < 5)
//					{
//						if (bytes_remain > 0)
//							main_state = MAIN_READING_TCP;
//					}
//				}
				if (ReadSocket (messages, sizeof (messages)))
				{
					//tengo datos, me fijo que son
					main_state = MAIN_READING_TCP;
				}

				//IF CHECK S2
				if (CheckS2())
				{
					LCD_1ER_RENGLON;
					LCDTransmitStr((char *) (const char *) "Going to SW Mode");
					main_state = MAIN_SWITCH_ONLY;
					FuncStandAloneCertReset();
					timer_standby = 3000;
				}

    			break;

			case MAIN_SWITCH_ONLY:
				resp = FuncStandAloneCert();

				if ((CheckS2()) && !timer_standby)
				{
					LCD_1ER_RENGLON;
					LCDTransmitStr((char *) (const char *) "Go to WIFI Mode ");
					new_room = 0;
					main_state = MAIN_WAIT_CONNECT_0;
					timer_standby = 3000;
				}
				break;

			case MAIN_READING_TCP:
				//estoy como en modo transparente y tengo el buffer guardado
				bytes_read = 0;
//				tcp_msg = CheckTCPMessage(bufftcp_transp, &new_room, &new_lamp, &bytes_read);
				tcp_msg = CheckTCPMessage((char *)messages, &new_room, &new_lamp, &bytes_read);

				if (tcp_msg != NONE_MSG)	//es un mensaje valido
					tcp_kalive_timer = TT_KALIVE;
				else
					bytes_remain = 0;

				if (tcp_msg == KEEP_ALIVE)
				{
					resp = TCPSendData(0, (char *) (const char *)"kAL_ACK\r\n");
					if (resp == RESP_NOK)
					{
						LCD_2DO_RENGLON;
						LCDTransmitStr((char *) (const char *) "No free buffer  ");
					}
				}

				if (tcp_msg == GET_A)	//tira error en apk de android
				{
//						USARTSend((char *) (const char *) "t,50,50,50,50;\r\n");
				}

				if ((tcp_msg == LAMP_BRIGHT) || (tcp_msg == LIGHTS_OFF) || (tcp_msg == ROOM_BRIGHT))
				{
					need_ack = 1;
				}

				if (bytes_read < bytes_remain)
					bytes_remain -= bytes_read;
				else
				{
					bytes_remain = 0;
					main_state = MAIN_WAIT_CONNECT_2;

					//mando ACK de luces solo al final del ultimo mensaje de paquete
					if (need_ack)
					{
						need_ack = 0;
						TCPSendData(0, "ACK\r\n");
					}
				}
    			break;

			case MAIN_ERROR:
				if (!timer_standby)
					main_state = MAIN_INIT;
				break;

			default:
				main_state = MAIN_INIT;
				break;
    	}
    	//Procesos continuos
    	ESP_ATProcess ();
    	TCPProcess();
		UpdateSwitches();
		UpdateACSwitch();

//    	///PRUEBA RAPIDA 28-09
//		resp = FuncStandAloneCert();
////		UpdateSwitches();
//		UpdateACSwitch();
//		///

		if (CheckS1())
		{
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "S1 -> Break     ");
			ii = 1;
		}

    	if (!timer_wifi_bright)
    	{
    		timer_wifi_bright = 5;	//muevo un punto cada 5ms
    		if (new_room > last_bright)		//TODO: en vez de new_room deberia utilizar un filtro de los ultimos valores recibidos
    		{
    			last_bright++;
    			Update_TIM3_CH1 (last_bright);
    		}
    		else if (new_room < last_bright)
    		{
    			last_bright--;
    			Update_TIM3_CH1 (last_bright);
    		}

    		//prendo relay
//    		if (last_bright > 20)
//    		{
//    			if (!RELAY)
//    				RELAY_ON;
//    		}
//    		else if (last_bright < 10)
//    		{
//    			if (RELAY)
//    				RELAY_OFF;
//    		}
    	}
#endif

#ifdef USE_HLK_WIFI
    	switch (main_state)
    	{
			case MAIN_INIT:
				USARTSend("HLK_RM04 Test...\r\n");
				timer_standby = 100;
				main_state++;
				break;

			case MAIN_INIT_1:
				if (!timer_standby)
				{
					main_state++;

					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "Send to HLK Conf");
					resp = HLK_SendConfig (CMD_RESET);
				}
				break;

			case MAIN_SENDING_CONF:
				resp = HLK_SendConfig (CMD_PROC);

    			if ((resp == RESP_TIMEOUT) || (resp == RESP_NOK))
    			{
					LCD_2DO_RENGLON;
					if (resp == RESP_TIMEOUT)
						LCDTransmitStr((const char *) "HLK: Timeout    ");
					else
						LCDTransmitStr((const char *) "HLK: Error      ");
					main_state = MAIN_ERROR;
					timer_standby = 20000;	//20 segundos de error
				}

    			if (resp == RESP_OK)
    			{
					LCD_2DO_RENGLON;
					LCDTransmitStr((const char *) "HLK: Configured ");
					timer_standby = 20000;								//TODO: ojo 20000 va bien
																		//con 10000 tiene que pegar una vuelta por los comandos AT
					main_state = MAIN_WAIT_CONNECT_0;
				}
				break;

			case MAIN_WAIT_CONNECT_0:
				if (!timer_standby)
				{
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "Enable connect  ");
					LCD_2DO_RENGLON;
					LCDTransmitStr(s_blank_line);
					resp = HLK_EnableNewConn (CMD_RESET);
					main_state = MAIN_WAIT_CONNECT_1;
				}
				break;

			case MAIN_WAIT_CONNECT_1:
				resp = HLK_EnableNewConn (CMD_PROC);

				if (resp == RESP_OK)
				{
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "Waiting new conn");
					LCD_2DO_RENGLON;
					LCDTransmitStr(s_blank_line);
					main_state = MAIN_WAIT_CONNECT_2;
				}

				if ((resp == RESP_TIMEOUT) || (resp == RESP_NOK))
				{
					LCD_2DO_RENGLON;
					if (resp == RESP_TIMEOUT)
					{
						LCDTransmitStr((const char *) "HLK: Timeout    ");
						main_state = MAIN_WAIT_CONNECT_0;
					}
					else
					{
						LCDTransmitStr((const char *) "HLK: Error go AT");
						main_state = MAIN_WAIT_CONNECT_3;
						resp = HLKToATMode (CMD_RESET);
					}
					timer_standby = 10000;
				}
    			break;

			case MAIN_WAIT_CONNECT_2:
				//seguro vengo desde AT entonces cambio rapido a transparente
				resp = HLK_GoTransparent (CMD_ONLY_CHECK);
				main_state = MAIN_TRANSPARENT;

    			break;

			case MAIN_WAIT_CONNECT_3:
				resp = HLKToATMode (CMD_PROC);

				if (resp == RESP_OK)
				{
					LCD_2DO_RENGLON;
					LCDTransmitStr((const char *) "HLK: in AT Mode ");
					main_state = MAIN_WAIT_CONNECT_0;
				}
    			break;

			case MAIN_TRANSPARENT:
				if (hlk_transparent_finish)
				{
					//tengo un mensage reviso cual es
					tcp_msg = CheckTCPMessage(data, &new_room, &new_lamp);
					hlk_transparent_finish = 0;

					if (tcp_msg != NONE_MSG)	//es un mensaje valido
						tcp_kalive_timer = TT_KALIVE;

					if (tcp_msg == KEEP_ALIVE)
					{
						USARTSend((char *) (const char *) "kAL_ACK\r\n");
					}

					if (tcp_msg == GET_A)	//tira error en apk de android
					{
//						USARTSend((char *) (const char *) "t,50,50,50,50;\r\n");
					}

					if (tcp_msg == ROOM_BRIGHT)
					{
						USARTSend((char *) (const char *) "ACK\r\n");
					}

					if ((tcp_msg == LAMP_BRIGHT) || (tcp_msg == LIGHTS_OFF))
					{
						USARTSend((char *) (const char *) "ACK\r\n");
					}
				}

				if (!tcp_kalive_timer)
				{
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "Connection drop ");
				}
    			break;

			case MAIN_ERROR:
				if (!timer_standby)
					main_state = MAIN_INIT_1;
				break;

			default:
				main_state = MAIN_INIT;
				break;

    	}

    	//Procesos continuos
    	HLK_ATProcess ();

    	if (!timer_wifi_bright)
    	{
    		timer_wifi_bright = 5;	//muevo un punto cada 5ms
    		if (new_room > last_bright)		//TODO: en vez de new_room deberia utilizar un filtro de los ultimos valores recibidos
    		{
    			last_bright++;
    			Update_TIM3_CH1 (last_bright);
    		}
    		else if (new_room < last_bright)
    		{
    			last_bright--;
    			Update_TIM3_CH1 (last_bright);
    		}
    	}
#endif

    }
#endif
    //---------- Fin Prueba AT ESP8266 & HLK_RM04 --------//

	//---------- Prueba temp --------//
	/*
	while (1)
	{
		local_meas = GetTemp();
		if (local_meas != local_meas_last)
		{
			LED_ON;
			local_meas_last = local_meas;
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Brd Temp:       ");
			local_meas = ConvertTemp(local_meas);
			sprintf(s_lcd, "%d", local_meas);
			Lcd_SetDDRAM(0x40 + 10);
			LCDTransmitStr(s_lcd);
			LED_OFF;
		}

		UpdateTemp();
	}
	*/
	//---------- Fin prueba temp --------//

	//---------- Prueba 1 to 10V --------//
	/*
	local_meas = 0;
	while (1)
	{
		LCD_2DO_RENGLON;
		LCDTransmitStr((const char *) "1 to 10V:       ");
		fcalc = local_meas;
		fcalc = fcalc * K_1TO10;
		one_int = (short) fcalc;
		fcalc = fcalc - one_int;
		fcalc = fcalc * 10;
		one_dec = (short) fcalc;

		sprintf(s_lcd, "%02d.%01d V", one_int, one_dec);
		Lcd_SetDDRAM(0x40 + 10);
		LCDTransmitStr(s_lcd);

		Wait_ms (1000);
		if (local_meas <= 255)
			local_meas = 0;
		else
			local_meas++;
	}
	*/
	//---------- Fin prueba 1 to 10V --------//

    //---------- Programa de Certificacion S.E. --------//
#ifdef USE_CE_PROGRAM
	while (1)
	{
		resp = FuncStandAloneCert();


		UpdateSwitches();
		UpdateACSwitch();
		UpdatePackets();
		UpdateTemp();
		UpdateIGrid();
		UpdateVGrid();

	}	//termina while(1)
#endif
	//---------- Fin Programa de Certificacion S.E. --------//

    //---------- Programa de Produccion solo LDR --------//
#ifdef USE_PROD_PROGRAM_ONLY_LDR
	while (1)
	{
		resp = FuncStandAloneOnlyLDR();


		UpdateSwitches();
		UpdateLDR();

		//UpdateACSwitch();
		//UpdatePackets();
		//UpdateTemp();
		//UpdateIGrid();
		//UpdateVGrid();

	}	//termina while(1)
#endif
	//---------- Fin Programa de Produccion solo LDR --------//

    //---------- Programa de Produccion --------//
#ifdef USE_PROD_PROGRAM
	//--- PRUEBA FUNCION MAIN_MENU
	//leo la memoria, si tengo configuracion de modo
	//entro directo, sino a Main Menu
	if (saved_mode == 0xFF)	//memoria borrada
		main_state = MAIN_INIT;
	else
		jump_the_menu = RESP_YES;

#ifdef VER_1_2
	Update_TIM3_CH2 (255);
#endif
	//Wait_ms(2000);
	while (1)
	{
		switch (main_state)
		{
			case MAIN_INIT:
				resp = FuncMainMenu();

				if (resp == MAINMENU_SHOW_STANDALONE_SELECTED)	//TODO deberia forzar init
					main_state = MAIN_STAND_ALONE;

				if (resp == MAINMENU_SHOW_GROUPED_SELECTED)
					main_state = MAIN_GROUPED;

				if (resp == MAINMENU_SHOW_NETWORK_SELECTED)
					main_state = MAIN_NETWORKED;

				jump_the_menu = RESP_NO;
				break;

			case MAIN_STAND_ALONE:
				resp = FuncStandAlone();

				if (resp == RESP_CHANGE_ALL_UP)
				{
					FuncStandAloneReset();
					main_state = MAIN_INIT;
				}

				break;

			case MAIN_GROUPED:
				resp = FuncGrouped();

				if (resp == RESP_CHANGE_ALL_UP)
				{
					FuncGroupedReset();
					main_state = MAIN_INIT;
				}

				break;

			case MAIN_NETWORKED:
				resp = FuncNetworked(jump_the_menu);
				jump_the_menu = RESP_NO_CHANGE;
				main_state++;
				break;

			case MAIN_NETWORKED_1:
				resp = FuncNetworked(jump_the_menu);

				if (resp == RESP_CHANGE_ALL_UP)
					main_state = MAIN_INIT;

				break;

			default:
				main_state = MAIN_INIT;
				break;

		}

		UpdateSwitches();
		UpdateACSwitch();
		UpdatePackets();
	}

	//--- FIN PRUEBA FUNCION MAIN_MENU
#endif
	//---------- Fin Programa de Procduccion --------//

	return 0;
}

//--- End of Main ---//




void prepare_json_pkt (uint8_t * buffer)
{
      int32_t d1 = 1, d2 = 2, d3 = 3, d4 = 4, d5 = 5, d6 = 6;
      char tempbuff[40];
      volatile float HUMIDITY_Value;
      volatile float TEMPERATURE_Value;
      volatile float PRESSURE_Value;



      strcpy((char *)buffer,"{\"d\":{\"myName\":\"Nucleo\"");
//      BSP_HUM_TEMP_GetTemperature((float *)&TEMPERATURE_Value);
//      floatToInt(TEMPERATURE_Value, &d1, &d2, 2);
      sprintf(tempbuff, ",\"A_Temperature\":%lu.%lu",d1, d2);
      strcat((char *)buffer,tempbuff);

//      BSP_HUM_TEMP_GetHumidity((float *)&HUMIDITY_Value);
//      floatToInt(HUMIDITY_Value, &d3, &d4, 2);
      sprintf(tempbuff, ",\"A_Humidity\":%lu.%lu",d3,d4 );
      strcat(  (char *)buffer,tempbuff);

//      BSP_PRESSURE_GetPressure((float *)&PRESSURE_Value);
//      floatToInt(PRESSURE_Value, &d5, &d6, 2);
      sprintf(tempbuff, ",\"A_Pressure\":%lu.%lu",d5,d6 );
      strcat((char *)buffer,tempbuff);


      strcat((char *)buffer,"}}");

      return;
}

#ifdef USE_DMX
void UpdatePackets (void)
{
	if (Packet_Detected_Flag)
	{
		if (data[0] == 0x00)
			DMX_packet_flag = 1;

		if (data[0] == 0xCC)
			RDM_packet_flag = 1;

		Packet_Detected_Flag = 0;
	}
}

void EXTI4_15_IRQHandler(void)
{
	unsigned short aux;

//--- SOLO PRUEBA DE INTERRUPCIONES ---//
//	if (DMX_INPUT)
//		LED_ON;
//	else
//		LED_OFF;
//
//	EXTI->PR |= 0x0100;

	if(EXTI->PR & 0x0100)	//Line8
	{

		//si no esta con el USART detecta el flanco	PONER TIMEOUT ACA?????
		if ((dmx_receive_flag == 0) || (dmx_timeout_timer == 0))
		//if (dmx_receive_flag == 0)
		{
			switch (signal_state)
			{
				case IDLE:
					if (!(DMX_INPUT))
					{
						//Activo timer en Falling.
						TIM14->CNT = 0;
						TIM14->CR1 |= 0x0001;
						signal_state++;
					}
					break;

				case LOOK_FOR_BREAK:
					if (DMX_INPUT)
					{
						//Desactivo timer en Rising.
						aux = TIM14->CNT;

						//reviso BREAK
						//if (((tim_counter_65ms) || (aux > 88)) && (tim_counter_65ms <= 20))
						if ((aux > 87) && (aux < 210))	//Consola STARLET 6
						//if ((aux > 87) && (aux < 2000))		//Consola marca CODE tiene break 1.88ms
						{
							LED_ON;
							//Activo timer para ver MARK.
							//TIM2->CNT = 0;
							//TIM2->CR1 |= 0x0001;

							signal_state++;
							//tengo el break, activo el puerto serie
							DMX_channel_received = 0;
							//dmx_receive_flag = 1;

							dmx_timeout_timer = DMX_TIMEOUT;		//activo el timer cuando prendo el puerto serie
							//USARTx_RX_ENA;
						}
						else	//falso disparo
							signal_state = IDLE;
					}
					else	//falso disparo
						signal_state = IDLE;

					TIM14->CR1 &= 0xFFFE;
					break;

				case LOOK_FOR_MARK:
					if ((!(DMX_INPUT)) && (dmx_timeout_timer))	//termino Mark after break
					{
						//ya tenia el serie habilitado
						//if ((aux > 7) && (aux < 12))
						dmx_receive_flag = 1;
					}
					else	//falso disparo
					{
						//termine por timeout
						dmx_receive_flag = 0;
						//USARTx_RX_DISA;
					}
					signal_state = IDLE;
					LED_OFF;
					break;

				default:
					signal_state = IDLE;
					break;
			}
		}

		EXTI->PR |= 0x0100;
	}
}
#endif

void TimingDelay_Decrement(void)
{
	if (wait_ms_var)
		wait_ms_var--;

//	if (display_timer)
//		display_timer--;

	if (timer_standby)
		timer_standby--;

	if (switches_timer)
		switches_timer--;

	if (acswitch_timer)
		acswitch_timer--;

#ifdef USE_DMX
	if (dmx_timeout_timer)
		dmx_timeout_timer--;
#endif

//	if (prog_timer)
//		prog_timer--;

	if (take_temp_sample)
		take_temp_sample--;

	if (take_ldr_sample)
		take_ldr_sample--;

	if (filter_timer)
		filter_timer--;

	if (grouped_master_timeout_timer)
		grouped_master_timeout_timer--;

	if (show_select_timer)
		show_select_timer--;

	if (scroll1_timer)
		scroll1_timer--;

	if (scroll2_timer)
		scroll2_timer--;

	if (standalone_timer)
		standalone_timer--;

//	if (standalone_menu_timer)
//		standalone_menu_timer--;

	if (standalone_enable_menu_timer)
		standalone_enable_menu_timer--;

#ifdef USE_HLK_WIFI
	if (hlk_timeout)
		hlk_timeout--;

	if (hlk_mini_timeout)
		hlk_mini_timeout--;

	if (tcp_kalive_timer)
		tcp_kalive_timer--;

	if (timer_wifi_bright)
		timer_wifi_bright--;
#endif

#ifdef USE_ESP_WIFI
	if (esp_timeout)
		esp_timeout--;

	if (esp_mini_timeout)
		esp_mini_timeout--;

	if (tcp_kalive_timer)
		tcp_kalive_timer--;

	if (timer_wifi_bright)
		timer_wifi_bright--;

	if (tcp_send_timeout)
		tcp_send_timeout--;
#endif

	if (wifi_func_timer)
		wifi_func_timer--;

	if (mqtt_func_timer)
		mqtt_func_timer--;

	//cuenta de a 1 minuto
	if (secs > 59999)	//pasaron 1 min
	{
		minutes++;
		secs = 0;
	}
	else
		secs++;

}

//------ EOF -------//
