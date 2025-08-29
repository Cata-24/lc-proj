#include <lcom/lcf.h>

int (setBIOSMode)(uint16_t mode);
int (setVBECommand)(uint16_t command, uint16_t mode);
int (get_map_mem)(uint16_t mode);


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color );
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
