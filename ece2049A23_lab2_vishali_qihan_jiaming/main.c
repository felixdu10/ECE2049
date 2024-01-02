#include <msp430.h>
#include <stdlib.h>
#include "peripherals.h"
#include <time.h>
#include <math.h>
#include <string.h>
#include "note.h"
//#include <stdio.h>

unsigned int NOTES_H_;

unsigned int state = 0;

int buttonCheck(int notes);

void configButton(void);
void swDelay(char numLoops);
void configUserLED(char inbits); // example prototype
void startTimerA2(void);
void a2delay(float numLoops);
unsigned int note = 0;
double delay=2;
char TDIR = 0;
long int timer_cnt = 0;

int corrBut = 0;
int bPress = 0;
unsigned int score = 100;
int miss = 0;
int wPress = 0;
char final[] = {};
unsigned char currKey=0, inputB =0;

//Declare globals here
unsigned int pitch[] = {E,Eb,E,Eb,E,B,D,C1,A,0,C1,E,A,B,0,E,G,B,C1,0,E,E,Eb,E,Eb,E,B,D,C1,A,0,C1,E,A};
    unsigned int duration[] = {2,2,2,2,2,2,2,2,4,2,2,2,4,2,2,2,4,2,2,2,2,2,2,2,2,2,4,2,2,2};
    int leds[] = {LED_4,LED_3,LED_4,LED_3,LED_4,LED_1,LED_2,LED_1,LED_1,LED_0,LED_1,LED_4,LED_1,LED_1,LED_0,LED_4,LED_4,LED_1,LED_1,LED_0,LED_4,LED_4,LED_3,LED_4,LED_3,LED_4,LED_1,LED_2,LED_1,LED_0,LED_1,LED_4,LED_1};

    int arr[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i=0;
    int count=4;
    int a;



void main(void)

{
    WDTCTL = WDTPW | WDTHOLD;
    initLeds();
    configDisplay();
    configKeypad();
    _enable_interrupts();
    Graphics_clearDisplay(&g_sContext);
    unsigned char currKey=0;

    int count=4;
  //  double delay=2;


while (1){
    switch (state) {
    case 0: // Display Guitar Hero
        startTimerA2();
        Graphics_drawStringCentered(&g_sContext, "Guitar Hero", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);// Display welcome screen
        Graphics_drawStringCentered(&g_sContext, "Press * to start", AUTO_STRING_LENGTH, 48, 30, TRANSPARENT_TEXT);// Display welcome screen
        Graphics_flushBuffer(&g_sContext);

        note = 0;
        state = 1;
     break;

    case 1: // Press * key
        currKey = getKey();
                if (currKey == '*'){
                    state = 2;}
                else{
                    state = 0;}
     break;

    case 2: // Display 3-2-1 and 4 LEDs; count down
        startTimerA2();
        Graphics_clearDisplay(&g_sContext); // Clear the display
        Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 30, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext); // update display
        setLeds(2);//LED 3 on

      swDelay(1);//The number will appear for .5 second

        //a2delay(20);//the resolution is the resolution is 0.01s/bit.
        setLeds(0);

        Graphics_clearDisplay(&g_sContext); // Clear the display
        Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 30, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext); // update display
        setLeds(4);//LED2
      swDelay(1);
        //a2delay(20);
        setLeds(0);

        Graphics_clearDisplay(&g_sContext); // Clear the display
        Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 30, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext); // update display
        setLeds(8);//LED 1 on
      swDelay(1);
      //  a2delay(20);
        setLeds(0);

        Graphics_clearDisplay(&g_sContext); // Clear the display
        Graphics_drawStringCentered(&g_sContext, "GO", AUTO_STRING_LENGTH, 48, 30, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext); // update display
        setLeds(14);//all three LEDs on
//        a2delay(1);
        swDelay(1);
        setLeds(0);

        state = 3;//Test
        break;

    case 3://play song
        Graphics_drawStringCentered(&g_sContext, "play song", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
                        Graphics_flushBuffer(&g_sContext);



                  BuzzerOn(pitch[note]);
                        setLeds(leds[note]);
                        currKey = getKey();
                        if (currKey == '#'){
                            BuzzerOff();
                            setLeds(1);
                            Graphics_clearDisplay(&g_sContext);
                            state = 0;
                        }else{


                            timer_cnt = 0;
                            note=note+1;
                            state = 3;

                            if (note == 34){
                                state = 4;
                            }
                        }

                        break;




    case 4: // Display 4 LEDs
//        for (a=0; a<5; a++){



            for (i = 0; i<1; i++)
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
         state=5;
         break;


        case 5: // Check player input

            currKey = getKey();

            for (i=0;i<1;i++)
            {
                while(currKey == 0 ){

                 currKey=getKey();
               }
                if ((currKey - '0') != arr[i]){

                           state = 6;
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
            if (state==6){
                break;
            }
            state = 4;
            break;

        case 6: // Display player error message
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "Game Over", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "You lose", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            swDelay(2);
            Graphics_clearDisplay(&g_sContext);
            count=4;
            state=0;
            break;


        case 7: // Display player error message
                   Graphics_clearDisplay(&g_sContext);
                   Graphics_drawStringCentered(&g_sContext, "Game Over", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
                   Graphics_drawStringCentered(&g_sContext, "WIN", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);

                   Graphics_flushBuffer(&g_sContext);
                   swDelay(2);
                   Graphics_clearDisplay(&g_sContext);
                   count=4;
                   state=0;
                   break;








//    case 4: //check each button
//
//
//        for(i = 0; i< 34; i++){
//            Graphics_clearDisplay(&g_sContext);
//            Graphics_flashBuffer(&g_sContext);
//            setLeds(0);
//            timer=0;
//            timer_on = 0;
//            while(1){
//                timer_on = 1;
//                if(timer_on){
//                    if (timer % 10) == 0){
//                        displayingA2Timer(timer);
//
//
//                    }
//                    if ((sec == 1)&& (total_dec == 10)){
//                        tellTheNote(i);
//                        SltoS4to
//                    }
//                }
//            }
//        }
//
//
//
//
//
//
////        currKey = getKey();
////                      if (currKey == '*'){
////                          state = 4;}
////                      else{
////                          state = 5;}
//
//
//
//        currKey = getKey();
//        inputB = getButton();
//        corrBut = buttonCheck(pitch[note-1]);
//        if (inputB != 0){
//            bPress = 1;
//        }else{
//            bPress = 0;
//        }
//        if (currKey == '#'){
//            BuzzerOff();
//            setLeds(LED_0);
//            Graphics_clearDisplay(&g_sContext);
//            state = 0;
//        }else if(timer_cnt >= 100*duration[note-1]){
//            bPress = 0;
//            state = 3;
//            BuzzerOff();
//            setLeds(LED_0);
//        }
//
//        if (inputB != 0 && inputB != corrBut && bPress == 1){
//            wPress += 1;
//        }else if(inputB != 0 && inputB != corrBut && bPress == 1){
//            miss += 1;
//        }
//
//        score -= (wPress * 3 + miss*1);
//        currKey = getKey();
//        if (note == 34 || currKey == '0' || score >= 90){
//            BuzzerOff();
//            setLeds(0x00);
//            Graphics_clearDisplay(&g_sContext); // Clear the display
//            //winning page
//            Graphics_drawStringCentered(&g_sContext, "Final Score:", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
//           // printf(final,10, "%d",score);
//            Graphics_drawStringCentered(&g_sContext, "Congratulation!!", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
//            Graphics_flushBuffer(&g_sContext);
//
//            BuzzerOn(C1);
//            swDelay(1);
//            BuzzerOff();
//            swDelay(1);
//            BuzzerOn(D);
//            swDelay(1);
//            BuzzerOff();
//            swDelay(1);
//            BuzzerOn(E);
//            swDelay(1);
//            BuzzerOff();
//            swDelay(1);
//            BuzzerOn(G);
//            swDelay(1);
//            BuzzerOff();
//            swDelay(1);
//            BuzzerOn(F);
//            swDelay(1);
//            BuzzerOff();
//            swDelay(1);
//            BuzzerOn(G);
//            swDelay(1);
//            BuzzerOff();
//            swDelay(1);
//            BuzzerOn(A2);
//            swDelay(1);
//            BuzzerOff();
//            swDelay(1);
//            state = 5;
//        }else if(score <= 90 ){
//            state = 4;
//        }
//
//
//
//        break;
//
//    case 5://loss
//        currKey = getKey();
//                  if (currKey == '#'){
//                      BuzzerOff();
//                      setLeds(LED_0);
//                      Graphics_clearDisplay(&g_sContext);
//                      state = 0;
//                  }
//
//        Graphics_clearDisplay(&g_sContext);
//        Graphics_drawStringCentered(&g_sContext, "Game Over", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
//        Graphics_flushBuffer(&g_sContext);
//        swDelay(1);
//        Graphics_clearDisplay(&g_sContext);
//        count=4;
//        state=0;
//        break;
//
//
//    case 6://victory
//
//
//
//        break;


    }



}
}

void configButton(void){


    P7SEL &= ~(BIT4|BIT0);
    P3SEL &= ~(BIT6);
    P2SEL &= ~(BIT2);

    P7DIR &= (BIT4|BIT0);
    P3DIR &= (BIT6);
    P2DIR &= (BIT2);

    P7REN |= (BIT4|BIT0);
    P3REN |= (BIT6);
    P2REN |= (BIT2);
}

int buttonCheck(int notes){
     unsigned int ret_val = 0;
     if (notes==E){
         ret_val = 0x01;
     }
     else if (notes==Eb)   {
         ret_val = 0x02;
     }
     else if (notes==D)   {
         ret_val = 0x04;
     }
     else if (notes==C || notes==B || notes==A)   {
         ret_val = 0x08;
     }
     else if (notes==0){
         ret_val = 0;
     }

     return ret_val;
}


 void configUserLED(char inbits){

    if (inbits == '1'){
        setLeds(1);
    }
    else if (inbits == '2'){
        setLeds(2);
    }
    else if (inbits == '3'){
        setLeds(3);
    }
    else if (inbits == '4'){
        setLeds(4); // update display
    }
}

void startTimerA2(void)
  {
    // Use ACLK (TASSEL_1), clock divider of 1 (ID_0)
    // and start timer counting in Up mode ---MC_1
    TA2CTL = TASSEL_1 + MC_1 + ID_0;
    TA2CCR0 = 163;
    TA2CCTL0 = CCIE;      // TA2CCR0 interrupt enabled
    _BIS_SR(GIE);
  }


#pragma vector=TIMER2_A0_VECTOR
  __interrupt void Timer_A2_ISR(void)
  {
  // timer and leap_cnt are Global unsigned integers
      if (TDIR)
         {
           timer_cnt++; // counting up
           if (timer_cnt == 60000)
            timer_cnt = 0;
           if (timer_cnt%100==0)  // then blink LEDs once a second
           {
            P1OUT = P1OUT ^ BIT0;
                  P4OUT ^= BIT7;
       } }
         else
           timer_cnt--;  // then counting down
}

void a2delay(float numLoops){
       timer_cnt = 0; // the timer_cnt increments 100 times per second
       while(timer_cnt < numLoops){

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
