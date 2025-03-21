#include "globals.h"

void plot_pixel(int x, int y, short int line_color) {
  volatile short int *one_pixel_address =
      (volatile short int *)(pixel_buffer_start + (y << 10) + (x << 1));

  *one_pixel_address = line_color;
}

void wait_for_vsync() {
  volatile int *pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
  *pixel_ctrl_ptr = 1;
  int status = *(pixel_ctrl_ptr + 3);
  while ((status & 0x01) != 0) {
    status = *(pixel_ctrl_ptr + 3);
  }
}

void clear_screen() {
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
      plot_pixel(x, y, 0x0000);
    }
  }
}