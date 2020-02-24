
#define MAX_N_ADDRESS_PINS 16
#define MAX_N_DATA_PINS 8

struct ChipConfig {
  int8_t n_pins;
  int8_t n_address_pins;
  int8_t address_pins[MAX_N_ADDRESS_PINS];
  int8_t write_enable_pin;
  int8_t chip_enable_pin;
  int8_t output_enable_pin;
  int8_t n_data_pins;
  int8_t data_pins[MAX_N_DATA_PINS];
};

#ifdef MEGA_SHIELD
#include "driver_mega_shield.h"
#else
#include "driver_ben_eater.h"
#endif

#define AT28C_POLL_DELAY_MS     2
#define AT28C_TIME_WRITE_US     1
#define AT28C_TIME_OUTPUT_US    1


void at28c_init(const ChipConfig &conf)
{
  driver_setup_pins(conf);
}

int32_t at28c_get_nbytes() {
  return 1 << n_address_pins;
}

byte at28c_read_byte(uint16_t addr)
{
  driver_setup_read_mode();
  driver_set_address(addr);

  driver_set_oe(HIGH);
  delayMicroseconds(AT28C_TIME_OUTPUT_US);

  driver_set_oe(LOW);
  delayMicroseconds(AT28C_TIME_OUTPUT_US);

  byte val = driver_get_data();

  driver_set_oe(HIGH);
  delayMicroseconds(AT28C_TIME_OUTPUT_US);

  return val;
}

void at28c_write_byte(uint16_t addr, byte data)
{
  driver_setup_write_mode();
  driver_set_address(addr);

  byte msb = data & 0x80;
  driver_set_data(data);

  driver_set_we(LOW);
  delayMicroseconds(AT28C_TIME_WRITE_US);
  driver_set_we(HIGH);

  // Poll for successful write
  byte b = at28c_read_byte(addr);
  while((b & 0x80) != msb) {
    delay(AT28C_POLL_DELAY_MS);
    b = at28c_read_byte(addr);
  }
}
