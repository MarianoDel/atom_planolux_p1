/*
 * pid.h
 *
 *  Created on: 03/05/2017
 *      Author: Mariano
 */

#ifndef PID_H_
#define PID_H_

//------- de los PID ---------
#define K1 (KP + KI + KD)
#define K2 (KP + KD + KD)
#define K3 (KD)


// ------- Funciones del modulo -------
unsigned short PidFixedSP (unsigned short);


#endif /* PID_H_ */
