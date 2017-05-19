//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "ADC.h"
#include "VT100.h"
#include "delays.h"

#define DC_MODE 0
#define AC_MODE 1
#define SAMPLING_RATE 9690//hz
#define SAMPLING_PERIOD 0.0001032//seconds
#define ONE_MILI_VOLT 4.94

void set_mode(int m);
void printFreq(int out);
void printP2P(int out);
void printRMSTrue(int out);
void printDCGraph(int out);

static int mode = AC_MODE;
static int DC_Data[];
void main(void)
{
    volatile unsigned int i;
    int DCavg = 0;
    float freq = 0;
    int freqOut = 0;
    int p2pOut = 0;
    int rmsTrueOut = 0;
    int rmsCalcOut = 0;
    int dcOut = 0;

    float VPeakToPeak = 0;
    float VRMSCalc = 0;
    float VRMSTrue = 0;
    int DCoffset = 0;
    //setting up P3.0 for simple GPIO to Measure Sampling Rate
    P3->DIR |= BIT0;
    P3->OUT |= BIT0;




    //set SMCLK to 1.5Mhz
    set_DCO(FREQ_3_MHz);

    //initialize UART
    UART0_init();

    //Print screen for DMM
    clearScreen();
    setupScreen();

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
                dcOut = (int)(DCoffset / ONE_MILI_VOLT) + 1;//(ADC_VALUE / TEN_MILI_VOLTS)*10;
                setDC();
                printP2P(dcOut);
                setDCGraph();
                printDCGraph(dcOut);
                fillBuffer();
                //find DC Offset
                //calculate frequency
                //freq = 1/(SAMPLING_PERIOD * calcFrequency());
                //freq = 4;
                calACvals(DCoffset);
                freq = getFreq() / TIME_TO_FILL_BUFFER;
                freqOut = (int)freq;
                setFreq();
                printFreq(freqOut);
                VPeakToPeak = getVpp() / ONE_MILI_VOLT;
                p2pOut = (int)VPeakToPeak;
                setPeak();
                printP2P(p2pOut);
                VRMSTrue = getVRMS_TRUE();
                rmsTrueOut = (int)VRMSTrue;
                setTrue();
                printP2P(rmsTrueOut);
                VRMSCalc = getVRMS_CALC();
                rmsCalcOut = (int)VRMSCalc;
                setCalc();
                printP2P(rmsCalcOut);

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

void printFreq(int out){
    int i, temp;
    int divider = 1000;

    if(out/1000){
        for(i=0; i<4; i++){
            int temp = out/divider;
            while(!(EUSCI_A0->IFG & 0x02)) { }
            EUSCI_A0->TXBUF = temp+48;
            out %= divider;
            divider /= 10;
        }
    }
    else if(out/100){
        divider = 100;
        for(i=0; i<3; i++){
            int temp = out/divider;
            while(!(EUSCI_A0->IFG & 0x02)) { }
            EUSCI_A0->TXBUF = temp+48;
            out %= divider;
            divider /= 10;
        }
    }
    else if(out/10){
        divider = 10;
        for(i=0; i<2; i++){
            int temp = out/divider;
            while(!(EUSCI_A0->IFG & 0x02)) { }
            EUSCI_A0->TXBUF = temp+48;
            out %= divider;
            divider /= 10;
        }
    }
    else{
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = out+48;
    }
}

void printP2P(int out){
    int i, temp;
    int divider = 1000;

    if(out/1000){
        for(i=0; i<4; i++){
            if(i == 1){
                while(!(EUSCI_A0->IFG & 0x02)) { }
                EUSCI_A0->TXBUF = '.';
            }
            int temp = out/divider;
            while(!(EUSCI_A0->IFG & 0x02)) { }
            EUSCI_A0->TXBUF = temp+48;
            out %= divider;
            divider /= 10;
        }
    }
    else if(out/100){
        divider = 100;
        for(i=0; i<3; i++){
            if(i == 0){
                while(!(EUSCI_A0->IFG & 0x02)) { }
                EUSCI_A0->TXBUF = '0';
                while(!(EUSCI_A0->IFG & 0x02)) { }
                EUSCI_A0->TXBUF = '.';
            }
            int temp = out/divider;
            while(!(EUSCI_A0->IFG & 0x02)) { }
            EUSCI_A0->TXBUF = temp+48;
            out %= divider;
            divider /= 10;
        }
    }
    else if(out/10){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = '0';
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = '.';
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = '0';
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = out/10 + 48;
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = out/100 + 48;
    }
    else{
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = '0';
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = '.';
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = '0';
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = '0';
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = out/100 + 48;
    }
}

void printDCGraph(int out){
    int i;
    float voltage = out;
    for(i = 0; i< (int)(voltage/3300*50); i++){
        while(!(EUSCI_A0->IFG & 0x02)) { }
        EUSCI_A0->TXBUF = '|';
    }
}
