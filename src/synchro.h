/*
 * synchro.h
 *
 *  Created on: 02/12/2015
 *      Author: Mariano
 */

#ifndef SYNCHRO_H_
#define SYNCHRO_H_

//----------- FRECUENCIAS DE LA RED -----------//
#define GRID_FREQ_MIN_REALLY_GOOD		947	//49.50Hz tiempo que corresponde a la freq minima 21.33us por tick
#define GRID_FREQ_MAX_REALLY_GOOD		927	//50.57Hz
#define GRID_FREQ_MIN		967				//48.50Hz
#define GRID_FREQ_MAX		910				//51.50Hz

//----------- TENSIONES DE LA RED -----------//
#define VGRID_MAX			3578	//equivale a 230Vrms
//#define VGRID_MAX			3000	//equivale a 200Vrms
//#define VGRID_MAX			2600
//#define VGRID_MIN			1100	//equivale a 210Vpp
//#define VGRID_MIN			550	//equivale a 440mVpp	34,2Vrms
//#define VGRID_MIN			2700	//equivale a 2160mVpp	180Vrms
#define VGRID_MIN			3046	//equivale a 200Vrms
#define VGRID_SAMPLES_RESET		60

// ------- para determinar IGrid -------
#define IGRID_SAMPLES_RESET		60		//tengo muestras cada 400us 50 son 1 ciclo completo


//----------- TENSIONES DE LA ENTRADA CC -----------//
//en frio 320Vcc 1020mV
//en frio 368Vcc 1230mV
#define VIN_MAX		1840	//400Vcc estimado
#define VIN_MIN		1266	//320Vcc medido

//RESPUESTAS DE NOISE
//#define BAD_NOISE				0
//#define REGULAR_NOISE			1
//#define GOOD_NOISE				2
//#define REALLY_GOOD_NOISE		3

//RESPUESTAS DE FGRID
//#define BAD_FREQ				0
//#define GOOD_FREQ				1
//#define REALLY_GOOD_FREQ		2

//--- RESPUESTAS DE LOS FILTROS Y CHEQUEOS (NOISE / FGRID / VGRID / VIN) ---//
enum Answers {

	BAD = 0,
	REGULAR,		//1
	GOOD,			//2
	REALLY_GOOD	//3
};



unsigned char CheckVGrid (void);
unsigned char CheckFGrid (void);
unsigned char CheckFGridFast (unsigned short);
unsigned char CheckGridNoise (void);
unsigned char CheckVin (void);
unsigned short GetIGrid (void);
unsigned short GetVGrid (void);

void UpdateVGrid (void);
void UpdateFGrid (void);
void UpdateSyncNoise (void);
void UpdateVin (unsigned short);
void UpdateIGrid (void);


#endif /* SYNCHRO_H_ */
