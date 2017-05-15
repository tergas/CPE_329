/*
 * ADC.h
 *
 *  Created on: May 12, 2017
 *      Author: josep_000
 */

#ifndef ADC_H_
#define ADC_H_

#include "msp.h"
#include "UART.h"
#include "delays.h"

#define SAMPLING_FREQ 2.5589
#define TEN_MILI_VOLTS 49.3//used to convert ADC_VALUE to a voltage
#define DATA_SIZE 2500

//static variable to limit scope to this file
static int ADC_UPDATED = 0;//flag that gets raised when ADC gets a new value
static int ADC_VALUE = 0;//Value stored by adc
static int calibrated_adc_val;//adc value calilbrated to a voltage in mV
static int ADC_BUFFER[DATA_SIZE];
static int DC_avg;

int Check_ADC(void);
int Get_ADC_VALUE();
int calibrate_ADC_VALUE(void);
void print_value_to_terminal(int num);
int find_DC_avg(void);
void fillBuffer(void);
void ADC_init(void);


void ADC_init(void){
    // Configure P5.4 for ADC
       P5->SEL1 |= BIT4;
       P5->SEL0 |= BIT4;

       // Enable ADC interrupt in NVIC module
       NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

       // Sampling time, S&H=16, ADC14 on, choose SMCLK(DC0), Pre-divide by 4
       ADC14->CTL0 = ADC14_CTL0_SHT0__32 | ADC14_CTL0_SHP | ADC14_CTL0_ON | ADC14_CTL0_SSEL__SMCLK | ADC14_CTL0_PDIV__1;
       ADC14->CTL1 = ADC14_CTL1_RES_3;         // Use sampling timer, 14-bit conversion results

       ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;   // A1 ADC input select; Vref=AVCC
       ADC14->IER0 |= ADC14_IER0_IE0;          // Enable ADC conv complete interrupt

       SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;   // Wake up on exit from ISR

}
//fills ADC_BUFFER with 1 second of data sampled at Fs ~ 2.5Khz
void fillBuffer(void){
    while(ADC_UPDATED == 0){
               ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
       }
}



int find_DC_avg(void){
    int i = 0;
    int sum = 0;
    for(i = 0; i < DATA_SIZE; i++){
        sum += ADC_BUFFER[i];
    }
    return (int) sum/DATA_SIZE;
}

//checks if ADC is updated, ADC_UPDATED can be accessed from other files
int Check_ADC(void){
    return ADC_UPDATED;
}

//gets the ADV_VALUE calibrated to number of mV
int calibrate_ADC_VALUE(void){
    float temp = (ADC_VALUE / TEN_MILI_VOLTS)*10;
    (int) temp;
    return temp;
}

/*breaks argument into characters and transmits them to the
  terminal. prints in format "X.YZ".  For example passing
  2350 into function would print "2.35" to the terminal.
*/
void print_value_to_terminal(int num){
    int i;
    int divider = 1000;
    for(i=0; i< 4; i++){
        if(i == 1){//hardcode second charater to a decimal point
            Write_to_terminal('.');
        }
        else{
            int temp = num/divider;
            Write_to_terminal((char)temp+48);
            num %= divider;
            divider /= 10;
        }
    }
    newLine();
}





#endif /* ADC_H_ */
