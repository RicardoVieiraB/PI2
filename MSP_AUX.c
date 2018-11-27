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

volatile unsigned int i=0,j=0,k=0,l=0,e=0;

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
	
	P2OUT &= ~(MPORTA2|RECEPT1|RECEPT2|RECEPT3);
	P2DIR |= MPORTA2|IOSTART;
	P2OUT &= ~MPORTA2;
	P2OUT |= IOSTART;
	P2DIR &= ~(IOCTL|RECEPT1|RECEPT2|RECEPT3);
	P2REN |= IOCTL;
	P2OUT |= IOCTL;
  
}
void Atraso(volatile unsigned int t)
{
   	TACTL |= TACLR;
	TACCR0 = 1000-1;
	TACTL = TASSEL_2 + ID_0 + MC_1;
	while(t--){
		
		while((TACTL&TAIFG)==0);
		TACTL &= ~TAIFG;
	  }
	TACTL = MC_0;
}

void servo_molho()
{
	i=0;
	j=0;
	k=0;
	TACTL = TACLR;
	TA0CCR1 = 304;
    	TA0CCR2 = 2296;
    	TA0CCR0 = TA0CCR2;

    	TA0CTL = TASSEL_2 + ID_3 + MC_1; // clk/8 modo up
	P1OUT |= SMOLHO;
	
	TA0CCTL1 &= ~CCIFG;
	TA0CCTL2 &= ~CCIFG;
	
    	while(k<=3)
    	{
        	while((TA0CCTL1 & CCIFG)==0);
        	P1OUT ^= SMOLHO;
        	TA0CCTL1 &= ~CCIFG;
        	i++;
        	j++;
        	while((TA0CCTL2 & CCIFG)==0);
        	P1OUT ^= SMOLHO;
        	TA0CCTL2 &= ~CCIFG;
        	if(i==100){
        	    TA0CCR1 = 61;
        	    TA0CCR2 = 2539;
        	    TA0CCR0 = TA0CCR2;
        	    k++;
        	}
        	if(j==200){
        	    TA0CCR1 = 304;
        	    TA0CCR2 = 2296;
        	    TA0CCR0 = TA0CCR2;
        	    i = 0;
        	    j = 0;
        	 }
    	}
	P1OUT &= SMOLHO;
	TACTL = MC_0;
}

int main(void)
{

	config_geral();
	
	while(1){
	
		if(((P2IN&IOCTL) == 0) & (e == 0)){
			P1OUT |= BUZZER;
			atraso(2000);
			P1OUT &= ~BUZZER;
			P1OUT |= LEDPEDIDO;
			
			P2DIR |= IOCTL;
			P2DIR |= IOCTL;
			e = 1;
		}
		if(((P1IN&BTNINICIO) == 0) & (e == 1)){
			P2OUT &= ~IOCTL;
			atraso(500);
			P2OUT |= IOCTL;
			
			P2DIR &= ~IOCTL;
			P2REN |= IOCTL;
			P2OUT |= IOCTL;
			
			if(((P1IN&LDR)==0) & ((P1IN&BOIA)==0) & ((P2IN&RECEPT1)==0) & ((P2IN&RECEPT2)==0) & ((P2IN&RECEPT3)==0)){
				P1OUT &= ~LEDS;
				P2OUT |= IOSTART;
			}else{
				P1OUT |= LEDS;
				P1OUT &= ~LEDPEDIDO;
				P2OUT &= ~IOSTART;
				atraso(500);
				P2OUT |= IOSTART;
				
				P2DIR &= ~IOSTART;
				P2REN |= IOSTART;
				P2OUT |= IOSTART;		
			}		
		}
		if(((P2IN&IOSTART) == 0) & (e == 1)){
			servo_molho();
			
			P2DIR |= IOSTART;
			P2OUT |= IOSTART;
		}
		if(((P2IN&IOCTL) == 0) & (e == 1)){
			P1OUT |= MPORTA1;
			P2OUT &= ~MPORTA2;
			atraso(2000);
			P1OUT &= ~MPORTA1;
			P2OUT &= ~MPORTA2;
			atraso(3000);
			P1OUT &= ~MPORTA1;
			P2OUT |= MPORTA2;
			atraso(2000);
			P1OUT &= ~MPORTA1;
			P2OUT &= ~MPORTA2;
	
			if(l=1){
			P1OUT |= BUZZER;
			atraso(500);
			P2OUT &= ~BUZZER;
			atraso(500);
			P1OUT |= BUZZER;
			atraso(500);
			P2OUT &= ~BUZZER;
			atraso(500);
			P1OUT |= BUZZER;
			atraso(500);
			P2OUT &= ~BUZZER;
			atraso(500);
				
			e = 0;
			}
			l++;
		}
	}	
}
	










