#include <avr/io.h>
#include <avr/interrupt.h>
#include "avr_common/uart.h"

#define F_CPU   16000000UL
#include <util/delay.h>

#define BUTTON1_PIN PD2
#define BUTTON2_PIN PD3

void setup_pins() {
    // Set buttons as input
    DDRD &= ~(1 << BUTTON1_PIN);
    DDRD &= ~(1 << BUTTON2_PIN);

    // Enable pull-up resistors
    PORTD |= (1 << BUTTON1_PIN);
    PORTD |= (1 << BUTTON2_PIN);
}

void send_midi_packet(uint8_t pitch, uint16_t duration) {
    uint8_t velocity = (duration < 128) ? duration : 127;

    // Note ON
    usart_putchar(0x90);    // Note ON, channel 1
    usart_putchar(pitch);
    usart_putchar(velocity);

    _delay_ms(100); // Simulate delay

    // Note OFF
    usart_putchar(0x80);    // Note OFF, channel 1
    usart_putchar(pitch);
    usart_putchar(0x00);
}

int main(void) {
    // Initialize UART for MIDI communication
    printf_init();

    // Configure pins
    setup_pins();

    uint16_t button1PressTime = 0;
    uint16_t button2PressTime = 0;

    // Setup Timer1 for timing
    TCCR1B |= (1 << CS11);  // Prescaler set to 8, Timer at 2 MHz (0.5 us resolution)

    while (1) {
        // Button 1
        if (!(PIND & (1 << BUTTON1_PIN))) {
            if (button1PressTime == 0) {
                button1PressTime = TCNT1;   // Start timing
            }
        }
        else if (button1PressTime != 0) {
            uint16_t duration = TCNT1 - button1PressTime;
            send_midi_packet(60, duration); // Send MIDI with pitch 60 (C4)
            button1PressTime = 0;
        }

        // Button 2
        if (!(PIND & (1 << BUTTON2_PIN))) {
            if (button2PressTime == 0) {
                button2PressTime = TCNT1;   // Start timing
            }
        }
        else if (button2PressTime != 0) {
            uint16_t duration = TCNT1 - button2PressTime;
            send_midi_packet(62, duration); // Send MIDI with pitch 62 (D4)
            button2PressTime = 0;
        }
    }

    return 0;
}
