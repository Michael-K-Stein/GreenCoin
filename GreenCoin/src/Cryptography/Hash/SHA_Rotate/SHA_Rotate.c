
#include "SHA_Rotate.h"

uint32_t	bit_rotate_left(uint32_t x, char rotations)
{
	return ((x << rotations) | (x >> (32 - rotations)));
}

uint32_t	bit_rotate_right(uint32_t x, char rotations)
{
	return ((x >> rotations) | (x << (32 - rotations)));
}

uint64_t	bit_rotate_right_64(uint64_t x, char rotations)
{
	return ((x >> rotations) | (x << (64 - rotations)));
}