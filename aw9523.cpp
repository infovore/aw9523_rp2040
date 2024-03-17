/*!
 *  AW9523 library. Ported from Adafruit. 
 */

#include "aw9523.h"
#include "pico/stdio.h"
#include "hardware/i2c.h"
#include <stdio.h>

/*!
 *    @brief  Instantiates a new AW9523 class
 */
AW9523::AW9523(i2c_inst_t *i2cPort, uint8_t addr) : i2c(i2cPort), i2cAddress(addr) {
  msgBuffer = new uint8_t[2];
  pinValues = 0;
  interruptEnValues = 0;
  pinDirections = 0;
  ccMode = 0;
}

AW9523::~AW9523(void) {}


/*!
 *    @brief  Get the full I2C address for the expander based on the 3-bit set address
 *    @return Full I2C address as integer
 */
uint8_t AW9523::fullAddress() {
  return (AW9523_DEFAULT_ADDR) | i2cAddress;
}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  addr The I2C address for the expander
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @return True if initialization was successful, otherwise false.
 */
bool AW9523::begin() {
  // check chip id
  msgBuffer[0] = AW9523_REG_CHIPID;
  uint8_t chipIdRetval;
  i2c_write_blocking(i2c, fullAddress(), msgBuffer, 1, false);
  i2c_read_blocking(i2c, fullAddress(), &chipIdRetval, 1, false);

  if (chipIdRetval != 0x23) {
    return false;
  }

  configureDirection(0x0); // all inputs!
  openDrainPortZero(false);   // push pull default
  interruptEnableGPIO(0);  // no interrupt

  return true;
}

/*!
 *    @brief  Perform a soft reset over I2C
 *    @return True I2C reset command was acknowledged
 */
bool AW9523::reset(void) {
  msgBuffer[0] = AW9523_REG_SOFTRESET;
  msgBuffer[1] = 0;
  return i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
}

/*!
 *    @brief  Sets output value (1 == high) for all 16 GPIO
 *    @param  pins 16-bits of binary output settings
 *    @return True I2C write command was acknowledged
 */
bool AW9523::outputGPIO(uint16_t pins) {
  msgBuffer[0] = AW9523_REG_OUTPUT0;
  msgBuffer[1] = pins & 0xFF;
  uint8_t result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

  if (!result) {
    return false;
  }

  msgBuffer[0] = AW9523_REG_OUTPUT0 + 1;
  msgBuffer[1] = pins >> 8;
  result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

  if (!result) {
    return false;
  }

  pinValues = pins;
  return true;
}

/*!
 *    @brief  Reads input value (1 == high) for all 16 GPIO
 *    @return 16-bits of binary input (0 == low & 1 == high)
 */
uint16_t AW9523::inputGPIO(void) {
  uint8_t input0reg, input1reg;
  // tell port0 we want to read from it
  msgBuffer[0] = AW9523_REG_INPUT0;
  msgBuffer[1] = 0x00;
  i2c_write_blocking(i2c, fullAddress(), msgBuffer, 1, false);
  // and now read from it
  i2c_read_blocking(i2c,fullAddress(), &input0reg, 1, false);

  msgBuffer[0] = AW9523_REG_INPUT0 + 1;
  msgBuffer[1] = 0x00;
  i2c_write_blocking(i2c, fullAddress(), msgBuffer, 1, false);
  // and now read from it
  i2c_read_blocking(i2c,fullAddress(), &input1reg, 1, false);

  pinValues = ((uint16_t)input1reg << 8) | (uint16_t)input0reg;
  return pinValues;
}

/*!
 *    @brief  Sets interrupt watch for all 16 GPIO, 1 == enabled
 *    @param  pins 16-bits of pins we may want INT enabled
 *    @return True I2C write command was acknowledged
 */
bool AW9523::interruptEnableGPIO(uint16_t pins) {
  msgBuffer[0] = AW9523_REG_INTENABLE0;
  msgBuffer[1] = ~(pins & 0xFF);
  uint8_t result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

  if (!result) {
    return false;
  }

  printf("Found device at address 0x%x\n", fullAddress());

  msgBuffer[0] = AW9523_REG_INTENABLE0 + 1;
  msgBuffer[1] = ~(pins >> 8);
  result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

  if (!result) {
    return false;
  }

  interruptEnValues = pins;
  return true;
}

/*!
 *    @brief  Sets direction for all 16 GPIO, 1 == output, 0 == input
 *    @param  pins 16-bits of pins we want to set direction
 *    @return True I2C write command was acknowledged
 */
bool AW9523::configureDirection(uint16_t pins) {
  printf("Configuring direction for bank 0");
  msgBuffer[0] = AW9523_REG_CONFIG0;
  msgBuffer[1] = ~(pins & 0xFF);
  uint8_t result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

  if (!result) {
    return false;
  }
  printf("Configured");

  printf("Configuring direction for bank 1");
  msgBuffer[0] = AW9523_REG_CONFIG0 + 1;
  msgBuffer[1] = ~(pins >> 8);
  result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

  if (!result) {
    return false;
  }
  printf("Configured");
  pinDirections = pins;
  return true;
}

/*!
 *    @brief  Sets constant-current mode for all 16 GPIO
 *    @param  pins 16-bits of pins we want to set CC mode (1 == enabled)
 *    @return True I2C write command was acknowledged
 */
bool AW9523::configureLEDMode(uint16_t pins) {
  msgBuffer[0] = AW9523_REG_LEDMODE;
  msgBuffer[1] = ~(pins & 0xFF);
  uint8_t result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

  if (!result) {
    return false;
  }

  msgBuffer[0] = AW9523_REG_LEDMODE + 1;
  msgBuffer[1] = ~(pins >> 8);
  result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

  if (!result) {
    return false;
  }
  ccMode = pins;
  return true;
}

/*!
 *    @brief  Sets constant-current setting for one pin
 *    @param  pin GPIO to set, from 0 to 15 inclusive
 *    @param  val Ratio to set, from 0 (off) to 255 (max current)
 */
void AW9523::analogWrite(uint8_t pin, uint8_t val) {
  uint8_t reg;

  // See Table 13. 256 step dimming control register
  if (pin <= 7) {
    reg = 0x24 + pin;
  } else if ((pin >= 8) && (pin <= 11)) {
    reg = 0x20 + pin - 8;
  } else if ((pin >= 12) && (pin <= 15)) {
    reg = 0x2C + pin - 12;
  } else {
    return; // failed
  }

  msgBuffer[0] = reg;
  msgBuffer[1] = val;
  i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);
}

/*!
 *    @brief  Sets digital output for one pin
 *    @param  pin GPIO to set, from 0 to 15 inclusive
 *    @param  val True for high value, False for low value
 */
void AW9523::digitalWrite(uint8_t pin, bool val) {
  // clear the bit at `pin`, and then set it to `val`
  uint16_t newPinvalues =  (pinValues & ~(1 << pin)) | (val << pin);

  uint8_t outputReg = AW9523_REG_OUTPUT0 + (pin / 8);
  uint8_t outputVal = newPinvalues & 0xFF;
  if(pin > 7) {
    outputVal = newPinvalues >> 8;
  }

  msgBuffer[0] = outputReg;
  msgBuffer[1] = newPinvalues;
  uint8_t result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

  if(result) {
    pinValues = newPinvalues;
  }
}

/*!
 *    @brief  Reads digital input for one pin
 *    @param  pin GPIO to set, from 0 to 15 inclusive
 *    @returns True for high value read, False for low value read
 */
bool AW9523::digitalRead(uint8_t pin) {
  uint8_t inputreg;
  // tell port0 we want to read from it
  msgBuffer[0] = AW9523_REG_INPUT0 + (pin / 8);
  msgBuffer[1] = 0x00;
  i2c_write_blocking(i2c, fullAddress(), msgBuffer, 1, false);
  // and now read from it
  i2c_read_blocking(i2c,fullAddress(), &inputreg, 1, false);

  return (inputreg >> (pin % 8)) & 0x01;
}

/*!
 *    @brief  Sets interrupt enable for one pin
 *    @param  pin GPIO to set, from 0 to 15 inclusive
 *    @param  en True to enable Interrupt detect, False for ignore
 */
void AW9523::enableInterrupt(uint8_t pin, bool en) {
  uint16_t newInterruptEnValues =  (interruptEnValues & ~(1 << pin)) | (en << pin);

  uint8_t outputReg = AW9523_REG_INTENABLE0 + (pin / 8);
  uint8_t outputVal = newInterruptEnValues & 0xFF;
  if(pin > 7) {
    outputVal = newInterruptEnValues >> 8;
  }

  msgBuffer[0] = outputReg;
  msgBuffer[1] = newInterruptEnValues;
  uint8_t result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

  if(result) {
    interruptEnValues = newInterruptEnValues;
  }
}

/*!
 *    @brief  Sets pin mode / direction for one pin
 *    @param  pin GPIO to set, from 0 to 15 inclusive
 *    @param  mode Can be INPUT, OUTPUT for GPIO digital, or AW9523_LED_MODE for
 * constant current LED drive
 */
void AW9523::pinMode(uint8_t pin, uint8_t mode) {
  uint8_t directionRegister = AW9523_REG_CONFIG0 + (pin / 8);
  uint8_t ledModeRegister = AW9523_REG_LEDMODE + (pin / 8);

  if (mode == OUTPUT) {
    msgBuffer[0] = directionRegister;
    msgBuffer[1] = 0x00;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);
    msgBuffer[0] = ledModeRegister;
    msgBuffer[1] = 0x01;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

    // pin direction for this pin is output
    pinDirections =  (pinValues & ~(1 << pin)) | (1 << pin);
    ccMode =  (pinValues & ~(1 << pin)) | (0 << pin);
  }
  if (mode == INPUT) {
    msgBuffer[0] = directionRegister;
    msgBuffer[1] = 0x01;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);
    msgBuffer[0] = ledModeRegister;
    msgBuffer[1] = 0x01;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

    // pin direction for this pin is input
    pinDirections =  (pinValues & ~(1 << pin)) | (0 << pin);
    ccMode =  (pinValues & ~(1 << pin)) | (0 << pin);
  }

  if (mode == AW9523_LED_MODE) {
    msgBuffer[0] = directionRegister;
    msgBuffer[1] = 0x00;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);
    msgBuffer[0] = ledModeRegister;
    msgBuffer[1] = 0x00;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);

    // pin direction for this pin is output
    pinDirections =  (pinValues & ~(1 << pin)) | (1 << pin);
    ccMode =  (pinValues & ~(1 << pin)) | (1 << pin);
  }
}

/*!
 *    @brief  Turns on/off open drain output for ALL port 0 pins (GPIO 0-7)
 *    @param  od True to enable open drain, False for push-pull
 *    @return True if I2C write command was acknowledged, otherwise false
 */
bool AW9523::openDrainPortZero(bool od) {
  uint8_t outputReg = AW9523_REG_GCR;

  uint8_t configRegValue;
  msgBuffer[0] = outputReg;
  msgBuffer[1] = 0x00;
  i2c_write_blocking(i2c, fullAddress(), msgBuffer, 1, true);
  i2c_read_blocking(i2c, fullAddress(), &configRegValue, 1, false);

  msgBuffer[0] = outputReg;
  msgBuffer[1] = (configRegValue & ~(1 << 4)) | (!od << 4);
  uint8_t result = i2c_write_blocking(i2c, fullAddress(), msgBuffer,2, false);
  return result;
}

// this is tom's old PCA9555 class
// AW9523::AW9523(i2c_inst_t *i2cPort, uint8_t addr) : i2c(i2cPort), i2cAddress(addr) {
//   port0Configuration = 0b11111111;
//   port1Configuration = 0b11111111;
//   port0Inversion = 0b00000000;
//   port1Inversion = 0b00000000;
//   msgBuffer = new uint8_t[2];
// }

// uint8_t AW9523::fullAddress() {
//   return (0b0100 << 3 ) | i2cAddress;
// }


// void AW9523::pinMode(uint8_t port, uint8_t pin, uint8_t mode) {
//   if(port > 1) return;
//   if(port == 0) {
//     if (mode == 0) {
//       // output
//       portMode(0, port0Configuration &= ~(1 << pin));
//     } else {
//       portMode(0, port0Configuration |= (1 << pin));
//     }
//   }

//   if(port == 1) {
//     if (mode == 0) {
//       // output
//       portMode(1, port1Configuration &= ~(1 << pin));
//     } else {
//       portMode(1, port1Configuration |= (1 << pin));
//     }
//   }
// }

// void AW9523::portMode(uint8_t port, uint8_t modeMask) {
//   if(port > 1) return;
//   if(port == 0) {
//     port0Configuration = modeMask;
//     msgBuffer[0] = 0x06;
//     msgBuffer[1] = port0Configuration;
//     i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
//   }
//   if(port == 1) {
//     port1Configuration = modeMask;
//     msgBuffer[0] = 0x07;
//     msgBuffer[1] = port1Configuration;
//     i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
//   }
// }

// uint8_t AW9523::readPort(uint8_t port) {
//   if(port > 1) return 0;

//   if(port == 0) {
//     msgBuffer[0] = 0x00;
//     msgBuffer[1] = 0x00;
//     i2c_write_blocking(i2c, fullAddress(), msgBuffer, 1, false);
//     i2c_read_blocking(i2c,fullAddress(), &port0Value, 1, false);
//     return port0Value;
//   } else {
//     msgBuffer[0] = 0x01;
//     msgBuffer[1] = 0x00;
//     i2c_write_blocking(i2c, fullAddress(), msgBuffer, 1, false);
//     i2c_read_blocking(i2c,fullAddress(), &port1Value, 1, false);
//     return port1Value;
//   }
// }

// uint8_t AW9523::readPin(uint8_t port, uint8_t pin) {
//   if(port > 1) return 0;
//   if(port == 0) {
//     readPort(0);
//     return (port0Value >> pin) & 0x01;
//   } else {
//     readPort(1);
//     return (port1Value >> pin) & 0x01;
//   }
// }

// void AW9523::writePort(uint8_t port, uint8_t value) {
//   if(port > 1) return;
//   if(port == 0) {
//     port0Value = value;
//     msgBuffer[0] = 0x02;
//     msgBuffer[1] = port0Value;
//     i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
//   } else {
//     port1Value = value;
//     msgBuffer[0] = 0x03;
//     msgBuffer[1] = port1Value;
//     i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
//   }
// }

// void AW9523::writePin(uint8_t port, uint8_t pin, uint8_t value) {
//   if(port > 1) return;
//   if(port == 0) {
//     if(value) {
//       port0Value |= (1 << pin);
//     } else {
//       port0Value &= ~(1 << pin);
//     }
//     writePort(0,port0Value);
//   } else {
//     if(value) {
//       port1Value |= (1 << pin);
//     } else {
//       port1Value &= ~(1 << pin);
//     }
//     writePort(1,port1Value);
//   }
// }


// void AW9523::invertPort(uint8_t port, uint8_t value) {
//   if(port > 1) return;
//   if(port == 0) {
//     port0Value = value;
//     // TODO: write
//     msgBuffer[0] = 0x04;
//     msgBuffer[1] = port0Value;
//     i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
//   } else {
//     port1Value = value;
//     msgBuffer[0] = 0x05;
//     msgBuffer[1] = port1Value;
//     i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
//   }
// }

// void AW9523::invertPin(uint8_t port, uint8_t pin) {
//   if(port > 1) return;
//   if(port == 0) {
//     port0Inversion |= (1 << pin);
//     invertPort(0,port0Inversion);
//   } else {
//     port1Inversion |= (1 << pin);
//     invertPort(1,port1Inversion);
//   }
// }
