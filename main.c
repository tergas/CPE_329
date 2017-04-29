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
int period = 2*INT_ONCE_PER_MS;//2ms or 500Hz
int frequency = 500;
int duty_cycle = 5;
int sine_data[200];
int main(void)
{
    //Initialize sine wave data
    double max_arg = 2*pi;
        double step = max_arg / (200);
        double arg = 0;
        int i = 0;
        sine_data[0] = sin(0) * ONE_VOLT;
        for(i = 1; i < 200; i++){
            arg += step;
            sine_data[i] = sin(arg) * 2.5 * ONE_VOLT + 2.5 * ONE_VOLT;
        }
    //start other stuff
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
        key_pressed = poll_keys();//wait for a button tio be pressed
        delayMs(500);//debounce
        if(key_pressed > 0 & key_pressed <7){
            switch(key_pressed){
            case 1 :
                period = 10  * INT_ONCE_PER_MS;//10ms, 100Hz
                frequency = 100;
                break;
            case 2 :
                period = 5 * INT_ONCE_PER_MS;//5ms, 200Hz
                frequency = 200;
                break;
            case 6 :
                period = 80000;//hard code to 3.33333ms to avoid int math issues
                frequency = 300;
                break;         //300Hz
            case 4 :
                period = INT_ONCE_PER_MS/2 + 2*INT_ONCE_PER_MS;//2.5ms, 400Hz
                frequency = 400;
                break;
            case 5:
                period = 2 * INT_ONCE_PER_MS;//2ms, 500Hz
                frequency = 500;
                break;
            default:
                printf("do nothing");
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
                //if DC > 10%, decrease DC by 10%
                if(duty_cycle > 10){
                    duty_cycle -= 5;
                }
                break;
            case 11 :
                //set DC to 50%
                duty_cycle = 50;
                break;
            case 12 :
                //if DC < 90%, increase DC by 10%
                if(duty_cycle < 90){
                    duty_cycle += 5;
                }
                break;
            default :
                printf("do nothing");
            }

            //waveform = SAWTOOTH_WAVE;
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
                if (isr_square_count > (100-duty_cycle) & isr_square_count < 100){//set to 2V for 10ms
                    TempDAC_Value = ONE_VOLT*5;//controls size of step
                    Drive_DAC(TempDAC_Value);
                }
                else if(isr_square_count <= (100-duty_cycle)){//set to 0V for 10ms
                    TempDAC_Value = 0;
                    Drive_DAC(TempDAC_Value);
                }
                else{//reset the count when reaches 20ms
                    isr_square_count = 0;
                }
              isr_square_count++;
              TIMER_A0->CCR[0] += (period / 100);//call ISR 100 times per period of the wave
              break;
          case SAWTOOTH_WAVE :
              if (isr_square_count > 100 & isr_square_count < 200){//set to 2V for 10ms
                  TempDAC_Value += (ONE_VOLT*5)/200;//controls size of step
                  Drive_DAC(TempDAC_Value);
              }
              else if(isr_square_count <= 100){//set to 0V for 10ms
                  TempDAC_Value += (ONE_VOLT*5)/200;//controls size of step
                  Drive_DAC(TempDAC_Value);
              }
              else{//reset the count when reaches 20ms
                  isr_square_count = 0;
              }
              isr_square_count++;
              TIMER_A0->CCR[0] += (period / 200);//call ISR 20 times per period of the wave
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
               //set voltae to 0V if no waveform is specified
               Drive_DAC(0);

      }


}



