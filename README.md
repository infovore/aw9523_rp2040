# AW9523

A demo of the PCA9555 I2C GPIO expander for Raspberry Pi Pico

## Connections

Connect an AW9523 over I2C, connecting SDA/SCL to GPIO8/9, respectively.

Connect three push buttons to AW9523 Port 1, IOs 0-2. These need resistor pullups to VCC. The other side of the button should go to ground.

Connect the **cathodes** three LEDs to PCA9555 Port 0, IOs 0-2. Connect the **anode** to VCC. You do not need resistors.

Connect A0, A1, and A2 address pins of the PCA9555 to Ground.

Connect PCA9555 VDD to 3V3, and VSS to Ground.

## Usage

TODO TODO
