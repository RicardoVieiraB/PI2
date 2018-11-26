#include <msp430g2553.h>

#define ENPASSO BIT0
#define CLKPASSO BIT3
#define DIRPASSO BIT5
#define FORNO BIT6
#define BOMBA BIT7
#define SQUEIJO BIT0
#define SPRES BIT1
#define SOREG BIT2 
#define IOSTART BIT3
#define IOCTL BIT4
#define LED38K BIT5

#define MISO BIT1
#define MOSI BIT2
#define SCLK BIT4

volatile unsigned int i=0,j=0,k=0,e=0;

void config_geral()
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop WDT
    BCSCTL1 = CALBC1_1MHZ;      //MCLK e SMCLK @ 1MHz
    DCOCTL = CALDCO_1MHZ;       //MCLK e SMCLK @ 1MHz
  
    P1OUT &= ~(ENPASSO|CLKPASSO|DIRPASSO|FORNO|BOMBA);
    P1DIR |= ENPASSO|CLKPASSO|DIRPASSO|FORNO|BOMBA;
    P1OUT &= ~(CLKPASSO|DIRPASSO|BOMBA);
    P1OUT |= ENPASSO|FORNO;
  
    P1SEL2 = P1SEL = MOSI+MISO+SCLK;
  
    P2OUT &= ~(SQUEIJO|SPRES|SOREG|LED38K);
    P2DIR |= SQUEIJO|SPRES|SOREG|LED38K|IOCTL;
    P2DIR &= ~IOSTART;
    P2OUT |= IOCTL;
    P2OUT &= ~(SQUEIJO|SPRES|SOREG|LED38K);
  
    UCA0CTL1 = UCSWRST;                                //reseta para configurar
    UCA0CTL0 = UCCKPH + UCMSB + UCMODE_0 + UCSYNC;     //muda dado + MSB primeiro + modo spi 3 pinos + modo sincrono
    UCA0CTL0 &= ~UCMST;                                //slave mode
    UCA0CTL1 &= ~UCSWRST;                              //disable reset
   
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

void Send_Data(volatile unsigned int c)
{ 
    IFG2 &= ~UCA0TXIFG;
    while((IFG2&UCA0TXIFG)==0);
    UCA0TXBUF = c;
    IFG2 &= ~UCA0TXIFG;
}

void servo_queijo()
{
    
    i=0;
    j=0;
    TACTL |= TACLR;
    TA0CCR1 = 210;              //contagens q o PWM fica em HIGH          
    TA0CCR2 = 2390;             //contagens q o PWM fica em LOW
    TA0CCR0 = TA0CCR2;          //serve para saber quando timer zerou

    TA0CTL = TASSEL_2 + ID_3 + MC_1; // clk/8 modo up
    TA0CCTL1 &= ~CCIFG;
    TA0CCTL2 &= ~CCIFG;
    
    P2OUT |= SQUEIJO;
  
    while(1)
    { //gerador de PWM
        while((TA0CCTL1 & CCIFG)==0); //confere primeiro register e inverte flag
        P2OUT ^= SQUEIJO;
        TA0CCTL1 &= ~CCIFG;
        i++;  //variaveis de controle para inverter sentido
        j++;
        while((TA0CCTL2 & CCIFG)==0); //confere segundo register e inverte flag
        P2OUT ^= SQUEIJO;
        TA0CCTL2 &= ~CCIFG;
        if(i==100){   //valores para um sentido
            TA0CCR1 = 100;   //parado entre 180 e 195
            TA0CCR2 = 2500;
            TA0CCR0 = TA0CCR2;
        }
        if(j==200){   //valores para outro sentido
            TA0CCR1 = 210;
            TA0CCR2 = 2390;
            TA0CCR0 = TA0CCR2;
            i = 0;
            j = 0;
         }
    }
    P2OUT &= ~SQUEIJO;
    TACTL = MC_0;
}

void servo_presunto()
{    
    i=0;
    j=0;
    TACTL |= TACLR;
    TA0CCR1 = 210;              //contagens q o PWM fica em HIGH          
    TA0CCR2 = 2390;             //contagens q o PWM fica em LOW
    TA0CCR0 = TA0CCR2;          //serve para saber quando timer zerou

    TA0CTL = TASSEL_2 + ID_3 + MC_1; // clk/8 modo up
    TA0CCTL1 &= ~CCIFG;
    TA0CCTL2 &= ~CCIFG;
    
    P2OUT |= SPRES;
  
    while(1)
    { //gerador de PWM
        while((TA0CCTL1 & CCIFG)==0); //confere primeiro register e inverte flag
        P2OUT ^= SPRES;
        TA0CCTL1 &= ~CCIFG;
        i++;  //variaveis de controle para inverter sentido
        j++;
        while((TA0CCTL2 & CCIFG)==0); //confere segundo register e inverte flag
        P2OUT ^= SPRES;
        TA0CCTL2 &= ~CCIFG;
        if(i==100){   //valores para um sentido
            TA0CCR1 = 100;   //parado entre 180 e 195
            TA0CCR2 = 2500;
            TA0CCR0 = TA0CCR2;
        }
        if(j==200){   //valores para outro sentido
            TA0CCR1 = 210;
            TA0CCR2 = 2390;
            TA0CCR0 = TA0CCR2;
            i = 0;
            j = 0;
         }
    }
    P2OUT &= ~SPRES;
    TACTL = MC_0;
}

void servo_oregano()
{
    i=0;
    j=0;
    TACTL |= TACLR;
    TA0CCR1 = 290;              //contagens q PWM fica em HIGH
    TA0CCR2 = 2310;             //contagens q PWM fica em LOW
    TA0CCR0 = TA0CCR2;          //p/ zerar timer

    TA0CTL = TASSEL_2 + ID_3 + MC_1; // clk/8 modo up
    TA0CCTL1 &= ~CCIFG;
    TA0CCTL2 &= ~CCIFG;
  
    P2OUT |= SOREG;

    while(1)
    {
        while((TA0CCTL1 & CCIFG)==0);
        P2OUT ^= SOREG;
        TA0CCTL1 &= ~CCIFG;
        i++;
        j++;
        while((TA0CCTL2 & CCIFG)==0);
        P2OUT ^= SOREG;
        TA0CCTL2 &= ~CCIFG;
        if(i==100){
            TA0CCR1 = 80;
            TA0CCR2 = 2520;
            TA0CCR0 = TA0CCR2;
        }
        if(j==200){
            TA0CCR1 = 290;
            TA0CCR2 = 2310;
            TA0CCR0 = TA0CCR2;
            i = 0;
            j = 0;
         }
    }
    P2OUT &= ~SOREG;
    TACTL = MC_0;
}

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
    P1OUT &= ~(CLKPASSO|ENPASSO);
  
    while(p--)
    {
       while((TA0CTL & TAIFG)==0);
       P2OUT ^= CLKPASSO;
       TA0CTL &= ~TAIFG;
    }
    P1OUT &= ~(CLKPASSO);
    P1OUT |= ENPASSO;
    TACTL = MC_0;
}
void molho()
{
    P1OUT |= BOMBA;
    P2OUT &= ~IOSTART;
    atraso(1000);
    P2OUT |= IOSTART;
    atraso(5000);
    P1OUT &= ~BOMBA;
  
    P2DIR &= ~IOSTART;
}
void detectar_solido()
{
    i=0;
    TACTL |= TACLR;
    TA0CCR0 = 13-1;
    TA0CTL = TASSEL_2 + ID_0 + MC_1;

    while(i<60000)
    {
        while((TA0CTL & TAIFG)==0);
        P2OUT ^= LED38K;
        TA0CTL &= ~TAIFG;
        i++;
    }
    P2OUT &= ~LED38K;
    TACTL = MC_0;
}

int main(void)
{
  
  config_geral();
  
  while(1){
    
    	while((IFG2&UCA0RXIFG)==0);
    	IFG2 &= ~UCA0RXIFG;
    	if(UCA0RXBUF==0x01){
    		P2OUT &= ~IOCTL;
    		atraso(1000);
     		P2OUT |= IOCTL;
        
      		P2DIR &= ~IOCTL;
      		Send_Data(0xAB);
      		e = 1;
	}
    	while(e==1){
	     	if((P2IN&IOCTL)==0){
			detectar_solido();
			
		}
		if((P2IN&IOSTART)==0){
			P2DIR |= IOSTART;
        		//...
			Send_Data(0xBA);
			e = 0;
		}
    	}
  }
  
}
