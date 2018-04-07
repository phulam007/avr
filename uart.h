/*
 * uart.h
 *
 *  Created on: May 14, 2016
 *      Author: Administrator
 */

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include "global.h"

#define BAUD 	9600
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)

void uartInit();
void sendByte(unsigned char c);

#endif /* UART_H_ */
