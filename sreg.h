
// ------------------------------------------
// Shift registers
// ------------------------------------------

#define SREG_DATA          2
#define SREG_SHIFT         3
#define SREG_LATCH         4
#define SREG_LATCH_TIME_US 1
#define SREG_SHIFT_TIME_US 1
#define SREG_SETUP_TIME_US 1

void sreg_init()
{
  pinMode(SREG_DATA,OUTPUT);
  digitalWrite(SREG_SHIFT, HIGH);
  pinMode(SREG_SHIFT,OUTPUT);
  digitalWrite(SREG_LATCH, HIGH);
  pinMode(SREG_LATCH,OUTPUT);
  delayMicroseconds(SREG_LATCH_TIME_US+SREG_SHIFT_TIME_US);
}

void sreg_pulse_latch()
{
  // latch pin is assumed to always be in HIGH state outside of this function
  digitalWrite(SREG_LATCH,LOW);
  delayMicroseconds(SREG_LATCH_TIME_US);
  digitalWrite(SREG_LATCH,HIGH);
  delayMicroseconds(SREG_LATCH_TIME_US);
}

void sreg_shift_bit(byte b)
{
  digitalWrite(SREG_DATA,b);
  delayMicroseconds(SREG_SETUP_TIME_US);
  // shift pin is assumed to be in HIGH state outside of this function
  digitalWrite(SREG_SHIFT,LOW);
  delayMicroseconds(SREG_SHIFT_TIME_US);
  digitalWrite(SREG_SHIFT,HIGH);
  delayMicroseconds(SREG_SHIFT_TIME_US);
}
