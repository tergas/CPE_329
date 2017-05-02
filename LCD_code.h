/*
 * LCD_code.h
 *
 *  Created on: Apr 13, 2017
 *      Author: josep_000
 */

#ifndef LCD_CODE_H_
#define LCD_CODE_H_
/* www.MicroDigitalEd.com
 * p3_3.c: Initialize and display "hello" on the LCD using 4-bit data mode.
 * Data and control pins share Port 4.
 * This program does not poll the status of the LCD.
 * It uses delay to wait out the time LCD controller is busy.
 * Timing is more relax than the HD44780 datasheet to accommodate the
 * variations among the LCD modules.
 * You may want to adjust the amount of delay for your LCD controller.
 *
 * Tested with Keil 5.20 and MSP432 Device Family Pack V2.2.0
 * on XMS432P401R Rev C.
 */
#include "msp.h"

#define RS 1     /* P5.0 mask */
#define RW 2     /* P5.1 mask */
#define EN 4     /* P5.2 mask */

void delayMs(int n);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void Clear_LCD(void);
void Home_LCD(void);
void ShiftRight_LCD(void);
void write_to_LCD(char word[]);

void Clear_LCD(void){//clear the LCD
    LCD_command(1);
}

void Home_LCD(void){//move cursor home
    LCD_command(2);
}

void ShiftRight_LCD(void){//shift cursor to the right
    LCD_command(0x06);
}

void write_to_LCD(char word[16]){
    int i = 0;
    LCD_command(1);//clear LCD
    LCD_command(0x80);//set Cursor home
    for(i = 0; i < sizeof(word); i++){
        LCD_data(word[i]);
    }
   // Home_LCD();
}


////////////////////////////////////BOOK CODE BELOW//////////////////////////////////////////////////////////////////////////

void LCD_init(void) {
    P5->DIR = 0xFF;     /* make P5 pins output for data and controls */
    delayMs(30);                /* initialization sequence */
    LCD_nibble_write(0x30, 0);
    delayMs(10);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
    delayMs(1);

    LCD_command(0x28);      /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x06);      /* move cursor right after each char */
    LCD_command(0x01);      /* clear screen, move cursor to home */
    LCD_command(0x0F);      /* turn on display, cursor blinking */
}

/* With 4-bit mode, each command or data is sent twice with upper
 * nibble first then lower nibble.
 */
void LCD_nibble_write(unsigned char data, unsigned char control) {
    data &= 0xF0;           /* clear lower nibble for control */
    control &= 0x0F;        /* clear upper nibble for data */
    P5->OUT = data | control;       /* RS = 0, R/W = 0 */
    P5->OUT = data | control | EN;  /* pulse E */
    delayMs(0);
    P5->OUT = data;                 /* clear E */
    P5->OUT = 0;
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);    /* upper nibble first */
    LCD_nibble_write(command << 4, 0);      /* then lower nibble */

    if (command < 4)
        delayMs(4);         /* commands 1 and 2 need up to 1.64ms */
    else
        delayMs(1);         /* all others 40 us */
}


void LCD_data(unsigned char data) {
    LCD_nibble_write(data & 0xF0, RS);    /* upper nibble first */
    LCD_nibble_write(data << 4, RS);      /* then lower nibble  */

    delayMs(1);
}

/* delay milliseconds when system clock is at 3 MHz */
void delayMs(int n) {
    int i, j;

    for (j = 0; j < n; j++)
        for (i = 300; i > 0; i--);      /* Delay */
}





#endif /* LCD_CODE_H_ */
