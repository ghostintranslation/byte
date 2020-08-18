# BYTE

![GitHub version](https://img.shields.io/github/v/release/ghostintranslation/byte.svg?include_prereleases)

BYTE is a modular drum sequencer, based on eurorack physical format it has however no patch cables in the front but has rather midi jacks in the back.

It is based on [Motherboard12](https://github.com/ghostintranslation/motherboard12).

## Features

* 8 steps
* 8 bars
* 8 voices
* Sequencer, Trigger, Mixer modes
* MIDI over USB
* MIDI over mini jack

## Getting Started

### Prerequisites

What things you need to make it work:

```
1 Teensy 4.0
1 MOTHERBOARD12A pcb
1 MOTHERBOARD12B pcb
1 BYTE front panel pcb
11 push buttons
1 vertical rotary encoder
12 LED
3 14 pins male header
5 14 pins female header
6 CD4051 multiplexers
6 DIP16 IC sockets (optional)
2 3.5mm jack connectors
1 resistor ~ 22ohm
1 4 positions dipswitch (optional)
```

In order to run any sketch on the Teensy you have to install the Teensyduino add-on to Arduino.
Follow the instructions from the official page:
https://www.pjrc.com/teensy/teensyduino.html

Then open the sketch located in the Byte folder of this repo.

In the Tools -> USB Type menu, choose "Serial + midi".

Then you are ready to compile and upload the sketch.

# How to use

```
Default screen
    - Each pair of LED-Button represent a step within the current Voice and Bar.
    - The LED indicates when the step is active.
    - The button toggles the step.

Clock
    - Usage
        - A long press on the Clock rotary will enter the Clock mode screen.
        - Turn the rotary while in this mode to select between Follower and Leader modes.
        - Press to validate and exit
    - Display
        - Leader is a slow blink, Follower is a fast blink
    - Modes
        1. Leader
            - Sends MIDI clock and song position messages
            - Turn the rotary to increments or decrements the tempo
            - Press to toggle Play/Stop
        2. Follower
            - Synchronize its steps to the received clocks and song position messages
            - Turn the rotary to choose between 8 values to divide the received clock   (TODO)
            - Press to toggle Play/Stop

Voice
    - Usage
        - Press once the Voice button to enter the Voice screen.
        - Press again the button while in this screen to change the Voice.
        - Choose between 8 voices.
    - Display
        - The selected voice is indicated by a blinking LED.
        - Screen stays 1s after last press.

Bar
    - Usage
        - Press once the Bar button to enter the Bar screen.
        - Press again the button while in this screen to change the Bar.
        - Choose between 8 bars.
    - Display
        - The selected scale is indicated by a blinking LED.
        - Screen stays 1s after last press.

Mode
    - Usage
        - Press once the Mode button to enter the Mode screen.
        - Press again the button while in this screen to change the Mode.
        - Choose between 3 modes.
    - Display
        - The selected mode is indicated by a blinking LED.
        - Screen stays 1s after last press.
    - Modes
        1. Sequencer
            - Default screen
            - Use the buttons to toggle the steps
            - Use Voice and Bar to select the current voice and bar
        2. Trigger
            - Use the buttons to trigger each voice
            - Voice and Bar buttons are disabled
        3. Mixer
            - Use the buttons to toggle each voice
            - Voice and Bar buttons are disabled

```

## About me
You can find me on Bandcamp, Instagram, Youtube and mu own website:

https://ghostintranslation.bandcamp.com/

https://www.instagram.com/ghostintranslation/

https://www.youtube.com/channel/UCcyUTGTM-hGLIz4194Inxyw

https://www.ghostintranslation.com/


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details