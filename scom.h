
enum Command {
  READ=1,
  WRITE=2,
  CLEAR=3,
};

enum Response {
  ACK=10,
  ERROR=20
};

Command read_cmd()
{
  return static_cast<Command>(Serial.read());
}

void write_ack()
{
  Serial.write(ACK);
}
void check_ack()
{
  Response resp = static_cast<Response>(Serial.read());
  if(resp != ACK) {
    char buf[30];
    sprintf(buf,"Did not receive ACK, instead %d",resp);
    BAIL(buf);
  }
}

uint16_t read_u16()
{
  char bts[2];
  Serial.readBytes(bts,2);
  return ((uint16_t)bts[0]) | (((uint16_t)bts[1])<<8);
}

uint32_t read_u32()
{
  char bts[4];
  Serial.readBytes(bts,4);
  return ((uint32_t)bts[0]) | (((uint32_t)bts[1])<<8) | (((uint32_t)bts[2])<<16) | (((uint32_t)bts[3])<<24);
}

void write_u32(uint32_t v)
{
  uint8_t bts[4] = {(uint8_t) (v & 0xff), (uint8_t) ((v >> 8) & 0xff), (uint8_t) ((v >> 16) & 0xff), (uint8_t) ((v >> 24) & 0xff)};
  Serial.write(bts,4);
}

void write_bytes(byte *mem, long nbytes) {
  long bytesWritten = (long)Serial.write(mem,nbytes);
  if(bytesWritten != nbytes) {
    BAIL("Failed writing correct number of bytes");
  }
}

void read_bytes(byte *mem, long nbytes) {
  long  bytesRead = (long)Serial.readBytes((char*)mem,nbytes);
  if(bytesRead != nbytes) {
    BAIL("Failed reading correct number of bytes");
  }
}


void scom_init()
{
  Serial.begin(115200);
}

bool scom_available()
{
  return Serial.available()>0;
}
