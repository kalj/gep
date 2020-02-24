void driver_setup_read_mode()
{
  for(int i=0; i<n_data_pins; i++) {
    pinMode(data_pins[i],INPUT);
  }
}

void driver_setup_write_mode()
{
  for(int i=0; i<n_data_pins; i++) {
    pinMode(data_pins[i],OUTPUT);
  }
}

void driver_set_oe(byte b) {
  digitalWrite(output_enable_pin, b);
}

void driver_set_we(byte b) {
  digitalWrite(write_enable_pin, b);
}

void driver_set_data(byte data) {
  for(int i=0; i<8; i++) {
    byte write_bit = (data >> i) & 1;
    digitalWrite(data_pins[i], write_bit);
  }
}

byte driver_get_data() {
  byte val;
  for(int i=0; i<n_data_pins; i++) {
    val = val | (digitalRead(data_pins[i]) << i);
  }
  return val;
}
