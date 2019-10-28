#include "common.h"
#include "eeprom.h"
#include "scom.h"

// ----------------------------------------
// High-level functions
// ----------------------------------------

byte data[1024];

void gep_read(int n_bytes)
{
  for(int i=0; i<n_bytes; i++) {
    data[i] = read_byte(i);
  }
}

void gep_write_data(int n_bytes) {
  for(int i=0; i<n_bytes; i++) {
    write_byte(i,data[i]);
  }
}

void gep_fill(byte v, int n_bytes) {
  for(int i=0; i<n_bytes; i++) {
    data[i] = v;
  }
  gep_write_data(n_bytes);
}

void gep_clear(int n_bytes) {
  gep_fill(0,n_bytes);
}

void gep_set_range(int n_bytes) {
  for(int i=0; i<n_bytes; i++) {
    data[i] = i;
  }
  gep_write_data(n_bytes);
}

void process_cmd()
{
  if(scom_available()) {
    log_println("serial data available");
    status_set(true);

    Command cmd = read_cmd();
    switch(cmd) {
    case HANDSHAKE:
      {
        char msgbuf[40];
        log_println("Received HANDSHAKE command");
        uint32_t data = read_u32();
        write_ack();
        data = ~data;
        write_u32(data);
        check_ack();
      }
      break;
    case READ:
      {
        log_println("Received READ command");
        int offset = (int)read_u16();
        long nbytes = (long)read_u16();
        write_ack();
        gep_read(nbytes);
        write_bytes(data,nbytes);
        gep_write_data(nbytes);
        check_ack();
      }
      break;
    case WRITE:
      {
        log_println("Received WRITE command");
        int offset = (int)read_u16();
        long nbytes = (long)read_u16();
        write_ack();
        read_bytes(data,nbytes);
        gep_write_data(nbytes);
        write_ack();
      }
      break;
    case CLEAR:
      log_println("Received CLEAR command");
      gep_clear(1024);
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
  log_init();
  status_init();
  scom_init();
  eeprom_init();

  status_set(true);
  delay(1000);
  status_set(false);
}


void loop()
{
  process_cmd();
  delay(250);
}
