#include <assert.h>

#define N_ZIF_PINS 28

const int zif_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A10, A9, A11, A12, A13,
                        20, 19, 18, 17, 16, 15, 14,  2,  3,   4,  5,   6,   7,   8};

bool initialized = false;
int8_t n_data_pins = -1;
int8_t data_pins[MAX_N_DATA_PINS] = {-1};
int8_t write_enable_pin = -1;
int8_t chip_enable_pin = -1;
int8_t output_enable_pin = -1;
int8_t n_address_pins = -1;
int8_t address_pins[MAX_N_ADDRESS_PINS] = {-1};

#include "driver_common.h"

int8_t translate_pin(int8_t pin_offset, int8_t pin_id_on_chip) {
    const int8_t pin_id_on_zif = pin_id_on_chip+pin_offset;
    const int8_t actual_pin_id = zif_pins[pin_id_on_zif-1];
    return actual_pin_id;
}

void driver_setup_pins(const ChipConfig &conf) {
  assert(conf.n_pins <= N_ZIF_PINS);
  assert(conf.n_address_pins <= MAX_N_ADDRESS_PINS);
  assert(conf.n_data_pins <= MAX_N_DATA_PINS);

  // Setup pin mappings
  n_data_pins = conf.n_data_pins;
  n_address_pins = conf.n_address_pins;

  const int8_t pin_offset = (N_ZIF_PINS-conf.n_pins)/2;
  write_enable_pin = translate_pin(pin_offset, conf.write_enable_pin);
  chip_enable_pin = translate_pin(pin_offset, conf.chip_enable_pin);
  output_enable_pin = translate_pin(pin_offset, conf.output_enable_pin);

  for(int i=0; i<conf.n_data_pins; i++) {
    data_pins[i] = translate_pin(pin_offset, conf.data_pins[i]);
  }

  for(int i=0; i<conf.n_address_pins; i++) {
    address_pins[i] = translate_pin(pin_offset, conf.address_pins[i]);
  }

  // first set all pins to input
  for(int i=0; i<N_ZIF_PINS; i++) {
    pinMode(zif_pins[i], INPUT);
  }

  // Now set up  output pins (address, CE, OE, and WE)
  digitalWrite(chip_enable_pin, LOW);
  pinMode(chip_enable_pin, OUTPUT);
  driver_set_we(HIGH);
  pinMode(write_enable_pin, OUTPUT);
  driver_set_oe(HIGH);
  pinMode(output_enable_pin, OUTPUT);

  for(int i=0; i<n_address_pins; i++) {
    pinMode(address_pins[i], OUTPUT);
  }

  // hardly necessary...
  driver_setup_read_mode();
}

void driver_set_address(uint16_t addr)
{
  for(int i=0; i<n_address_pins; i++) {
    digitalWrite(address_pins[i], (addr >> i) & 1);
  }
}
