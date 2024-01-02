/************** ECE2049 DEMO CODE ******************/
/**************  13 March 2019   ******************/
/***************************************************/

#include <msp430.h>
#include "peripherals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CALADC12_15V_30C  *((unsigned int *)0x1A1A)
#define CALADC12_15V_85C  *((unsigned int *)0x1A1C)
#define MA_PER_BIT        (0.0244f)

// Function Prototypes
void runTimerA2(void);
void stopTimerA2(void);
void swDelay(char numLoops);
void delay(int number_of_seconds);
void displayTime(long unsigned int inTime, unsigned int sel);
void displayTemp(unsigned int inAvgTempC);
void configADC_12(void);

// Declare globals here
char TDIR = 1;
unsigned long int secs = 15757200,
    M = 0,
    D = 0,
    h = 0,
    m = 0,
    s = 0;
float temp[] = {0,0,0,0,0,0,0,0,0,0};
int monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};//12 months with days respectively
int arrayL = 0;
float averageTemp;
int p;
unsigned char button;
int month_sel;
long unsigned int time_cnt = 0;
long unsigned int startTime = 0;
unsigned int state = 0;
char count =0;
unsigned int temp_vals[256];
long int time[256];
long int timer = 0;
unsigned int avg_array[256];

void main(void)

{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
    __bis_SR_register(GIE);

    // Useful code starts here
    initLeds();
    configDisplay();
    configKeypad();
    initPushButons();
    configADC_12();
    configTimerA2();

    startTime = time_cnt;

    while (1){
        switch (state){

        case 0: //Display
            configADC_12();
            if (arrayL >= 10) arrayL = 10;
            else arrayL++;
            for (p=0; p<arrayL; p++) {
                temp[p] = setTemp();
                averageTemp += temp[p];
            }
            averageTemp = setTemp();
            displayTemp(averageTemp);
            displayTime(secs,0);
            button = setUserButton();
            if(button == 2){
                stopTimerA2();
                state = 1;
            }

        break;

        case 1: //months
            button = setUserButton();
            configADC_12();
            ADC12CTL0 &= ~ADC12SC;
            ADC12CTL0 |= ADC12SC;
            while (ADC12CTL0 & ADC12BUSY)
                __no_operation();
            in_current = ADC12MEM0;
            M = 0;
            month_sel = in_current / 342;
            for(p=0; p<month_sel; p++){
                M += monthDays[p] * 86400;
            }
            secs = M;
            displayTime(M,1);
            if(button == 0){
                button = setUserButton();
                if(button == 2){
                    state = 2;
                }else if(button == 1){
                    state = 0;
                }
            }

        break;

        case 2: //days
            button = setUserButton();
            configADC_12();
            ADC12CTL0 &= ~ADC12SC;
            ADC12CTL0 |= ADC12SC;
            while (ADC12CTL0 & ADC12BUSY)
             __no_operation();
            in_current = ADC12MEM0;
            D = (in_current / (4095/monthDays[month_sel] + 1)) * 86400;
            secs = M + D;
            displayTime(secs,2);
            if (button == 0){
                button = setUserButton();
                if(button == 2){
                    state = 3;
                }
                else if(button == 1){
                    runTimerA2();
                    state = 0;
                }
            }

        break;

        case 3: //hours
            button = setUserButton();
            configADC_12();
            ADC12CTL0 &= ~ADC12SC;
            ADC12CTL0 |= ADC12SC;
            while (ADC12CTL0 & ADC12BUSY)
             __no_operation();
            in_current = ADC12MEM0;
            h = (in_current/171) * 3600;
            if (h==2864) h = 68400;
            else if (h == 6464) h = 72000;
            else if (h == 10064) h = 75600;
            else if (h == 13664) h = 79200;
            else if (h == 17264) h = 82800;
            secs = M + D + h;
            displayTime(secs,3);
            // switch to edit month if left button pressed
            if (button == 0){
                button = setUserButton();
                if(button == 2){
                   state = 4;
                }
                else if(button == 1){
                    runTimerA2();
                    state = 0;
                }
            }

        break;

        case 4: //minutes
            button = setUserButton();
            configADC_12();
            ADC12CTL0 &= ~ADC12SC;
            ADC12CTL0 |= ADC12SC;
            while (ADC12CTL0 & ADC12BUSY)
             __no_operation();
            in_current = ADC12MEM0;
            m = (in_current/69) * 60;
            secs = M + D + h + m;
            displayTime(secs,4);
            // switch to edit month if left button pressed
            if (button == 0){
                button = setUserButton();
                if(button == 2){
                   state = 5;
                }
                else if(button == 1){
                    runTimerA2();
                    state = 0;
                }
            }

        break;

        case 5: //seconds
            button = setUserButton();
            configADC_12();
            ADC12CTL0 &= ~ADC12SC;
            ADC12CTL0 |= ADC12SC;
            while (ADC12CTL0 & ADC12BUSY)
             __no_operation();
            in_current = ADC12MEM0;
            s = (in_current/69);
            secs = M + D + h + m + s;
            displayTime(secs,5);
            // switch to edit month if left button pressed
            if (button == 0){
                button = setUserButton();
                if(button == 2){
                   state = 1;
                }
                else if(button == 1){
                    runTimerA2();
                    state = 0;
                }
            }

        break;

        }
    }
}


void swDelay(char numLoops)
{

	volatile unsigned int i,j;	// volatile to prevent removal in optimization
			                    // by compiler. Functionally this is useless code

	for (j=0; j<numLoops; j++)
    {
    	i = 50000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}
void displayTime(long unsigned int inTime, unsigned int sel){
    unsigned char date[6];
    unsigned char time[8];
    unsigned long int hours, minutes, seconds;
    seconds = inTime%60;
    minutes = (inTime/60)%60;
    hours = (inTime/3600)%24;
    long int days = inTime / 86400 + 1;
    long int day = days;
    if (days>=1 && days <=31){
        date[0] = 'J';
        date[1] = 'A';
        date[2] = 'N';
    }else if(days>= 32 && days <= 59){
        date[0] = 'F';
        date[1] = 'E';
        date[2] = 'B';
        day -= 31;
    }else if(days>= 60 && days <= 90){
        date[0] = 'M';
        date[1] = 'A';
        date[2] = 'R';
        day -= 59;
    }else if(days>= 91 && days <= 120){
        date[0] = 'A';
        date[1] = 'P';
        date[2] = 'R';
        day -= 90;
    }else if(days>= 121 && days <= 151){
        date[0] = 'M';
        date[1] = 'A';
        date[2] = 'Y';
        day -= 120;
    }else if(days>= 152 && days <= 181){
        date[0] = 'J';
        date[1] = 'U';
        date[2] = 'N';
        day -= 151;
    }else if(days>= 182 && days <= 212){
        date[0] = 'J';
        date[1] = 'U';
        date[2] = 'L';
        day -= 181;
    }else if(days>= 213 && days <= 243){
        date[0] = 'A';
        date[1] = 'U';
        date[2] = 'G';
        day -= 212;
    }else if(days>= 244 && days <= 273){
        date[0] = 'S';
        date[1] = 'E';
        date[2] = 'P';
        day -= 243;
    }else if(days>= 274 && days <= 304){
        date[0] = 'O';
        date[1] = 'C';
        date[2] = 'T';
        day -= 273;
    }else if(days>= 305 && days <= 334){
        date[0] = 'N';
        date[1] = 'O';
        date[2] = 'V';
        day -= 304;
    }else if(days >=335 && days <= 365){
        date[0] = 'D';
        date[1] = 'E';
        date[2] = 'C';
        day -= 334;
    }
        date[3] = '-';
        time[2] = ':';
        time[5] = ':';
        if(day/10 == 0){
            date[4] = ' ';
        }else{
            date[4] = (day/10) + 0x30;
        }
        if(day%10 == 0){
            date[5] = 0x30;
        }else{
            date[5] = (day%10) + 0x30;
        }
        if(hours/10 == 0){
            time[0] = ' ';
        }else{
            time[0] = hours/10 + 0x30;
        }
        if(hours%10 == 0){
            time[1] = 0x30;
        }else{
            time[1] = hours%10 + 0x30;
        }
        if(minutes/10 == 0){
            time[3] = 0x30;
        }else{
            time[3] = minutes/10 + 0x30;
        }
        if(minutes%10 == 0){
            time[4] = 0x30;
        }else{
            time[4] = minutes%10 + 0x30;
        }
        if(seconds/10 == 0){
            time[6] = 0x30;
        }else{
            time[6] = seconds/10 + 0x30;
        }
        if(seconds%10 == 0){
            time[7] = 0x30;
        }else{
            time[7] = seconds%10 + 0x30;
        }
        //show
        Graphics_drawStringCentered(&g_sContext, date , 6, 50, 35, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, time , 8, 50, 45, OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
        if(sel == 3){
            time[0] = ' ';
            time[1] = ' ';
        }else if(sel == 4){
            time[3] = ' ';
            time[4] = ' ';
        }else if(sel == 5){
            time[6] = ' ';
            time[7] = ' ';
        }else if(sel == 1){
            date[0] = ' ';
            date[1] = ' ';
            date[2] = ' ';
        }else if(sel == 2){
            date[4] = ' ';
            date[5] = ' ';
        }
        Graphics_drawStringCentered(&g_sContext, date , 6, 50, 35, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, time , 8, 50, 45, OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
}
void displayTemp(unsigned int in_temp){

        volatile float tempDegC;
        volatile float tempDegF;
        volatile float degC_per_bit;
        volatile unsigned int bits30, bits85;

        bits30 = CALADC12_15V_30C;
        bits85 = CALADC12_15V_85C;
        degC_per_bit = ((float)(85.0 - 30.0))/((float)(bits85-bits30));

        unsigned char TempF[7];
        unsigned char TempC[7];
        tempDegC = (float)((long)in_temp - CALADC12_15V_30C) * degC_per_bit +30.0;
        tempDegF = (float)((tempDegC* 1.8) + 32.0);

        TempC[0] = (int)(tempDegC/100)%10 + 0x30;
        TempC[1] = (int)(tempDegC/10)%10 + 0x30;
        TempC[2] = (int)tempDegC%10 + 0x30;
        TempC[3] = '.';
        TempC[4] = (int)(tempDegC*10)%10 + 0x30;
        TempC[5] = ' ';
        TempC[6] = 'C';

        TempF[0] = (int)(tempDegF/100)%10 + 0x30;
        TempF[1] = (int)(tempDegF/10)%10 + 0x30;
        TempF[2] = (int)tempDegF%10 + 0x30;
        TempF[3] = '.';
        TempF[4] = (int)(tempDegF*10)%10 + 0x30;
        TempF[5] = ' ';
        TempF[6] = 'F';
        //show
        Graphics_drawStringCentered(&g_sContext, TempF , sizeof(TempF)/sizeof(TempF[0]), 50, 65, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, TempC , sizeof(TempC)/sizeof(TempC[0]), 50, 75, OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
        count++;
}
void runTimerA2(void) {
    TA2CTL = TASSEL_1 + ID_0 + MC_1;
    TA2CCR0 = 32768;    // t_int = 1sec
    TA2CCTL0 = CCIE;    // enable TA2CCR0 interrupt
}
void stopTimerA2() {
    TA2CTL = MC_0;      // stop timer
    TA2CCTL0 &= ~CCIE;  // TA2CCR0 interrupt disabled
    secs =0;           // reset tcount
}
void configTimerA2 (void){
    TA2CTL = TASSEL_1 + MC_1 + ID_0;
    TA2CCR0 = 32767;
    TA2CCTL0 = CCIE;
    _BIS_SR(GIE);
    }
    #pragma vector=TIMER2_A0_VECTOR
    __interrupt void TimerA2_ISR (void)
    {
    if (TDIR)
      {
        secs++;
        if (secs%100==0)
        {
         P1OUT = P1OUT ^ BIT0;
               P4OUT ^= BIT7;
    }
      }else
          secs--;
}
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void) {
    ADC12CTL0 |= ADC12SC + ADC12ENC;
    in_current = ADC12MEM0;
    in_temp = ADC12MEM1;
}
