#include "msp.h"
#include "delays.h"
#define TEN_MILI_VOLTS 49.3//used to convert ADC_VALUE to a voltage
//static variable to limit scope to this file
static int ADC_UPDATED = 1;//flag that gets raised when ADC gets a new value
static int ADC_VALUE = 0;//Value stored by adc
static int calibrated_adc_val;//adc value calilbrated to a voltage in mV

int Check_ADC(void);
int Get_ADC_VALUE();
int calibrate_ADC_VALUE(void);
void print_value_to_terminal(int num);

int main(void) {
    volatile unsigned int i;
    // Stop WDT
    WDT_A->CTL = WDT_A_CTL_PW |
        WDT_A_CTL_HOLD;

    // Configure P5.4 for ADC
    P5->SEL1 |= BIT4;
    P5->SEL0 |= BIT4;

    //setting up P3.0 for simple GPIO to Measure Sampling Rate
    P3->DIR |= BIT0;
    P3->OUT &= ~BIT0;

    P5->DIR |= BIT0;
    P5->OUT &= ~BIT0;

   // set_DCO(FREQ_24_MHz);

    // Enable global interrupt
    __enable_irq();

    // Enable ADC interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

    // Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_3;         // Use sampling timer, 14-bit conversion results

    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;   // A1 ADC input select; Vref=AVCC
    ADC14->IER0 |= ADC14_IER0_IE0;          // Enable ADC conv complete interrupt

    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;   // Wake up on exit from ISR
    //int value = 0;
    int threshold = 9500;
    int lowThreshold = 8500;
    int step = 0;
    while (1)
    {
        if(ADC_UPDATED){
            //P3->OUT |= BIT0;
            if(step == 0 && ADC_VALUE > threshold){
               step = 1;
               delayMs(50);
               ADC_UPDATED = 0;
            }//end step 0
            else if(step == 1 && ADC_UPDATED){
                //P3->OUT |= BIT0;
                if(ADC_VALUE < lowThreshold && ADC_VALUE > 6000){
                    step = 2;
                    delayMs(120);
                }
                else{
                    step = 0;
                }
                ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
                ADC_UPDATED = 0;
            }
            else if(step == 2 && ADC_UPDATED){
                if(ADC_VALUE > threshold){
                    P3->OUT ^= BIT0;
                    //step = 3;
                    step = 0;
                }
                ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
                ADC_UPDATED = 0;
            }
            ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
            ADC_UPDATED = 0;
        }

    }//end while

}//end main

//checks if ADC is updated, ADC_UPDATED can be accessed from other files
int Check_ADC(void){
    return ADC_UPDATED;
}


// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    // int ADC_val;
    P5->OUT |= BIT0;
    ADC_VALUE = ADC14->MEM[0];//1240 = 1 Volt
    ADC_UPDATED = 1;
    P5->OUT &= ~BIT0;
}
