/*
 * lcd20x4.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Administrator
 */

#ifndef LCD20X4_H_
#define LCD20X4_H_
#include <avr/io.h>
#include <avr/delay.h>
#include "global.h"
// THƯ VIỆN LCD 20X4
//PIN DATA
#define LCD_DB4   PA0
#define LCD_DB5   PA1
#define LCD_DB6   PA2
#define LCD_DB7   PA3
#define LCD_LED   PA7
// DDR
#define LCD_DDB4	DDRA
#define LCD_DDB5	DDRA
#define LCD_DDB6	DDRA
#define LCD_DDB7	DDRA
#define LCD_DLED	DDRA
// PORT
#define LCD_PDB4	PORTA
#define LCD_PDB5	PORTA
#define LCD_PDB6	PORTA
#define LCD_PDB7	PORTA
#define LCD_PLED	PORTA
//PIN
#define LCD_PINDB4	PINF
#define LCD_PINDB5	PINF
#define LCD_PINDB6	PINF
#define LCD_PINDB7	PINF


#define LCD_RS    PA6
#define LCD_PRS	  PORTA
#define LCD_DRS	  DDRA
#define LCD_RW    PA5
#define LCD_PRW	  PORTA
#define LCD_DRW	  DDRA
#define LCD_E     PA4
#define LCD_PE	  PORTA
#define LCD_DE	  DDRA

#define LCD_START_LINE1 0x00
#define LCD_START_LINE2 0x40
#define LCD_START_LINE3 0x14
#define LCD_START_LINE4 0x54
#define LCD_DDRAM 7
//-------------------------------------

void lcd_init(void);
void lcd_gotoxy(u08 x, u08 y);
void lcd_chr(char c);
void lcd_cmd(u08 cmd);
void lcd_send4bit(u08 data);
void draw_menu(unsigned char str[], uint8_t line);
#endif /* LCD20X4_H_ */
