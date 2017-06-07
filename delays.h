/*
 * delays.h
 *
 *  Created on: Apr 19, 2017
 *      Author: josep_000
 */

#ifndef DELAYS_H_
#define DELAYS_H_

/* p2_1.c Toggling green LED in C using header file register definitions.
 * This program toggles green LED for 0.5 second ON and 0.5 second OFF.
 * The green LED is connected to P2.1.
 * The LEDs are high active (a '1' turns ON the LED).
 *
 * Tested with Keil 5.20 and MSP432 Device Family Pack V2.2.0
 * on XMS432P401R Rev C.
 */

#include "msp.h"

#define FREQ_1_5_MHz 0
#define FREQ_3_MHz 1
#define FREQ_6_MHz 2
#define FREQ_12_MHz 3
#define FREQ_24_MHz 4
#define FREQ_48_MHz 5

void delayMs(int n);
int delay_ms(int n, int freq);
int delay_ns(int n, int freq);
void set_DCO(int freq);



/* delay milliseconds when system clock is at 3 MHz for Rev C MCU */
void delayMs(int n) {
    int i, j;

    for (j = 0; j < n; j++)
        for (i = 300; i > 0; i--);      /* Delay 1 ms*/
}

int delay_ms(int n, int freq){
    int i, j;
    if(freq == 0){
        for(j = 0; j < n; j++)
            for(i = 150; i > 0; i--);
    }
    else if(freq == 1){
        for(j = 0; j < n; j++)
            for(i = 300; i > 0; i--);
    }
    else if(freq == 2){
        for(j = 0; j < n; j++)
            for(i = 600; i > 0; i--);
    }
    else if(freq == 3){
        for(j = 0; j < n; j++)
            for(i = 1200; i > 0; i--);
    }
    else if(freq == 4){
        for(j = 0; j < n; j++)
            for(i = 2400; i > 0; i--);
    }
    else if(freq == 5){
        for(j = 0; j < n; j++)
            for(i = 4800; i > 0; i--);
    }
    else
        return 0;




    return 0;
}

/*nanosecond delay function - minimum delay = 3980 ns*/
int delay_ns(int n, int freq){
    int i, period, conditionals;

    if(freq == 0 && n >= 33200){
        period = 6800;
        conditionals = 24600;
    }
    else if(freq == 1 && n >= 18500){
        period = 3300;
        conditionals = 12600;
    }
    else if(freq == 2 && n >= 10000){
        period = 1500;
        conditionals = 6500;
    }
    else if(freq == 3 && n >= 5440){
        period = 840;
        conditionals = 3120;
    }
    else if(freq == 4 && n >= 2500){
        period = 360;
        conditionals = 3540;
    }
    else if(freq == 5){
        period = 0;
        conditionals = 0;
    }
    else
        return 0;

    for(i=0; i<(n-conditionals)/period; i++){

    }

    return 0;

}

void set_DCO(int freq){
    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;
    if(freq == FREQ_1_5_MHz){
        CS->CTL0 = CS_CTL0_DCORSEL_0;
    }
    else if(freq == FREQ_3_MHz){
        CS->CTL0 = CS_CTL0_DCORSEL_1;
    }
    else if(freq == FREQ_6_MHz){
        CS->CTL0 = CS_CTL0_DCORSEL_2;
    }
    else if(freq == FREQ_12_MHz){
        CS->CTL0 = CS_CTL0_DCORSEL_3;
    }
    else if(freq == FREQ_24_MHz){
        CS->CTL0 = CS_CTL0_DCORSEL_4;
    }
    else if(freq == FREQ_48_MHz){
        CS->CTL0 = CS_CTL0_DCORSEL_5;
    }
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;
}




#endif /* DELAYS_H_ */
