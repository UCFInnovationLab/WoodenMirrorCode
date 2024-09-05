#include <msp430.h>

// Function to initialize UART
void uart_init(void) {
    // Disable the Watchdog Timer
    WDTCTL = WDTPW + WDTHOLD;

    // Configure GPIO for UART
    P1SEL |= BIT1 + BIT2; // Set P1.1 and P1.2 to UART mode
    P1SEL2 |= BIT1 + BIT2; // Set P1.1 and P1.2 to UART mode

    // Set the UART Baud Rate to 9600 with a 1MHz clock
    UCA0CTL1 |= UCSWRST; // Put UART in reset
    UCA0CTL1 = UCSSEL_2; // Use SMCLK (1 MHz)
    UCA0BR0 = 104;       // 9600 Baud Rate, 1 MHz SMCLK
    UCA0BR1 = 0;         // 9600 Baud Rate, 1 MHz SMCLK
    //UCA0MCTL = UCBRS_1 + UCBRF_0; // Modulation
    UCA0MCTL = UCBRS_0 + UCOS16 + UCBRF_3;    // Modulation UCBRSx = 0
    UCA0CTL1 &= ~UCSWRST; // Release UART from reset

    // Enable UART transmitter
    UCA0CTL1 |= UCSSEL_2; // Use SMCLK
    UCA0CTL1 &= ~UCSWRST; // Initialize UART state machine
    IE2 |= UCA0TXIE;      // Enable UART TX interrupt
}

void UART_SendChar(char c) {
    while (!(IFG2 & UCA0TXIFG)); // Wait for TX buffer to be ready for new data
    UCA0TXBUF = c;               // Send the character
}

char UART_ReceiveChar(void) {
    while (!(IFG2 & UCA0RXIFG)); // Wait for a character to be received
    return UCA0RXBUF;            // Return the received character
}

// Function to transmit a single character via UART
//void uart_transmit(char c) {
//    while (!(IFG2 & UCA0TXIFG)); // Wait for the TX buffer to be ready
//    UCA0TXBUF = c;               // Send the character
//}
//
//char uart_receive(void){
//    while (!(IFG2 & UCA0RXIFG)); // Wait for a character to be received
//        return UCA0RXBUF;            // Return the received character
//}

void initClockTo16MHz()
{
    if (CALBC1_16MHZ==0xFF)                  // If calibration constant erased
    {
        while(1);                            // do not load, trap CPU!!
    }
    DCOCTL = 0;                              // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;                  // Set DCO
    DCOCTL = CALDCO_16MHZ;
}

volatile char receiveChar;


int main(void) {
    initClockTo16MHz();
    uart_init(); // Initialize UART


    __bis_SR_register(GIE);

    while (1) {
//             receiveChar = UART_ReceiveChar(); // Wait for a character
//
//            UART_SendChar(receiveChar);            // Echo the received character
        }




}



// Interrupt Service Routine for RX
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
    receiveChar = UCA0RXBUF;
    UART_SendChar(receiveChar);
//    __bic_SR_register_on_exit(CPUOFF); // Wake up to handle received character
}

//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void USCI0RX_ISR(void) {
//    if (UCA0STAT & UCOE) {
//        // Overrun error occurred
//        UCA0STAT &= ~UCOE; // Clear overrun error flag
//        // You can choose to handle the error, e.g., reset buffer, log error, etc.
//    }
//
//    // Read received character to clear other flags
//    receiveChar = UCA0RXBUF;
//
//    // Proceed with normal character processing
//    UART_SendChar(receiveChar); // Echo the character back
//}
