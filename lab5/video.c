#include <lcom/lcf.h>
#include "video.h"

uint8_t* video_mem;
vbe_mode_info_t mode_info;

int (setBIOSMode)(uint16_t mode) {
    reg86_t r86;

    memset(&r86, 0, sizeof(r86));

    r86.intno = 0x10; 
    r86.ah = 0x00;   
    r86.al = mode;
    r86.bx = 0x0000;

    if( sys_int86(&r86) != 0 ) {
        printf("\tFailed setting mode \n");
    return 1;
    }
    return 0;
}

int (setVBECommand)(uint16_t command, uint16_t mode) {
    reg86_t r86;

    memset(&r86, 0, sizeof(r86));

    r86.intno = 0x10; 
    r86.ah = 0x4F;
    r86.al = command;

    r86.bx = mode | BIT(14);

    if( sys_int86(&r86) != 0 ) {
        printf("\tFailed setting mode \n");
    return 1;
    }
    return 0;
}

int (get_map_mem)(uint16_t mode) {
    unsigned int vram_size;
    unsigned int vram_base;
    struct minix_mem_range mr;

    if (vbe_get_mode_info(mode, &mode_info) != 0) return 1;

    vram_base = mode_info.PhysBasePtr;
    vram_size = mode_info.XResolution * mode_info.YResolution * (mode_info.BitsPerPixel / 8);

    mr.mr_base = vram_base;
    mr.mr_limit = vram_base + vram_size;

    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != 0) return 1;

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED) {
        printf("Couldn't map memory");
        return 1;
    } else return 0;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
    if (x > mode_info.XResolution || y > mode_info.YResolution) return 1;

    if (mode_info.BitsPerPixel != 32) {
        color = color & (BIT(mode_info.BitsPerPixel) - 1);
    }

    uint8_t BytesPerPixel = (mode_info.BitsPerPixel + 7) / 8;
    uint32_t pixel_location = (y * mode_info.XResolution + x) * BytesPerPixel;

    if (memcpy(&video_mem[pixel_location], &color, BytesPerPixel) == NULL) return 1;
    else return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (int i = x; i < x + len; i++) {
        if (vg_draw_pixel(i, y, color) != 0) return 1;
    }
    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color ) {
    for (int i = y; i < y + height; i++) {
        if (vg_draw_hline(x, i, width, color) != 0) return 1;
    }
    return 0;
}

