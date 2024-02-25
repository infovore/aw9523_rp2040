#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "PicoDebounceButton.hpp"

#include "aw9523.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

int main() {
  stdio_init_all();

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400*1000);
  
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  // set gpio 2 as an input
  gpio_init(2);
  gpio_set_dir(2, GPIO_IN);
  gpio_pull_up(2);

  picodebounce::PicoDebounceButton otherButton(2, 10, false, false);

  picodebounce::PicoDebounceButton buttons[3] = {
    picodebounce::PicoDebounceButton(10,false,false),
    picodebounce::PicoDebounceButton(10,false,false),
    picodebounce::PicoDebounceButton(10,false,false),
  };

  uint8_t buttonStates[3] = {0,0,0};

  AW9523 aw9523(I2C_PORT, 0x00);

  // pca9555.pinMode(0,0,0);
  // pca9555.pinMode(0,1,0);
  // pca9555.pinMode(0,2,0);
  aw9523.portMode(0,0);
  aw9523.writePort(0,0);

  aw9523.readPort(1);

  while(1) {
    if(otherButton.update()) {
      printf("%d", otherButton.getState());
      if(otherButton.getState() == 0) {
        buttonStates[0] = !buttonStates[0];
      }
    }
    for(uint8_t i = 0; i < 3; i++) {
      uint8_t val = aw9523.readPin(1,i);
      if(buttons[i].update(val)) {
        if(buttons[i].getState() == picodebounce::PicoDebounceButton::PRESSED) {
          buttonStates[i] = !buttonStates[i];
        }
      }
      aw9523.writePin(0, i, buttonStates[i]);
    }
  }

  return 0;
}
