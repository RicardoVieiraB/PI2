#include <msp430g2553.h>

#define RECEPT  BIT5

//int main(void)
int receptor38k(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    P1DIR &= ~RECEPT;
    P1REN |= RECEPT;
    P1OUT |= RECEPT;

    while(1)
    {
     //   if((P1IN&RECEPT)==0)

    //    else

    }
}
