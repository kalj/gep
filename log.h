
#define LOG_BAUD_RATE 115200

#include <SoftwareSerial.h>
// RX GREEN, TX WHITE
#define LOG_RX_PIN A2
#define LOG_TX_PIN A3
#define LOG_DEV mySerial
SoftwareSerial LOG_DEV(LOG_RX_PIN, LOG_TX_PIN);


void log_println(const char *msg)
{
  LOG_DEV.println(msg);
}

void log_init() {
  LOG_DEV.begin(LOG_BAUD_RATE);
  log_println("log_init()");
}
