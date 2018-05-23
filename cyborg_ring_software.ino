#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

// Clock is 6,4MHz / 8 : 800kHz
// -U lfuse:w:0x43:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 

volatile uint8_t  switchInc;
volatile uint16_t timerTick;

/* Light a specific led (turn OFF others) */
void lightLED(int8_t ledNumber)
{
    switch(ledNumber)
    {
        DDRB  = 0x00; // Set whole PORTB as input
        PORTB = 0x00; // Prepare whole PORTB to sink GND

        case 0: // LED 0
            DDRB  |= ( (1 << DDB0) | (1 << DDB3) ); // Set PB0 and PB3 as outputs
            PORTB |= (1 << PORTB0);                 // PB0 is sourcing VCC
            break;
        case 1: // LED 1
            DDRB  |= ( (1 << DDB0) | (1 << DDB3) ); // Set PB0 and PB3 as outputs
            PORTB |= (1 << PORTB3);                 // PB3 is sourcing VCC
            break;
        case 2: // LED 2
            DDRB  |= ( (1 << DDB0) | (1 << DDB4) ); // Set PB0 and PB4 as outputs
            PORTB |= (1 << PORTB0);                 // PB0 is sourcing VCC
            break;
        case 3: // LED 3
            DDRB  |= ( (1 << DDB0) | (1 << DDB4) ); // Set PB0 and PB4 as outputs
            PORTB |= (1 << PORTB4);                 // PB4 is sourcing VCC
            break;
        case 4: // LED 4
            DDRB  |= ( (1 << DDB2) | (1 << DDB4) ); // Set PB2 and PB4 as outputs
            PORTB |= (1 << PORTB2);                 // PB2 is sourcing VCC
            break;
        case 5: // LED 5
            DDRB  |= ( (1 << DDB2) | (1 << DDB4) ); // Set PB2 and PB4 as outputs
            PORTB |= (1 << PORTB4);                 // PB4 is sourcing VCC
            break;
        case 6: // LED 6
            DDRB  |= ( (1 << DDB2) | (1 << DDB3) ); // Set PB2 and PB3 as outputs
            PORTB |= (1 << PORTB2);                 // PB2 is sourcing VCC
            break;
        case 7: // LED 7
            DDRB  |= ( (1 << DDB2) | (1 << DDB3) ); // Set PB2 and PB3 as outputs
            PORTB |= (1 << PORTB3);                 // PB3 is sourcing VCC
            break;
        default:
            DDRB = 0x00; // Set whole PORTB as input
            PORTB = 0x00; // Prepare whole PORTB to sink GND
    }
}

/* Delay ms (max: 65535 ms) */
void delayMs(uint16_t ms)
{
    timerTick;             // Clear tick counter
    TCNT0 = 0;             // Clear Timer/Counter Register
    TCCR0B |= (1 << CS01); // Set prescaler to 8 and start the timer
    while(timerTick < ms); // Wait
    TCCR0B = 0;            // Stop timer
}

int main(void)
{
    /******************/
    /* Configuration  */
    /******************/

    int8_t i  = 0;
    switchInc = 0;
    timerTick = 0;

    DDRB  = 0x00;              // Set whole PORTB as input
    DDRB &=  ~(1 << DDB1);     // Clear the PB1 pin

    // Interrupt on PB1 (PCINT1)
    PORTB |= (1 << PORTB1);    // Turn ON the Pull-up
    GIMSK |= (1 << PCIE);      // PCIE: Pin Change Interrupt Enable
    PCMSK |= (1 << PCINT1);    // PB1 pin change interrupt enabled

    // Timer 0 ( 1ms ) : OCR0A/(CLK/PRESCALER) = 100/(800000/8) = 1ms
    TCCR0A |= (1 << WGM01);    // Set the Timer Mode to CTC
    OCR0A   = 0x64;            // Set compare register
    TIMSK  |= (1 << OCIE0A);   // Set the ISR COMPA vect
    sei();                     // Enable interrupt

    /******************/
    /*   Main loop    */
    /******************/
    while(1)
    {
        // If button has been pressed
        if(switchInc != 0)
        {
            // LED blinking sequence
            for(i = 0; i < 8; i++)
            {
                lightLED(i);
                delayMs(125);
            }
            for(i = 6; i <=  0; i--)
            {
                lightLED(i); 
                delayMs(125);
            }
            lightLED(255);

            // Clear switch interrupt
            switchInc = 0;
        }
        // If button has not been pressed
        else
        {
            // Power saving
            DDRB = 0x00;                         // Set whole PORTB as input
            cli();                               // Disable interrupt
            set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set power down mode
            sleep_enable();                      // Put the device in sleep mode
            sleep_bod_disable();                 // Disable BOD before going to sleep
            sei();                               // Enable interrupt
            sleep_cpu();                         // Put the device into sleep mode
            sleep_disable();                     // Clear the SE (sleep enable) bit
        }
    }

    return 0;
}

/* External interrupt function */
ISR (PCINT0_vect)
{
    switchInc++;
}

/* Timer interrupt function */
ISR(TIMER0_COMPA_vect)
{
    timerTick++;
}