
#ifdef MEGA_SHIELD
#define N_ZIF_PINS 28

int zif_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A10, A9, A11, A12, A13,
                  20, 19, 18, 17, 16, 15, 14,  2,  3,   4,  5,   6,   7,   8};
int LED_PIN = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello!");

  pinMode(LED_PIN, OUTPUT);

  for(int i=0; i<N_ZIF_PINS; i++) {
    pinMode(zif_pins[i], INPUT_PULLUP);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println();
  Serial.println("  1  2  3  4  5  6  7  8  9 10 11 12 13 14|15 16 17 18 19 20 21 22 23 24 25 26 27 28");
  for(int i=0; i<N_ZIF_PINS; i++) {
    Serial.print("  ");
    Serial.print(digitalRead(zif_pins[i]));
  }
  Serial.println();

  digitalWrite(LED_PIN, 1);
  delay(500);
  digitalWrite(LED_PIN, 0);
  delay(500);
}

#else

#include "common.h"
#include "eeprom.h"
#include "scom.h"

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
  read_bytes(&b,1,"Failed reading command.");
  return static_cast<Command>(b);
}

void write_ack()
{
  uint8_t b = (uint8_t)ACK;
  write_bytes(&b,1,"Failed writing ACK");
}

void check_ack()
{
  byte b;
  read_bytes(&b,1,"Failed reading response.");

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
        uint32_t data = read_u32();
        write_ack();
        data = ~data;
        write_u32(data);
        check_ack();
        write_ack();
      }
      break;
    case READ:
      {
        log_println("Received READ command");
        uint16_t offset = read_u16();
        uint16_t nbytes = read_u16();
        char buf[50];
        sprintf(buf,"Parameters: offset=%u nbytes=%u",offset,nbytes);
        log_println(buf);
        write_ack();
        gep_read_data(offset,nbytes);
        log_println("Read data");
        write_bytes(data,nbytes);
        log_println("Transmitted data");
        check_ack();
        write_ack();
      }
      break;
    case WRITE:
      {
        log_println("Received WRITE command");
        uint16_t offset = read_u16();
        uint16_t nbytes = read_u16();
        char buf[50];
        sprintf(buf,"Parameters: offset=%u nbytes=%u",offset,nbytes);
        log_println(buf);
        write_ack();
        read_bytes(data,nbytes);
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
#endif
