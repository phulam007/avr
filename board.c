/*
 * board.c
 *
 *  Created on: May 14, 2016
 *      Author: Administrator
 */


#include "board.h"
#include "lcd20x4.h"

void gpioInit()
{
	// khai bao output
	sbi(DDRD,TRIGGER);		// output TRIGGER
	cbi(pTRIGGER, TRIGGER);
	sbi(DDRD,TRIGGER1);		// output TRIGGER 1
	cbi(pTRIGGER1, TRIGGER1);
	sbi(DDRB,RIGHT);		// output RIGHT
	cbi(pRIGHT, RIGHT);
	sbi(DDRB,LEFT);		// output LEFT
	cbi(pLEFT, LEFT);
	sbi(DDRE,BUZZER);		// output LEFT
	cbi(PORTE, BUZZER);
	sbi(DDRE,COIL);		// output LEFT
	cbi(PORTE, COIL);
	// khai bao input
	cbi(DDRD,ECHO);	// input ECHO
	cbi(pECHO, ECHO);
	cbi(DDRD,ECHO1);	// input ECHO1
	cbi(pECHO1, ECHO1);
	cbi(DDRB,SWITCH1);	// input sw1
	sbi(PORTB, SWITCH1);
	cbi(DDRB,SWITCH2);	// input sw2
	sbi(PORTB, SWITCH2);
	// KEYPAD
	cbi(DDRC,(1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)); // KEYPAD
	sbi(PORTC, (1<<PC0)|(1<PC1)|(1<<PC2)|(1<<PC3)); // PULL UP

}

u08 getDistance(uint16_t timeout)
{
	u08 distance;
	sbi(pTRIGGER, TRIGGER);
	_delay_ms(10);
	cbi(pTRIGGER, TRIGGER);
	TCNT1 = 0;
	while(((inb(PIND)&(1<<ECHO))==0)&&(TCNT1<timeout));
	if(TCNT1>=timeout) return 0;
	TCNT1 = 0;
	while((inb(PIND)&(1<<ECHO))&&(TCNT1<timeout));
	if(TCNT1>=timeout) return 0;
	distance = TCNT1*23/58;
	TCNT1 = 0;
	return distance;
}

u08 getDistance1(uint16_t timeout)
{
	u08 distance;
	sbi(pTRIGGER1, TRIGGER1);
	_delay_ms(10);
	cbi(pTRIGGER1, TRIGGER1);
	TCNT1 = 0;
	while(((inb(PIND)&(1<<ECHO1))==0)&&(TCNT1<timeout));
	if(TCNT1>=timeout) return 0;
	TCNT1 = 0;
	while((inb(PIND)&(1<<ECHO1))&&(TCNT1<timeout));
	if(TCNT1>=timeout) return 0;
	distance = TCNT1*23/58;
	TCNT1 = 0;
	return distance;
}

void openDoor()
{
	cbi(pLEFT, LEFT);
	sbi(pRIGHT, RIGHT);

}

void closeDoor()
{
	cbi(pRIGHT, RIGHT);
	sbi(pLEFT, LEFT);
}

void stopDoor()
{
	cbi(pRIGHT, RIGHT);
	cbi(pLEFT, LEFT);
}

u08 keyScan(u16 time)
{
	u08 st, cur_st;
	for(st=0; st <= 4; st++)
	{
		switch(st)
		{
			case 0 : 	DDRC = 0x10;			// row 1
						pKEYPAD = ~0x10;
						break;
			case 1 : 	DDRC = 0x20;			// row 2
						pKEYPAD = ~0x20;
						break;
			case 2 : 	DDRC = 0x40;			// row 3
						pKEYPAD = ~0x40;
						break;
			case 3 : 	DDRC = 0x80;			// row 4
						pKEYPAD = ~0x80;
						break;
			case 4 :    if((TCNT3 > 0)&&(TCNT3 < time)) return keyCode(0x0f);
					 	 TCNT3 = 0;
					 	 TCCR3B &= 0xf8;
					 	 return keyCode(0x0f);
		}
		cur_st = inkey|(st<<4);
		if(inkey != 0x0f)
			break;
	}
	_delay_ms(10);
	if((TCNT3 > 10)&&(TCNT3 < time))  return keyCode(0x0f);
	TCNT3 = 0;
	TCCR3B |= (1 << CS32);
	return keyCode(cur_st);
}


u08 keyCode(u08 key)
{
	u08 c;
	switch(key)
	{
	case 0x37 : c = ' 1'; break;
	case 0x27 : c =  '2'; break;
	case 0x17 : c =   '3'; break;
	case 0x07 : c =  11; break;   // f1
	case 0x3b : c = '4'; break;
	case 0x2b : c = '5'; break;
	case 0x1b : c = '6'; break;
	case 0x0b : c = 12; break;    // f2
	case 0x3d : c = '7'; break;
	case 0x2d : c = '8'; break;
	case 0x1d : c = '9'; break;
	case 0x0d : c = 13; break;    // f3
	case 0x3e : c = 14; break;    // mode
	case 0x2e : c = '0'; break;
	case 0x1e : c = 15; break;    // cancel
	case 0x0e : c = 16; break;    // enter
	default : c = 0; break;
	}
	return c;
}

uint8_t keyScanlcd(uint8_t key, uint8_t x, uint8_t y, unsigned char str[], uint8_t *ct, uint8_t type, uint8_t len)
{
	 if(key != 0)
	 {
		 if((key!=12)&&(key!=13)&&(key!=14)&&(key!=15)&&(key!=16))
		 {
			 if((key==11)&&((*ct)>=0))
			 {
				 str[(*ct)] =0;
				 if((*ct)>0)
					 (*ct)--;
				 lcd_gotoxy((*ct)+x,y);
				 str[(*ct)] =' ' ;
				 lcd_chr(str[(*ct)]);
				 lcd_gotoxy((*ct)+x,y);
			 }
			 else if((*ct) < len)
			 {
			 	 str[(*ct)] = key;
			 	 lcd_gotoxy((*ct)+x,y);
			 	 lcd_chr(str[(*ct)]);
			 	 if (type)
			 	 {
			 	 _delay_ms(300);
			 	 lcd_gotoxy((*ct)+x,y);
			 	 lcd_chr('*');
			 	 }
			 	(*ct)+=1;str[(*ct)] = 0;
			 }
				_delay_ms(10);
		 }
		 else if(key == 16)
		{
			 return 1;
		}
	 }
	 return 0;
}
