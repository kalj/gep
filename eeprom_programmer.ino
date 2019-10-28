#include "common.h"
#include "eeprom.h"
#include "scom.h"

// ----------------------------------------
// High-level functions
// ----------------------------------------

byte data[1024];

void ep_read(int n_bytes)
{
  for(int i=0; i<n_bytes; i++) {
    data[i] = read_byte(i);
  }
}

void ep_write_data(int n_bytes) {
  for(int i=0; i<n_bytes; i++) {
    write_byte(i,data[i]);
  }
}

void ep_fill(byte v, int n_bytes) {
  for(int i=0; i<n_bytes; i++) {
    data[i] = v;
  }
  ep_write_data(n_bytes);
}

void ep_clear(int n_bytes) {
  ep_fill(0,n_bytes);
}

void ep_set_range(int n_bytes) {
  for(int i=0; i<n_bytes; i++) {
    data[i] = i;
  }
  ep_write_data(n_bytes);
}

void process_cmd()
{
  if(scom_available()) {
    status_set(true);

    Command cmd = read_cmd();
    switch(cmd) {
    case READ:
      {
        int offset = (int)read_u16();
        long nbytes = (long)read_u16();
        write_ack();
        write_bytes(&memory[offset],nbytes);
        check_ack();
      }
      break;
    case WRITE:
      {
        int offset = (int)read_u16();
        long nbytes = (long)read_u16();
        write_ack();
        read_bytes(&memory[offset],nbytes);
        write_ack();
      }
      break;
    case CLEAR:
      clear_memory();
      write_ack();
      break;
    default:
      BAIL("Invalid command given");
      break;
    }
    status_set(false);
  }
}

void setup()
{
  scom_init();
  eeprom_init();
  status_init();

  status_set(true);
  delay(1000);
  status_set(false);
}


void loop()
{
  process_cmd();
  delay(250);
}