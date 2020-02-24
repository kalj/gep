
#include <assert.h>

#include "sreg.h"

const int8_t write_enable_pin = A0;
const int8_t output_enable_pin = A1;
const int8_t n_data_pins = 8;
const byte data_pins[8] = {5, 6, 7, 8, 9, 10, 11, 12};
int8_t n_address_pins = -1;
bool initialized = false;

#include "driver_common.h"

void driver_setup_pins(const ChipConfig &conf)
{
  sreg_init();

  n_address_pins = conf.n_address_pins;

  driver_set_oe(HIGH);
  pinMode(output_enable_pin,OUTPUT);
  driver_set_we(HIGH);
  pinMode(write_enable_pin,OUTPUT);

  driver_setup_read_mode();

  initialized = true;
}

void driver_set_address(uint16_t addr)
{
  for(int i=0; i<n_address_pins; i++) {
    sreg_shift_bit((addr >> (n_address_pins-1-i)) & 1);
  }
  //shiftOut(SREG_DATA, SREG_SHIFT, MSBFIRST, addr >> 8);
  //shiftOut(SREG_DATA, SREG_SHIFT, MSBFIRST, addr);
  sreg_pulse_latch();
}
