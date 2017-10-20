/*
 * EEL4511_CircBuff.h
 *
 *  Created on: Oct 17, 2016
 *      Author: Adrian
 */

#ifndef EEL4511_CIRCBUFF_H_
#define EEL4511_CIRCBUFF_H_

//#define BUFFER_SIZE 1048570
//#define BUFFER_SIZE 262144
//#define BUFFER_SIZE 65545
//#define BUFFER_SIZE 2048
#define BUFFER_SIZE 1024
//#define BUFFER_SIZE 512
//#define BUFFER_SIZE 256
//#define BUFFER_SIZE 128
//#define BUFFER_SIZE 64


typedef volatile struct {
	volatile int32 head;
	volatile int32 tail;
	volatile int32 buffer[BUFFER_SIZE];
} circBuff_16;

typedef volatile struct {
	volatile int32 head;
	volatile int32 tail;
	volatile float buffer[BUFFER_SIZE];
} circBuff_f;

Uint16 find_range( Uint16 buff[], Uint32 buffp, Uint32 offset);
Uint32 find_tail(int32 head, int32 offset);

#endif /* EEL4511_CIRCBUFF_H_ */
