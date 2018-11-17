#include <msp430g2452.h>

#define LED BIT0

//int main(void)
int led38k(void)
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop WDT
    BCSCTL1 = CALBC1_8MHZ;      //MCLK e SMCLK @ 1MHz
    DCOCTL = CALDCO_8MHZ;       //MCLK e SMCLK @ 1MHz

    P1OUT &= ~LED;
    P1DIR |= LED;
    TA0CCR0 = 13-1;
    TA0CTL = TASSEL_2 + ID_3 + MC_1;

    while(1)
    {
        while((TA0CTL & TAIFG)==0);
        P1OUT ^= LED;
        TA0CTL &= ~TAIFG;
    }
}
