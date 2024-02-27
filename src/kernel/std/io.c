#include "io.h"

void outb(u16 port, u8 val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
    /* There's an outb %al, $imm8 encoding, for compile-time constant port numbers that fit in 8b. (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

u8 inb(u16 port)
{
    u8 ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

u16 inw(u16 portid)
{
	u16 ret;
	__asm__ volatile ("inw %%dx, %%ax":"=a"(ret):"d"(portid));
	return ret;
}

u32 inl(u16 portid)
{
	u32 ret;
	__asm__ volatile ("inl %%dx, %%eax":"=a"(ret):"d"(portid));
	return ret;
}

void outw(u16 portid, u16 value)
{
	__asm__ volatile ("outw %%ax, %%dx": :"d" (portid), "a" (value));
}

void outl(u16 portid, u32 value)
{
	__asm__ volatile ("outl %%eax, %%dx": :"d" (portid), "a" (value));
}

