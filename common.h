#include "log.h"

#define STATUS_LED_PIN 13

void status_init()
{
  pinMode(STATUS_LED_PIN,OUTPUT);
}

void status_set(bool status)
{
  digitalWrite(STATUS_LED_PIN, status ? HIGH : LOW);
}

void error(const char *msg) {
  while(true) {
    log_println(msg);
    status_set(false);
    delay(250);
    status_set(true);
    delay(250);
    status_set(false);
    delay(250);
    status_set(true);
    delay(250);
  }
}

#define BAIL(MSG)                                       \
  do {                                                  \
    char bailbuf[80];                                   \
    sprintf(bailbuf,"Bailing in %s line %d: %s",        \
            __FUNCTION__,__LINE__,MSG);                 \
    error(bailbuf);                                     \
  } while(0)
