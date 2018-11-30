#include <msp430g2452.h>

#define BUZZER BIT0        //P1.0        sinal sonoro para indicar pedido pendente ou termino do processo
#define MPORTA1 BIT1        //P1.1        servo porta do forno
#define LDR BIT2           //P1.2        sensor de luminosidade LDR
#define BTN BIT3           //P1.3        BOTAO INICIAR
#define LEDS BIT4        //P1.4        led para indicar falta de ingrediente ou massa
#define LEDESPERA BIT5      //P1.5        led que indica funcionamento da máquina
#define BOIA BIT6          //P1.6        sensor de nível para compartimento do molho
#define SMOLHO BIT7        //P1.7        servo para girar bomba de molho
#define MPORTA2 BIT0              //P2.0
#define IOCTL BIT1                //P2.1        pino saida(ativação LED IR)/entrada(ativação servo porta)
#define IOSTART BIT2              //P2.2        pino saida(start da maquina)/entrada(ativação servo molho)
#define RECEPT1 BIT3              //P2.3        receptor IR para compartimento de queijo
#define RECEPT2 BIT4              //P2.4        receptor IR para compartimento de presunto
#define RECEPT3 BIT5              //P2.5        receptor IR para compartimento de oregano

volatile unsigned int i=0,j=0,k=0,c=0,d=0;

void config_geral()                                       //configura fonte de clk e pinos
{

    WDTCTL = WDTPW + WDTHOLD;                             //para WDT
    BCSCTL1 = CALBC1_1MHZ;                                //MCLK e SMCLK de 1MHz
    DCOCTL = CALDCO_1MHZ;                                 //MCLK e SMCLK de 1MHz

    P1OUT &= ~(BOIA|BTN|LDR|MPORTA1|LEDS|LEDESPERA|SMOLHO|BUZZER);
    P1DIR |= MPORTA1|LEDS|LEDESPERA|SMOLHO|BUZZER;               //portas P1.x como saidas
    P1OUT &= ~(MPORTA1|LEDS|LEDESPERA|SMOLHO|BUZZER;
    P1DIR &= ~(LDR|BTN|BOIA);                             //portas P1.x como entrada
    P1REN |= (BTN|BOIA);                                  //habilita resistor PU/PD
    P1REN |= LDR;
    P1OUT |= BTN|BOIA|LDR;                                    //pull-up
    //P1OUT &= ~(LDR);                                      //pull-down

    P2OUT &= ~(MPORTA2|IOCTL|IOSTART|RECEPT1|RECEPT2|RECEPT3);
    P2DIR |= MPORTA2;
    P2OUT &= ~MPORTA2;
    P2DIR &= ~(RECEPT1|RECEPT2|RECEPT3);                  //portas P2.x como entradas
    P2DIR |= IOSTART;
    P2OUT |= IOSTART;
    P2DIR &= ~IOCTL;
    P2REN |= IOCTL;                               //habilita resistor PU/PD
    P2OUT |= IOCTL;                               //pull-up
    P2REN |= RECEPT1|RECEPT2|RECEPT3;                     //habilita resistor PU/PD
    P2OUT |= RECEPT1|RECEPT2|RECEPT3;                     //pull-up

}

void atraso(volatile unsigned int t) //gera um atraso de t*1000us
{

    TA0CTL |= TACLR;
    TA0CCR0 = 1000-1;
    TA0CTL = TASSEL_2 + ID_0 + MC_1; //clk/1 modo up

    while(t--)
    {
        while((TA0CTL & TAIFG) == 0);
        TA0CTL &= ~TAIFG;
    }
    TA0CTL = MC_0;                    //desabilita timer
}
void servo_molho()                   //controla servo da bomba de molho
{

    i=0;j=0;k=0;                     //reinicia variaveis de controle
    TA0CTL |= TACLR;                 //reinicia configs do timer
    TA0CCR1 = 304;                   //inicia com duty de 12% p/ posição 180°
    TA0CCR2 = 2296;
    TA0CCR0 = TA0CCR2;
    TA0CTL = TASSEL_2 + ID_3 + MC_1; //clk/8 modo up
    P1OUT |= SMOLHO;                 //inicia sinal PWM em nivel logico alto

    TA0CCTL1 &= ~CCIFG;
    TA0CCTL2 &= ~CCIFG;

    while(k<=2)
    {
        while((TA0CCTL1 & CCIFG)==0); //gerador de PWM
        P1OUT ^= SMOLHO;
        TA0CCTL1 &= ~CCIFG;
        i++;
        j++;
        while((TA0CCTL2 & CCIFG)==0);
        P1OUT ^= SMOLHO;
        TA0CCTL2 &= ~CCIFG;
        if(i==150){                    //valores para 0°
            TA0CCR1 = 61;             //duty de 2,5%
            TA0CCR2 = 2539;
            TA0CCR0 = TA0CCR2;
        }
        if(j==300){                   //valores para 180°
            TA0CCR1 = 304;            //duty de 12%
            TA0CCR2 = 2296;
            TA0CCR0 = TA0CCR2;
            i = 0;
            j = 0;
            k++;
         }
     }
    TA0CTL = MC_0;                    //desabilita timer
    P1OUT &= ~SMOLHO;                 //mantem pino PWM em nivel logico baixo quando fora do funcionamento
}

int main(void)
{

    config_geral();                   //configurações gerais necessarias para o projeto

    while(1)
    {
        if(((P2IN & IOCTL)==0) & (c == 0)){
            P1OUT |= BUZZER;
            atraso(1100);
            P2DIR |= IOCTL;
            atraso(100);
            P2OUT |= IOCTL;

            //P1OUT |= LEDGREEN;
            atraso(2000);
            P1OUT |= LEDESPERA;
            P1OUT &= ~BUZZER;
            c = 1;

        }
        if(((P1IN&BTN) == 0) & (c == 1)){           //aguarda botão de inicio ser apertado
            P2OUT &= ~IOCTL;           //ativa pino de comando para LEDs IR
            atraso(500);
            P2OUT |= IOCTL;

            if(((P2IN&RECEPT1)==0) | ((P2IN&RECEPT2)==0) | ((P2IN&RECEPT3)==0) | ((P1IN&BOIA)==0) | ((P1IN&LDR)==0)){ //confere se algum sensor acusa
                P1OUT |= LEDS;      //indica falta de ingrediente com led vermelho
                //P2OUT |= IOSTART;    //sem comando para inicio da operação
                //P2OUT |= IOCTL;
            }else{                    //se sensores nao acusarem
                P1OUT &= ~LEDS;
                P1OUT &= ~LEDESPERA;
                P2OUT &= ~IOSTART;     //comando para que se inicie o funcionamento da maquina
                atraso(1000);
                P2OUT |= IOSTART;
                atraso(100);

                P2DIR &= ~(IOCTL|IOSTART); //configura pinos como entrada para comandos
                P2REN |= IOCTL|IOSTART;    //habilita resistor PU/PD
                P2OUT |= (IOCTL|IOSTART);
            }
        }
        if(((P2IN & IOSTART) == 0) & (c==1)){           //aguarda comando para ativação de função

            atraso(1100);
            P2DIR |= IOSTART;
            P2OUT |= IOSTART;

            servo_molho();                 //ativa servo da bomba do molho

        }
        if(((P2IN & IOCTL) == 0) & (c==1)){             //aguarda comando para ativação de função
                atraso(20000);
                P1OUT |= MPORTA1;
                P2OUT &~ ~MPORTA2;
                atraso(1500);
                P1OUT &= ~MPORTA1;
                P2OUT &= ~MPORTA2;
                atraso(45000);
                P1OUT &= ~MPORTA1;
                P2OUT |= MPORTA2;
                atraso(1500);
                P1OUT &= ~MPORTA1;
                P2OUT &= ~MPORTA2;

                d++;

            if(d==2){
                P1OUT |= BUZZER;
                atraso(500);
                P1OUT &= ~BUZZER;
                atraso(500);
                P1OUT |= BUZZER;
                atraso(500);
                P1OUT &= ~BUZZER;
                atraso(500);
                P1OUT |= BUZZER;
                atraso(500);
                P1OUT &= ~BUZZER;
                atraso(500);
                P1OUT |= BUZZER;
                atraso(500);
                P1OUT &= ~BUZZER;
                c = 0;
                d = 0;
            }
        }
    }
}
