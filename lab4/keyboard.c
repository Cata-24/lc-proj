#include <lcom/lcf.h>

#include <stdint.h>

#include "i8042.h"

uint8_t scancode = 0;
int hook_id = 1;

int(keyboard_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  *bit_no = BIT(hook_id);
  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int(keyboard_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

int(read_kbc_status)(uint8_t *st) {
  return util_sys_inb(KBC_ST_REG, st);
}

int read_kbc_output(uint8_t port, uint8_t *output, uint8_t mouse) {

  uint8_t st;
  uint8_t attemps = 10;

  while (attemps) {

    if (read_kbc_status(&st) != 0) {
      printf("Error: couldn't read status\n");
      return 1;
    }

    if ((st & BIT(0)) != 0) {
      if (util_sys_inb(port, output) != 0) {
        printf("Error: couldn't read output\n");
        return 1;
      }
      if ((st & BIT(7)) != 0) {
        printf("Parity error - invalid data\n");
        return 1;
      }
      if ((st & BIT(6)) != 0) {
        printf("Timeout error - invalid data\n");
        return 1;
      }
      if (mouse && !(st & BIT(5))) {
        printf("Error: Mouse data\n");
        return 1;
      }
      if (!mouse && (st & BIT(5))) {
        printf("Error: Keyboard output not found\n");
        return 1;
      }
      return 0;
    }
    tickdelay(micros_to_ticks(20000));
    attemps--;
  }
  return 1;
}

int(write_kbc_command)(uint8_t port, uint8_t commandByte) {

  uint8_t st;
  uint8_t attemps = 10;

  while (attemps) {

    if (read_kbc_status(&st) != 0) {
      printf("Error: couldn't read status\n");
      return 1;
    }

    if ((st & INPUT_BUFFER_FULL) == 0) {

      if (sys_outb(port, commandByte) != 0) {
        printf("Error: Couldn't write command Byte!\n");
        return 1;
      }

      return 0;
    }
    tickdelay(micros_to_ticks(WAIT_KBC));
    attemps--;
  }

  return 1;
}

void(kbc_ih)() {
  if (read_kbc_output(KBC_CMD_OUT, &scancode, 0) != 0)
    printf("Error: Couldn't read the scancode!\n");
}

int(keyboard_restore)() {
  uint8_t commandByte;

  if (write_kbc_command(KBC_CMD_IN, KBC_CMD_READ) != 0)
    return 1;
  if (read_kbc_output(KBC_CMD_OUT, &commandByte, 0) != 0)
    return 1;

  commandByte |= ENABLE_INT;

  if (write_kbc_command(KBC_CMD_IN, KBC_CMD_WRITE) != 0)
    return 1;
  if (write_kbc_command(KBC_CMD_WRITE, commandByte) != 0)
    return 1;

  return 0;
}
