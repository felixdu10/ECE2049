#include <msp430.h>
#include <stdlib.h>
#include "peripherals.h"

unsigned int state = 0;
void swDelay(char numLoops);

void main(void)

{
    WDTCTL = WDTPW | WDTHOLD;
    initLeds();
    configDisplay();
    configKeypad();
    Graphics_clearDisplay(&g_sContext);
    unsigned char currKey=0;
    int arr[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i;
    int count=4;
    double delay=2;


while (1){
    switch (state) {
    case 0: // Display Simon
        Graphics_drawStringCentered(&g_sContext, "SIMON", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);// Display welcome screen
        Graphics_flushBuffer(&g_sContext);
        state = 1;
     break;

    case 1: // Press * key
        currKey = getKey();
                if (currKey == '*'){
                    state = 2;}
                else{
                    state = 0;}
     break;

    case 2: // Display 3-2-1 and 4 LEDs
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        swDelay(1);
        Graphics_clearDisplay(&g_sContext);
        state=3;
        break;

    case 3: // Display 4 LEDs
        if (count==33){
            count = 4;
            state = 0;
            break;
        }
        for (i = 0; i<count; i++)
        {
            int num = (rand() % 4) + 1;
            if (num==1){
                setLeds(0x08);
                BuzzerOn(75);
                swDelay(delay);
                setLeds(0);
                BuzzerOff();
                swDelay(delay);
            }
            else if (num==2){
                setLeds(0x04);
                BuzzerOn(125);
                swDelay(delay);
                setLeds(0);
                BuzzerOff();
                swDelay(delay);
            }
            else if (num==3){
                setLeds(0x02);
                BuzzerOn(175);
                swDelay(delay);
                setLeds(0);
                BuzzerOff();
                swDelay(delay);
            }
            else if (num==4){
                setLeds(0x01);
                BuzzerOn(225);
                swDelay(delay);
                setLeds(0);
                BuzzerOff();
                swDelay(delay);
            }
            arr[i]=num;
        }
     if (state==0)break;
     state=4;
     break;

    case 4: // Check player input

        currKey = getKey();

        for (i=0;i<count;i++)
        {
            while(currKey == 0 ){

             currKey=getKey();
           }
            if ((currKey - '0') != arr[i]){

                       state = 5;
                       break;
                }
            else {
                if (arr[i]==1){
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 15, 25, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(1);
                Graphics_clearDisplay(&g_sContext);
                }
                else if (arr[i]==2){
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 25, 25, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(1);
                Graphics_clearDisplay(&g_sContext);
                }
                else if (arr[i]==3){
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 35, 25, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(1);
                Graphics_clearDisplay(&g_sContext);
                }
                else{
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "4", AUTO_STRING_LENGTH, 45, 25, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(1);
                Graphics_clearDisplay(&g_sContext);
                }
            }
            while(currKey != 0){
                currKey=getKey();
            }

        }
        count++;
        delay=delay-0.1;
        if (state==5){
            break;
        }
        state = 3;
        break;

    case 5: // Display player error message
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "Game Over", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        swDelay(1);
        Graphics_clearDisplay(&g_sContext);
        count=4;
        state=0;
        break;
    }
}
}

void swDelay(char numLoops)
{

    volatile unsigned int i,j;


    for (j=0; j<numLoops; j++)
    {
        i = 50000 ;
        while (i > 0)
           i--;
    }
}
