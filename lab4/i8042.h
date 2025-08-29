#ifndef _MOUSE_8042_H_
#define _MOUSE_8042_H_

#include <lcom/lcf.h>

#define IRQ_MOUSE 12
#define IRQ_KEYBOARD 1 

#define OUT_BUF 0x60
#define ST_REG 0x64

#define IN_FULL BIT(1)
#define OUT_FULL BIT(0)
#define MOUSE_ST BIT(5)

#endif

