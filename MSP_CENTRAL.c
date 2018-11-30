#include <msp430g2553.h>

#define ENPASSO BIT0        //P1.0        enable driver do motor de passo
#define CLKPASSO BIT3       //P1.3        clk do motor de passo
#define DIRPASSO BIT5       //P2.5        direção do motor de passo
#define FORNO BIT6          //P2.6        ativação do forno
#define BOMBA BIT7          //P2.7        ativação da bomba
#define SQUEIJO BIT0               //P2.0        servo dispersor de queijo
#define SPRES BIT1                 //P2.1        servo dispersor de presunto
#define SOREG BIT2                 //P2.2        servo dispersor de oregano
#define IOSTART BIT3               //P2.3        pino entrada(start da maquina)/saida(ativação servo molho)
#define IOCTL BIT4                 //P2.4        pino entrada(ativação LED IR)/saida(ativação servo porta)
#define LED38K BIT5                //P2.4        pino saida para LED IR

#define MISO BIT1           //P1.1        master input/slave output SPI
#define MOSI BIT2           //P1.2        master output/slave input SPI
#define SCLK BIT4         //P1.4        master CLK SPI

volatile unsigned int i=0,j=0,k=0,e=0;

void config_geral()                                       //configura fonte de clk, pinos e comunicação SPI
{

    WDTCTL = WDTPW + WDTHOLD;                             //desabilita WDT
    BCSCTL1 = CALBC1_1MHZ;                                //MCLK e SMCLK de 1MHz
    DCOCTL = CALDCO_1MHZ;                                 //MCLK e SMCLK de 1MHz

    P1OUT &= ~(ENPASSO|CLKPASSO|DIRPASSO|FORNO|BOMBA);
    P1DIR |= ENPASSO|CLKPASSO|DIRPASSO|FORNO|BOMBA;       //portas P1.x como saídas
    P1OUT &= ~(CLKPASSO|DIRPASSO|BOMBA);                  //iniciando portas em nivel logico baixa
    P1OUT |= FORNO|ENPASSO;                               //iniciando portas em nivel logico alto

    P2OUT &= ~(SQUEIJO|SPRES|SOREG|LED38K|IOCTL|IOSTART);
    P2DIR |= SQUEIJO|SPRES|SOREG|LED38K|IOCTL;            //portas P2.x como saídas
    P2OUT &= ~(SQUEIJO|SPRES|SOREG|LED38K);               //inciam com nivel logico baixo
    P2OUT |= IOCTL;                                       //inicia P2.4 em nivel logico alto

    P2DIR &= ~IOSTART                                      //porta P2.3 primeiramente como entrada
    P2REN |= IOSTART;                                      //habilita resistor PU/PD
    P2OUT |= IOSTART;                                      //pull-up

    P1SEL2 = P1SEL = MOSI+MISO+SCLK;                      //modo secundário de periférico para pinos do SPI

    UCA0CTL1 = UCSWRST;                                   //reseta para configurar
    UCA0CTL0 = UCCKPH + UCMSB + UCMODE_0 + UCSYNC;        //muda dado + MSB primeiro + modo spi 3 pinos + modo sincrono
    UCA0CTL0 &= ~UCMST;                                   //slave mode
    UCA0CTL1 &= ~UCSWRST;                                 //disable reset

}

void send_data(volatile unsigned char c)
{
    IFG2 &= ~UCA0TXIFG;
    while((IFG2&UCA0TXIFG)==0);      //aguarda preenchimento do buffer
    UCA0TXBUF = c;                   //guarda variável no buffer
    IFG2 &= ~UCA0TXIFG;              //zera flag para nova interação
}

void atraso(volatile unsigned int t) //gera um atraso de t*1000us
{

    TA0CTL |= TACLR;                 //limpa configs do timer
    TA0CCR0 = 1000-1;                //referencia de contagem
    TA0CTL = TASSEL_2 + ID_0 + MC_1; // clk/1 modo up

    while(t--)                       //a cada volta no while decrementa t
    {
        while((TA0CTL & TAIFG) == 0);//para o codigo até atingir valor de TA0CCR0
        TA0CTL &= ~TAIFG;            //zera flag para nova interação
    }
    TA0CTL = MC_0;                   //desabilita timer ao término do atraso
}

void servo_queijo()                  //controla giro do servo do queijo
{

    i=0; j=0; k=0;                   //reinicia variaveis de controle
    TA0CTL |= TACLR;                 //reinicia configs do timer
    TA0CCR1 = 210;                   //parado entre 180 e 195
    TA0CCR2 = 2390;
    TA0CCR0 = TA0CCR2;
    TA0CTL = TASSEL_2 + ID_3 + MC_1; // clk/8 modo up
    P2OUT |= SQUEIJO;                //começa em nivel alto (PWM)

    TA0CCTL1 &= ~CCIFG;
    TA0CCTL2 &= ~CCIFG;

    while(k<=2)
    {
        while((TA0CCTL1 & CCIFG)==0); //gerador de PWM
        P2OUT ^= SQUEIJO;
        TA0CCTL1 &= ~CCIFG;
        i++;
        j++;
        while((TA0CCTL2 & CCIFG)==0);
        P2OUT ^= SQUEIJO;
        TA0CCTL2 &= ~CCIFG;
        if(i==50){                   //inverte sentido de giro
            TA0CCR1 = 150;           //duty de 6%
            TA0CCR2 = 2450;          //faz girar em sentido horario
            TA0CCR0 = TA0CCR2;
        }
        if(j==100){                  //inverte sentido de giro
            TA0CCR1 = 210;           //duty de 8%
            TA0CCR2 = 2390;          //faz girar em sentido anti-horario
            TA0CCR0 = TA0CCR2;
            i = 0;
            j = 0;
            k++;
         }
    }
    P2OUT &= ~SQUEIJO;               //mantem pino PWM em nivel logico baixo quando fora do funcionamento
    TA0CTL = MC_0;                   //desabilita timer

}

void servo_presunto()                //controla giro do servo do presunto
{

    i=0; j=0; k=0;                   //reinicia variaveis de controle
    TA0CTL |= TACLR;                 //reinicia configs do timer
    TA0CCR1 = 210;                   //parado entre 180 e 195
    TA0CCR2 = 2390;
    TA0CCR0 = TA0CCR2;
    TA0CTL = TASSEL_2 + ID_3 + MC_1; //clk/8 modo up
    P2OUT |= SPRES;                  //inicia em nivel logico alto (PWM)

    TA0CCTL1 &= ~CCIFG;
    TA0CCTL2 &= ~CCIFG;

    while(k<=2)
    {
        while((TA0CCTL1 & CCIFG)==0); //gerador de PWM
        P2OUT ^= SPRES;
        TA0CCTL1 &= ~CCIFG;
        i++;
        j++;
        while((TA0CCTL2 & CCIFG)==0);
        P2OUT ^= SPRES;
        TA0CCTL2 &= ~CCIFG;
        if(i==50){                   //inverte sentido de giro
            TA0CCR1 = 150;           //duty 6%
            TA0CCR2 = 2450;          //sentido horario
            TA0CCR0 = TA0CCR2;
        }
        if(j==100){                  //inverte sentido de giro
            TA0CCR1 = 210;           //duty 8%
            TA0CCR2 = 2390;          //sentido anti-horario
            TA0CCR0 = TA0CCR2;
            i = 0;
            j = 0;
            k++;
         }
    }
    P2OUT &= ~SPRES;                 //mantem pino PWM em nivel logico baixo quando fora do funcionamento
    TA0CTL = MC_0;                   //desabilita timer
}

void motor_passo(volatile unsigned int p, volatile unsigned int d) //controla quantidade de passos e sentido do motor de passo
{

    p = p*2;                         //multiplica passos*2, pois o while gera um periodo de clk a cada 2 loops
    TA0CTL |= TACLR;                 //reinicia configs do timer
    TA0CCR0 = 140-1;                 //define quantos passos por segundo o motor dará
    TA0CTL = TASSEL_2 + ID_3 + MC_1; //clk/8 modo up
    P1OUT &= ~CLKPASSO;              //inicia clk em 0
    P1OUT &= ~ENPASSO;               //define enable em 0, DRIVER ATIVADO

    if(d==1){                        //de acordo com d, define a direção de giro do motor
        P1OUT |= DIRPASSO;
    }else{
        P1OUT &= ~DIRPASSO;
    }

    while(p--)                       //p/2 = quantidade de passos
    {
        while((TA0CTL & TAIFG)==0);  //gera meio periodo do clk
        P1OUT ^= CLKPASSO;
        TA0CTL &= ~TAIFG;
    }
    P1OUT &= ~CLKPASSO;              //finaliza clk em 0
    //P1OUT &= ~DIRPASSO;              //finaliza pino de direção em 0
    P1OUT |= ENPASSO;                //finaliza pino enable em 1, DRIVER DESATIVADO
    TA0CTL = MC_0;                   //desabilita timer
}

void servo_oregano()                 //controla giro servo do oregano
{

    i=0;j=0;k=0;                     //reinicia variaveis de controle
    TA0CTL |= TACLR;                 //reinicia configs do timer
    TA0CCR1 = 280;                   //inicia com duty de 12% p/ posição 180°
    TA0CCR2 = 2320;
    TA0CCR0 = TA0CCR2;
    TA0CTL = TASSEL_2 + ID_3 + MC_1; //clk/8 modo up
    P2OUT |= SOREG;                  //inicia sinal PWM em nivel logico alto

    TA0CCTL1 &= ~CCIFG;
    TA0CCTL2 &= ~CCIFG;

    while(k<=2)
    {
        while((TA0CCTL1 & CCIFG)==0); //gerador de PWM
        P2OUT ^= SOREG;
        TA0CCTL1 &= ~CCIFG;
        i++;
        j++;
        while((TA0CCTL2 & CCIFG)==0);
        P2OUT ^= SOREG;
        TA0CCTL2 &= ~CCIFG;
        if(i==50){                    //valores para 0°
            TA0CCR1 = 90;             //duty de 2,5%
            TA0CCR2 = 2510;
            TA0CCR0 = TA0CCR2;
        }
        if(j==100){                   //valores para 180°
            TA0CCR1 = 280;            //duty de 12%
            TA0CCR2 = 2320;
            TA0CCR0 = TA0CCR2;
            i = 0;
            j = 0;
            k++;
         }
     }
    P2OUT &= ~SOREG;                  //mantem pino PWM em nivel logico baixo quando fora do funcionamento
    TA0CTL = MC_0;                    //desabilita timer
}

void molho()                          //função que ativa bomba de molho e envia nivel logico para controlar servo
{
    P1OUT |= BOMBA;                   //liga bomba
    atraso(100);

    P2OUT &= ~IOSTART;                //nivel logico baixo para MSP_AUX ativar PWM do servo do molho
    atraso(1000);
    P2OUT |= IOSTART;
    atraso(100);

    P2DIR &= ~IOSTART;                //pino P2.3 novamente como entrada para outra função
    P2REN |= IOSTART;                 //habilita res PU/PD
    P2OUT |= IOSTART;                //pull-up

    atraso(6000);
    P1OUT &= ~BOMBA;                  //desliga bomba
    atraso(100);

}

void detectar_solido()                //gera pulsos com frequência de 38 kHz para LED IR
{

    i = 0;                            //reinicia variavel de controle
    TA0CTL |= TACLR;                  //reinicia configs do timer
    TA0CCR0 = 13-1;                   //valor para freq de 38kHz
    TA0CTL = TASSEL_2 + ID_0 + MC_1;  //clk/1 modo up
    P2OUT &= ~LED38K;                 //inicia pino em 0

    while(i<60000)                    //define tempo q pulso ficara operando
    {
        while((TA0CTL & TAIFG)==0);   //gera meio periodo da freq
        P2OUT ^= LED38K;
        TA0CTL &= ~TAIFG;
        i++;
    }
    P2OUT &= ~LED38K;                 //garante que pino ficará inativo após a função
    TA0CTL = MC_0;                    //desabilita timer
}

int main(void)
{

    config_geral();                   //configurações gerais necessárias do projeto

    while(1){

        while((IFG2&UCA0RXIFG)==0);   //aguarda comunicação SPI
        IFG2 &= ~UCA0RXIFG;           //zera flag para proxima interação
            if((UCA0RXBUF==0x01) | (UCA0RXBUF==0x02) | (UCA0RXBUF==0x03) | (UCA0RXBUF==0x04) | (UCA0RXBUF==0x05) | (UCA0RXBUF==0x06)){
                //confere o que receu no buffer
                P2OUT &= ~IOCTL;      //pino P2.4 em nivel logico alto para outro MSP
                atraso(1000);
                P2OUT |= IOCTL;

                atraso(100);
                P2DIR &= ~IOCTL;      //pino P2.4 como entrada para novas funções
                P2REN |= IOCTL;       //habilita resistor PU/PD
                P2OUT |= IOCTL;       //pull-up
                atraso(100);
                e = 1;                //flag de controle para entrar na parte de automação
                send_data(0x00);      //carrega buffer com variável
            }

        while(e==1){                        //while para a automação

            //0x01 mussarela
            //0x02 presunto
            //0x03 mussarela oregano
            //0x04 presunto oregano
            //0x05 mussarela presunto
            //0x06 completa

            if((P2IN&IOCTL)==0){          //entrada para ativação do LED IR a 38 kHz

                detectar_solido();        //gera pulsos por um periodo de tempo

            }

            if((P2IN&IOSTART)==0){        //entrada para iniciar a sequencia de automação do projeto

                atraso(1100);
                P2DIR |= IOCTL|IOSTART;
                P2DIR |= IOCTL|IOSTART;
                atraso(100);

                P1OUT &= ~FORNO;           //nivel logico baixo para ativação do forno em modo assar com MSP_FORNO
                motor_passo(12000,1);      //numero de passos, direção
                atraso(500);              //atraso em ms
                molho();                  //ativa bomba e servo do molho
                atraso(500);
                motor_passo(12000,1);
                atraso(500);
                servo_queijo();           //ativa servo do queijo
                atraso(500);
                motor_passo(15000,1);
                atraso(500);
                servo_presunto();         //ativa servo do presunto
                atraso(500);
                motor_passo(14000,1);
                atraso(500);
                servo_oregano();          //ativa servo do oregano
                atraso(500);

                P2OUT &= ~IOCTL;          //nivel logico baixo em P2.4 para MSP_AUX controlar servo da porta (abrir e fechar)
                atraso(1000);
                P2OUT |= IOCTL;           //nivel logico alto para aguardar nova interação

                motor_passo(30000,1);
                motor_passo(7000,1);
                atraso(500);

                atraso(10000);
                //atraso(10000);          //atrasos para aguardar pizza assar
                //atraso(10000);
                //atraso(10000);
                //atraso(10000);
                //...
                P1OUT |= FORNO;          //desliga modo assar do forno
                atraso(1000);

                P2OUT &= ~IOCTL;           //nivel logico baixo em P2.4 para MSP_AUX controlar servo da porta (abrir e fechar)
                atraso(1000);
                P2OUT |= IOCTL;
                atraso(22000);

                motor_passo(30000,0);

                atraso(10000);
                motor_passo(30000,0);
                motor_passo(30000,0);

                atraso(500);
                send_data(0xFF);
                atraso(2000);
                e = 0;
            }
        }
    }
}
