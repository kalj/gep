
#include "sreg.h"

// ------------------------------------------
// EEPROM chip (AT28C256)
// ------------------------------------------

#define N_ADDR_BITS              15
#define EEPROM_TIME_WRITE_US     1
#define EEPROM_TIME_OUTPUT_US    1
#define EEPROM_WRITE_EN          A0
#define EEPROM_OUTPUT_EN         A1

const byte DATA_PINS[8] = {5, 6, 7, 8, 9, 10, 11, 12};

void setup_read_mode()
{
  for(int i=0; i<8; i++) {
    pinMode(DATA_PINS[i],INPUT);
  }
}

void setup_write_mode()
{
  for(int i=0; i<8; i++) {
    pinMode(DATA_PINS[i],OUTPUT);
  }
}

void eeprom_init() {
  setup_read_mode();
  sreg_init();

  digitalWrite(EEPROM_OUTPUT_EN,HIGH);
  pinMode(EEPROM_OUTPUT_EN,OUTPUT);
  digitalWrite(EEPROM_WRITE_EN,HIGH);
  pinMode(EEPROM_WRITE_EN,OUTPUT);
}


void set_address(uint16_t addr)
{
  for(int i=0; i<N_ADDR_BITS; i++) {
    shift_bit((addr >> (N_ADDR_BITS-1-i)) & 1);
  }
  //shiftOut(SREG_DATA, SREG_SHIFT, MSBFIRST, addr >> 8);
  //shiftOut(SREG_DATA, SREG_SHIFT, MSBFIRST, addr);
  pulse_latch();
}

byte read_byte(uint16_t addr)
{
  setup_read_mode();
  set_address(addr);

  digitalWrite(EEPROM_OUTPUT_EN, HIGH);
  delayMicroseconds(EEPROM_TIME_OUTPUT_US);

  digitalWrite(EEPROM_OUTPUT_EN, LOW);
  delayMicroseconds(EEPROM_TIME_OUTPUT_US);

  byte val = 0;
  for(int i=0; i<8; i++) {
    val = val | (digitalRead(DATA_PINS[i]) << i);
  }

  digitalWrite(EEPROM_OUTPUT_EN, HIGH);
  delayMicroseconds(EEPROM_TIME_OUTPUT_US);

  return val;
}

void write_byte(uint16_t addr, byte data)
{
  setup_write_mode();
  set_address(addr);

  byte msb = data & 0x80;
  for(int i=0; i<8; i++) {
    byte write_bit = (data >> i) & 1;
    digitalWrite(DATA_PINS[i], write_bit);
  }
  digitalWrite(EEPROM_WRITE_EN, LOW);
  delayMicroseconds(EEPROM_TIME_WRITE_US);
  digitalWrite(EEPROM_WRITE_EN, HIGH);

  // Poll for successful write

  byte b = read_byte(addr);
  while((b & 0x80) != msb) {
    delay(2);
    b = read_byte(addr);
  }
}
