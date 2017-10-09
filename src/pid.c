/*
 * pid.c
 *
 *  Created on: 03/05/2017
 *      Author: Mariano
 */


#include "pid.h"

// ------- Defines para el lazo PID ----------
#define KP	5500		//KPV = 60, KIV = 29
#define KI	160			//100 47uF solo y prot corriente > 1400 va OK
						//15000 mal nuevamente
						//11000 pero solo con 47uF de salida vuelve a prender apagar en 45Hz
#define KD	0

#define SP		170
#define DMAX	400			//maximo D permitido	511 - 25	OK 14-10


// ------- Globales del modulo -------
short error_z1 = 0;
short error_z2 = 0;
short d_z1 = 0;


// ------- Funciones del modulo -------
unsigned short PidFixedSP (unsigned short medida)
{
	int acc = 0;
	short error = 0;
	short d = 0;

	short val_k1 = 0;
	short val_k2 = 0;
	short val_k3 = 0;


	acc = medida;
	error = SP - acc;

	//K1
	acc = K1 * error;		//5500 / 32768 = 0.167 errores de hasta 6 puntos
	val_k1 = acc >> 15;

	//K2
	acc = K2 * error_z1;		//K2 = no llega pruebo con 1
	val_k2 = acc >> 15;			//si es mas grande que K1 + K3 no lo deja arrancar

	//K3
	acc = K3 * error_z2;		//K3 = 0.4
	val_k3 = acc >> 15;

	d = d_z1 + val_k1 - val_k2 + val_k3;

	if (d < 0)
		d = 0;
	else if (d > DMAX)
		d = DMAX;

	//Update variables PID
	error_z2 = error_z1;
	error_z1 = error;
	d_z1 = d;

	return (unsigned short) d;
}







