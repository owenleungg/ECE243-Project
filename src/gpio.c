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
    uint32_t word = read_GPIO_byte(port);
    word ^= (value << pin);
    (port->data) = word;
  }
  return;
}