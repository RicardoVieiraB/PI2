#include <msp430g2553.h>
#define LED BIT0
#define LDR  BIT5

//int main(void)
int ldr(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    P1OUT |= LED;
    P1DIR |= LED;
    P1DIR &= ~LDR;
    P1REN |= LDR;
    P1OUT |= LDR;
    while(1)
    {
        if((P1IN&LDR)==0)
            P1OUT |= LED;
        else
            P1OUT &= ~LED;
    }
}

