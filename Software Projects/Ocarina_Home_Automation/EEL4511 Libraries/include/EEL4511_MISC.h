/*
 * EEL4511_MISC.h
 *
 *  Created on: Oct 17, 2016
 *      Author: Adrian
 */

#ifndef EEL4511_MISC_H_
#define EEL4511_MISC_H_

Uint16 float2word(float number);

float word2float(Uint16 value);

void keypad_init(void);

char getKey(void);

typedef volatile struct {
	volatile unsigned int point;
	volatile float coeff[];
}Filter;

#endif /* EEL4511_MISC_H_ */
