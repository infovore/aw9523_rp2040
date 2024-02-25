#pragma once

#include <pico/stdlib.h>
#include <hardware/i2c.h>

class AW9523 {
  uint8_t port0Configuration;
  uint8_t port1Configuration;
  uint8_t port0Inversion;
  uint8_t port1Inversion;
  uint8_t port0Value;
  uint8_t port1Value;
  i2c_inst_t *i2c;
  uint8_t i2cAddress;
  uint8_t *msgBuffer;

public:
  AW9523(i2c_inst_t *i2cPort, uint8_t addr);
  uint8_t fullAddress();
  void pinMode(uint8_t port, uint8_t pin, uint8_t mode);
  void portMode(uint8_t port, uint8_t modeMask);
  uint8_t readPort(uint8_t port);
  uint8_t readPin(uint8_t port, uint8_t pin);
  void writePort(uint8_t port, uint8_t value);
  void writePin(uint8_t port, uint8_t pin, uint8_t value);
  void invertPort(uint8_t port, uint8_t value);
  void invertPin(uint8_t port, uint8_t pin);
};
