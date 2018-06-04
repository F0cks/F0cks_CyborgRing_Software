/*Author: F0cks (29/05/2018)                                                            */
/* Find all information about this code on https://blog.f0cks.net/projects/Cyborg-ring/ */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

// Clock is 6,4MHz / 8 : 800kHz

volatile uint16_t timerTick;

/* Light a specific led (turn OFF others) */
void lightLED(uint8_t ledNumber)
{
    DDRB = 0; // Set whole PORTB as input
    PORTB &= ~( (1<<PORTB0) | (1<<PORTB2) | (1<<PORTB3) | (1<<PORTB4)); // Clear PB0 PB2 PB3 PB4

    switch(ledNumber)
    {
        case 0: // LED 0
            DDRB  |= ( (1 << DDB0) | (1 << DDB3) ); // Set PB0 and PB3 as outputs
            PORTB |= (1 << PORTB0);                 // PB0 is sourcing VCC
            PORTB &= ~(1 << PORTB3);                // PB3 is sinking 0V
            break;
        case 1: // LED 1
            DDRB  |= ( (1 << DDB0) | (1 << DDB3) ); // Set PB0 and PB3 as outputs
            PORTB |= (1 << PORTB3);                 // PB3 is sourcing VCC
            PORTB &= ~(1 << PORTB0);                // PB0 is sinking 0V
            break;
        case 2: // LED 2
            DDRB  |= ( (1 << DDB0) | (1 << DDB4) ); // Set PB0 and PB4 as outputs
            PORTB |= (1 << PORTB0);                 // PB0 is sourcing VCC
            PORTB &= ~(1 << PORTB4);                // PB4 is sinking 0V
            break;
        case 3: // LED 3
            DDRB  |= ( (1 << DDB0) | (1 << DDB4) ); // Set PB0 and PB4 as outputs
            PORTB |= (1 << PORTB4);                 // PB4 is sourcing VCC
            PORTB &= ~(1 << PORTB0);                // PB0 is sinking 0V
            break;
        case 4: // LED 4
            DDRB  |= ( (1 << DDB2) | (1 << DDB4) ); // Set PB2 and PB4 as outputs
            PORTB |= (1 << PORTB2);                 // PB2 is sourcing VCC
            PORTB &= ~(1 << PORTB4);                // PB4 is sinking 0V
            break;
        case 5: // LED 5
            DDRB  |= ( (1 << DDB2) | (1 << DDB4) ); // Set PB2 and PB4 as outputs
            PORTB |= (1 << PORTB4);                 // PB4 is sourcing VCC
            PORTB &= ~(1 << PORTB2);                // PB2 is sinking 0V
            break;
        case 6: // LED 6
            DDRB  |= ( (1 << DDB2) | (1 << DDB3) ); // Set PB2 and PB3 as outputs
            PORTB |= (1 << PORTB2);                 // PB2 is sourcing VCC
            PORTB &= ~(1 << PORTB3);                // PB3 is sinking 0V
            break;
        case 7: // LED 7
            DDRB  |= ( (1 << DDB2) | (1 << DDB3) ); // Set PB2 and PB3 as outputs
            PORTB |= (1 << PORTB3);                 // PB3 is sourcing VCC
            PORTB &= ~(1 << PORTB2);                // PB2 is sinking 0V
            break;
        default:
            ;
    }
}

/* Delay ms (max: 65535 ms) */
void delayMs(uint16_t ms)
{
    timerTick = 0;                           // Clear tick counter
    TCNT0 = 0;                               // Clear Timer/Counter Register
    TCCR0B |= ( (1 << CS01) | (1 << CS00) ); // Set prescaler to 64 and start the timer
    while(timerTick < ms);                   // Wait
    TCCR0B = 0;                              // Stop timer
}

int main(void)
{
    /******************/
    /* Configuration  */
    /******************/
    int8_t i = 0;
    int8_t j = 0;
    timerTick = 0;

    DDRB  = 0x00;              // Set whole PORTB as input
    DDRB &=  ~(1 << DDB1);     // Clear the PB1 pin

    // Interrupt on PB1 (PCINT1)
    PORTB |= (1 << PORTB1);    // Turn ON the Pull-up
    GIMSK |= (1 << PCIE);      // PCIE: Pin Change Interrupt Enable
    PCMSK |= (1 << PCINT1);    // PB1 pin change interrupt enabled

    // Timer 0 ( 1ms ) : OCRA =  [ (clock_speed / Prescaler_value) * Desired_time_in_Seconds ] - 1
    // OCRA = [ (6400000/64)*0.001 ] - 1 ] = 99 = 0x63
    // Not really accurate: internal OSC ... but we don't really care
    TCCR0A |= (1 << WGM01);    // Set the Timer Mode to CTC
    OCR0A   = 0x63;            // Set compare register
    TIMSK  |= (1 << OCIE0A);   // Set the ISR COMPA vect
    sei();                     // Enable interrupt

    /******************/
    /*   Main loop    */
    /******************/
    while(1)
    {
        // Power saving
        DDRB = 0x00;                         // Set whole PORTB as input
        cli();                               // Disable interrupt
        set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set power down mode
        sleep_enable();                      // Put the device in sleep mode
        sleep_bod_disable();                 // Disable BOD before going to sleep
        sei();                               // Enable interrupt
        sleep_cpu();                         // Put the device into sleep mode
        // If button has been pressed
        sleep_disable();                     // Clear the SE (sleep enable) bit
        
        // LED blinking sequence
        for(j = 0; j < 4; j++)
        {
            for(i = 0; i < 8; i++)
            {
                lightLED(i);
                delayMs(30);
            }
            for(i = 6; i > -1; i--)
            {
                lightLED(i);
                delayMs(30);
            }
            lightLED(-1);
            delayMs(200);                
        }
    }

    return 0;
}

/* External interrupt function */
ISR (PCINT0_vect)
{
    // Button interrupt
}

/* Timer interrupt function */
ISR(TIMER0_COMPA_vect)
{
    timerTick++;
}