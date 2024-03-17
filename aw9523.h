#pragma once

#include <pico/stdlib.h>
#include <hardware/i2c.h>

#define AW9523_DEFAULT_ADDR 0x58 ///< The default I2C address for our breakout

#define AW9523_LED_MODE 0x99 ///< Special pinMode() macro for constant current

#define AW9523_INPUT 0x00
#define AW9523_OUTPUT 0x01
#define AW9523_REG_CHIPID 0x10     ///< Register for hardcode chip ID
#define AW9523_REG_SOFTRESET 0x7F  ///< Register for soft resetting
#define AW9523_REG_INPUT0 0x00     ///< Register for reading input values
#define AW9523_REG_OUTPUT0 0x02    ///< Register for writing output values
#define AW9523_REG_CONFIG0 0x04    ///< Register for configuring direction
#define AW9523_REG_INTENABLE0 0x06 ///< Register for enabling interrupt
#define AW9523_REG_GCR 0x11        ///< Register for general configuration
#define AW9523_REG_LEDMODE 0x12    ///< Register for configuring const current

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the AW9523 I2C GPIO expander
 */
class AW9523{
public:
  AW9523();
  AW9523(i2c_inst_t *i2cPort, uint8_t addr);
  ~AW9523();

  bool begin();
  bool reset(void);
  bool openDrainPortZero(bool od);
  uint8_t fullAddress();

  // All 16 pins at once
  bool outputGPIO(uint16_t pins);
  uint16_t inputGPIO(void);
  bool configureDirection(uint16_t pins);
  bool configureLEDMode(uint16_t pins);
  bool interruptEnableGPIO(uint16_t pins);

  // Individual pin control
  void pinMode(uint8_t pin, uint8_t mode);
  void digitalWrite(uint8_t pin, bool val);
  bool digitalRead(uint8_t pin);
  void analogWrite(uint8_t pin, uint8_t val);
  void enableInterrupt(uint8_t pin, bool en);

protected:
  i2c_inst_t *i2c = NULL; ///< Pointer to I2C bus interface
  uint8_t i2cAddress;
  uint8_t *msgBuffer;
  uint16_t pinValues;
  uint16_t interruptEnValues;
  uint16_t pinDirections;
  uint16_t ccMode;
};
