#include "globals.h"

volatile int pixel_buffer_start; 
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];
int screen_width = 320;
int screen_height = 240;

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // initialize location and direction of rectangles(not shown)

    /* set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the  back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();

    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    while (1)
    {
     
        // draw video input
        draw();

        // wait for vsync
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

void plot_pixel(int x, int y, short int line_color) {
    volatile short int *one_pixel_address = (volatile short int *)(pixel_buffer_start + (y << 10) + (x << 1));

    *one_pixel_address = line_color;
}


void wait_for_vsync() {
    volatile int* pixel_ctrl_ptr = (int *)0xFF203020;;
    *pixel_ctrl_ptr = 1;
    int status = *(pixel_ctrl_ptr + 3);
    while ((status & 0x01) !=0 ) {
        status = *(pixel_ctrl_ptr + 3);
    }
}

void swap(int *point1, int *point2) {
    int temp = *point1;
    *point1 = *point2;
    *point2 = temp;
    return;
}

