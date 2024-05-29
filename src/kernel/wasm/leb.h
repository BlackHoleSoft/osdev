#include "../std/types.h"

#define BUFF_SIZE 64

u64 prepareSignedValue(long value);

u8 writeULeb128(char buffer[BUFF_SIZE], u64 val);

u8 readULeb128(char* addr, u64* ret);

u8 writeSLeb128(char buffer[BUFF_SIZE], long val);

u8 readSLeb128(char* addr, long* ret);