# rpmFlagsBar_iRacing
Connects to an iRacing instance and displays either the rpm, or flags state on a bar of LEDs, through a serial connection.
Currently implemented states:
* Inactive - All LEDs off
* Waiting for connection - Cyan LEDs counting up to the end of the bar
* Pit Limiter - Flashing Magenta lights indicate the pit limiter is on
* Checkered - Alternating white and less bright white lights indicate that the checkered flag is out
* Red - Flashing Red lights indicate a red flag is out, the session has been stopped
* Yellow - Flashing Yellow lights indicate a yellow flag is out, there is an incident ahead
* Green - Flashing Green lights indicate a yellow or red flag has ended, the green flag is out
* Blue - Flashing Blue lights indicate that you are about to be lapped
* White - Flashing White lights indicate the start of the last lap
* RPM - Displays the current car rpm as a percentage of the bars width, from left to right, with a colour skew from green to yellow to red. The rpm for 0% and 100% are set in the configuration files

For example, in RPM state at 100%.
![Rpm Bar LEDs](images/rpmBar.png?raw=true "RPM LED Display Bar")
In Yellow state, the flashing Yellow Flag lights.
![Rpm Bar LEDs](images/rpmBar2.png?raw=true "Yellow Flag LED Display Bar")


## Hardware Requirements
- An Arduino, current implementation is using an Arduino Nano
- NeoPixel Addressable LED Strip, current LED count is 17
- 1000 uF Capacitor
- 300-500 Ohm Resistor
- 5V Power Source
- Usb to Mini-B Usb cable

## Circuit Layout
Current PWM LED Output Pin is 17. Change as required.
See https://learn.adafruit.com/adafruit-neopixel-uberguide/basic-connections

## Explanation
- Compiling and running rpmFlagsBar.cpp/exe on a PC, will connect to a currently running iRacing instance, and received session information. This executable attempts to connect to an Arduino over USB Serial on the COM port defined in config/config.cfg. By default this is COM Port 11
- The Arduino requires the sketch in src/arduinoRpmFlagsBar to be uploaded to the Arduino
- The executable instances determines when to change state, and issues a command to the Arduino to display state X
- The Arduino receives the command and displays the appropriate LEDs

## Configuration
There are three locations to define parameters.
- The config/config.cfg file
    * Here the com port and car rpm lower and upper bounds are defined
    * For the RPM percentage to be displayed correctly, the car that is being drive must be defined in config/confg.cfg, under Cars. This requires the Car Name, lower bound rpm value and upper bound rpm value. The car name can be found by observing the command line output when the executable connects to the iRacing instance.
- src/stateMachine/StateMachine.h
    * Here the duration of flashing flags are set by changing the <color>Duration variables
    * This relates to parameters affecting how the executable behaves
- src/arduinoRpmFlagsBar/ardinoRpmFlagsBar.ino
    * This determines how the Arduino behaves, and where it expects to get data from
    * NEO_PIN - Sets the PWM pin to output the signal to the LEDs on
    * LED_COUNT - The number of LEDs in your strip
    * LED_BRIGHTNESS - The brightness value from 0 to 255, set once at the start of the script. The LEDs can be pretty glaring, so this is set to a low value
    * <color>_FLAG_MS - The time for half a cycle of the flag, i.e. how long the LEDs are 'off' or 'on' for
    * <color>Color = The colour definition for the flags
    
## Debugging with the Arduino Serial monitor
- Set the Board and Com Port (Tools > Board and Tools > Port)
- Open the serial monitor and set the baud rate to 250000
- The LEDs should now be in a mode looking for a connection, with cyan lights counting up across the bar

You can test the LEDs using the following commands:

Description                           | Mode           | Command
--------------------------------------|----------------|-------------------
All LEDs off                          | INACTIVE       | <0>
Flashing magenta LEDs                 | PIT_LIMITER    | <1>
Alternating White and Less White LEDs | CHECKERED_FLAG | <2>
Flashing red LEDs                     | RED_FLAG       | <3>
Flashing yellow LEDs                  | YELLOW_FLAG    | <4>
Flashing green LEDs                   | GREEN_FLAG     | <5>
Flashing blue LEDs                    | BLUE_FLAG      | <6>
Flashing white LEDs                   | WHITE_FLAG     | <7>
RPM Display, green to yellow to red   | RPM            | <8,x> Where x is the rpm percentage, the percentage of the bar to light up

## Running
* Ensure the Arduino is connected via USB
* Ensure the LEDs are powered
* Ensure the correct COM Port is set in config/config.cfg
* Ensure the rpm values for the required car are set in config/config.cfg
* cd build/src
* Run the executable rpmFlagsBar_iRacing.exe
* Startup an iRacing session


