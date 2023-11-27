#include <msp430.h>

// Configurar USCI_A0
void USCI_A0_config(void){
    UCA0CTL1 = UCSWRST; //RST=1 para USCI_A0
    UCA0CTL0 = 0; //sem paridade, 8 bits, 1 stop, modo UART
    UCA0BRW = 3; //Divisor
    UCA0MCTL = UCBRS_3; //Modulador = 3 e UCOS=0
    P3SEL |= BIT4 | BIT3; //Disponibilizar pinos
    UCA0CTL1 = UCSSEL_1; //RST=0, ACLK
}

// ER8.6 USCI_A0 com HC-05 - Brincadeira com Frases
//
// HC-05: VCC = +5 V
// HC-05: GND = GND
// HC-05: RXD = P3.3 (USCI_A0: TXD)
// HC-05: TXD = P3.4 (USCI_A0: RXD)
// USCI_A0 Recebe números e envia frases
// 9.600 bauds (ACLK), 8 bits, sem paridade, 1 stop

#include <msp430.h>
#define TRUE 1
#define FALSE 0
#define TOTAL 5 //Quantidade de frases
char bt_ler(void);
void bt_str(char *vet);
void bt_char(char c);
void USCI_A0_config(void);
char *frase[]={
               "Opcao nao valida.\n", //0
               "O otimista nao sabe o que o espera.\n", //1, Millor
               "De onde menos se espera, dai eh que nao sai nada.\n", //2, B. de Itararé
               "Em terra de saci, todo chute eh uma voadora.\n", //3, Ditado pop.
               "Eu pretendo viver para sempre, ou morrer tentando.\n",//4, Groucho Marx
               "A mao que joga a pedra, eh a mesma que apedreja.\n"}; //5, Falcão
int main(void){
    char opc;
    char msg[]={"Opcoes de 1 ateh X.\n"}; //Mensagem de limite
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    msg[17]=0x30+TOTAL; //Limite de escolhas
    USCI_A0_config();
    while(TRUE){
        bt_str("\n>>> Digite um numero ou '?'.");
        opc=bt_ler();
        bt_str(" ["); bt_char(opc); bt_str("]\n"); //Indicar escolha
        if (opc=='?') bt_str(msg); //Escolha = ?
        else{
            opc = opc-0x30; //Foi número?
            if ( opc>0 && opc<(TOTAL+1)) bt_str(frase[opc]);
            else bt_str(frase[0]);
        }
    }
    return 0;
}
// Receber um caractere serial. Elimina '\n' ou '\r'
// Prende esperando chegar algo
char bt_ler(void){
    char c='\n';
    while (c=='\n' || c=='\r'){ //Consumir '\n' e '\r'
        while ( (UCA0IFG&UCRXIFG)==0); //Esperar RXIFG=1
        c=UCA0RXBUF; //Ler buffer
    }
    return c;
}
// Enviar uma string pela serial
void bt_str(char *vet){
    unsigned int i=0;
    while (vet[i] != '\0')
        bt_char(vet[i++]);
}
// Enviar um caracter pela serial
void bt_char(char c){
    while ( (UCA0IFG&UCTXIFG)==0); //Esperar TXIFG=1
    UCA0TXBUF=c;
}
