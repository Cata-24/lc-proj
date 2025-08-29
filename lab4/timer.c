#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int timer_counter = 0;
int hook_id_t = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  // *bit_no = (uint8_t) hook_id;
  *bit_no = BIT(hook_id_t);
  if (sys_irqsetpolicy(0, IRQ_REENABLE, &hook_id_t) != 0)
    return 1;
  return 0;
}

int(timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id_t) != 0)
    return 1;
  return 0;
}

void(timer_int_handler)() {
  timer_counter++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (timer < 0 | timer > 2 | st == NULL)
    return 1;

  uint8_t rb = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));

  if (sys_outb(TIMER_CTRL, rb) != 0)
    return 1;

  return (util_sys_inb(TIMER_0 + timer, st));
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  union timer_status_field_val value;

  switch (field) {
    case tsf_all:
      value.byte = st;
      break;

    case tsf_initial: {
      uint8_t initial_mode = (st & 0x30) >> 4;
      switch (initial_mode) {
        case 0: value.in_mode = INVAL_val; break;
        case 1: value.in_mode = LSB_only; break;
        case 2: value.in_mode = MSB_only; break;
        case 3: value.in_mode = MSB_after_LSB; break;
      }
    } break;

    case tsf_mode: {
      uint8_t mode = (st & 0xE) >> 1;
      if (mode == 6)
        mode = 2;
      else if (mode == 7)
        mode = 3;
      value.count_mode = mode;
    } break;

    case tsf_base: {
      uint8_t base = (st & 0x1);
      value.bcd = (base == 1) ? true : false;
    } break;
    default:
      return 1;
  }

  return timer_print_config(timer, field, value);
}
