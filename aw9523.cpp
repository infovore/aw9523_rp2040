#include "aw9523.h"

AW9523::AW9523(i2c_inst_t *i2cPort, uint8_t addr) : i2c(i2cPort), i2cAddress(addr) {
  port0Configuration = 0b11111111;
  port1Configuration = 0b11111111;
  port0Inversion = 0b00000000;
  port1Inversion = 0b00000000;
  msgBuffer = new uint8_t[2];
}

uint8_t AW9523::fullAddress() {
  return (0b0100 << 3 ) | i2cAddress;
}


void AW9523::pinMode(uint8_t port, uint8_t pin, uint8_t mode) {
  if(port > 1) return;
  if(port == 0) {
    if (mode == 0) {
      // output
      portMode(0, port0Configuration &= ~(1 << pin));
    } else {
      portMode(0, port0Configuration |= (1 << pin));
    }
  }

  if(port == 1) {
    if (mode == 0) {
      // output
      portMode(1, port1Configuration &= ~(1 << pin));
    } else {
      portMode(1, port1Configuration |= (1 << pin));
    }
  }
}

void AW9523::portMode(uint8_t port, uint8_t modeMask) {
  if(port > 1) return;
  if(port == 0) {
    port0Configuration = modeMask;
    msgBuffer[0] = 0x06;
    msgBuffer[1] = port0Configuration;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
  }
  if(port == 1) {
    port1Configuration = modeMask;
    msgBuffer[0] = 0x07;
    msgBuffer[1] = port1Configuration;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
  }
}

uint8_t AW9523::readPort(uint8_t port) {
  if(port > 1) return 0;

  if(port == 0) {
    msgBuffer[0] = 0x00;
    msgBuffer[1] = 0x00;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer, 1, false);
    i2c_read_blocking(i2c,fullAddress(), &port0Value, 1, false);
    return port0Value;
  } else {
    msgBuffer[0] = 0x01;
    msgBuffer[1] = 0x00;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer, 1, false);
    i2c_read_blocking(i2c,fullAddress(), &port1Value, 1, false);
    return port1Value;
  }
}

uint8_t AW9523::readPin(uint8_t port, uint8_t pin) {
  if(port > 1) return 0;
  if(port == 0) {
    readPort(0);
    return (port0Value >> pin) & 0x01;
  } else {
    readPort(1);
    return (port1Value >> pin) & 0x01;
  }
}

void AW9523::writePort(uint8_t port, uint8_t value) {
  if(port > 1) return;
  if(port == 0) {
    port0Value = value;
    msgBuffer[0] = 0x02;
    msgBuffer[1] = port0Value;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
  } else {
    port1Value = value;
    msgBuffer[0] = 0x03;
    msgBuffer[1] = port1Value;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
  }
}

void AW9523::writePin(uint8_t port, uint8_t pin, uint8_t value) {
  if(port > 1) return;
  if(port == 0) {
    if(value) {
      port0Value |= (1 << pin);
    } else {
      port0Value &= ~(1 << pin);
    }
    writePort(0,port0Value);
  } else {
    if(value) {
      port1Value |= (1 << pin);
    } else {
      port1Value &= ~(1 << pin);
    }
    writePort(1,port1Value);
  }
}


void AW9523::invertPort(uint8_t port, uint8_t value) {
  if(port > 1) return;
  if(port == 0) {
    port0Value = value;
    // TODO: write
    msgBuffer[0] = 0x04;
    msgBuffer[1] = port0Value;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
  } else {
    port1Value = value;
    msgBuffer[0] = 0x05;
    msgBuffer[1] = port1Value;
    i2c_write_blocking(i2c, fullAddress(), msgBuffer, 2, false);
  }
}

void AW9523::invertPin(uint8_t port, uint8_t pin) {
  if(port > 1) return;
  if(port == 0) {
    port0Inversion |= (1 << pin);
    invertPort(0,port0Inversion);
  } else {
    port1Inversion |= (1 << pin);
    invertPort(1,port1Inversion);
  }
}
