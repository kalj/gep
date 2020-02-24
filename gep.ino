
#include "common.h"
#include "scom.h"

#ifdef MEGA_SHIELD
#include "mega_shield_programmer.h"
#else
#include "nano_eater_programmer.h"
#endif


#define EEPROM_SIZE 32768

enum Command {
  READ=1,
  WRITE=2,
  CLEAR=3,
  HANDSHAKE=4
};

enum Response {
  ACK=10,
  ERROR=20
};

// -----------------------------------------------
// Helper functions for communication protocol
// -----------------------------------------------

Command read_cmd()
{
  byte b;
  scom_read_bytes(&b,1,"Failed reading command.");
  return static_cast<Command>(b);
}

void write_ack()
{
  uint8_t b = (uint8_t)ACK;
  scom_write_bytes(&b,1,"Failed writing ACK");
}

void check_ack()
{
  byte b;
  scom_read_bytes(&b,1,"Failed reading response.");

  Response resp = static_cast<Response>(b);
  if(resp != ACK) {
    char buf[30];
    sprintf(buf,"Did not receive ACK, instead %d",resp);
    BAIL(buf);
  }
}

// ----------------------------------------
// High-level EEPROM functions
// ----------------------------------------

#define DATA_SIZE 512
byte data[DATA_SIZE];

void gep_read_data(uint16_t offset, uint16_t n_bytes)
{
  for(uint16_t i=0; i<n_bytes; i++) {
    data[i] = read_byte(offset+i);
  }
}

void gep_write_data(uint16_t offset, uint16_t n_bytes) {
  for(uint16_t i=0; i<n_bytes; i++) {
    write_byte(offset+i,data[i]);
  }
}

void gep_fill(byte v) {
  for(uint16_t i=0; i<EEPROM_SIZE; i++) {
    write_byte(i,v);
  }
}

void gep_clear() {
  gep_fill(0xff);
}


// ----------------------------------------
// Main command handler
// ----------------------------------------

void process_cmd()
{
  if(scom_available()) {
    log_println("serial data available");
    status_set(true);

    Command cmd = read_cmd();
    switch(cmd) {
    case HANDSHAKE:
      {
        log_println("Received HANDSHAKE command");
        uint32_t data = scom_read_u32();
        write_ack();
        data = ~data;
        scom_write_u32(data);
        check_ack();
        write_ack();
      }
      break;
    case READ:
      {
        log_println("Received READ command");
        uint16_t offset = scom_read_u16();
        uint16_t nbytes = scom_read_u16();
        char buf[50];
        sprintf(buf,"Parameters: offset=%u nbytes=%u",offset,nbytes);
        log_println(buf);
        write_ack();
        gep_read_data(offset,nbytes);
        log_println("Read data");
        scom_write_bytes(data,nbytes);
        log_println("Transmitted data");
        check_ack();
        write_ack();
      }
      break;
    case WRITE:
      {
        log_println("Received WRITE command");
        uint16_t offset = scom_read_u16();
        uint16_t nbytes = scom_read_u16();
        char buf[50];
        sprintf(buf,"Parameters: offset=%u nbytes=%u",offset,nbytes);
        log_println(buf);
        write_ack();
        scom_read_bytes(data,nbytes);
        log_println("Received data");
        gep_write_data(offset,nbytes);
        log_println("Wrote data");
        write_ack();
      }
      break;
    case CLEAR:
      log_println("Received CLEAR command");
      gep_clear();
      write_ack();
      break;
    default:
      BAIL("Invalid command given");
      break;
    }
    log_println("Done processing command");
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

  log_println("Starting up");
}


void loop()
{
  process_cmd();
  delay(250);
}
