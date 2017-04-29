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
int waveform = 0;
int frequency = 100;
int duty_cycle = 50;
int sine_data[10000];
int main(void)
{
    //Initialize sine wave data
    int max_arg = 2*pi;
    int step = max_arg / (INT_ONCE_PER_MS / 10);
    int arg = 0;
    int i = 0;
    /*
    sine_data[0] = sin(0) * ONE_VOLT;
    for(i = 1; i < INT_ONCE_PER_MS / 10; i++){
        arg += step;
        sine_data[i] = sin(arg) * ONE_VOLT + 2*ONE_VOLT + ONE_VOLT/2;
    }
    */
    //poll keypad for button press
    //Set up for DAC
    DAC_init();
    char key_pressed;
    keypad_init();
    TimerA0_Init();
    while(1){
        key_pressed = poll_keys();
        delayMs(500);
        if(key_pressed > 0 & key_pressed <6){
            switch(key_pressed){
            case 1 :
                frequency = 100;
                break;
            case 2 :
                frequency = 200;
                break;
            case 3 :
                frequency = 300;
                break;
            case 4 :
                frequency = 400;
                break;
            case 5:
                frequency = 500;
                break;
            default:
                printf("YOU SHALL NOT PASS");
            }
        }
        else if(key_pressed > 6 & key_pressed < 10){
            switch(key_pressed){
            case 7 :
                waveform = SQUARE_WAVE;
                break;
            case 8 :
                waveform = SAWTOOTH_WAVE;
                break;
            case 9 :
                waveform = SINE_WAVE;
                break;
            default :
                printf("do nothing");
            }
        }
        else{
            switch(key_pressed){
            case 10 :
                if(duty_cycle > 10){
                    duty_cycle -= 10;
                }
                break;
            case 11 :
                duty_cycle = 50;
                break;
            case 12 :
                if(duty_cycle < 90){
                    duty_cycle += 10;
                }
                break;
            default :
                printf("do nothing");
            }

            waveform = SAWTOOTH_WAVE;
        }
    }

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
               TIMER_A0->CCR[0] += INT_ONCE_PER_MS / 10;//enter isr about once time every 0.1ms

          default :
              Drive_DAC(0);

      }


}



