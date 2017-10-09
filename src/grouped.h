/*
 * grouped.h
 *
 *  Created on: 07/03/2016
 *      Author: Mariano
 */

#ifndef GROUPED_H_
#define GROUPED_H_

//estructura del modo Grouped
typedef struct Grouped
{
	//parte master igual a StandAlone
	unsigned char move_sensor_enable;
	unsigned char ldr_enable;
	unsigned char ldr_value;
	unsigned char max_dimmer_value_percent;
	unsigned char max_dimmer_value_dmx;
	unsigned char min_dimmer_value_percent;
	unsigned char min_dimmer_value_dmx;
	unsigned short power_up_timer_value;	//msecs
	unsigned short dimming_up_timer_value;	//msecs

	//parte slave
	unsigned char grouped_mode;
	unsigned short grouped_dmx_channel;
	unsigned char dummy1;
	unsigned char dummy2;

} Grouped_Typedef;	//16 bytes alingned

//estados del modo GROUPED
#define GROUPED_INIT				0
#define GROUPED_OFF					1
#define GROUPED_OFF_1				2
#define GROUPED_OFF_2				3
#define GROUPED_RISING				4
#define GROUPED_ON					5
#define GROUPED_ON_1				6
#define GROUPED_FALLING				7
#define GROUPED_DIMMING_LAST		8
#define GROUPED_DIMMING_UP			9
#define GROUPED_DIMMING_DOWN		10
#define GROUPED_SLAVE_INIT			11
#define GROUPED_SLAVE_WORKING		12

#define GROUPED_UPDATE				20

//estados del menu GROUPED
#define GROUPED_MENU_INIT					0
#define GROUPED_MENU_MOV_SENS				1
#define GROUPED_MENU_LDR					2
#define GROUPED_MENU_MAX_DIMMING			3
#define GROUPED_MENU_MIN_DIMMING			4
#define GROUPED_MENU_RAMP_ON_START			5
#define GROUPED_MENU_RAMP_ON_DIMMING		6
#define GROUPED_MENU_MASTER_ENABLE			7
#define GROUPED_MENU_SLAVE_ENABLE			8

#define GROUPED_MENU_MOV_SENS_SELECTED			10
#define GROUPED_MENU_MOV_SENS_SELECTED_1		11
#define GROUPED_MENU_MOV_SENS_SELECTED_2		12
#define GROUPED_MENU_LDR_SELECTED				13
#define GROUPED_MENU_LDR_SELECTED_1				14
#define GROUPED_MENU_LDR_SELECTED_2				15
#define GROUPED_MENU_LDR_SELECTED_3				16
#define GROUPED_MENU_LDR_SELECTED_4				17
#define GROUPED_MENU_LDR_SELECTED_5				18
#define GROUPED_MENU_MAX_DIMMING_SELECTED		19
#define GROUPED_MENU_MAX_DIMMING_SELECTED_1		20
#define GROUPED_MENU_MIN_DIMMING_SELECTED		21
#define GROUPED_MENU_MIN_DIMMING_SELECTED_1		22
#define GROUPED_MENU_RAMP_ON_START_SELECTED		23
#define GROUPED_MENU_RAMP_ON_START_SELECTED_1	24
#define GROUPED_MENU_RAMP_ON_DIMMING_SELECTED	25
#define GROUPED_MENU_RAMP_ON_DIMMING_SELECTED_1	26
#define GROUPED_MENU_MASTER_ENABLE_SELECTED		27
#define GROUPED_MENU_MASTER_ENABLE_SELECTED_1	28
#define GROUPED_MENU_MASTER_ENABLE_SELECTED_2	29
#define GROUPED_MENU_SLAVE_ENABLE_SELECTED		30
#define GROUPED_MENU_SLAVE_ENABLE_SELECTED_1	31
#define GROUPED_MENU_SLAVE_ENABLE_SELECTED_2	32
#define GROUPED_MENU_SLAVE_ENABLE_SELECTED_3	33

#define GROUPED_MENU_UPDATE					40
#define GROUPED_MENU_UPDATE_1				41

#define GROUPED_MODE_MASTER	0
#define GROUPED_MODE_SLAVE	1

#define GROUPED_INITIAL_CHANNEL		1

//estados de Menu Selections
#define MENU_ON			0
#define MENU_SELECTED	1
#define MENU_OFF		2
#define MENU_WAIT_FREE	3

#define DIM_UP		0
#define DIM_DOWN	1

//#define TT_RISING_FALLING	5		//update del 1 a 10V (rampa subida y bajada)
//#define TT_RISING_FALLING_FIRST_TIME	20
//#define TT_STARTING			400		//tarda 940 msegs en arrancar la fuente MAS O MENOS OK

#define TT_RISING_FALLING	1		//update del 1 a 10V (rampa subida y bajada)
#define TT_RISING_FALLING_FIRST_TIME	1
#define TT_STARTING			400		//tarda 940 msegs en arrancar la fuente MAS O MENOS OK
#define TT_MASTER_TIMEOUT	100
#define TT_MENU_TIMEOUT		30000

//#define TT_STARTING			100		//tarda 940 msegs en arrancar la fuente

#define ONE_TEN_INITIAL		51
//#define ONE_TEN_INITIAL		10		//valor inicial 1 a 10V para que la fuente arranue en regimen
#define MIN_DIMMING		5

//-------- Functions -------------
unsigned char FuncGrouped (void);
unsigned char MenuGrouped (void);

void FuncGroupedReset (void);
void MenuGroupedReset (void);

#endif /* GROUPED_H_ */
