/*
Codigo para o MSP430 funcionar como escravo SPI de
acordo com o seguinte protocolo:
    I. Receber o byte 0x55 e enviar o byte 0xAA,
        o que indica o começo de uma conversão AD.
    II. Receber os bytes 0x01 e 0x02, e enviar o
        byte menos significativo e o mais
        significativo da conversão de 10 bits do
        sinal analogico no pino P1.0, nesta ordem.


   P1.1: conexao SPI MISO
   P1.2: conexao SPI MOSI
   P1.4: conexao clock SPI
*/

#include <msp430g2553.h>

#define MISO BIT1
#define MOSI BIT2
#define SCLK BIT4
#define RED BIT0
#define GREEN BIT6

void Send_Data(volatile unsigned char c)
{
    while((IFG2&UCA0TXIFG)==0);
    UCA0TXBUF = c;
    IFG2 &= ~UCA0TXIFG;
}

//int main(void)
int spi(void)
{
    WDTCTL = WDTPW + WDTHOLD;
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;

    P1SEL2 = P1SEL = MOSI+MISO+SCLK;                   //modo secundário de periférico para pinos do SPI
    P1OUT &= ~(RED|GREEN);
    P1DIR |= RED|GREEN;                                //saidas

    UCA0CTL1 = UCSWRST;                                //reseta para configurar
    UCA0CTL0 = UCCKPH + UCMSB + UCMODE_0 + UCSYNC;     //muda dado + MSB primeiro + modo spi 3 pinos + modo sincrono
    UCA0CTL0 &= ~UCMST;                                //slave mode
    UCA0CTL1 &= ~UCSWRST;                              //disable reset
    //Send_Data(0xAA);

    while(1)
    {
        //Send_Data(0xAA);
        while((IFG2&UCA0RXIFG)==0);
        if(UCA0RXBUF==0x55){
            P1OUT ^= GREEN;
            IFG2 &= ~UCA0RXIFG;
            Send_Data(0x66);
        }
        if(UCA0RXBUF==0xCC){
            P1OUT ^= RED;
            IFG2 &= ~UCA0RXIFG;
            Send_Data(0xBB);
            }

        //IFG2 &= ~UCA0RXIFG;
    }
}
