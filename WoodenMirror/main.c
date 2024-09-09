////******************************************************************************
//// UART Initialization *********************************************************
////******************************************************************************



#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "ws2812.h"

//******************************************************************************
// UART Initialization *********************************************************
//******************************************************************************

#define SMCLK_115200     0
#define SMCLK_9600      1
#define ACLK_9600       2

#define UART_MODE       SMCLK_115200

#define RESET_TIMEOUT 32768     // Timeout value for resetting (1 second at 32kHz ACLK)


volatile uint8_t reset = 1;
volatile uint8_t stored_byte = 0; // Store the received byte
volatile uint8_t i = 0;

void initUART()
{
#if UART_MODE == SMCLK_115200
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 138;                            // 16MHz 115200 138.888
    UCA0BR1 = 0;                              // 16MHz 115200
    UCA0MCTL = UCBRS_7;                       // Modulation UCBRSx = 7
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == SMCLK_9600
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 16MHz 9600 104.1
    UCA0BR1 = 0;                              // 16MHz 9600
    UCA0MCTL = UCBRS_0 + UCOS16 + UCBRF_3;    // Modulation UCBRSx = 0
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == ACLK_9600
    UCA0CTL1 |= UCSSEL_1;                     // ACLK
    UCA0BR0 = 3;                              // 32768Hz 9600 3.41
    UCA0BR1 = 0;                              // 32768Hz 9600
    UCA0MCTL = UCBRS_3;                       // Modulation UCBRSx = 3
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#else
    #error "Select UART Baud Rate of 115200 or 9600"
#endif
}

//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************

void initClock()
{
    if (CALBC1_16MHZ == 0xFF)                 // If calibration constant erased
    {
        while(1);                             // do not load, trap CPU!!
    }
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;                   // Set DCO
    DCOCTL = CALDCO_16MHZ;

//    BCSCTL2 = DIVS_1;  // SMCLK = DCO / 2 = 8MHz

}

void initGPIO()
{
    P1SEL = BIT1 + BIT2;                      // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2;

    P1SEL &= ~(BIT0 + BIT6);                  // Clear selection for P1.0 and P1.6
    P1DIR |= BIT0 + BIT6;                    // Set P1.0 and P1.6 as outputs (LEDs)
    P1OUT &= ~(BIT0 + BIT6);                 // Initialize LEDs to be off

    P2DIR |= BIT0;              // Set P2.0 as output
    P2OUT &= ~BIT0;             // Set P2.0 to low (0V) to turn on the LED
}


void initServo()
{
    TA0CCTL1  = OUTMOD_7;          // Reset/set
    TA0CCR0   = 65535;             // Period 20000, timer period
    TA0CCR1   = 4000;              // Duty-cycle 2000 (2 ms)
    TA0CTL    = (TASSEL_2 | MC_1 | ID_2); // SMCLK, timer in up-mode, divide by 4

    TA0CCTL0 = CCIE;               // timer interrupt

    P1DIR    |=  0x40;             // Set P1.6 to output-direction
    P1SEL    |=  0x40;             // Set selection register 1 for timer-function
    P1SEL2   &= ~0x40;             // Clear selection register 2 for timer-function (not needed, as it is 0 after startup)
}

void initTimerA1()
{
    TA1CCTL1 = OUTMOD_7;
    TA1CCR0 = 2000;
    TA1CCR1 = 1000;
    TA1CTL = (TASSEL_2 | MC_1 | ID_3);

}

void UART_SendChar(char data) {
    while (!(IFG2 & UCA0TXIFG)); // Wait for TX buffer to be ready for new data
    UCA0TXBUF = data;               // Send the character
}

char UART_ReceiveChar(void) {
    while (!(IFG2 & UCA0RXIFG)); // Wait for a character to be received
    return UCA0RXBUF;            // Return the received character
}

/*using 4mhz clock as smclk, we have 0.25 microsecond period. we want range from 1000us to 2000us
 so 1000/0.25 is a count of 4000 so we need counts from 4000 to 8000.
 given a byte input of 256 values we need 256 mapped to range of 4000 so 4000/256=15.6
 and add 4000 for the start */
int convertCharToServoPos(char data) //    62.5 + 8000
{
    int asciiCode = (int)data;

    asciiCode = (asciiCode * 15) + 4000;

    return asciiCode;
}

void moveServo(uint8_t data)
{
    int servoPos;

    servoPos = convertCharToServoPos((char)data);
    TA0CCR1 = servoPos;
}

void delay(void)
{
    volatile unsigned int i;
    for (i = 10000000; i > 0; i--);  // Adjust value for desired blink rate
}


void gradualFill(u_int n, u_char r, u_char g, u_char b){
    int i;
    for (i = 0; i < n; i++){        // n is number of LEDs
        setLEDColor(i, r, g, b);
        showStrip();
        _delay_cycles(1000000);       // lazy delay
    }
}

//******************************************************************************
// Main ************************************************************************
//******************************************************************************

void main()
{
//    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    WDTCTL    = WDTPW | WDTHOLD;   // Stop watchdog timer

    initClock();
    initGPIO();
    initUART();
    initTimerA1();
    initServo();
    initStrip();
    fillStrip(0x00, 0xFF, 0x00);


  __bis_SR_register(GIE);       // Since SMCLK is source, enter LPM0, interrupts enabled


  while( 1 )                     // Endless-loop (main-program)
  {
//      gradualFill(NUM_LEDS, 0x00, 0xFF, 0x00);  // green
//      gradualFill(NUM_LEDS, 0x00, 0x00, 0xFF);  // blue
//      gradualFill(NUM_LEDS, 0xFF, 0x00, 0xFF);  // magenta
//      gradualFill(NUM_LEDS, 0xFF, 0xFF, 0x00);  // yellow
//      gradualFill(NUM_LEDS, 0x00, 0xFF, 0xFF);  // cyan
//      gradualFill(NUM_LEDS, 0xFF, 0x00, 0x00);  // red


      int timer = 0;
      int timer2 = 0;

      timer = TA1R;

      int i;
      for (i = 0; i < 20000; i++);

      timer2 = TA1R;
      if (timer > 65000 && timer2 > 65000)
      {
          TA1R = 0;                       // Reset Timer A1 counter

          int i = 0;
          for (i = 0; i < 1000000; i++)
          {
              P2OUT ^= BIT0;          // Toggle P2.0 (LED on/off)
              delay();
          }
      }


  }

}

//******************************************************************************
// UART RX Interrupt ***********************************************************
//******************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (IFG2 & UCA0RXIFG)
    {

        uint8_t rx_val = UCA0RXBUF;     // Read the received byte (clears interrupt flag)

        if (reset == 1)
        {
            stored_byte = rx_val;           // Store received byte
            moveServo(stored_byte);              // Move the servo to the new position


            reset = 0;
        }
        else
        {

            UART_SendChar((char)rx_val);

        }
//
//        // Reset the timer for handling timeouts if needed
//        TA0R = 0;                       // Reset Timer A counter
//
//        reset = 1;                      // Set reset flag to indicate a new byte was received
//
//        if (reset == 1)
//        {
//                // If reset flag is set, reset the servo position after timeout
//                TA0CCR1 = 4000;  // Reset servo to original position (mid-point)
//
//                reset = 0;       // Clear the reset flag
//         }
//
//        __enable_interrupt();             // Re-enable global interrupts

    }
}


