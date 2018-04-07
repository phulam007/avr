/*
 * lcd20x4.c
 *
 *  Created on: Mar 16, 2015
 *      Author: Administrator
 */


#include "lcd20x4.h"

#define RW_HI	sbi(LCD_PRW,LCD_RW)
#define RW_LO	cbi(LCD_PRW,LCD_RW)
#define E_HI	sbi(LCD_PE,LCD_E)
#define	E_LO	cbi(LCD_PE,LCD_E)
#define RS_HI	sbi(LCD_PRS,LCD_RS)
#define RS_LO	cbi(LCD_PRS,LCD_RS)
#define CMD_MODE	RW_LO; RS_LO;
#define DATA_MODE	RW_LO; RS_HI;
#define E_TOGGLE	E_HI; _delay_us(700); E_LO;_delay_us(750);
#define READ_MODE_BF	RW_HI; RS_LO;
#define READ_MODE_DATA	RW_HI; RS_HI;

void lcd_init(void)
{
	// SET PIN CONTROL
	sbi(LCD_DRS,LCD_RS);
	sbi(LCD_DRW,LCD_RW);
	sbi(LCD_DE,LCD_E);
	sbi(LCD_DLED,LCD_LED);	// data 4
	sbi(LCD_DDB4,LCD_DB4);	// data 4
	sbi(LCD_DDB5,LCD_DB5);	// data 4
	sbi(LCD_DDB6,LCD_DB6);	// data 4
	sbi(LCD_DDB7,LCD_DB7);	// data 4
	cbi(LCD_PLED,LCD_LED);
	CMD_MODE;
	lcd_send4bit(0x03);
	E_TOGGLE
	_delay_ms(5);
	lcd_send4bit(0x03);
	E_TOGGLE
	_delay_ms(2);
	lcd_send4bit(0x03);
	E_TOGGLE
	lcd_send4bit(0x02);
	E_TOGGLE
	lcd_cmd(0x28);
	_delay_ms(2);
	lcd_cmd(0x0c);
	_delay_ms(2);
	lcd_cmd(0x14);
	_delay_ms(2);
	lcd_cmd(0x01);
	_delay_ms(2);
	lcd_cmd(0x06);
	_delay_ms(2);
}


void lcd_send4bit(u08 data)
{
	if(data&0x01) sbi(LCD_PDB4,LCD_DB4);
	else	cbi(LCD_PDB4,LCD_DB4);
	if(data&0x02) sbi(LCD_PDB5,LCD_DB5);
	else	cbi(LCD_PDB5,LCD_DB5);
	if(data&0x04) sbi(LCD_PDB6,LCD_DB6);
	else	cbi(LCD_PDB6,LCD_DB6);
	if(data&0x08) sbi(LCD_PDB7,LCD_DB7);
	else	cbi(LCD_PDB7,LCD_DB7);
}
void lcd_cmd(u08 cmd)
{
	CMD_MODE;
	lcd_send4bit(cmd>>4);
	E_TOGGLE;
	lcd_send4bit(cmd);
	E_TOGGLE;
}

void lcd_chr(char c)
{
	DATA_MODE;
	lcd_send4bit(c>>4);
	E_TOGGLE;
	lcd_send4bit(c);
	E_TOGGLE;
}

void lcd_gotoxy(u08 x, u08 y )
{
	    if ( y==0 )
	        lcd_cmd((1<<LCD_DDRAM)+LCD_START_LINE1+x);
	    else if ( y==1)
	    	lcd_cmd((1<<LCD_DDRAM)+LCD_START_LINE2+x);
	    else if ( y==2)
	    	lcd_cmd((1<<LCD_DDRAM)+LCD_START_LINE3+x);
	    else /* y==3 */
	    	lcd_cmd((1<<LCD_DDRAM)+LCD_START_LINE4+x);
}

void draw_menu(unsigned char str[], uint8_t line)
{
	//lcd_cmd(0x01);	// clear screen
	if(line==0)
		lcd_gotoxy(0,0);
	if(line==1)
		lcd_gotoxy(0,1);
	if(line==2)
		lcd_gotoxy(0,2);
	if(line==3)
		lcd_gotoxy(0,3);

		for(uint8_t i = 0; i < 20; i++)
		{
			lcd_chr(str[i]);
		}
}

