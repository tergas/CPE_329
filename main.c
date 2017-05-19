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
#define SAMPLING_RATE 9690//hz
#define SAMPLING_PERIOD 0.0001032//seconds
#define ONE_MILI_VOLT 4.94

void set_mode(int m);

static int mode = AC_MODE;
static int DC_Data[];
void main(void)
{
    volatile unsigned int i;
    int DCavg = 0;
    float freq = 0;
    float VPeakToPeak = 0;
    float VRMSTrue = 0;
    int DCoffset = 0;
    //setting up P3.0 for simple GPIO to Measure Sampling Rate
    P3->DIR |= BIT0;
    P3->OUT |= BIT0;

    //set SMCLK to 1.5Mhz
    set_DCO(FREQ_1_5_MHz);

    //set up ADC
    ADC_init();

    //Sample for 1 second at 10Khz
    fillBuffer();

    // Enable global interrupt
    __enable_irq();

    while (1){
        //do nothing
        switch(mode){
        case (DC_MODE) :
                //fill buffer, take first 10ms of buffer(250 samples)
                //and compute the average.  Print value to terminal
                DCavg = (int)((find_DC_avg(250)) / ONE_MILI_VOLT);//(ADC_VALUE / TEN_MILI_VOLTS)*10;
                //print value to terminal
                fillBuffer();//see if i can make this faster
                break;
        case (AC_MODE) :
                //fill buffer(takes 1 second);
                DCoffset = (find_DC_avg(9000));
                fillBuffer();
                //find DC Offset
                //calculate frequency
                //freq = 1/(SAMPLING_PERIOD * calcFrequency());
                //freq = 4;
                calACvals(DCoffset);
                freq = getFreq() / TIME_TO_FILL_BUFFER;
                VPeakToPeak = getVpp() / ONE_MILI_VOLT;
                VRMSTrue = getVRMS_TRUE();
                //freq = calcFrequency();
                //choose RMS time based on frequency
                //DC offset calculation, Peak-to-Peak, and RMS calculation
                //print values to terminal
                break;
        default :
                printf("do nothing");
        }
    }
}

void set_mode(int m){
    mode = m;
}






