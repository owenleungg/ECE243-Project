// ECE243 Project
// by Michael Cheng, Owen Leung
// Main

#include <stdint.h>
#include <stdbool.h>

typedef struct GPIO {
	volatile uint32_t data;
	volatile uint32_t direction;
	volatile uint32_t interrupt;
	volatile uint32_t edge;
} GPIO_t;

typedef struct switches {
	volatile uint32_t data;
} switches_t;

typedef struct LED {
	volatile uint32_t data;
} LED_t;

int main(void) {
  GPIO_t* const GPIO_0 = (GPIO_t*)0xFF200060;
  GPIO_t* const GPIO_1 = (GPIO_t*)0xFF200070;
  switches_t* const sw = (switches_t*)0xFF200040;
  LED_t* const LED = (LED_t*)0xFF200000;
  


  return 0;
}

bool read_GPIO_bit(GPIO_t* port, uint8_t pin) {
  uint32_t mask = (port->data) & (1 << pin);
  return mask > 0;
}
