
#include <msp430g2553.h>

//#define ENPASSO BIT0
#define ENPASSO BIT1     //pq BIT0 do msp tá queimado, acoplar no BIT0 da placa
#define CLKPASSO BIT2    //acoplar no bit3 da placa
#define DIRPASSO BIT5
#define BTN BIT3

void atraso(volatile unsigned int t)
{
  TACTL |= TACLR;
	TACCR0 = 1000-1;
	TACTL = TASSEL_2 + ID_0 + MC_1;
	while(t--)
	{
		while((TACTL&TAIFG)==0);
		TACTL &= ~TAIFG;
	}
	TACTL = MC_0;
  
void motor_passo(volatile unsigned int p, volatile unsigned int d)
{
    p = p*2
    TACTL |= TACLR;
    TA0CCR0 = 130-1;
    TA0CTL = TASSEL_2 + ID_3 + MC_1;
    if(d==0){
      P1OUT &= ~DIRPASSO;
    }else{
      P1OUT |= DIRPASSO;
    }
    P1OUT &= ~CLKPASSO;
  
    while(p--)
    {
       P1OUT &= ~ENPASSO;
       while((TA0CTL & TAIFG)==0);
       P1OUT ^= CLKPASSO;
       TA0CTL &= ~TAIFG;
    }
    P1OUT &= ~(CLKPASSO);
    P1OUT |= ENPASSO;
    TACTL = MC_0;
}
  
int main(void)
{
  
    P1OUT &= ~(ENPASSO|CLKPASSO|DIRPASSO);
    P1DIR |= ENPASSO|CLKPASSO|DIRPASSO;
    P1DIR &= ~BTN;
    P1REN |= BTN;
    P1OUT |= BTN;
    P1OUT &= ~(CLKPASSO|DIRPASSO);
    P1OUT |= ENPASSO;
  
    while(1){
      
        if((P1IN&BTN)==0){
            motor_passo(1000,1);            //quantidade de passos, direção
            atraso(1000);                   //atraso de 1s
            motor_passo(1000,0);
            atraso(1000);
            //...
        }
    }     
}
