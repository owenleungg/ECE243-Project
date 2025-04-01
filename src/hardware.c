#include "globals.h"

void delay_us(uint32_t c)
{
  timer->control = 0x8; // stop the timer
  timer->status = 0;    // reset TO
  timer->periodlo = ((TIMERVAL * c) & 0x0000FFFF);
  timer->periodhi = ((TIMERVAL * c) & 0xFFFF0000) >> 16;
  timer->control = 0x4;
  while ((timer->status & 0x1) == 0)
    ;
  timer->status = 0; // reset TO
  return;
}

uint32_t time_stamp()
{
  return (timer->snaphi << 16) & timer->snaplo;
}

void GPIO_setup(GPIO_t *port)
{
  uint32_t dir = 0x0;
  // dir |= (1 << FRAME_PIN);
  // dir |= (1 << PIX_PIN);
  // dir |= (1 << SDA_PIN);
  // dir |= (1 << SCL_PIN);
  port->direction = dir;
  port->data = 0x0;

  // write_GPIO_bit(port, SDA_PIN, 1); // Ensure SDA is high
  // write_GPIO_bit(port, SCL_PIN, 1); // Ensure SCL is high
  return;
}

uint32_t read_GPIO_word(GPIO_t *port)
{
  return (port->data);
}

void write_GPIO_word(GPIO_t *port, uint32_t word)
{
  (port->data) = word;
  return;
}

bool read_GPIO_bit(GPIO_t *port, uint8_t pin)
{
  uint32_t mask = (port->data) & (1 << pin);
  return mask > 0;
}

void write_GPIO_bit(GPIO_t *port, uint8_t pin, bool value)
{
  if (value != read_GPIO_bit(port, pin))
  {
    uint32_t word = read_GPIO_word(port);
    word ^= (1 << pin);
    (port->data) = word;
  }
  return;
}

void wait_for_rising_edge_GPIO(GPIO_t *port, uint8_t pin)
{
  while (read_GPIO_bit(port, pin) == 1)
  {
    ;
  }
  while (read_GPIO_bit(port, pin) == 0)
  {
    ;
  }
  return;
}

void wait_for_falling_edge_GPIO(GPIO_t *port, uint8_t pin)
{
  while (read_GPIO_bit(port, pin) == 0)
  {
    ;
  }
  while (read_GPIO_bit(port, pin) == 1)
  {
    ;
  }
  return;
}

uint16_t read_pixel(GPIO_t* port, uint8_t plk_pin) 
{
  wait_for_rising_edge_GPIO(port, plk_pin);
  uint32_t p = read_GPIO_word(port);
  uint32_t byte_1 = ((p & 0x80000000) >> 24) | ((p & 0x3c000000) >> 23) | ((p & 0x01c00000) >> 22);
  wait_for_rising_edge_GPIO(port, plk_pin);
  p = read_GPIO_word(port);
  uint32_t byte_2 = ((p & 0x80000000) >> 24) | ((p & 0x3c000000) >> 23) | ((p & 0x01c00000) >> 22);
  return ((byte_1 << 0) | (byte_2 << 8)) & 0xffff;
}

uint16_t read_pixel2(GPIO_t *port)
{
  uint16_t result = 0x0;
  for (int i = 0; i < 8; ++i) {
    write_GPIO_bit(GPIO_1, PIX_PIN, 1);
    delay_us(1);
    uint32_t current = read_GPIO_word(port) & (0x03);
    result |= (current << (7-i)*2);
    write_GPIO_bit(GPIO_1, PIX_PIN, 0);
    delay_us(1);
  }
  return result;
}

void plot_pixel(uint32_t x, uint32_t y, uint16_t line_color)
{
  volatile uint16_t *one_pixel_address =
      (volatile uint16_t *)(pixel_buffer_start + (y << 10) + (x << 1));

  *one_pixel_address = line_color;
  return;
}

void wait_for_vsync()
{
  volatile uint32_t *pixel_ctrl_ptr = (uint32_t*)PIXEL_BUF_CTRL_BASE;
  *pixel_ctrl_ptr = 1;
  uint32_t status = *(pixel_ctrl_ptr + 3);
  while ((status & 0x01) != 0)
  {
    status = *(pixel_ctrl_ptr + 3);
  }
  return;
}

void clear_screen()
{
  for (uint32_t x = 0; x < SCREEN_WIDTH; x++)
  {
    for (uint32_t y = 0; y < SCREEN_HEIGHT; y++)
    {
      plot_pixel(x, y, 0x0000);
    }
  }
  return;
}
