#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "avr_common/uart.h"

#define MAX_EVENTS  16

// MIDI Note ON/OFF message constants
#define MIDI_NOTE_ON    0x90
#define MIDI_NOTE_OFF   0x80
#define BASE_MIDI_NOTE  60  // C3

// Structure to represent a key event
typedef struct {
    uint8_t status: 1;  // 1 = pressed, 0 = released
    uint8_t key: 7; // key number (0 to 15)
} KeyEvent;

uint16_t key_status = 0;    // Current key status

// Function to scan the keyboard matrix
uint8_t keyScan(KeyEvent* events) {
    uint16_t new_status = 0;    // New key status
    int num_events = 0; // Number of events
    uint8_t key_num = 0;    // Key number (0..15)

    for (int r = 0; r < 4; ++r) {
        uint8_t row_mask = ~(1 << (r + 4)); // Mask for the output row
        PORTA = row_mask;   // Set row mask
        _delay_us(100); // Stabilize signal

        uint8_t row = ~(0x0F & PINA);   // Read the first 4 bits (negated)

        for (int c = 0; c < 4; ++c) {
            uint16_t key_mask = 1 << key_num;
            uint16_t col_mask = 1 << c;

            uint8_t cs = (row & col_mask) != 0; // 1 if key pressed

            if (cs) {
                new_status |= key_mask; // Update new key status
            }

            uint8_t ps = (key_mask & key_status) != 0;  // Previous key status

            // If the key status has changed, register an event
            if (cs != ps) {
                KeyEvent e;
                e.key = key_num;
                e.status = cs;
                events[num_events] = e;
                ++num_events;
            }
            ++key_num;
        }
    }
    key_status = new_status;    // Update global key status
    return num_events;  // Return number of events
}

void send_midi(uint8_t status, uint8_t note, uint8_t velocity) {
    usart_putchar(status);  // Send Note ON/OFF
    usart_putchar(note);    // Send the Note value
    usart_putchar(velocity);    // Send the velocity
}

int main(void) {
    // Initialize UART for MIDI communication
    printf_init();

    DDRA = 0xF0;    // Set the 4 most significant bits as output, rest as input
    PORTA = 0x0F;   // Enable pull-up resistors on input bits

    KeyEvent events[MAX_EVENTS];

    while (1) {
        uint8_t num_events = keyScan(events);   // Scan the keyboard for events
        for (uint8_t k = 0; k < num_events; ++k) {
            KeyEvent e = events[k];
            uint8_t note = BASE_MIDI_NOTE + e.key;  // Map key number to MIDI note
            if (e.status == 1) {
                send_midi(MIDI_NOTE_ON, note, 127); // Note ON, velocity 127
            }
            else {
                send_midi(MIDI_NOTE_OFF, note, 0);  // Note OFF, velocity 0
            }
        }
    }
}
