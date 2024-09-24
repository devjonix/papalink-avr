# PapaLink AVR programs

PapaLink is an open-source connector and communication protocol
designed for scientific and industrial use.
It is developed by DEV Joni since 2023.

Programs or firmware in this repository are designed to be run on
AVR microcontrollers and build within the Arduino development
enviroment (tested v1.8.19).

Contents

- contprog: PapaLink AVR USB-Controller Firmware
- papaprog: PapaLink AVR Device Firmware

Current protocol version: **0** (unreleased)

# Hardware

In theory, you can use any micro-controller that
supports I2C and has enough free pins and adapt
the code provided here to your project.
We have tested the code to work on an ATMega4809 MCU
present on the Arduino Nano Every development board.

The USB controller firmware also requies the MCU to have
serial capability.

PapaLink connectors, cables, hubs etc. are
available at [Shop DEV Joni](https://devjoni.com/shop/).
You can also build them
yourself provided you have some tools and materials.


# Flashing firmware

Example 1. Flash to Nano Every on Linux

```bash
arduino --upload papaprog.ino --board arduino:megaavr:nona4809 --port /dev/ttyACM0
```

# Modifying the papaprog for your own project

It is easiest to start by copying the skeleton program

```
cp userprog_skeleton.h userprog_myproject.h
```

and then modify it.
Your own functionality will be called from within the
`osa_setup`, `osa_loop` and `osa_receive` functions.

Finally, include your userprog near the beginning of
the *papaprog.ino* file and flash as above.

# Bugs and issues

You can report bugs and issues [here on the issue tracker].
If unsure that your issues are because of the AVR programs
in this repository,
feel free to ask people [Forum DEV Joni](https://devjoni.com/forum/)
for help.

