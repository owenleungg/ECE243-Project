// ECE243 Project
// by Michael Cheng, Owen Leung
// Main

#include "globals.h"

GPIO_t *const GPIO_1 = (GPIO_t *)JP2_BASE;
switches_t *const sw = (switches_t *)SW_BASE;
LED_t *const LED = (LED_t *)LEDR_BASE;

volatile int pixel_buffer_start;
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

int main(void)
{

  volatile int *pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;

  /* set front pixel buffer to Buffer 1 */
  *(pixel_ctrl_ptr + 1) = (int)&Buffer1; // first store the address in the  back buffer
  /* now, swap the front/back buffers, to set the front buffer location */
  wait_for_vsync();

  /* initialize a pointer to the pixel buffer, used by drawing functions */
  pixel_buffer_start = *pixel_ctrl_ptr;
  clear_screen(); // pixel_buffer_start points to the pixel buffer

  /* set back pixel buffer to Buffer 2 */
  *(pixel_ctrl_ptr + 1) = (int)&Buffer2;
  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
  clear_screen();                             // pixel_buffer_start points to the pixel buffer

  printf("Initialized\n");

  while (true)
  {
    int r = 0;
    int c = 0;
    wait_for_rising_edge_GPIO(GPIO_1, VS_PIN);
    wait_for_falling_edge_GPIO(GPIO_1, VS_PIN);

    // printf("VS\n");

    r = 0;
    // !read_GPIO_bit(GPIO_1, VS_PIN) && 
    while (r < SCREEN_HEIGHT)
    {
      // printf("HS1\n");
      // wait_for_rising_edge_GPIO(GPIO_1, HS_PIN);
      // printf("HS2\n");

      c = 0;
      // read_GPIO_bit(GPIO_1, HS_PIN) &&
      while (c < SCREEN_WIDTH)
      {
        // printf("start row\n");
        uint16_t pixel = read_pixel(GPIO_1, PLK_PIN);
        // if (c % 40 == 0 && r % 40 == 0)
        // {
        //   printf("%u, %u, %u\n", c, r, pixel);
        // }
        plot_pixel(c, r, pixel);
        c += 1;
      }
      r += 1;
    }

    // wait for vsync
    wait_for_vsync();                           // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
  }

  return 0;
}
