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
    int serial_fd = open(port_name, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_fd == -1) {
        perror("Unable to open serial port");
        exit(1);
    }

    struct termios options;
    memset(&options, 0, sizeof options);

    if (tcgetattr(serial_fd, &options) < 0) {
        perror("Failed to get terminal attributes");
        close(serial_fd);
        exit(1);
    }

    cfsetispeed(&options, B38400);  // Set baud rate to 31250 for MIDI (38400 for systems without MIDI baud rate)
    cfsetospeed(&options, B38400);

    options.c_cflag |= (CLOCAL | CREAD);    // Enable receiver and set local mode
    options.c_cflag &= ~(PARENB | PARODD);  // No parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE;  // Clear size mask
    options.c_cflag |= CS8; // 8 data bits

    options.c_cflag &= ~CRTSCTS;    // No hardware flow control
    options.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    options.c_cflag &= ~(IXON | IXOFF | IXANY); // No software flow control
    options.c_cflag &= ~OPOST;  // Raw output

    // cfmakeraw(&options);    // Set raw mode
    tcflush(serial_fd, TCIOFLUSH);   // Flush the input buffer

    if (tcsetattr(serial_fd, TCSANOW, &options) < 0) {
        perror("Failed to set terminal attributes");
        close(serial_fd);
        exit(1);
    }

    return serial_fd;
}

// Function to send a MIDI message via ALSA
void send_midi_message(snd_rawmidi_t* midi_out, uint8_t* message) {
    snd_rawmidi_write(midi_out, message, 3);
    snd_rawmidi_drain(midi_out);
}

int main() {
    int serial_fd = configure_serial_port(SERIAL_PORT);

    snd_rawmidi_t* midi_out;
    if (snd_rawmidi_open(NULL, &midi_out, "virtual", SND_RAWMIDI_NONBLOCK) < 0) {
        fprintf(stderr, "ERROR opening ALSA MIDI port.\n");
        return 1;
    }

    uint8_t midi_message[3];
    while (1) {
        if (read(serial_fd, midi_message, 3) == 3) {
            printf("MIDI message received: %02X %02X %02X\n", midi_message[0], midi_message[1], midi_message[2]);
            send_midi_message(midi_out, midi_message);
        }
    }

    close(serial_fd);
    snd_rawmidi_close(midi_out);

    return 0;
}
