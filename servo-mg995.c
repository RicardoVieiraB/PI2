#include <msp430g2553.h>

#define PWM BIT0
#define FREQ 5000

unsigned int m = 0;
unsigned int n = 0;

//int main(void)
int servopq(void)
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop WDT
    BCSCTL1 = CALBC1_1MHZ;      //MCLK e SMCLK @ 1MHz
    DCOCTL = CALDCO_1MHZ;       //MCLK e SMCLK @ 1MHz

    P1OUT &= ~PWM;
    P1DIR |= PWM;
    P1OUT |= PWM;

    TA0CCR1 = 180;
    TA0CCR2 = 2420;
    TA0CCR0 = TA0CCR2;

    TA0CTL = TASSEL_2 + ID_3 + MC_1; // clk/8 modo up


    while(1)
    {
        while((TA0CCTL1 & CCIFG)==0);
        P1OUT ^= PWM;
        TA0CCTL1 &= ~CCIFG;
        m++;
        n++;
        while((TA0CCTL2 & CCIFG)==0);
        P1OUT ^= PWM;
        TA0CCTL2 &= ~CCIFG;
        if(m==100){
            TA0CCR1 = 100;   //parado entre 180 e 195
            TA0CCR2 = 2500;
            TA0CCR0 = TA0CCR2;
           // m = 0;
        }
        if(n==200){
            TA0CCR1 = 210;
            TA0CCR2 = 2390;
            TA0CCR0 = TA0CCR2;
            m = 0;
            n = 0;
         }
    }
}
