#include <msp430g2553.h>

#define BUZZER BIT0
#define MPORTA1 BIT1
#define LDR BIT2
#define BTNINICIO BIT3
#define LEDS BIT4
#define LEDPEDIDO BIT5
#define BOIA BIT6
#define SMOLHO BIT7
#define MPORTA2 BIT0
#define IOCTL BIT1
#define IOSTART BIT2
#define RECEPT1 BIT3
#define RECEPT2 BIT4
#define RECEPT3 BIT5

volatile unsigned int i=0,j=0,k=0;

void config_geral()
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop WDT
    BCSCTL1 = CALBC1_1MHZ;      //MCLK e SMCLK @ 1MHz
    DCOCTL = CALDCO_1MHZ;       //MCLK e SMCLK @ 1MHz
  
    P1OUT &= ~(BUZZER|MPORTA1|LEDS|LEDPEDIDO|SMOLHO);
    P1DIR |= BUZZER|MPORTA1|LEDS|LEDPEDIDO|SMOLHO);
    P1OUT &= ~(BUZZER|MPORTA1|LEDS|LEDPEDIDO|SMOLHO);
    P1DIR &= ~(LDR|BTNINICIO|BOIA);
    P1REN |= LDR|BTNINICIO|BOIA;
    P1OUT |= LDR|BTNINICIO|BOIA;
  
    
  
}
void Atraso(volatile unsigned int t)
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
}
