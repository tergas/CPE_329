/*
 * UART.h
 *
 *  Created on: May 11, 2017
 *      Author: josep_000
 */

#ifndef UART_H_
#define UART_H_
#include "msp.h"

void UART0_init(void);
void Write_to_terminal(char character);
void UART_INIT_init(void);
void newLine(void);

void UART0_init(void) {
	EUSCI_A0->CTLW0 |= 1;     /* put in reset mode for config */
	EUSCI_A0->MCTLW = 0;      /* disable oversampling */
	EUSCI_A0->CTLW0 = 0x0081; /* 1 stop bit, no parity, SMCLK, 8-bit data */
	EUSCI_A0->BRW = 26;       /* 3000000 / 115200 = 26 */
	P1->SEL0 |= 0x0C;         /* P1.3, P1.2 for UART */
	P1->SEL1 &= ~0x0C;
	EUSCI_A0->CTLW0 &= ~1;    /* take UART out of reset mode */
}

void Write_to_terminal(char character){
	while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
	EUSCI_A0-> TXBUF = character;
}

void UART_INT_init(void){
	EUSCI_A0->IE |= 1;        /* enable receive interrupt */
}

void newLine(void){
	while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
	EUSCI_A0-> TXBUF = 10;
	while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
	EUSCI_A0-> TXBUF = 13;
}
#endif /* UART_H_ */
