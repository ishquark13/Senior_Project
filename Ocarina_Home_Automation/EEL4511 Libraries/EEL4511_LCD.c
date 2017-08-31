/*
 * EEL4511_LCD.c
 *
 *  Created on: Oct 17, 2016
 *      Author: Adrian
 */

#include<DSP28x_Project.h>
#include "../include/EEL4511_LCD.h"
#include "OneToOneI2CDriver.h"

void LCD_float( float value)
{
	char val1, val2, val3;// to get digit values
	val1 = (int) value; //cast voltage as int to get first digit
	LCD_char(val1 + '0');	//display first digit
	float value2 = 10*(value - val1); //move next digit to 1s place
	val2 = (int) value2;	//get second digit via type cast
	LCD_char(val2 + '0');	//display 2nd digit
	float value3 = 10*(value2 - val2); //get and display 3rd digit
	val3 = (int) value3;
	LCD_char(val3+ '0');

	return;
}

void Init_LCD(void)
{
	EALLOW;
	I2C_O2O_Master_Init(0x3F, 150, 10); //slave addr , sys clock (MHz), i2c clock (kHz)
	EALLOW;

	LCD_command(0x33);
	DELAY_US(5000);
	LCD_command(0x32);
	DELAY_US(5000);
	LCD_command(0x28);
	DELAY_US(5000);
	LCD_command(0x0F);
	DELAY_US(5000);
	LCD_command(0x01);
	EDIS;

	return;
}

void LCD_char(char val)
{
	char valH;
	char valL;
	valH = ( val & 0xF0 );
	valL = ( (val & 0x0F) << 4 )  ;
	I2C_O2O_SendBytes( valH | 0x0D);
	I2C_O2O_SendBytes( valH | 0x09);
	I2C_O2O_SendBytes( valL | 0x0D);
	I2C_O2O_SendBytes( valL | 0x09);

	return;
}

void LCD_command(char val)
{
	char valH;
	char valL;
	valH = ( val & 0xF0 );
	valL = ( (val & 0x0F) << 4 )  ;
	I2C_O2O_SendBytes( valH | 0x0C);
	I2C_O2O_SendBytes( valH | 0x08);
	I2C_O2O_SendBytes( valL | 0x0C);
	I2C_O2O_SendBytes( valL | 0x08);

	return;
}

void LCD_string(char string[10])
{
	for(int i = 0; string[i] != 0; i++)
	{
		LCD_char(string[i]);
	}

	return;
}

void LCD_clear(void)
{
	LCD_command(0x01);
	return;
}

void LCD_home(void)
{
	LCD_command(0x02);
	return;
}
