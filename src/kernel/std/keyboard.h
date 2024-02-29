#include "types.h"
#include "io.h"

void kbd_ack(void);
void kbd_leds(u8 value);
u8 kbd_keycode();
char kbd_symbol(u8 keycode);
void kbd_enable();
void kbd_disable();