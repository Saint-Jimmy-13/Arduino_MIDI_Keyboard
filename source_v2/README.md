### USAGE:
###### You need to have FluidSynth installed!
    sudo apt-get install fluidsynth

1. Flash the keyboard_listener on the Arduino
    make -f linear_listener.mk linear_keyboard_listener.hex

2. Compile and start the midi_player
    make -f player.mk
    ./midi_player

3. Connect a wire to a pin and to GND in order to obtain the following notes:

        C:  22
        C#: 23
        D:  24
        D#: 25
        E:  26
        F:  27
        F#: 28
        G:  29
        G#: 37
        A:  36
        A#: 35
        B:  34

4. To close the player connect a wire between pin 12 and GND

5. The image in this folder provides a raw way to simulate a piano keyboard using some buttons and a breadboard
    ![Piano Keyboard](images/piano_keyboard.jpg)

6. Start the music and enjoy!
