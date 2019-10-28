#define SCOM_TIMEOUT 5000
#define SCOM_BAUD_RATE 115200

enum Command {
  READ=1,
  WRITE=2,
  CLEAR=3,
};

enum Response {
  ACK=10,
  ERROR=20
};

void scom_init()
{
  Serial.begin(SCOM_BAUD_RATE);
  Serial.setTimeout(SCOM_TIMEOUT);
}

bool scom_available()
{
  return Serial.available()>0;
}

void write_bytes(byte *mem, long nbytes, const char *errmsg = NULL) {
  long bytesWritten = (long)Serial.write(mem,nbytes);
  if(bytesWritten != nbytes) {
    BAIL(errmsg!=NULL?errmsg:"Failed writing correct number of bytes");
  }
}

void read_bytes(byte *mem, long nbytes, const char *errmsg = NULL) {
  long  bytesRead = (long)Serial.readBytes((char*)mem,nbytes);
  if(bytesRead != nbytes) {
    BAIL(errmsg!=NULL?errmsg:"Failed reading correct number of bytes");
  }
}

Command read_cmd()
{
  byte b;
  read_bytes(&b,1,"Failed reading command.");
  return static_cast<Command>(b);
}

void write_ack()
{
  Serial.write(ACK);
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

uint16_t read_u16()
{
  char bts[2];
  read_bytes(bts,2,"Failed reading u16.");
  return ((uint16_t)bts[0]) | (((uint16_t)bts[1])<<8);
}

void write_u16(uint16_t v)
{
  uint8_t bts[2] = { (uint8_t)(v & 0xff), (uint8_t)((v>>8) & 0xff)};
  write_bytes(bts,2,"Failed writing u16");
}

uint32_t read_u32()
{
  char bts[4];
  read_bytes(bts,4,"Failed reading u32.");
  return ((uint32_t)bts[0]) | (((uint32_t)bts[1])<<8) | (((uint32_t)bts[2])<<16) | (((uint32_t)bts[3])<<24);
}

void write_u32(uint32_t v)
{
  uint8_t bts[4] = {(uint8_t) (v & 0xff), (uint8_t) ((v >> 8) & 0xff), (uint8_t) ((v >> 16) & 0xff), (uint8_t) ((v >> 24) & 0xff)};
  write_bytes(bts,4,"Failed writing u32");
}
