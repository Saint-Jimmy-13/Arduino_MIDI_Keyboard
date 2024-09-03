### USAGE:
If there are errors, see the **MANUAL USAGE** section to find out problems.

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


### MANUAL USAGE:
You can use the program this way removing the function *start_synth_and_connect()* in *main* of *midi_player.c* (line 98).

1. Flash the **keyboard_listener** on the Arduino
    make -f listener.mk keyboard_listener.hex

2. Compile and start the **midi_player**
    make -f player.mk
    ./midi_player

3. Install and launch a synth (e. g. FLUID Synth)
    sudo apt-get install fluidsynth
    fluidsynth

4. Type "aconnect -l" to see current connections, you should see something like

        cliente 0: 'System' [tipo=kernel]
            0 'Timer           '
            1 'Announce        '
        cliente 14: 'Midi Through' [tipo=kernel]
            0 'Midi Through Port-0'
        cliente 128: 'Client-128' [tipo=utente]
            0 'Virtual RawMIDI '
        cliente 129: 'FLUID Synth' [tipo=utente]
            0 'Synth input port (10513:0)'

5. Connect the player program with the synth
    aconnect 128:0 129:0

6. Start the music and enjoy!
