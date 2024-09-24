# PapaLink AVR programs

PapaLink is an open-source connector and communication protocol
meant to power and control scientific and industrial devices.
It is developed by DEV Joni since 2023.

On the communications parts, PapaLink builds upon
the industry standard I2C protocol, adding some features
to facilitate firmware building and use. Those features 
are defined here.

Programs in this repository are designed to be run on
AVR microcontrollers and build within the Arduino development
environment (tested v1.8.19).

**Contents**

- *contprog.ino*: PapaLink AVR USB-Controller Firmware
- *papaprog.ino*: PapaLink AVR Device Firmware

Protocol version: **0** (unreleased)


## Requirements and use

### Hardware

In theory, you can use any micro-controller that
supports I2C and has enough free pins and adapt
the code provided here to your project.
We have tested the code to work on an ATMega4809 MCU
present on the Arduino Nano Every development board.

The USB controller firmware also requires the MCU to have
serial capability.

PapaLink connectors, cables, hubs etc. are
available at [Shop DEV Joni](https://devjoni.com/shop/).
You can also build them
yourself provided you have some tools and materials.


### Flashing firmware

Example 1. Flash to Nano Every on Linux

```bash
arduino --upload papaprog.ino --board arduino:megaavr:nona4809 --port /dev/ttyACM0
```

### Modifying for your own project

It is easiest to start by copying the skeleton program

```bash
cp userprog_skeleton.h userprog_myproject.h
```

Then, modify it.
Your own functionality that you add will be called from within the
`osa_setup`, `osa_loop` and `osa_receive` functions.
See also other userprogs for examples.

Finally, include your new userprog near the beginning of
the *papaprog.ino* file and flash as shown above.

## Bugs and issues

You can report bugs and issues
[here on the issue tracker](https://github.com/devjonix/papalink-avr/issues).
If unsure that your issues are caused by the programs
of this repository,
feel free to ask us at [Forum DEV Joni](https://devjoni.com/forum/)
for help.

