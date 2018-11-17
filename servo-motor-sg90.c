#include <msp430g2553.h>

#define PWM BIT

unsigned int i = 0;
unsigned int j = 0;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop WDT
    BCSCTL1 = CALBC1_1MHZ;      //MCLK e SMCLK @ 1MHz
    DCOCTL = CALDCO_1MHZ;       //MCLK e SMCLK @ 1MHz

    P1OUT &= ~PWM;
    P1DIR |= PWM;
    P1OUT |= PWM;

    TA0CCR1 = 250;
    TA0CCR2 = 2250;
    TA0CCR0 = TA0CCR2;

    TA0CTL = TASSEL_2 + ID_3 + MC_1; // clk/8 modo up


    while(1)
    {
        while((TA0CCTL1 & CCIFG)==0);
        P1OUT ^= PWM;
        TA0CCTL1 &= ~CCIFG;
        i++;
        j++;
        while((TA0CCTL2 & CCIFG)==0);
        P1OUT ^= PWM;
        TA0CCTL2 &= ~CCIFG;
        if(i==50){
            TA0CCR1 = 61;
            TA0CCR2 = 2539;
            TA0CCR0 = TA0CCR2;
            //i = 0;
        }
        if(j==100){
            TA0CCR1 = 304;
            TA0CCR2 = 2296;
            TA0CCR0 = TA0CCR2;
            i = 0;
            j = 0;
         }
    }
}
