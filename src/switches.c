#include <stdint.h>
#include <stdbool.h>

#define SW_BASE 0xFF200040

typedef struct switches {
    volatile uint32_t data;
} switches_t;




