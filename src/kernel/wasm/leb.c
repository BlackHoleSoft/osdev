#include "./leb.h"


u64 prepareSignedValue(long value) {
    return (u64)value;
}

u8 writeULeb128(char buffer[BUFF_SIZE], u64 val) {    
    u8 count = 0;

    do {
        u8 byte = val & 0x7f;
        val >>= 7;

        if (val != 0)
        byte |= 0x80;  // mark this byte to show that more bytes will follow

        buffer[BUFF_SIZE - 1 - count] = byte;
        count++;
    } while (val != 0);

    for (int i=0; i<BUFF_SIZE; i++) {
        buffer[i] = i < count ? buffer[BUFF_SIZE - 1 - i] : 0;
    }

    return count;
}

u8 readULeb128(char* addr, u64* ret) {
  u64 result = 0;
  int shift = 0;
  u8 count = 0;

  while (1) {
    u8 byte = *(u8*)addr;
    addr++;
    count++;

    result |= (byte & 0x7f) << shift;
    shift += 7;

    if (!(byte & 0x80)) break;
  }

  *ret = result;

  return count;
}

// WARNING: signed LEB128 needs to conver signed integers from wasm binary format
// TODO: write signed leb conversion algorithm

u8 writeSLeb128(char buffer[BUFF_SIZE], long val) {
    return writeULeb128(buffer, prepareSignedValue(val));
}

u8 readSLeb128(char* addr, long* ret) {
    return readULeb128(addr, (u64*)ret);
}