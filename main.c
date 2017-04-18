/* p3_5.c: Matrix keypad scanning
 *
 * This program scans a 4x4 matrix keypad and returns a unique number
 * for each key pressed.
 *
 * Port 4 7-4 are connected to the columns and Port 4 3-0 are connected
 * to the rows of the keypad.
 *
 * Tested with Keil 5.20 and MSP432 Device Family Pack V2.2.0
 * on XMS432P401R Rev C.
 */

#include "msp.h"
#include "LCD_code.h"
#include "string.h"
#include <stdio.h>
void delay(void);
void keypad_init(void);
char keypad_getkey(void);
void LED_init(void);
void LED_set(int value);
void print_key(int value);
void startUp_screen(void);
unsigned char check_keys(void);
int compareArrays(unsigned char attempt[], unsigned char pass[]);
void helloWorld(void);
unsigned char * newPass(unsigned char new[]);
void newPass_screen(void);
unsigned char poll_keys(void);

int main(void) {
    unsigned char key[4];
    int correct = 0, j = 0;
    unsigned char password[] = {1, 2, 3, 4};
    keypad_init();
    LED_init();
    LCD_init();
    startUp_screen();

while(correct == 0){
    for(j = 0; j<4; j++){
        key[j] = '0';
    }

    while(1) {
        key[0] = keypad_getkey();  /* read the keypad */
            if(key[0] > 0){
                break;
           }
   }
    delayMs(500);
    if(key[0] == 12){
        correct = 0;
        print_key(key[0]);
        delayMs(500);
        LCD_init();
        newPass_screen();
        for(j = 0; j<4; j++){
            password[j] = poll_keys();
            print_key(password[j]);
            delayMs(500);
        }
        LCD_init();

    }
    else if(key[0] == 10){
        correct = 0;
        LCD_init();
        delayMs(500);
    }
    else{
        print_key(key[0]);
        P6->DIR &= 0;
        keypad_init();

        while(1) {
            key[1] = keypad_getkey();  /* read the keypad */
                if(key[1] > 0){
                    break;
                }
        }
        delayMs(500);
        if(key[1] == 12){
            print_key(key[0]);
            delayMs(500);
            correct = 0;
            LCD_init();
            newPass_screen();
            for(j = 0; j<4; j++){
                password[j] = poll_keys();
                print_key(password[j]);
                delayMs(500);
            }
            LCD_init();
        }
        else if(key[1] == 10){
            correct = 0;
            LCD_init();
            delayMs(500);
        }
        else{
            print_key(key[1]);
            P6->DIR &= 0;
            keypad_init();

            while(1) {
                key[2] = keypad_getkey();  /* read the keypad */
                    if(key[2] > 0){
                        break;
                    }
            }

            delayMs(500);
            if(key[2] == 12){
                correct = 0;
                print_key(key[0]);
                delayMs(500);
                LCD_init();
                newPass_screen();
                for(j = 0; j<4; j++){
                    password[j] = poll_keys();
                    print_key(password[j]);
                    delayMs(500);
                }
                LCD_init();
            }
            else if(key[2] == 10){
                correct = 0;
                LCD_init();
                delayMs(500);
            }
            else{
                print_key(key[2]);
                P6->DIR &= 0;
                keypad_init();

                while(1) {
                    key[3] = keypad_getkey();  /* read the keypad */
                        if(key[3] > 0){
                            break;
                        }
                }
                delayMs(500);
                if(key[3] == 12){
                    correct = 0;
                    print_key(key[0]);
                    delayMs(500);
                    LCD_init();
                    newPass_screen();
                    for(j = 0; j<4; j++){
                        password[j] = poll_keys();
                        print_key(password[j]);
                        delayMs(500);
                    }
                    LCD_init();
                }
                else if(key[3] == 10){
                    correct = 0;
                    LCD_init();
                    delayMs(500);
                }
                else{
                    print_key(key[3]);
                    P6->DIR &= 0;
                    keypad_init();
                    LCD_init();
                }
            }
        }
    }
        if(compareArrays(key, password)){

            correct = 1;
            helloWorld();
        }
        else{
            startUp_screen();
        }


    }
}

/* this function initializes Port 4 that is connected to the keypad.
 * All pins are configured as GPIO input pin. The column pins have
 * the pull-up resistors enabled.
 */
void keypad_init(void) {
    P6->DIR = 0;
    P6->REN = 0x70;     /* enable pull resistor for column pins */
    P6->OUT = 0x70;     /* make column pins pull-ups */
}

/*
 * This is a non-blocking function to read the keypad.
 * If a key is pressed, it returns a unique code for the key. Otherwise,
 * a zero is returned.
 * The upper nibble of Port 4 is used as input and connected to the columns.
 * Pull-up resistors are enabled so when the keys are not pressed, these pins
 * are pulled high.
 * The lower nibble of Port 4 is used as output that drives the keypad rows.
 * First all rows are driven low and the input pins are read. If no key is pressed,
 * they will read as all one because of the pull up resistors. If they are not
 * all one, some key is pressed.
 * If some key is pressed, the program proceeds to drive one row low at a time and
 * leave the rest of the rows inactive (float) then read the input pins.
 * Knowing which row is active and which column is active, the program
 * can decide which key is pressed.
 *
 * Only one row is driven so that if multiple keys are pressed and row pins are shorted,
 * the microcontroller will not be damaged. When the row is being deactivated,
 * it is driven high first otherwise the stray capacitance may keep the inactive
 * row low for some time.)
 */
char keypad_getkey(void) {
    int row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08}; /* one row is active */

    /* check to see any key pressed */
    P6->DIR |= 0x0F;            /* make all row pins output */
    P6->OUT &= ~0x0F;           /* drive all row pins low */
    delay();                    /* wait for signals to settle */
    col = P6->IN & 0x70;        /* read all column pins */
    P6->OUT |= 0x0F;            /* drive all rows high before disable them */
    P6->DIR &= ~0x0F;           /* disable all row pins drive */
    if (col == 0x70)            /* if all columns are high */
        return 0;               /* no key pressed */

    /* If a key is pressed, it gets here to find out which key.
     * It activates one row at a time and read the input to see
     * which column is active. */
    for (row = 0; row < 4; row++) {
        P6->DIR &= 0x0F;                /* disable all rows */
        P6->DIR |= row_select[row];     /* enable one row at a time */
        P6->OUT &= ~row_select[row];    /* drive the active row low */
        delay();                        /* wait for signal to settle */
        col = P6->IN & 0x70;            /* read all columns */
        P6->OUT |= row_select[row];     /* drive the active row high */
        if (col != 0x70) break;         /* if one of the input is low, some key is pressed. */
    }
    P6->OUT |= 0x0F;                    /* drive all rows high before disable them */
    P6->DIR &= 0x0F;                    /* disable all rows */
    if (row == 4)
        return 0;                       /* if we get here, no key is pressed */

    /* gets here when one of the rows has key pressed, check which column it is */
    if (col == 0x60) return row * 3 + 1;    /* key in column 0 */
    if (col == 0x50) return row * 3 + 2;    /* key in column 1 */
    if (col == 0x30) return row * 3 + 3;    /* key in column 2 */
    //if (col == 0x70) return row * 4 + 4;    /* key in column 3 */

    return 0;   /* just to be safe */
}

/* Initialize tri-color LEDs on the LaunchPad board.
 * P2.0 - red
 * P2.1 - green
 * P2.2 - blue
 */
void LED_init(void) {
    P2->DIR = 0x07;         /* make LED pins output */
    P2->OUT &= ~0x07;       /* turn the LEDs off */
}

/* turn on or off the LEDs according to bit 2-0 of the value */
void LED_set(int value) {
    value &= 0x07;          /* only bit 2-0 are affected */
    P2->OUT = (P2->OUT & ~0x07) | value;
}

/* make a small delay */
void delay(void) {
}

void print_key(int value){
    if(value == 1){
        LCD_data('1');
    }
    else if(value == 2){
        LCD_data('2');
    }
    else if(value == 3){
        LCD_data('3');
    }
    else if(value == 4){
        LCD_data('4');
    }
    else if(value == 5){
        LCD_data('5');
    }
    else if(value == 6){
        LCD_data('6');
    }
    else if(value == 7){
        LCD_data('7');
    }
    else if(value == 8){
        LCD_data('8');
    }
    else if(value == 9){
        LCD_data('9');
    }
    else if(value == 10){
        LCD_data('*');
    }
    else if(value == 11){
        LCD_data('0');
    }
    else if(value == 12){
        LCD_data('#');
    }
}

void startUp_screen(void){
    char lockStart[6] = "LOCKED";
    char enterKey[10] = "ENTER KEY ";
    int i = 0;

    for(i = 0; i < sizeof(lockStart); i++){
            LCD_data(lockStart[i]);
        }

    LCD_command(0xC0);

    for(i = 0; i < sizeof(enterKey); i++){
        LCD_data(enterKey[i]);
    }
}

void helloWorld(void){
    char hello[11] = "HELLO WORLD";
    int i = 0;

    for(i = 0; i < sizeof(hello); i++){
        LCD_data(hello[i]);
    }
}

int compareArrays(unsigned char attempt[], unsigned char pass[]){
    int j = 0;
    for(j = 0; j < 4; j++){
            if(attempt[j] != pass
                    [j]){
                return 0;
            }
    }

    return 1;
}

unsigned char poll_keys(void){
    char key = 0;
    while(key <= 0){
        key = keypad_getkey();
    }
    return key;
}

/*unsigned char * newPass(unsigned char* new){
    while(1){
        *new = keypad_getkey();  /* read the keypad /
            if(*new > 0){
                break;
            }
    }

    delayMs(500);
    print_key(*new);

    while(1){
        *(new+1) = keypad_getkey();
            if(*(new+1) > 0){
                break;
            }
    }

    delayMs(500);
    print_key(*(new+1));

    while(1){
        *(new+2) = keypad_getkey();
            if(*(new+2) > 0){
                break;
            }
    }

    delayMs(500);
    print_key(*(new+2));

    while(1){
        *(new+3) = keypad_getkey();
            if(*(new+3) > 0){
                break;
            }
    }

    delayMs(500);
    print_key(*(new+3));

    *(new+4) = '\0';

    return new;

}*/

void newPass_screen(void){
    char line_1[12] = "NEW PASSWORD";
    char line_2[9] = "ENTER KEY";
    int i;

    for(i = 0; i < sizeof(line_1); i++){
            LCD_data(line_1[i]);
        }

    LCD_command(0xC0);

    for(i = 0; i < sizeof(line_2); i++){
        LCD_data(line_2[i]);
    }

}
