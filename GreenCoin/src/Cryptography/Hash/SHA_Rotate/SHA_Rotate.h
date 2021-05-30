#pragma once

#include <stdint.h>
#include <string.h>

uint32_t bit_rotate_left(uint32_t x, char rotations);
uint32_t bit_rotate_right(uint32_t x, char rotations);
uint64_t bit_rotate_right_64(uint64_t x, char rotations);