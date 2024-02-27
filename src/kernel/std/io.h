#include "types.h"

void outb(u16 port, u8 val);
u8 inb(u16 port);
u16 inw(u16 portid);
u32 inl(u16 portid);
void outw(u16 portid, u16 value);
void outl(u16 portid, u32 value);