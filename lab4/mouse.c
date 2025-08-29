#include <lcom/lcf.h>
#include "mouse.h"
#include "i8042.h"

int hook_id = 3;
uint8_t packet[3];
int count = 0;
bool lever = false;

int (mouse_int_subscribe)(uint8_t *bit_no) {
    if (bit_no == NULL) return 1;
    *bit_no = BIT(hook_id);
    return sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int (mouse_int_unsubscribe)() {
    return sys_irqrmpolicy(&hook_id);
}

void (mouse_ih)() {
    
    uint8_t temp;

    if (util_sys_inb(ST_REG, &temp) != 0) return;

    if (!(temp & OUT_FULL)) return;
    if (!(temp & MOUSE_ST)) return;


    if (util_sys_inb(OUT_BUF, &temp) != 0) return;

    if (!lever) {
        if (!(temp & BIT(3))) return;
        lever = true;
    }

    if (count % 3 == 0 && !(temp & BIT(3))) return;

    packet[count%3] = temp;
    count++;
}

int (mouse_assemble_packet)(uint8_t *packet, struct packet *pack) {
    for (int i = 0; i < 3; i++) {
        pack->bytes[i] = packet[i]; 
    }

    pack->rb = packet[0] & BIT(1);
    pack->lb = packet[0] & BIT(0);
    pack->mb = packet[0] & BIT(2);

    pack->x_ov = packet[0] & BIT(6);
    pack->y_ov = packet[0] & BIT(7);

    int16_t delta_x = (packet[0] & BIT(4)) ? (int16_t)(0xFF00 | packet[1]) : (int16_t)(packet[1]);
    int16_t delta_y = (packet[0] & BIT(5)) ? (int16_t)(0xFF00 | packet[2]) : (int16_t)(packet[2]);

    pack->delta_x = delta_x;
    pack->delta_y = delta_y;

    return 0;
}
