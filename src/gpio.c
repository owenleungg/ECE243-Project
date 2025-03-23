#include "globals.h"

uint32_t read_GPIO_word(GPIO_t* port) {
  return (port->data);
}

void write_GPIO_word(GPIO_t* port, uint32_t word) {
  (port->data) = word;
  return;
}

bool read_GPIO_bit(GPIO_t* port, uint8_t pin) {
  uint32_t mask = (port->data) & (1 << pin);
  return mask > 0;
}

void write_GPIO_bit(GPIO_t* port, uint8_t pin, bool value) {
  if (value != read_GPIO_bit(port, pin)) {
    uint32_t word = read_GPIO_word(port);
    word ^= (value << pin);
    (port->data) = word;
  }
  return;
}

void wait_for_rising_edge_GPIO(GPIO_t* port, uint8_t pin) {
  while (read_GPIO_bit(port, pin) == 1) {
    ;
  }
  while (read_GPIO_bit(port, pin) == 0) {
    ;
  }
  return;
}

void wait_for_falling_edge_GPIO(GPIO_t* port, uint8_t pin) {
  while (read_GPIO_bit(port, pin) == 0) {
    ;
  }
  while (read_GPIO_bit(port, pin) == 1) {
    ;
  }
  return;
}

uint16_t read_pixel(GPIO_t* port, uint8_t plk_pin) {
  wait_for_rising_edge_GPIO(port, plk_pin);
  uint8_t byte_1 = read_GPIO_word(port) >> 24;
  wait_for_rising_edge_GPIO(port, plk_pin);
  uint8_t byte_2 = read_GPIO_word(port) >> 24;
  // return ((byte_1 << 8) | byte_2);
  return 0xff;
}
