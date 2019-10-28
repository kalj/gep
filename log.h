
// RX GREEN, TX WHITE
#define LOG_RX_PIN A2
#define LOG_TX_PIN A3
#define LOG_BAUD_RATE 115200

#include <SoftwareSerial.h>

SoftwareSerial mySerial(LOG_RX_PIN, LOG_TX_PIN);

void log_println(const char *msg)
{
  mySerial.println(msg);
}

void log_init() {
  mySerial.begin(LOG_BAUD_RATE);
  log_println("log_init()");
}
