#include "util.h"

unsigned int swap_endian(unsigned int value) {
    return (value << 24) |
           ((value << 8) & 0x00FF0000) |
           ((value >> 8) & 0x0000FF00) |
           (value >> 24);
}
