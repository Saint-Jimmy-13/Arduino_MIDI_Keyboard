#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "avr_common/uart.h"

// Define MIDI commands
#define MIDI_NOTE_ON    0x90
#define MIDI_NOTE_OFF   0x80
#define BASE_MIDI_NOTE  60  // C4
#define NUM_NOTES       12
#define NUM_CONTROLS    4

// Define control pins
#define OCTAVE_DOWN_PIN PC4
#define OCTAVE_UP_PIN   PC5
#define VOLUME_DOWN_PIN PC6
#define VOLUME_UP_PIN   PC7

// MIDI state
uint8_t octave_offset = 0;
uint8_t volume = 127;

// Initialize UART and I/O pins
void init_system(void) {
    // Initialize UART
    printf_init();

    // Set PA0 - PA7 as inputs for notes (with pull-up resistors)
    DDRA &= 0x00;   // Set PA0 - PA7 as inputs
    PORTA |= 0xFF;  // Enable pull-ups on PA0 - PA7

    // Set PC0 - PC3 as inputs for notes (with pull-up resistors)
    DDRC &= 0xF0;   // Set PC0 - PC3 as inputs
    PORTC |= 0x0F;  // Enable pull-ups on PC0 - PC3

    // Set PC4 - PC7 as inputs for control buttons (with pull-up resistors)
    DDRC &= 0x0F;   // Set PC4 - PC7 as inputs
    PORTA |= 0xFF;  // Enable pull-ups on PC4 - PC7
}

// Read the state of the control buttons and update MIDI state
void check_controls(void) {
    // Read control buttons (PC4 - PC7)
    uint8_t control_state = PINC & 0xF0;

    if (!(control_state & (1 << (OCTAVE_DOWN_PIN - PC0)))) {
        if (octave_offset > -24) {
            octave_offset -= 12;
            _delay_ms(200); // Simple debounce
        }
    }

    if (!(control_state & (1 << (OCTAVE_UP_PIN - PC0)))) {
        if (octave_offset < 24) {
            octave_offset += 12;
            _delay_ms(200); // Simple debounce
        }
    }
    
    if (!(control_state & (1 << (VOLUME_DOWN_PIN - PC0)))) {
        if (volume > 16) {
            volume -= 16;
            _delay_ms(200); // Simple debounce
        }
    }

    if (!(control_state & (1 << (VOLUME_UP_PIN - PC0)))) {
        if (volume < 127) {
            volume += 16;
            _delay_ms(200); // Simple debounce
        }
    }
}

// Scan the note buttons and send MIDI messages
void scan_notes(void) {
    // Read note buttons (PA0 - PA7 for the first 8 notes)
    uint8_t note_state_A = PINA & 0xFF;

    // Read note buttons (PC0 - PC3 for the next 4 notes)
    uint8_t note_state_C = PINC & 0x0F;

    // Process the 8 notes connected to PA0 - PA7
    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t mask = (1 << i);
        uint8_t is_pressed = !(note_state_A & mask);

        uint8_t midi_note = BASE_MIDI_NOTE + i + octave_offset;

        if (is_pressed)
            send_midi(MIDI_NOTE_ON, midi_note, volume);
        else
            send_midi(MIDI_NOTE_OFF, midi_note, 0);
    }

    // Process the 4 notes connected to PC0 - PC3
    for (uint8_t i = 0; i < 4; ++i) {
        uint8_t mask = (1 << i);
        uint8_t is_pressed = !(note_state_C & mask);

        uint8_t midi_note = BASE_MIDI_NOTE + 8 + i + octave_offset;

        if (is_pressed)
            send_midi(MIDI_NOTE_ON, midi_note, volume);
        else
            send_midi(MIDI_NOTE_OFF, midi_note, 0);
    }
}

// Send MIDI message over UART
void send_midi(uint8_t status, uint8_t note, uint8_t velocity) {
    usart_putchar(status);
    usart_putchar(note);
    usart_putchar(velocity);
}

int main(void) {
    init_system();

    while (1) {
        check_controls();   // Check control buttons (octave, volume)
        scan_notes();   // Scan note buttons and send MIDI messages
        _delay_ms(10);  // Short delay to prevent excessive polling
    }

    return 0;
}
