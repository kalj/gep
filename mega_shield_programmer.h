
#define EEPROM_TIME_WRITE_US     1
#define EEPROM_TIME_OUTPUT_US    1
#define POLL_DELAY_MS 2

#define N_ZIF_PINS 28
#define MAX_N_ADDRESS_PINS 16
#define MAX_N_DATA_PINS 8

const int zif_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A10, A9, A11, A12, A13,
                        20, 19, 18, 17, 16, 15, 14,  2,  3,   4,  5,   6,   7,   8};

// ------------------------------------------
// EEPROM chip (AT28C256)
// ------------------------------------------

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

ChipConfig at28c256 = {28, 15,
                       {10, 9, 8, 7, 6, 5, 4, 3, 25, 24, 21, 23, 2, 26, 1},
                       27, 20, 22, 8, {11, 12, 13, 15, 16, 17, 18, 19}};

ChipConfig at28c16 = {24, 11,
                      {8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19},
                      21, 18, 20, 8, {9, 10, 11, 13, 14, 15, 16, 17}};


// struct Config {
  bool initialized = false;
  int8_t n_data_pins = -1;
  int8_t data_pins[MAX_N_DATA_PINS] = {-1};
  int8_t write_enable_pin = -1;
  int8_t chip_enable_pin = -1;
  int8_t output_enable_pin = -1;
  int8_t n_address_pins = -1;
  int8_t address_pins[MAX_N_ADDRESS_PINS] = {-1};
// };


int8_t translate_pin(int8_t pin_offset, int8_t pin_id_on_chip) {
    const int8_t pin_id_on_zif = pin_id_on_chip+pin_offset;
    const int8_t actual_pin_id = zif_pins[pin_id_on_zif-1];
    return actual_pin_id;
}


void setup_pins(const ChipConfig &conf) {
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
  pinMode(write_enable_pin, OUTPUT);
  pinMode(chip_enable_pin, OUTPUT);
  pinMode(output_enable_pin, OUTPUT);

  for(int i=0; i<n_address_pins; i++) {
    pinMode(address_pins[i], OUTPUT);
  }
}

void setup_read_mode()
{
  for(int i=0; i<n_data_pins; i++) {
    pinMode(data_pins[i],INPUT);
  }
}

void setup_write_mode()
{
  for(int i=0; i<n_data_pins; i++) {
    pinMode(data_pins[i],OUTPUT);
  }
}

void eeprom_init() {
  setup_read_mode();

  digitalWrite(output_enable_pin,HIGH);
  pinMode(output_enable_pin,OUTPUT);
  digitalWrite(write_enable_pin,HIGH);
  pinMode(write_enable_pin,OUTPUT);
  digitalWrite(chip_enable_pin,LOW);
  pinMode(chip_enable_pin,OUTPUT);
}


void set_address(uint16_t addr)
{
  for(int i=0; i<n_address_pins; i++) {
    digitalWrite(address_pins[i], (addr >> i) & 1);
  }
}

byte read_byte(uint16_t addr)
{
  setup_read_mode();
  set_address(addr);

  digitalWrite(output_enable_pin, HIGH);
  delayMicroseconds(EEPROM_TIME_OUTPUT_US);

  digitalWrite(output_enable_pin, LOW);
  delayMicroseconds(EEPROM_TIME_OUTPUT_US);

  byte val = 0;
  for(int i=0; i<n_data_pins; i++) {
    val = val | (digitalRead(data_pins[i]) << i);
  }

  digitalWrite(output_enable_pin, HIGH);
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
  digitalWrite(write_enable_pin, LOW);
  delayMicroseconds(EEPROM_TIME_WRITE_US);
  digitalWrite(write_enable_pin, HIGH);

  // Poll for successful write
  byte b = read_byte(addr);
  while((b & 0x80) != msb) {
    delay(POLL_DELAY);
    b = read_byte(addr);
  }
}
