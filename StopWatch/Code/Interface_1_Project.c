/*
 * Interface_1_Project.c
 *
 * Created on: Aug 27, 2025
 * Author: Abouzeid
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// ================== Global Variables ==================
unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours   = 0;

unsigned char mode = 0; // 0 = Count up mode, 1 = Countdown mode

// Flags to handle button debounce (one action per press)
unsigned char sec_inc_handled = 0;
unsigned char sec_dec_handled = 0;
unsigned char min_inc_handled = 0;
unsigned char min_dec_handled = 0;
unsigned char hr_inc_handled  = 0;
unsigned char hr_dec_handled  = 0;

// ================== Timer1 Initialization ==================
// Configures Timer1 in CTC mode for 1-second interrupts
void TIMER1_Init() {
	TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS12);  // CTC mode, prescaler = 1024
	OCR1A = 15624;   // Compare value for 1 second (16MHz / 1024)
	TIMSK |= (1 << OCIE1A);  // Enable Compare A interrupt
}

// ================== Timer1 ISR (1 second interval) ==================
ISR(TIMER1_COMPA_vect) {
	if (mode == 0) {
		// -------- Count Up Mode --------
		PORTD &= ~((1 << PD5) | (1 << PD0));  // Turn OFF PD5
		PORTD |= (1 << PD4);   // Turn ON PD4
		seconds++;
		if (seconds >= 60) {
			seconds = 0;
			minutes++;
			if (minutes >= 60) {
				minutes = 0;
				hours++;
				if (hours >= 24) {
					hours = 0;
				}
			}
		}
	} else {
		// -------- Count Down Mode --------
		if (hours == 0 && minutes == 0 && seconds == 0) {
			PORTD |= (1 << PD0);  // Turn ON buzzer when countdown hits 0
		} else {
			PORTD &= ~(1 << PD0);
			if (seconds == 0) {
				seconds = 59;
				if (minutes == 0) {
					minutes = 59;
					if (hours > 0)
						hours--;
				} else {
					minutes--;
				}
			} else {
				seconds--;
			}
		}
	}
}

// ================== Adjust Seconds ==================
// Handles increment/decrement of seconds via buttons
void Second_Adjust(void)
{
    // Increment seconds (button on PB6)
    if (!(PINB & (1 << PB6))) {
        if (!sec_inc_handled) {
            seconds++;
            if (seconds >= 60) {
                seconds = 0;
                minutes++;
                if (minutes >= 60) {
                    minutes = 0;
                    hours++;
                    if (hours >= 24) hours = 0;
                }
            }
            sec_inc_handled = 1; // Mark button as handled
        }
    } else {
        sec_inc_handled = 0;
    }

    // Decrement seconds (button on PB5)
    if (!(PINB & (1 << PB5))) {
        if (!sec_dec_handled) {
            if (seconds == 0) {
                seconds = 59;
                if (minutes == 0) {
                    minutes = 59;
                    if (hours > 0) hours--;
                } else {
                    minutes--;
                }
            } else {
                seconds--;
            }
            sec_dec_handled = 1;
        }
    } else {
        sec_dec_handled = 0;
    }
}

// ================== Adjust Minutes ==================
void Minutes_Adjust(void)
{
    // Increment minutes (button on PB4)
    if (!(PINB & (1 << PB4))) {
        if (!min_inc_handled) {
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
                hours++;
                if (hours >= 24) hours = 0;
            }
            min_inc_handled = 1;
        }
    } else {
        min_inc_handled = 0;
    }

    // Decrement minutes (button on PB3)
    if (!(PINB & (1 << PB3))) {
        if (!min_dec_handled) {
            if (minutes == 0) {
                minutes = 59;
                if (hours > 0) hours--;
            } else {
                minutes--;
            }
            min_dec_handled = 1;
        }
    } else {
        min_dec_handled = 0;
    }
}

// ================== Adjust Hours ==================
void Hours_Adjust(void)
{
    // Increment hours (button on PB1)
    if (!(PINB & (1 << PB1))) {
        if (!hr_inc_handled) {
            hours++;
            if (hours >= 24) hours = 0;
            hr_inc_handled = 1;
        }
    } else {
        hr_inc_handled = 0;
    }

    // Decrement hours (button on PB0)
    if (!(PINB & (1 << PB0))) {
        if (!hr_dec_handled) {
            if (hours == 0) {
                hours = 23;
            } else {
                hours--;
            }
            hr_dec_handled = 1;
        }
    } else {
        hr_dec_handled = 0;
    }
}

// ================== External Interrupts ==================

// INT0: Reset clock (triggered by PD2)
void INT0_Init(void) {
	DDRD &= ~(1 << PD2);
	MCUCR |= (1 << ISC01);  // Falling edge
	MCUCR &= ~(1 << ISC00);
	GICR |= (1 << INT0);    // Enable INT0
}
ISR(INT0_vect) {
	seconds = 0;
	minutes = 0;
	hours = 0;
}

// INT1: Pause clock (triggered by PD3)
void INT1_Init(void) {
	DDRD &= ~(1 << PD3);
	MCUCR |= (1 << ISC10) | (1 << ISC11); // Rising edge
	GICR |= (1 << INT1); // Enable INT1
	GIFR |= (1 << INTF1);
}
ISR(INT1_vect) {
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10)); // Stop timer
}

// INT2: Resume clock (triggered by PB2)
void INT2_Init(void) {
	DDRB &= ~(1 << PB2);
	PORTB |= (1 << PB2);    // Enable pull-up
	MCUCSR &= ~(1 << ISC2); // Falling edge
	GICR |= (1 << INT2);    // Enable INT2
}
ISR(INT2_vect) {
	TCCR1B |= (1 << CS12) | (1 << CS10); // Resume timer
}

// ================== Main Function ==================
int main() {
	DDRA = 0xFF;	// Configure PORTA as output for display selection
	PORTA = 0xFF;	// Enable NPN transistors
	DDRC = 0x0F;	// Lower nibble of PORTC as output (for digit data)
	PORTC &= 0xF0;	// Clear lower nibble

	sei();			// Enable global interrupts

	DDRB = 0x00;   	// Set PORTB as input (for buttons)
	PORTB = 0xFF;   // Enable internal pull-up resistors

	DDRD |= (1 << PD4) | (1 << PD5) | (1 << PD0); // Configure PD4, PD5, PD0 as outputs
	PORTD &= ~((1 << PD4) | (1 << PD5) | (1 << PD0)); // Initially OFF

	// Initialize Timer and External Interrupts
	TIMER1_Init();
	INT1_Init();
	INT2_Init();
	INT0_Init();

	while (1) {
		// Check and adjust time based on button inputs
		Second_Adjust();
		Minutes_Adjust();
		Hours_Adjust();

		// Toggle mode when PB7 is pressed
		if (!(PINB & (1 << PB7))) {
			PORTD ^= (1 << PD4);
			PORTD ^= (1 << PD5);
			_delay_ms(10);
			mode ^= 1; // Switch between count-up and countdown
		}
		// Copy current time values for display
		unsigned char h = hours;
		unsigned char m = minutes;
		unsigned char s = seconds;

		// Split time into separate digits
		unsigned char digits[6];
		digits[0] = h / 10;
		digits[1] = h % 10;
		digits[2] = m / 10;
		digits[3] = m % 10;
		digits[4] = s / 10;
		digits[5] = s % 10;

		// Multiplexing to display time on 6-digit 7-segment display
		for (unsigned char i = 0; i < 6; i++) {
			PORTC = (PORTC & 0xF0) | (digits[i] & 0x0F);
			PORTA = (1 << i);
			_delay_ms(2);
			PORTA = 0x00;
		}
	}
}
