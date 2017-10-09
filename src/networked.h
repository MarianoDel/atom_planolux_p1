/*
 * networked.h
 *
 *  Created on: 21/03/2016
 *      Author: Mariano
 */

#ifndef NETWORKED_H_
#define NETWORKED_H_

//estructura del modo Grouped
typedef struct Networked
{
	//parte slave
	unsigned short networked_dmx_channel;
	unsigned char dummy1;
	unsigned char dummy2;

} Networked_Typedef;	//16 bytes alingned

//estados del modo NETWORKED
#define NETWORKED_INIT		0
#define NETWORKED_WORKING		1
#define NETWORKED_SHOW_CONF		2

#define NETWORKED_UPDATE			20

//estados del menu GROUPED
#define NETWORKED_MENU_INIT					0
#define NETWORKED_MENU_INIT_1				1
#define NETWORKED_MENU_SELECT_CHANNEL		2
#define NETWORKED_MENU_SELECT_CHANNEL_1		3
#define NETWORKED_MENU_ENABLE_SELECTED_1	4
#define NETWORKED_MENU_ENABLE_SELECTED_2	5
#define NETWORKED_MENU_ENABLE_SELECTED_3	6

#define NETWORKED_MENU_UPDATE					40
#define NETWORKED_MENU_UPDATE_1				41

//estados de Menu Selections
#define MENU_ON			0
#define MENU_SELECTED	1
#define MENU_OFF		2
#define MENU_WAIT_FREE	3



#define NETWORKED_INITIAL_CHANNEL		1

//#define MENU_NORMAL		0
//#define MENU_UPDATE		1



//-------- Functions -------------
unsigned char FuncNetworked (unsigned char);
unsigned char MenuNetworked (void);


#endif /* NETWORKED_H_ */
