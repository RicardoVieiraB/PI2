#include <msp430g2553.h>


int main(void)
{

    WDTCTL = WDTPW + WDTHOLD;   //para WDT
    BCSCTL1 = CALBC1_1MHZ;      //MCLK e SMCLK de 1MHz
    DCOCTL = CALDCO_1MHZ;       //MCLK e SMCLK de 1MHz

    P1OUT &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
    P1DIR |= BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7;

    P2OUT &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
    P2DIR |= BIT0|BIT1|BIT2|BIT3|BIT4|BIT5;

    TA0CCR0 = 62500-1;
    TA0CTL = TASSEL_2 + ID_3 + MC_3;

    while(1)
    {

        while((TA0CTL & TAIFG)==0);
        P1OUT ^= BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7;
        P2OUT ^= BIT0|BIT1|BIT2|BIT3|BIT4|BIT5;
        TA0CTL &= ~TAIFG;
    }
}
