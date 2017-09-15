/*
 * EEL4511_LCD.h
 *
 *  Created on: Oct 17, 2016
 *      Author: Adrian
 */

#ifndef EEL4511_LCD_H_
#define EEL4511_LCD_H_

void Init_LCD(void);
void LCD_command(char val);
void LCD_char(char val);
void LCD_clear(void);
void LCD_string(char* string);
void LCD_float( float value);
void LCD_home(void);

#endif /* EEL4511_LCD_H_ */
