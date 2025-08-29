#include <lcom/lcf.h>
#include "timer.h"
#include "i8254.h"

int hook_id = 0;
int counter = 0;

int (timer_int_sub)(uint8_t *bit_no) {
    if (bit_no == NULL) return 1;
    *bit_no = BIT(hook_id);
    return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
}

int (timer_int_unsub)() {
    return sys_irqrmpolicy(&hook_id);
}

void (timer_ih)() {
    counter++;
}
