/*
 * timer0x.h
 *
 *  Created on: 8 kwi 2014
 *      Author: krzysztofklimas@interia.pl
 */

#ifndef TIMER0X_H_
#define TIMER0X_H_

#include <avr/io.h>

extern volatile uint16_t timer0T;

void tcc0_init(void);

#endif /* TIMER0X_H_ */
