/*
 * VT100.h
 *
 *  Created on: May 18, 2017
 *      Author: trevorergas
 */

#ifndef VT100_H_
#define VT100_H_

#include "msp.h"

#define ESC 27

void UART0_init(void);
void setupScreen(void);
void clearScreen(void);
void setDC(void);
void setPeak(void);
void setCalc(void);
void setTrue(void);
void setFreq(void);
void setDCGraph(void);
void setCalcGraph(void);

void UART0_init(void) {
    EUSCI_A0->CTLW0 |= 1;     /* put in reset mode for config */
    EUSCI_A0->MCTLW = 0;      /* disable oversampling */
    EUSCI_A0->CTLW0 = 0x0081; /* 1 stop bit, no parity, SMCLK, 8-bit data */
    EUSCI_A0->BRW = 1250;       /* 24000000 / 115200 = 1250 */
    P1->SEL0 |= 0x0C;         /* P1.3, P1.2 for UART */
    P1->SEL1 &= ~0x0C;
    EUSCI_A0->CTLW0 &= ~1;    /* take UART out of reset mode */
    EUSCI_A0->IE |= 1;        /* enable receive interrupt */
}

void setupScreen(void){
    char line1[] =  "DC Voltage:       ";
    char line2[] =  "--------------------------------";
    char line3[] =  "AC Voltage: ";
    char line4[] =  "    Peak-to-Peak: ";
    char line5[] =  "    Calc RMS:     ";
    char line6[] =  "    True RMS:     ";
    char line7[] =  "--------------------------------";
    char line8[] =  "Frequency:        ";
    char line9[] =  "--------------------------------";
    char line10[] = "  Calc-RMS: ";
    char line11[] = "DC Voltage: ";

    int i = 0;

    for(i=0; i<sizeof(line1); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line1[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
    for(i=0; i<sizeof(line2); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line2[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
    for(i=0; i<sizeof(line3); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line3[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
    for(i=0; i<sizeof(line4); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line4[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
    for(i=0; i<sizeof(line5); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line5[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
    for(i=0; i<sizeof(line6); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line6[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
    for(i=0; i<sizeof(line7); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line7[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
    for(i=0; i<sizeof(line8); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line8[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
    for(i=0; i<sizeof(line9); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line9[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
    for(i=0; i<sizeof(line10); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line10[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
    for(i=0; i<sizeof(line11); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = line11[i];
    }
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 10;
    while(!(EUSCI_A0->IFG & 0x02)) { }  /* wait until receive buffer is full */
    EUSCI_A0-> TXBUF = 13;
}

void clearScreen(void){
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '2';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'J';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'H';
}

void setDC(void){
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ';';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '9';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'f';
}

void setPeak(void){
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '4';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ';';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '9';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'f';
}

void setCalc(void){
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '5';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ';';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '9';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'f';
}

void setTrue(void){
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '6';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ';';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '9';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'f';
}

void setFreq(void){
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '8';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ';';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '9';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'f';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '0';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'K';

}

void setDCGraph(void){
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ';';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '3';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'f';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '0';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'K';
}

void setCalcGraph(void){
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '0';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ';';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '1';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '3';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'f';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = ESC;
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '[';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = '0';
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = 'K';
}


#endif /* VT100_H_ */
