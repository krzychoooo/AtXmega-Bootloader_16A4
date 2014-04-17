/*
 * usartd0.h
 *
 *  Created on: 21 gru 2013
 *      Author: krzysztof
 */

#ifndef USARTD0_H_
#define USARTD0_H_

#include <stdio.h>

extern volatile uint8_t* timerd0;
extern volatile unsigned char rx_counter_usartd0;

// USARTD0 initialization
void usartd0_init();

// USARTD0 Receiver buffer
#define RX_BUFFER_SIZE_USARTD0 8

int getchard0();
void registerTimerd0(volatile uint8_t* t);
char getchard0Time(uint8_t time);

// USARTD0 Transmitter buffer
#define TX_BUFFER_SIZE_USARTD0 8

// Write a character to the USARTD0 Transmitter buffer
// USARTD0 is used as the default output device by the 'putchar' function

void putchard0(char c);
int putchard0Stream(char c, FILE *stream);

#endif /* USARTD0_H_ */
