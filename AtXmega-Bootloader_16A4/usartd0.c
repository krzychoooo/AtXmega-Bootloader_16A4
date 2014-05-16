/*
 * usartd0.c
 *
 *  Created on: 21 gru 2013
 *      Author: krzysztof
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include "usartd0.h"
#include "xmega_baud.h"
#include "timer0x.h"

volatile uint8_t* timerd0;

char rx_buffer_usartd0[RX_BUFFER_SIZE_USARTD0];

// This flag is set on USARTD0 Receiver buffer overflow
char rx_buffer_overflow_usartd0=0;

char tx_buffer_usartd0[TX_BUFFER_SIZE_USARTD0];

volatile unsigned char rx_wr_index_usartd0=0,rx_rd_index_usartd0=0,rx_counter_usartd0=0;
volatile unsigned char tx_wr_index_usartd0=0,tx_rd_index_usartd0=0,tx_counter_usartd0=0;

// USARTD0 initialization
void usartd0_init()
{

// Note: the correct PORTC direction for the RxD, TxD and XCK signals
// is configured in the ports_init function

// Transmitter is enabled
// Set TxD=1
PORTD.OUTSET=0x08 + 0x04;
PORTD.DIRSET=0X08;

#ifdef USARTD0_DE_PORT
		// inicjalizujemy liniê steruj¹c¹ nadajnikiem
		USARTD0_DE_DIR |= 1<<USARTD0_DE_PIN;
		USARTD0_DE_ODBIERANIE;
// #else
// #error "nors485!"
#endif

#if F_CPU!=32000000
#error F_CPU
#endif

// Communication mode: Asynchronous USART
// Data bits: 8
// Stop bits: 1
// Parity: Disabled
USARTD0.CTRLC=USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;

// Receive complete interrupt: Low Level
// Transmit complete interrupt: Low Level
// Data register empty interrupt: Disabled
USARTD0.CTRLA=(USARTD0.CTRLA & (~(USART_RXCINTLVL_gm | USART_TXCINTLVL_gm | USART_DREINTLVL_gm))) |
	USART_RXCINTLVL_LO_gc | USART_TXCINTLVL_LO_gc | USART_DREINTLVL_OFF_gc;

#define BAUDRATE 19200
USARTD0.BAUDCTRLA = BSEL(F_CPU,BAUDRATE) & 0xff;
USARTD0.BAUDCTRLB = (BSCALE(F_CPU,BAUDRATE) << USART_BSCALE0_bp) | (BSEL(F_CPU,BAUDRATE) >> 8);
// Required Baud rate: 115200
// Real Baud Rate: 115211,5 (x1 Mode), Error: 0,0 %
//USARTD0.BAUDCTRLA=0x2E;
//USARTD0.BAUDCTRLB=((0x09 << USART_BSCALE_gp) & USART_BSCALE_gm) | 0x08;

// Receiver: On
// Transmitter: On
// Double transmission speed mode: Off
// Multi-processor communication mode: Off
USARTD0.CTRLB=(USARTD0.CTRLB & (~(USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm | USART_MPCM_bm | USART_TXB8_bm))) |
	USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm;
}




// USARTD0 Receiver interrupt service routine
//interrupt [USARTD0_RXC_vect] void usartd0_rx_isr(void)
ISR(USARTD0_RXC_vect)
{
unsigned char status;
uint8_t data;

status=USARTD0.STATUS;
data=USARTD0.DATA;
if ((status & (USART_FERR_bm | USART_PERR_bm | USART_BUFOVF_bm)) == 0)
   {
   rx_buffer_usartd0[rx_wr_index_usartd0++]=data;
   if (rx_wr_index_usartd0 == RX_BUFFER_SIZE_USARTD0) rx_wr_index_usartd0=0;
   if (++rx_counter_usartd0 == RX_BUFFER_SIZE_USARTD0)
      {
      rx_counter_usartd0=0;
      rx_buffer_overflow_usartd0=1;
      }
   }
   
}



int getchard0()
{
int data;

while (rx_counter_usartd0==0);
data=rx_buffer_usartd0[rx_rd_index_usartd0++];
if (rx_rd_index_usartd0 == RX_BUFFER_SIZE_USARTD0) rx_rd_index_usartd0=0;

asm("cli");
--rx_counter_usartd0;
asm("sei");
PORTA.OUTTGL = 1<<5;
return data;
}

void registerTimerd0(volatile uint8_t* t){
	timerd0 = t;
}


char getchard0Time(uint16_t time)
{
int data;

timer0T = time;
while ((rx_counter_usartd0 == 0) && (timer0T));
if(timer0T == 0){
	return 'E';		// brak znaków
}

while (rx_counter_usartd0==0);
data=rx_buffer_usartd0[rx_rd_index_usartd0++];
if (rx_rd_index_usartd0 == RX_BUFFER_SIZE_USARTD0) rx_rd_index_usartd0=0;

asm("cli");
--rx_counter_usartd0;
asm("sei");
return data;
}


// USARTD0 Transmitter interrupt service routine
ISR (USARTD0_TXC_vect)
{
	PORTA.OUTTGL = 1<<5;
	if (tx_counter_usartd0)
	   {
	   --tx_counter_usartd0;
	   USARTD0.DATA=tx_buffer_usartd0[tx_rd_index_usartd0++];
	   if (tx_rd_index_usartd0 == TX_BUFFER_SIZE_USARTD0) tx_rd_index_usartd0=0;
	   }
	else{
#ifdef USARTD0_DE_PORT

		USARTD0_DE_ODBIERANIE;	// zablokuj nadajnik RS485

#endif
	}
}


void putchard0(char c)
{
while (tx_counter_usartd0 == TX_BUFFER_SIZE_USARTD0);
asm("cli");
if (tx_counter_usartd0 || ((USARTD0.STATUS & USART_DREIF_bm)==0))
   {
   tx_buffer_usartd0[tx_wr_index_usartd0++]=c;
   if (tx_wr_index_usartd0 == TX_BUFFER_SIZE_USARTD0) tx_wr_index_usartd0=0;
   ++tx_counter_usartd0;
   }
else{
#ifdef USARTD0_DE_PORT
	USARTD0_DE_NADAWANIE;
#endif
	USARTD0.DATA=c;
}
asm("sei");
}


int putchard0Stream(char c, FILE *stream)
{
while (tx_counter_usartd0 == TX_BUFFER_SIZE_USARTD0);
asm("cli");
if (tx_counter_usartd0 || ((USARTD0.STATUS & USART_DREIF_bm)==0))
   {
   tx_buffer_usartd0[tx_wr_index_usartd0++]=c;
   if (tx_wr_index_usartd0 == TX_BUFFER_SIZE_USARTD0) tx_wr_index_usartd0=0;
   ++tx_counter_usartd0;
   }
else{
#ifdef USARTD0_DE_PORT
	USARTD0_DE_NADAWANIE;
#endif
	USARTD0.DATA=c;
}
asm("sei");
return 0;
}

