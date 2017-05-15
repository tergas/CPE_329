//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "ADC.h"
#include "UART.h"
#include "delays.h"

#define DC_MODE 0
#define AC_MODE 1
#define SAMPLING_RATE

void set_mode(int m);

static int mode = DC_MODE;
static int DC_Data[];
void main(void)
{
    volatile unsigned int i;
    //setting up P3.0 for simple GPIO to Measure Sampling Rate
    P3->DIR |= BIT0;
    P3->OUT |= BIT0;

    //set SMCLK to 1.5Mhz
    set_DCO(FREQ_1_5_MHz);

    //set up ADC
    ADC_init();

    //Sample for 1 second at 2.5Khz
    fillBuffer();

    // Enable global interrupt
    __enable_irq();

    while (1){
        //do nothing
    }
}

void set_mode(int m){
    mode = m;
}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    // int ADC_val;
    static int count = 0;
    static int ADC_uptate_count = 0;
    //dummy variable used to make else logic take the same amount of time as the else if
    int temp = 0;

    if(count == DATA_SIZE * 4){
        count = 0;
        ADC_UPDATED = 1;
        ADC_uptate_count ++;
    }
    else if (count % 4 == 0){
        P3->OUT |= BIT0;
        ADC_BUFFER[count / 4] = ADC14->MEM[0];
        //ADC_UPDATED = 0;
        count ++;
        P3->OUT &= ~BIT0;
    }
    else{
        temp = ADC14->MEM[0];
        ADC_UPDATED = 0;
        count ++;
    }
}
    /*
    P3->OUT |= BIT0;
    ADC_VALUE = ADC14->MEM[0];//1240 = 1 Volt
    ADC_UPDATED = 1;
    P3->OUT &= ~BIT0;
    */



