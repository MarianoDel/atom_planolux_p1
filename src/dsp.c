/*
 * dsp.c
 *
 *  Created on: 02/12/2015
 *      Author: Mariano
 */

#include "dsp.h"


#include <stdlib.h>
#include <math.h>


/* Externals variables ---------------------------------------------------------*/



/* Module functions ---------------------------------------------------------*/

unsigned short RandomGen (unsigned int seed)
{
	unsigned int random;

	//Random Generator
	srand (seed);
	random = rand();

	return (unsigned short) random;

}
unsigned short MAFilterFast (unsigned short new_sample, unsigned short * vsample)
{
	unsigned int total_ma;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7

	total_ma = new_sample + *(vsample) + *(vsample + 1) + *(vsample + 2);
	*(vsample + 2) = *(vsample + 1);
	*(vsample + 1) = *(vsample);
	*(vsample) = new_sample;

	total_ma >>= 2;

	return (unsigned short) total_ma;
}

unsigned short MAFilter8 (unsigned short new_sample, unsigned short * vsample)
{
	unsigned int total_ma;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7

	total_ma = new_sample + *(vsample) + *(vsample + 1) + *(vsample + 2) + *(vsample + 3) + *(vsample + 4) + *(vsample + 5) + *(vsample + 6);
	//*(vsample + 7) = *(vsample + 6);
	*(vsample + 6) = *(vsample + 5);
	*(vsample + 5) = *(vsample + 4);
	*(vsample + 4) = *(vsample + 3);
	*(vsample + 3) = *(vsample + 2);
	*(vsample + 2) = *(vsample + 1);
	*(vsample + 1) = *(vsample);
	*(vsample) = new_sample;

	total_ma >>= 3;

	return (unsigned short) total_ma;
}

unsigned short MAFilter32 (unsigned short new_sample, unsigned short * vsample)
{
	unsigned int total_ma;

	total_ma = new_sample + *(vsample) + *(vsample + 1) + *(vsample + 2) + *(vsample + 3) + *(vsample + 4) + *(vsample + 5) + *(vsample + 6);
	total_ma += *(vsample + 7) + *(vsample + 8) + *(vsample + 9) + *(vsample + 10) + *(vsample + 11) + *(vsample + 12) + *(vsample + 13) + *(vsample + 14);
	total_ma += *(vsample + 15) + *(vsample + 16) + *(vsample + 17) + *(vsample + 18) + *(vsample + 19) + *(vsample + 20) + *(vsample + 21) + *(vsample + 22);
	total_ma += *(vsample + 23) + *(vsample + 24) + *(vsample + 25) + *(vsample + 26) + *(vsample + 27) + *(vsample + 28) + *(vsample + 29) + *(vsample + 30);

	*(vsample + 30) = *(vsample + 29);
	*(vsample + 29) = *(vsample + 28);
	*(vsample + 28) = *(vsample + 27);
	*(vsample + 27) = *(vsample + 26);
	*(vsample + 26) = *(vsample + 25);
	*(vsample + 25) = *(vsample + 24);
	*(vsample + 24) = *(vsample + 23);

	*(vsample + 23) = *(vsample + 22);
	*(vsample + 22) = *(vsample + 21);
	*(vsample + 21) = *(vsample + 20);
	*(vsample + 20) = *(vsample + 19);
	*(vsample + 19) = *(vsample + 18);
	*(vsample + 18) = *(vsample + 17);
	*(vsample + 17) = *(vsample + 16);
	*(vsample + 16) = *(vsample + 15);

	*(vsample + 15) = *(vsample + 14);
	*(vsample + 14) = *(vsample + 13);
	*(vsample + 13) = *(vsample + 12);
	*(vsample + 12) = *(vsample + 11);
	*(vsample + 11) = *(vsample + 10);
	*(vsample + 10) = *(vsample + 9);
	*(vsample + 9) = *(vsample + 8);
	*(vsample + 8) = *(vsample + 7);

	*(vsample + 7) = *(vsample + 6);
	*(vsample + 6) = *(vsample + 5);
	*(vsample + 5) = *(vsample + 4);
	*(vsample + 4) = *(vsample + 3);
	*(vsample + 3) = *(vsample + 2);
	*(vsample + 2) = *(vsample + 1);
	*(vsample + 1) = *(vsample);
	*(vsample) = new_sample;

	total_ma >>= 5;

	return (unsigned short) total_ma;
}

//recibe vector de 32 muestras y calcula Moving Average
unsigned short MA32 (unsigned short * vsample)
{
	unsigned int total_ma;

	total_ma = *(vsample) + *(vsample + 1) + *(vsample + 2) + *(vsample + 3) + *(vsample + 4) + *(vsample + 5) + *(vsample + 6) + *(vsample + 7);
	total_ma += *(vsample + 8) + *(vsample + 9) + *(vsample + 10) + *(vsample + 11) + *(vsample + 12) + *(vsample + 13) + *(vsample + 14) + *(vsample + 15);
	total_ma += *(vsample + 16) + *(vsample + 17) + *(vsample + 18) + *(vsample + 19) + *(vsample + 20) + *(vsample + 21) + *(vsample + 22) + *(vsample + 23);
	total_ma += *(vsample + 24) + *(vsample + 25) + *(vsample + 26) + *(vsample + 27) + *(vsample + 28) + *(vsample + 29) + *(vsample + 30) + *(vsample + 31);

	total_ma >>= 5;

	return (unsigned short) total_ma;
}

unsigned short SeekMax32 (unsigned short * vsample)
{
	unsigned char i;
	unsigned short max = 0;

	for (i = 0; i < 31; i++)
	{
		if (*(vsample + i) > max)
			max = *(vsample + i);
	}

	return max;
}

unsigned short SeekMin32 (unsigned short * vsample)
{
	unsigned char i;
	unsigned short min = 4095;

	for (i = 0; i < 31; i++)
	{
		if (*(vsample + i) < min)
			min = *(vsample + i);
	}

	return min;
}

//Filtro circular, recibe
//new_sample, p_vec_samples: vector donde se guardan todas las muestras
//p_vector: puntero que recorre el vector de muestras, p_sum: puntero al valor de la sumatoria de muestras
//devuelve resultado del filtro
unsigned short MAFilter32Circular (unsigned short new_sample, unsigned short * p_vec_samples, unsigned char * index, unsigned int * p_sum)
{
	unsigned int total_ma;
	unsigned short * p_vector;

	p_vector = (p_vec_samples + *index);

	//agrego la nueva muestra al total guardado
	total_ma = *p_sum + new_sample;

	//guardo el nuevo sample y actualizo el puntero
	*p_vector = new_sample;
	if (p_vector < (p_vec_samples + 31))
	{
		p_vector++;
		*index += 1;
	}
	else
	{
		p_vector = p_vec_samples;
		*index = 0;
	}

	//resto la muestra - 32 (es la apuntada por el puntero porque es circular)
	total_ma -= *p_vector;
	*p_sum = (unsigned short) total_ma;

	return total_ma >> 5;
}
