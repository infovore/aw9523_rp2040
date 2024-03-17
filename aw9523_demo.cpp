#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "PicoDebounceButton.hpp"

#include "aw9523.h"

// I2C defines
// This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4 
#define I2C_SCL 5 

uint8_t gammaIndex = 0;

const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

int main() {
  stdio_init_all();

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400*1000);
  
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  // set gpio 2 as an input
  // gpio_init(2);
  // gpio_set_dir(2, GPIO_IN);
  // gpio_pull_up(2);

  // picodebounce::PicoDebounceButton otherButton(2, 10, false, false);

  // picodebounce::PicoDebounceButton buttons[3] = {
  //   picodebounce::PicoDebounceButton(10,false,false),
  //   picodebounce::PicoDebounceButton(10,false,false),
  //   picodebounce::PicoDebounceButton(10,false,false),
  // };

  // uint8_t buttonStates[3] = {0,0,0};

  AW9523 aw9523(&i2c0_inst, 0x00);
  aw9523.begin();

  aw9523.pinMode(1,AW9523_LED_MODE);
  aw9523.pinMode(2,AW9523_LED_MODE);
  aw9523.pinMode(3,AW9523_LED_MODE);
  aw9523.pinMode(4,AW9523_LED_MODE);

  // pca9555.pinMode(0,0,0);
  // pca9555.pinMode(0,1,0);
  // pca9555.pinMode(0,2,0);
  // aw9523.portMode(0,0);
  // aw9523.writePort(0,0);

  // aw9523.readPort(1);

  while(1) {
    for(uint8_t i = 1; i < 4; i++) {
      aw9523.analogWrite(i, gamma8[gammaIndex]);
    }
    // sleep for 250ms
    sleep_ms(250);
    gammaIndex = (gammaIndex + 16) % 255;
    // if(otherButton.update()) {
    //   printf("%d", otherButton.getState());
    //   if(otherButton.getState() == 0) {
    //     buttonStates[0] = !buttonStates[0];
    //   }
    // }
    // for(uint8_t i = 0; i < 3; i++) {
    //   uint8_t val = aw9523.readPin(1,i);
    //   if(buttons[i].update(val)) {
    //     if(buttons[i].getState() == picodebounce::PicoDebounceButton::PRESSED) {
    //       buttonStates[i] = !buttonStates[i];
    //     }
    //   }
    //   aw9523.writePin(0, i, buttonStates[i]);
    // }
  }

  return 0;
}
