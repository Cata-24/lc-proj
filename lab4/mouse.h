#ifndef _MOUSE_H_
#define _MOUSE_H_

#include <lcom/lcf.h>

int (mouse_int_subscribe)(uint8_t *bit_no);
int (mouse_int_unsubscribe)();
void (mouse_ih)();
int (mouse_assemble_packet)(uint8_t *packet, struct packet *pack);

#endif

