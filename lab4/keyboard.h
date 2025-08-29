#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include <lcom/lcf.h>
#include "i8042.h"

int (keyboard_subscribe_int)(uint8_t *bit_no);

int (keyboard_unsubscribe_int)();

void (kbc_ih)();

int(keyboard_restore)();

int(read_kbc_status)(uint8_t *st);

int read_kbc_output(uint8_t port, uint8_t *output, uint8_t mouse);

int(write_kbc_command)(uint8_t port, uint8_t commandByte);

#endif
