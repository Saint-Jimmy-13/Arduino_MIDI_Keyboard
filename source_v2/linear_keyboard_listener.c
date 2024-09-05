#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define MIDI_BAUD   38400   
#define MYUBRR  (F_CPU / 16 / MIDI_BAUD - 1)

#define MAX_EVENTS      12

// MIDI Note ON/OFF message constants
#define MIDI_NOTE_ON    0x90
#define MIDI_NOTE_OFF   0x80
#define BASE_MIDI_NOTE  60  // C3

// Special message for Ctrl+C
#define CTRL_C  0xFF

#define BUFFER_SIZE 64

// Structure to represent a key event
typedef struct {
    uint8_t status;  // 1 = pressed, 0 = released
    uint8_t key; // key number
} KeyEvent;

volatile uint16_t key_status = 0;   // Current key status
volatile uint8_t should_exit = 0;   // Flag to indicate if CTRL+C arrived

volatile uint8_t uart_buffer[BUFFER_SIZE];
volatile uint8_t uart_head = 0;
volatile uint8_t uart_tail = 0;

// Function to initialize UART
void uart_init(void) {
    uint16_t ubrr = MYUBRR; // Set baud rate to 38400
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << UDRIE0);   // Enable RX, TX and UDRIE interrupts
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits, 1 stop bit, no parity
}

// Function to enqueue data into UART buffer
void uart_enqueue(uint8_t data) {
    uint8_t next_head = (uart_head + 1) % BUFFER_SIZE;
    if (next_head != uart_tail) {
        uart_buffer[uart_head] = data;
        uart_head = next_head;
        UCSR0B |= (1 << UDRIE0);    // Enable UART Data Register Empty interrupt
    }
}

// Function to scan the keyboard for 12 keys across PORTA and PORTC
uint8_t keyScan(KeyEvent* events) {
    uint16_t new_status = 0;    // New key status
    int num_events = 0; // Number of events

    // Scan keys on PORTA (first 8 keys)
    for (uint8_t key_num = 0; key_num < 8; ++key_num) {
        uint16_t key_mask = 1 << key_num;
        uint8_t pin_state = PINA & (1 << key_num);  // Read the state of the pin for each key on PORTA

        uint8_t cs = (pin_state == 0);  // 1 if key pressed (active low)

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
    }

    // Scan keys on PORTC (next 4 keys)
    for (uint8_t key_num = 8; key_num < 12; ++key_num) {
        uint16_t key_mask = 1 << key_num;
        uint8_t pin_state = PINC & (1 << (key_num - 8));    // Read the state of the pin for each key on PORTC

        uint8_t cs = (pin_state == 0);  // 1 if key pressed (active low)

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
    }

    // Check exit signal on PORTB6
    _delay_us(100);
    uint8_t exit_signal = PINB & (1 << 6);  // Check bit 6 of PORTB (Pin 12)

    if (exit_signal == 0) {
        should_exit = 1;
        KeyEvent e;
        e.key = 12; // Use a special key number for the exit signal
        e.status = 1;
        events[num_events] = e;
        ++num_events;
    }

    key_status = new_status;    // Update global key status
    return num_events;  // Return number of events
}

// Send MIDI message over UART
void send_midi(uint8_t status, uint8_t note, uint8_t velocity) {
    uart_enqueue(status);  // Send Note ON/OFF
    uart_enqueue(note);    // Send the Note value
    uart_enqueue(velocity);    // Send the velocity
}

// Timer Interrupt Service Routine (ISR) for key scanning
ISR(TIMER0_COMPA_vect) {
    KeyEvent events[MAX_EVENTS];
    uint8_t num_events = keyScan(events);   // Scan the keyboard for events

    for (uint8_t k = 0; k < num_events; ++k) {
        KeyEvent e = events[k];
        if (e.key == 12 && e.status == 1) {
            send_midi(CTRL_C, 0, 0);    // Send Ctrl+C message
        }
        else {
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

// UART Data Register Empty ISR
ISR(USART0_UDRE_vect) {
    if (uart_tail != uart_head) {
        UDR0 = uart_buffer[uart_tail];
        uart_tail = (uart_tail + 1) % BUFFER_SIZE;
    }
    else {
        UCSR0B &= ~(1 << UDRIE0);   // Disable UART Data Register Empty interrupt
    }
}

// Initialize Timer0 in CTC mode
void timer_init(void) {
    TCCR0A = (1 << WGM01);  // Set CTC mode
    OCR0A = 156;    // Set compare value for 10ms intervals
    TIMSK0 = (1 << OCIE0A); // Enable compare match interrupt
    TCCR0B = (1 << CS02) | (1 << CS00); // Start timer with 1024 prescaler
}

int main(void) {
    // Initialize UART for MIDI communication
    uart_init();

    DDRA = 0x00;    // Set all bits of PORTA as input
    PORTA = 0xFF;   // Enable pull-up resistors on all pins of PORTA

    DDRC = 0x00;    // Set all bits of PORTC as input
    PORTC = 0xFF;   // Enable pull-up resistors on all pins of PORTC

    DDRB &= ~(1 << 6);  // Set PORTB6 as input
    PORTB |= (1 << 6);  // Enable pull-up resistor on PORTB6

    // Initialize timer and enable global interrupts
    timer_init();
    sei();

    while (!should_exit) {
        // Main loop is empty since all work is done in interrupts
    }
    
    return 0;
}
