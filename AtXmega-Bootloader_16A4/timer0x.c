/*
 * timer0.c
 *
 *  Created on: 8 kwi 2014
 *      Author: krzysztofklimas@interia.pl
 */

#include "timer0x.h"
#include <util/atomic.h>

volatile uint16_t timer0T;
volatile uint8_t timer_led;
// Timer/Counter TCC0 initialization
void tcc0_init(void)
{
	cli();

	TCC0.INTCTRLA     =    TC_OVFINTLVL_LO_gc;         // przepe³nienie ma generowaæ przerwanie LO
	//PMIC.CTRL         |=    PMIC_LOLVLEN_bm;            // odblokowanie przerwañ o priorytecie LO
	// konfiguracja timera
	TCC0.CTRLB        =    TC_WGMODE_NORMAL_gc;        // tryb normalny
	//  TCC0.CTRLFSET     =    TC0_DIR_bm;                 // liczenie w dó³
	TCC0.CTRLA        =    TC_CLKSEL_DIV256_gc;       // ustawienie preskalera i uruchomienie timera
	TCC0.PER = 1250;
	// Restore interrupts enabled/disabled state
	sei();
}

// Timer/Counter TCC0 Overflow/Underflow interrupt service routine
ISR (TCC0_OVF_vect)
{
	if (timer0T){
		timer0T--;
		timer_led--;
		if(timer_led==0){
			PORTA.OUTTGL = 1<<5;
			timer_led = 5;
		}
	}
}
