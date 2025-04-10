# Arduino_MIDI_Keyboard


## Implementation of a MIDI keyboard using ATMega2560
Using 12 pins (one for each note in the *chromatical scale*) and interrupts handling.

### USAGE:
###### You need to have FluidSynth installed!
    sudo apt-get install fluidsynth

##### Flash the keyboard_listener on the Arduino.
    make -f listener.mk keyboard_listener.hex

##### Compile and start the midi_player.
    make -f player.mk

    ./midi_player [sounds/<sound>.sf2]

##### Connect a wire to a pin and to GND in order to obtain the following notes:

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

##### To close the player connect a wire between pin 12 and GND.

##### The image in this folder provides a raw way to simulate a piano keyboard using some buttons and a breadboard.
![Piano Keyboard](images/piano_keyboard.jpg)

##### Start the music and enjoy!
