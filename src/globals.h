
#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdint.h>
#include <stdbool.h>

#include "address_map_nios2.h"

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

#endif
