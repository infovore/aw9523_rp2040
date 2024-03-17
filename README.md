# AW9523 for RP2040

A demo of the AW9523 I2C GPIO expander for RP2040. The library is ported from the [Adafruit Library for this](https://github.com/adafruit/Adafruit_AW9523); I've removed the `Adafruit_I2CRegister` abstraction provided by `Adafruit_BusIO` and just used raw Pico SDK i2c commands.

Currently this is configured for a `pico_w` in `CMakeLists.txt`; you should probably change that to `pico` for a regular board. I just had a Pico W on hand to test this with.

## Connections

Connect an AW9523 over I2C, connecting SDA/SCL to GPIO4/5, respectively.

Connect a push button between VCC and AW9523 pin 8. Connect a pull-up resistor between pin 8 and VCC. 

Connect the **cathodes** of four LEDs to AW9523 IOs 1-4. Connect the **anode** to VCC. You do not need resistors.

Connect AW9523 VDD to 3V3, and GND to Ground.

## Usage

By default, the demo will light LEDs on pins 1-4 at about 25% brightness.

Push the button to toggle to cycling them through all brightnesses; each LED will be a little out of phase, creating a scrolling effect.

## Library Usage

You need `aw9523.cpp` and `aw9523.h` included in your project via CMake, and then you'll be able to use and instantiate the library.
