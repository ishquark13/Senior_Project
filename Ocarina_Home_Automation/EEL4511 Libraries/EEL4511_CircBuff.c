/*
 * EEL4511_CircBuff.c
 *
 *  Created on: Oct 17, 2016
 *      Author: Adrian
 */

#include<DSP28x_Project.h>
#include "../include/EEL4511_CircBuff.h"

Uint16 find_range( Uint16 buff[], Uint32 buffp, Uint32 offset)
{
	Uint32 temp_buffp = buffp ;
	Uint16 max_dat = 0x0000;
	Uint16 min_dat = 0xFFFF;

	//NO LOOP AROUND
	if( temp_buffp >= offset )
	{
		//GET HIGHEST VALUE
		for(temp_buffp = (buffp - offset) ; temp_buffp < buffp ; temp_buffp++)
		{
			if (buff[temp_buffp] > max_dat)
			{
				max_dat = buff[temp_buffp];
			}
		}

		//GET LOWEST VALUE
		for(temp_buffp = (buffp - offset) ; temp_buffp < buffp ; temp_buffp++)
			{
				if (buff[temp_buffp] < min_dat)
				{
					min_dat = buff[temp_buffp];
				}
			}
	}
	else  //LOOP AROUND
	{
		//GET HIGHEST VALUE
		for(temp_buffp = (BUFFER_SIZE - 1) - (offset - buffp); temp_buffp < BUFFER_SIZE ; temp_buffp++)
			{
				if (buff[temp_buffp] > max_dat)
				{
					max_dat = buff[temp_buffp];
				}
			}

		for(temp_buffp = 0 ; temp_buffp < buffp ; temp_buffp++);
			{
				if (buff[temp_buffp] > max_dat)
				{
					max_dat = buff[temp_buffp];
				}
			}

		//GET LOWEST VALUE
		for(temp_buffp = (BUFFER_SIZE - 1) - (offset - buffp) ; temp_buffp < BUFFER_SIZE ; temp_buffp++)
			{
				if (buff[temp_buffp] < min_dat)
				{
					min_dat = buff[temp_buffp];
				}
			}
		for(temp_buffp = 0 ; temp_buffp < buffp ; temp_buffp++)
			{
				if (buff[temp_buffp] < min_dat)
				{
					min_dat = buff[temp_buffp];
				}
			}
	}

	return (max_dat - min_dat);
}

 Uint32 find_tail(int32 head, int32 offset)
 {
//	Uint32 tail;
//
//	if (head > offset)
//		tail = head - offset;
//	else
//		tail = BUFFER_SIZE - (offset - head);


	return ( ( head - offset ) & BUFFER_SIZE );;
 }
