#define STATUS_LED 13

void status_init()
{
  pinMode(STATUS_LED,OUTPUT);
}

void status_set(bool status)
{
  digitalWrite(STATUS_LED, status ? HIGH : LOW);
}

void error(const char *msg) {
  while(true) {
    Serial.println(msg);
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
