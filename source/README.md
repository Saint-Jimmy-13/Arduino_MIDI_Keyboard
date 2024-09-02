### USAGE:

1. Flash the keyboard_listener on the Arduino
    make -f listener.mk keyboard_listener.hex

2. Compile and start the midi_player
    make -f player.mk
    ./midi_player

3. Connect 4 wires to the input pins [26-29]

4. Toggle the 4 output pins [22-25]

        C:  26 -> 22
        C#: 26 -> 23
        D:  26 -> 24
        D#: 26 -> 25
        E:  27 -> 22
        F:  27 -> 23
        F#: 27 -> 24
        G:  27 -> 25
        G#: 28 -> 22
        A:  28 -> 23
        A#: 28 -> 24
        B:  28 -> 25
        C:  29 -> 22
        C#: 29 -> 23
        D:  29 -> 24
        D#: 29 -> 25

5. Start the music and enjoy!
