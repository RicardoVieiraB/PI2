#include <msp430g2553.h>

#define PWM BIT0

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
    P1OUT |= PWM;               //inicia com pulso do PWM sempre começando em HIGH
    //valores iniciais do timer (nao tao necessario)
    TA0CCR1 = 180;              //contagens q o PWM fica em HIGH          
    TA0CCR2 = 2420;             //contagens q o PWM fica em LOW
    TA0CCR0 = TA0CCR2;          //serve para saber quando timer zerou

    TA0CTL = TASSEL_2 + ID_3 + MC_1; // clk/8 modo up


    while(1)
    { //gerador de PWM
        while((TA0CCTL1 & CCIFG)==0); //confere primeiro register e inverte flag
        P1OUT ^= PWM;
        TA0CCTL1 &= ~CCIFG;
        m++;  //variaveis de controle para inverter sentido
        n++;
        while((TA0CCTL2 & CCIFG)==0); //confere segundo register e inverte flag
        P1OUT ^= PWM;
        TA0CCTL2 &= ~CCIFG;
        if(m==100){   //valores para um sentido
            TA0CCR1 = 100;   //parado entre 180 e 195
            TA0CCR2 = 2500;
            TA0CCR0 = TA0CCR2;
           // m = 0;
        }
        if(n==200){   //valores para outro sentido
            TA0CCR1 = 210;
            TA0CCR2 = 2390;
            TA0CCR0 = TA0CCR2;
            m = 0;
            n = 0;
         }
    }
}
