/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP432 CODE EXAMPLE DISCLAIMER
 *
 * MSP432 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see http://www.ti.com/tool/mspdriverlib for an API functional
 * library & https://dev.ti.com/pinmux/ for a GUI approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//******************************************************************************
//  MSP432P401 Demo - ADC14, Sample A1, AVcc Ref, Set P1.0 if A1 > 0.5*AVcc
//
//   Description: A single sample is made on A1 with reference to AVcc.
//   Software sets ADC14_CTL0_SC to start sample and conversion - ADC14_CTL0_SC
//   automatically cleared at EOC. ADC14 internal oscillator times sample (16x)
//   and conversion. In Mainloop MSP432 waits in LPM0 to save power until ADC14
//   conversion complete, ADC14_ISR will force exit from LPM0 in Mainloop on
//   reti. If A0 > 0.5*AVcc, P1.0 set, else reset. The full, correct handling of
//   and ADC14 interrupt is shown as well.
//
//
//                MSP432P401x
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//        >---|P5.4/A1      P1.0|-->LED
//
//   William Goh
//   Texas Instruments Inc.
//   June 2016 (updated) | November 2013 (created)
//   Built with CCSv6.1, IAR, Keil, GCC
//******************************************************************************
#include "msp.h"
#include "UART.h"
#define TEN_MILI_VOLTS 49.3
static int ADC_UPDATED = 1;//static variable to limit scope to this file
static int ADC_VALUE = 0;
static int calibrated_adc_val;

int Check_ADC(void);//checks if ADC is updated, ADC_UPDATED can be accessed from other files
int Get_ADC_VALUE();
int calibrate_ADC_VALUE(void);

int main(void) {
    volatile unsigned int i;

    WDT_A->CTL = WDT_A_CTL_PW |             // Stop WDT
                 WDT_A_CTL_HOLD;

    // GPIO Setup
    P1->OUT &= ~BIT0;                       // Clear LED to start
    P1->DIR |= BIT0;                        // Set P1.0/LED to output
    P5->SEL1 |= BIT4;                       // Configure P5.4 for ADC
    P5->SEL0 |= BIT4;

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

    UART0_init();//set up UART to write to terminal
    Write_to_terminal('B');

    while (1)
    {
        for (i = 20000; i > 0; i--);        // Delay

        // Start sampling/conversion
        if(ADC_UPDATED){
            calibrated_adc_val = calibrate_ADC_VALUE();
            print_value_to_terminal();
            ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
            ADC_UPDATED = 0;
        }

    }
}

int Check_ADC(void){
    return ADC_UPDATED;
}

int calibrate_ADC_VALUE(void){
    float temp = (ADC_VALUE / TEN_MILI_VOLTS)*10;
    (int) temp;
    return temp;
}

void print_value_to_terminal(int num){
    int i;
    int divider = 1000;
    for(i=0; i< 4; i++){
        if(i == 1){
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

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
   // int ADC_val;
    ADC_VALUE = ADC14->MEM[0];//1240 = 1 Volt
    ADC_UPDATED = 1;
}
