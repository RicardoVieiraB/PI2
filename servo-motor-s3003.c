#include <msp430g2553.h>

#define PWM BIT0

unsigned int k = 0;
unsigned int l = 0;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop WDT
    BCSCTL1 = CALBC1_1MHZ;      //MCLK e SMCLK @ 1MHz
    DCOCTL = CALDCO_1MHZ;       //MCLK e SMCLK @ 1MHz

    P1OUT &= ~PWM;
    P1DIR |= PWM;
    P1OUT |= PWM;               //inicia PWM sempre em nivel logico alto
    //valores iniciais do timer
    TA0CCR1 = 250;              //contagens q PWM fica em HIGH
    TA0CCR2 = 2350;             //contagens q PWM fica em LOW
    TA0CCR0 = TA0CCR2;          //p/ zerar timer

    TA0CTL = TASSEL_2 + ID_3 + MC_1; // clk/8 modo up


    while(1)
    {
        while((TA0CCTL1 & CCIFG)==0);
        P1OUT ^= PWM;
        TA0CCTL1 &= ~CCIFG;
        k++;
        l++;
        while((TA0CCTL2 & CCIFG)==0);
        P1OUT ^= PWM;
        TA0CCTL2 &= ~CCIFG;
        if(k==100){
            TA0CCR1 = 80;
            TA0CCR2 = 2520;
            TA0CCR0 = TA0CCR2;
            //i = 0;
        }
        if(l==200){
            TA0CCR1 = 290;
            TA0CCR2 = 2310;
            TA0CCR0 = TA0CCR2;
            k = 0;
            l = 0;
         }
    }
}
