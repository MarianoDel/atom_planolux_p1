/*
 * dsp.h
 *
 *  Created on: 02/12/2015
 *      Author: Mariano
 */

#ifndef DSP_H_
#define DSP_H_

unsigned short RandomGen (unsigned int);
unsigned char MAFilter (unsigned char, unsigned char *);
unsigned short MAFilterFast (unsigned short ,unsigned short *);
unsigned short MAFilter8 (unsigned short, unsigned short *);
unsigned short MAFilter32 (unsigned short, unsigned short *);

unsigned short MA32 (unsigned short *);
unsigned short SeekMax32 (unsigned short *);
unsigned short SeekMin32 (unsigned short *);

unsigned short MAFilter32Circular (unsigned short, unsigned short *, unsigned char *, unsigned int *);

#define MAFilter32Pote(X)  MAFilter32Circular(X, v_pote_samples, &v_pote_index, &pote_sumation)

#endif /* DSP_H_ */
