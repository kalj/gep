
#define SCOM_TIMEOUT 5000
#define SCOM_BAUD_RATE 9600

#define SCOM_DEV Serial

void scom_init()
{
  SCOM_DEV.begin(SCOM_BAUD_RATE);
  SCOM_DEV.setTimeout(SCOM_TIMEOUT);
}

bool scom_available()
{
  return SCOM_DEV.available()>0;
}

void write_bytes(byte *mem, long nbytes, const char *errmsg = NULL) {
  long bytesWritten = (long)SCOM_DEV.write(mem,nbytes);
  if(bytesWritten != nbytes) {
    if(errmsg) {
      BAIL(errmsg);
    }
    else {
      char buf[80];
      sprintf(buf,"Failed writing correct number of bytes, expected %ld, wrote %ld",nbytes,bytesWritten);
      BAIL(buf);
    }
  }
}

void read_bytes(byte *mem, long nbytes, const char *errmsg = NULL) {
  long  bytesRead = (long)SCOM_DEV.readBytes((char*)mem,nbytes);
  if(bytesRead != nbytes) {
    if(errmsg) {
      BAIL(errmsg);
    }
    else {
      char buf[80];
      sprintf(buf,"Failed reading correct number of bytes, expected %ld, got %ld",nbytes,bytesRead);
      BAIL(buf);
    }
  }
}

uint16_t read_u16()
{
  uint8_t bts[2];
  read_bytes((char *)bts,2,"Failed reading u16.");
  return ((uint16_t)bts[0]) | (((uint16_t)bts[1])<<8);
}

void write_u16(uint16_t v)
{
  uint8_t bts[2] = { (uint8_t)(v & 0xff), (uint8_t)((v>>8) & 0xff)};
  write_bytes(bts,2,"Failed writing u16");
}

uint32_t read_u32()
{
  uint8_t bts[4];
  read_bytes((char*)bts,4,"Failed reading u32.");
  return ((uint32_t)bts[0]) | (((uint32_t)bts[1])<<8) | (((uint32_t)bts[2])<<16) | (((uint32_t)bts[3])<<24);
}

void write_u32(uint32_t v)
{
  uint8_t bts[4] = {(uint8_t) (v & 0xff), (uint8_t) ((v >> 8) & 0xff), (uint8_t) ((v >> 16) & 0xff), (uint8_t) ((v >> 24) & 0xff)};
  write_bytes(bts,4,"Failed writing u32");
}
