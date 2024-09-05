#include <msp430g2553.h>

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;          // Set DCO to 1MHz
    DCOCTL = CALDCO_1MHZ;
    P1SEL = BIT1 + BIT2;            // Set P1.1 = RXD, P1.2 = TXD
    P1SEL2 = BIT1 + BIT2;
    UCA0CTL1 |= UCSSEL_2;           // Use SMCLK
    UCA0BR0 = 104;                  // Set baud rate to 9600 with 1MHz clock
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;              // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;           // Initialize USCI state machine

    const char message[] = "Hello from MSP430!\r\n";

    while (1) {
        int i = 0;
        for (i = 0; message[i] != '\0'; i++) {
            while (!(IFG2 & UCA0TXIFG)); // Wait for TX buffer to be ready
            UCA0TXBUF = message[i];      // Send character
        }

        __delay_cycles(1000000);  // 1-second delay at 1 MHz clock
    }
}
