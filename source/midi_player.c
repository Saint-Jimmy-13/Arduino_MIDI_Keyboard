#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

// MIDI serial port
#define SERIAL_PORT "/dev/ttyACM0"

// ALSA MIDI client and port names
#define MIDI_CLIENT_NAME    "MIDI Keyboard"
#define MIDI_PORT_NAME  "MIDI Out"

// Function to configure the serial port
int configure_serial_port(const char* port_name) {
    int serial_fd = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_fd == -1) {
        perror("Unable to open serial port");
        exit(1);
    }

    struct termios options;
    tcgetattr(serial_fd, &options);
    // TODO: How to compile using MIDI standard baud rate (31250)???
    cfsetispeed(&options, B19200);  // Set baud rate to 31250 for MIDI
    cfsetospeed(&options, B19200);

    options.c_cflag |= (CLOCAL | CREAD);    // Enable receiver and set local mode
    options.c_cflag &= ~PARENB; // No parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE;  // Clear size mask
    options.c_cflag |= CS8; // 8 data bits

    options.c_cflag &= ~CRTSCTS;    // No hardware flow control
    options.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    options.c_cflag &= ~(IXON | IXOFF | IXANY); // No software flow control
    options.c_cflag &= ~OPOST;  // Raw output

    tcsetattr(serial_fd, TCSANOW, &options);

    return serial_fd;
}

// Function to send a MIDI message via ALSA
void send_midi_message(snd_rawmidi_t* midi_out, uint8_t* message) {
    snd_rawmidi_write(midi_out, message, 3);
    snd_rawmidi_drain(midi_out);
}

int main() {
    // TODO
    return 0;
}
