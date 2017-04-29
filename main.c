//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "DAC_code.h"
#include "keyPad_code.h"
#include <math.h>

#define ONE_VOLT 815
#define SQUARE_WAVE 1
#define SAWTOOTH_WAVE 2
#define SINE_WAVE 3
#define INT_ONCE_PER_MS 24000
#define pi 3.14159265358979323846
void interrupt_intit(int cctl);
int waveform = SINE_WAVE;
int frequency = 100;
int sine_data[10000];
int main(void)
{
    double max_arg = 2*pi;
    double step = max_arg / (200);
    double arg = 0;
    int i = 0;
    sine_data[0] = sin(0) * ONE_VOLT;
    for(i = 1; i < 200; i++){
        arg += step;
        sine_data[i] = sin(arg) * 2.5 * ONE_VOLT + 2.5 * ONE_VOLT;
    }
//start main
//initialize sine wave value array
//poll keypad for button press
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;   // Stop watchdog timer
    // DCO = 24 MHz, SMCLK and MCLK = DCO
    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;
    CS->CTL0 = CS_CTL0_DCORSEL_4;   // DCO = 24 MHz
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;
    // Configure port bits for SPI
    P4->DIR |= BIT1;        // Will use BIT4 to activate /CE on the DAC
    P1SEL0 |= BIT6 + BIT5;  // Configure P1.6 and P1.5 for UCB0SIMO and UCB0CLK
    P1SEL1 &= ~(BIT6 + BIT5);

    // SPI Setup
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;// Put eUSCI state machine in reset
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST |//Remain eUSCI state machine in reset
                      EUSCI_B_CTLW0_MST   |// Set as SPI master
                      EUSCI_B_CTLW0_SYNC  |// Set as synchronous mode
                      EUSCI_B_CTLW0_CKPL  |// Set clock polarity high
                      EUSCI_B_CTLW0_MSB;   // MSB first

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK; // SMCLK
    // divide by 16, clock = fBRCLK/(UCBRx)
    EUSCI_B0->BRW = 0x01;
    // Initialize USCI state machine, SPI
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
    // now waiting for something to be placed in TXBUF
    EUSCI_B0->IFG |= EUSCI_B_IFG_TXIFG;  // Clear TXIFG flag
    //Timer setup including interrupt code
    TimerA0_Init();


} // end of main

//Timer A0 ISR
void TA0_0_IRQHandler(void) {
      TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
      static int isr_square_count = 0;
      static int isr_sawtooth_count = 0;
      static int isr_sine_count = 0;
      switch(waveform){
          case SQUARE_WAVE :
                if (isr_square_count > 10 & isr_square_count < 20){//set to 2V for 10ms
                    TempDAC_Value = ONE_VOLT*5;//controls size of step
                    Drive_DAC(TempDAC_Value);
                }
                else if(isr_square_count <= 10){//set to 0V for 10ms
                    TempDAC_Value = 0;
                    Drive_DAC(TempDAC_Value);
                }
                else{//reset the count when reaches 20ms
                    isr_square_count = 0;
                }
              isr_square_count++;
              TIMER_A0->CCR[0] += INT_ONCE_PER_MS ;//enter isr about once time every ms
              break;
          case SAWTOOTH_WAVE :
               TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
               if (isr_sawtooth_count <= 200){//rise from 0 to 2V in 10ms
                   TempDAC_Value +=25;//controls size of step
                   Drive_DAC(TempDAC_Value);
               }
               else{
                   isr_sawtooth_count =0;
               }
               isr_sawtooth_count++;
               TIMER_A0->CCR[0] += INT_ONCE_PER_MS  / 10;//enter isr about once time every 0.1ms
               break;
          case SINE_WAVE :
               TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
               TempDAC_Value = sine_data[isr_sine_count];//controls size of step
               Drive_DAC(TempDAC_Value);
               isr_sine_count ++;
               if(isr_sine_count == 200){
                   isr_sine_count = 0;
               }
               if(frequency == 500){
                   TIMER_A0->CCR[0] += 240;//enter isr about once time every 0.1ms
               }
               else if(frequency == 400){
                   TIMER_A0->CCR[0] += 300;//enter isr about once time every 0.1ms
               }
               else if(frequency == 300){
                   TIMER_A0->CCR[0] += 400;//enter isr about once time every 0.1ms
               }
               else if(frequency == 200){
                   TIMER_A0->CCR[0] += 600;//enter isr about once time every 0.1ms
               }
               else if(frequency == 100){
                   TIMER_A0->CCR[0] += 1200;//enter isr about once time every 0.1ms
               }
               break;
          default :
              printf ("Trevor is a bitch");

      }


}



