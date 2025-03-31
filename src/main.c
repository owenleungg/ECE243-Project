// ECE243 Project
// by Michael Cheng, Owen Leung
// Main

#include "globals.h"

GPIO_t *const GPIO_1 = (GPIO_t *)JP2_BASE;
timer_t *const timer = (timer_t *)TIMER_BASE;
switches_t *const sw = (switches_t *)SW_BASE;
LED_t *const LED = (LED_t *)LEDR_BASE;

volatile uint32_t pixel_buffer_start;
short int Buffer1[240][512]; 
short int Buffer2[240][512];

uint16_t input_frame[IMAGE_WIDTH * IMAGE_HEIGHT];
uint16_t grayscale[IMAGE_WIDTH * IMAGE_HEIGHT];
uint16_t blurred[IMAGE_WIDTH * IMAGE_HEIGHT];
uint16_t gradient[IMAGE_WIDTH * IMAGE_HEIGHT];
uint16_t suppressed[IMAGE_WIDTH * IMAGE_HEIGHT];
uint16_t thresholded[IMAGE_WIDTH * IMAGE_HEIGHT];
uint16_t hysteresis[IMAGE_WIDTH * IMAGE_HEIGHT];

double gaussian_kernal[KERNAL_SIZE][KERNAL_SIZE];
int gx_buffer[IMAGE_WIDTH * IMAGE_HEIGHT];
int gy_buffer[IMAGE_WIDTH * IMAGE_HEIGHT];
int thresholds[IMAGE_WIDTH * IMAGE_HEIGHT];

int main(void)
{
  // Initialize hardware
  GPIO_setup(GPIO_1);

  volatile uint32_t *pixel_ctrl_ptr = (uint32_t*)PIXEL_BUF_CTRL_BASE;

  /* set front pixel buffer to Buffer 1 */
  *(pixel_ctrl_ptr + 1) = (uint32_t)&Buffer1; // first store the address in the  back buffer
  /* now, swap the front/back buffers, to set the front buffer location */
  wait_for_vsync();

  /* initialize a pointer to the pixel buffer, used by drawing functions */
  pixel_buffer_start = *pixel_ctrl_ptr;
  clear_screen(); // pixel_buffer_start points to the pixel buffer

  /* set back pixel buffer to Buffer 2 */
  *(pixel_ctrl_ptr + 1) = (uint32_t)&Buffer2;
  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
  clear_screen();                             // pixel_buffer_start points to the pixel buffer

  struct switches *const switch_ptr = ((struct switches *)SW_BASE);

  build_gaussian_kernal();
  printf("Initialized\n");

  uint32_t frame_count = 0;
  while (true)
  {
    uint32_t r = 0;
    uint32_t c = 0;
    wait_for_rising_edge_GPIO(GPIO_1, VS_PIN);
    wait_for_falling_edge_GPIO(GPIO_1, VS_PIN);

    printf("Frame count: %lu\n", frame_count);

    r = 0;
    while (!read_GPIO_bit(GPIO_1, VS_PIN) && r < SCREEN_HEIGHT)
    {
      wait_for_rising_edge_GPIO(GPIO_1, HS_PIN);

      c = 0;
      while (read_GPIO_bit(GPIO_1, HS_PIN) && c < SCREEN_WIDTH)
      {
        input_frame[r * IMAGE_WIDTH + c] = read_pixel(GPIO_1, PLK_PIN);
        c += 1;
      }
      r += 1;
    }
    
    uint32_t stage = switch_ptr->data & 0x3F; // Only consider SW0–SW5

    if (stage >= 0x01) {
      printf("greyscale \n");
      apply_grey_scale();
    }
    if (stage >= 0x02) {
      printf("Gaussian Kernal \n");
      apply_gaussian_kernal();
    }
    if (stage >= 0x04) {
      printf("Sobel Operator \n");
      apply_sobel_operator();
    }
    if (stage >= 0x08) {
      printf("Non-max Suppression \n");
      apply_non_max_suppression();
    }
    if (stage >= 0x10) {
      printf("Double Threshold \n");
      apply_double_threshold();
    }
    if (stage >= 0x20) {
      printf("Edge Tracking \n");
      apply_edge_tracking();
    }

    // display image
    for (uint32_t x = 0; x < SCREEN_WIDTH; x++)
    {
      for (uint32_t y = 0; y < SCREEN_HEIGHT; y++)
      {
        uint16_t pixel;

        pixel = input_frame[y * IMAGE_WIDTH + x]; // original

        if (switch_ptr->data & 0x01)
        {
          pixel = grayscale[y * IMAGE_WIDTH + x];
        }

        if (switch_ptr->data & 0x02)
        {
          pixel = blurred[y * IMAGE_WIDTH + x];
        }

        if (switch_ptr->data & 0x04)
        {
          pixel = gradient[y * IMAGE_WIDTH + x];
        }

        if (switch_ptr->data & 0x08)
        {
          pixel = suppressed[y * IMAGE_WIDTH + x];
        }

        if (switch_ptr->data & 0x10)
        {
          pixel = thresholded[y * IMAGE_WIDTH + x];
        }

        if (switch_ptr->data & 0x20)
        {
          pixel = hysteresis[y * IMAGE_WIDTH + x];
        }

        if (pixel == 0x0) {
          pixel = grayscale[y * IMAGE_WIDTH + x];
        }

        plot_pixel(x, y, pixel);
      }
    }

    // wait for vsync
    wait_for_vsync();                           // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    frame_count += 1;
  }

  return 0;
}
