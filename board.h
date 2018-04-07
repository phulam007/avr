/*
 * board.h
 *
 *  Created on: May 14, 2016
 *      Author: Administrator
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <avr/io.h>
#include <util/delay.h>
#include "global.h"

#define TRIGGER		PD0
#define ECHO		PD1
#define pECHO		PORTD
#define pTRIGGER	PORTD
#define TRIGGER1	PD2
#define ECHO1		PD3
#define pECHO1		PORTD
#define pTRIGGER1	PORTD
#define RIGHT		PB0
#define LEFT		PB1
#define pRIGHT		PORTB
#define pLEFT		PORTB
#define pKEYPAD		PORTC
#define inkey		(inb(PINC) & (0x0f))
#define SWITCH1		PB2
#define SWITCH2		PB3
#define ISWITCH1	((inb(PINB) & (1<<PB2))!=0)
#define ISWITCH2	((inb(PINB) & (1<<PB3))!=0)
#define SENSOR1		(inb())
#define BUZZER		PE4
#define COIL		PE5
#define	FIRSTINIT	0
#define PIN_EEPROM	1	// ma pin
#define PIN_LEN	10	// chieu dai ma pin
#define MAXDIS1	13	// khoang cach 1
#define	MAXDIS2	14	// khoang cach 2
#define MINDIS1	15  // khoang cach min 1
#define MINDIS2	16 // khoang cach min 2
#define TIMECLOSE	17	// tg dong cua
#define MODE	18		// che do mo cua
#define TAGNUM	19
#define TIMECLOSE1	20
#define TAGINDEX 30
#define TAGID	100

void gpioInit();
u08 getDistance(uint16_t timeout);
u08 getDistance1(uint16_t timeout);
void openDoor();
void closeDoor();
void stopDoor();
u08 keyScan(u16 time);
u08 keyCode(u08 key);
uint8_t keyScanlcd(uint8_t key, uint8_t x, uint8_t y, unsigned char str[], uint8_t *ct, uint8_t type, uint8_t len);
#endif /* BOARD_H_ */
