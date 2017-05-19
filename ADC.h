/*
 * ADC.h
 *
 *  Created on: May 12, 2017
 *      Author: josep_000
 */

#ifndef ADC_H_
#define ADC_H_

#include "msp.h"
#include "delays.h"
#include <math.h>

#define SAMPLING_FREQ 9727.6
//9311//hz

#define TEN_MILI_VOLTS 49.3//used to convert ADC_VALUE to a voltage
#define DATA_SIZE 10000
#define TIME_TO_FILL_BUFFER 1.053
#define ONE_MILI_VOLT 4.94

//make data size a variable than can be changed by fill buffer

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
int find_DC_avg(int numSamples);
void fillBuffer(void);
void ADC_init(void);
int calcFrequency(int offset);
int calcACvals(int offset);
float getDCAvg(void);


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
    ADC_UPDATED = 0;
    while(ADC_UPDATED == 0){
               ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
       }
}



int find_DC_avg(int numSamples){
    int i = 50;
    int sum = 0;
    for(i = 0; i < numSamples; i++){
        sum += ADC_BUFFER[i];
    }
    return (int) sum/numSamples;
}
////Functions for Calculating AC MEasurements (freq, Vpp, RMS, ect..)
static float freq = 0;
static float Vpp = 0;
static float VrmsTrue = 0;
static float VrmsCalc = 0;
static int DC_avg = 0;

void calACvals(int offset){
    int i = 0;
        int currentVal = 0;
        int prevVal = 0;
        int count = 0;
        int rmsCount = 0;
        int crossingAxis = 0;
        //float freq = 0;
        int Vmax = 0;
        int Vmin = 100000;
        int trueSum = 0;
        int calcSum = 0;
        int DCSum = 0;
        for(i = 1; i < DATA_SIZE; i++){
            //Freg Calcs
            prevVal = ADC_BUFFER[i-1];
            currentVal = ADC_BUFFER[i];
            if(prevVal <= offset && currentVal > offset){
                count ++;
                if(crossingAxis < 150){
                crossingAxis ++;;//stays on for 1 period
                }
            }
            else if(prevVal >= offset && currentVal < offset){
                count ++;
            }//end Freq Calcs
            //Vpp calcs
            if(i > 100 && currentVal > Vmax){
                Vmax = currentVal;
            }
            else if(i > 100 && currentVal < Vmin){
                Vmin = currentVal;
            }//end Vpp Calcs
            //TrueRMS calcs
            if(crossingAxis <= 145){
                trueSum += (currentVal / ONE_MILI_VOLT) * (currentVal / ONE_MILI_VOLT);
                calcSum += ((currentVal - offset) / ONE_MILI_VOLT) * ((currentVal - offset) / ONE_MILI_VOLT);
                DCSum += (currentVal / ONE_MILI_VOLT);
                rmsCount ++;
            }
            //end TrueRMS calcs
         }
         freq = count/2;
         //value in ADC terms needs to be converted to voltage
         //minus 20 to compensate for noise spikes
         Vpp = Vmax - Vmin - 20;
         VrmsTrue = sqrt(trueSum / rmsCount);
         VrmsCalc = sqrt(calcSum / rmsCount);
         DC_avg = DCSum/rmsCount;

}

//returns frequency of AC input
float getFreq(void){
    return freq;
}

float getVpp(void){
    return Vpp;
}

float getVRMS_TRUE(void){
    return VrmsTrue;
}

float getVRMS_CALC(void){
    return VrmsCalc;
}

float getDCAvg(void){
    return DC_avg;
}
/*int calcFrequency(int offset){
    int i = 0;
    int currentVal = 0;
    int prevVal = 0;
    int count = 0;
    float freq = 0;
    for(i = 1; i < DATA_SIZE; i++){
        prevVal = ADC_BUFFER[i-1];
        currentVal = ADC_BUFFER[i];
        if(prevVal <= offset && currentVal > offset){
            count ++;
        }
        else if(prevVal >= offset && currentVal < offset){
            count ++;
        }
     }
     return count/2;
}*/



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
/*void print_value_to_terminal(int num){
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
*/

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    // int ADC_val;
    static int count = 0;
    static int ADC_uptate_count = 0;
    //dummy variable used to make else logic take the same amount of time as the else if
    int temp = 0;

    if(count == DATA_SIZE * 2){
        count = 0;
        ADC_UPDATED = 1;
        ADC_uptate_count ++;
    }
    else if (count % 2 == 0){
        P3->OUT |= BIT0;
        ADC_BUFFER[count / 2] = ADC14->MEM[0];
        //ADC_UPDATED = 0;
        count ++;
        P3->OUT &= ~BIT0;
    }
    else{
        temp = ADC14->MEM[0];
        //ADC_UPDATED = 0;
        count ++;
    }
}



#endif /* ADC_H_ */
