#include "eeprom.h"

// ----------------------------------------
// High-level functions
// ----------------------------------------

byte data[1024];
char buf[80];

#define BYTES_PER_LINE 16

void ep_read(int n_bytes)
{
  for(int i=0; i<n_bytes; i++) {
    data[i] = read_byte(i);
  }
}

void ep_dump(int n_bytes)
{
  int len = sprintf(buf," addr |");
  for(int i=0; i< BYTES_PER_LINE; i++) {
    len += sprintf(buf+len," %2d",i);
  }
  Serial.println(buf);

  for(int base=0; base<n_bytes; base += BYTES_PER_LINE) {
    len = sprintf(buf," %4d |",base);
    for(int i=0; i< BYTES_PER_LINE && (base+i)<n_bytes; i++) {
      byte b = read_byte(base+i);
      len += sprintf(buf+len," %02x",b);
    }
    Serial.println(buf);
  }
}

void ep_read_and_dump(int n_bytes)
{
  ep_read(n_bytes);
  int len = sprintf(buf," addr |");
  for(int i=0; i< BYTES_PER_LINE; i++) {
    len += sprintf(buf+len," %2d",i);
  }
  Serial.println(buf);

  for(int base=0; base<n_bytes; base += BYTES_PER_LINE) {
    len = sprintf(buf," %4d |",base);
    for(int i=0; i< BYTES_PER_LINE && (base+i)<n_bytes; i++) {
      len += sprintf(buf+len," %02x",data[base+i]);
    }
    Serial.println(buf);
  }
}

void ep_bindump(int n_bytes)
{
  Serial.println(" addr |     data");

  for(int a=0; a<n_bytes; a++) {
    byte byt = read_byte(a);
    sprintf(buf," %4d | %1d%1d%1d%1d%1d%1d%1d%1d", a,
            (byt >> 7) & 1, (byt >> 6) & 1, (byt >> 5) & 1, (byt >> 4) & 1,
            (byt >> 3) & 1, (byt >> 2) & 1, (byt >> 1) & 1, (byt >> 0) & 1);
    Serial.println(buf);
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

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  eeprom_init();

  delay(250);


//  Serial.println("Writing EEPROM...");
//  ep_clear(1024);

  Serial.println("Reading EEPROM...");
  ep_dump(128);
  Serial.println("Clearing EEPROM...");
  ep_set_range(256);
  Serial.println("Reading EEPROM...");
  ep_dump(128);
  Serial.println("Done!");
}

byte input[80];

void loop() {
  if(Serial.available() > 0) {
    Serial.read();
    Serial.println("Reading EEPROM...");
    ep_dump(512);
    Serial.println("Done!");
  }
}
