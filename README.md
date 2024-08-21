# Arduino_MIDI_Keyboard

WORK IN PROGRESS: this project is not finished yet

Implementation of a MIDI keyboard using ATMega2560


Usage:
    1. Flash the keyboard_listener on the Arduino
        make -f listener.mk keyboard_listener.hex

    2. Compile and start the midi_player
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
