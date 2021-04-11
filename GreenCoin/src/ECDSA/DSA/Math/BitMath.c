
#include "BitMath.h"

uint2048_t Init(uint64_t a) {
	uint64_t * t = (uint64_t*)malloc(sizeof(uint64_t));

	memset(t, 0, sizeof(uint64_t));

	memcpy(t, &a, sizeof(a));

	return t;
}
uint2048_t Init_from_val(uint2048_t a) { return Init(a); }
void Dinit(uint2048_t a) { free(a); }
void Print_Hex(uint2048_t a) {
	for (int i = 0; i < 1; i++) {
		printf("0x%.16llX ", (*a)[i]);
	}
	printf("\n");
}

int	Equ(uint2048_t a, uint2048_t b) { return (*a)[0] == (*b)[0]; }
void Add(uint2048_t a, uint2048_t b, uint2048_t out) { (*out)[0] = (*a)[0] + (*b)[0]; }
void Sub(uint2048_t a, uint2048_t b, uint2048_t out) { (*out)[0] = (*a)[0] - (*b)[0]; }
void Mul(uint2048_t a, uint2048_t b, uint2048_t modu, uint2048_t out) { 
	uint64_t mod = (*modu)[0];

	uint64_t e = 0;
	uint64_t c = 0;
	while (e < (*b)[0]) {
		c = (c + (*a)[0]) % mod;
		e++;
	}

	(*out)[0] = c;
}
void Div(uint2048_t a, uint2048_t b, uint2048_t out) { (*out)[0] = (*a)[0] / (*b)[0]; }
//void Pow(uint2048_t a, uint2048_t b, uint2048_t out) { (*out)[0] = pow((*a)[0], (*b)[0]); }
void Pow(uint2048_t a, uint2048_t b, uint2048_t modu, uint2048_t out) { 

	uint64_t mod = (*modu)[0];

	uint64_t e = 0;
	uint64_t c = 1;
	while (e < (*b)[0]) {
		c = (c * (*a)[0]) % mod;
		e++;
	}

	(*out)[0] = c;
}
void Mod(uint2048_t a, uint2048_t b, uint2048_t out) { (*out)[0] = (*a)[0] % (*b)[0]; }
uint64_t Inv_64(uint64_t a, uint64_t modu) {
	uint64_t k = a;
	uint64_t k_inv = 1;
	uint64_t q = modu;
	while ((k_inv * k) % q != 1) { k_inv++; }
	return k_inv;
}
void Inv(uint2048_t a, uint2048_t modu, uint2048_t out) {
	uint64_t k = (*a)[0];
	uint64_t k_inv = 1;
	uint64_t q = (*modu)[0];
	while ((k_inv * k) % q != 1) { k_inv++; }
	(*out)[0] = k_inv;
}

void Inc(uint2048_t a) { (*a)[0]++; }
void Dec(uint2048_t a) { (*a)[0]--; }

/*
uint2048_t Init(uint64_t a) {
	uint2048_ptr_t * t = (uint2048_ptr_t*)malloc(sizeof(uint2048_ptr_t));

	memset(t, 0, sizeof(uint2048_ptr_t));

	memcpy(&((*t)[31]), &a, sizeof(a));

	return t;
}
uint2048_t Init_from_val(uint2048_t a) {
	uint2048_ptr_t * t = (uint2048_ptr_t*)malloc(sizeof(uint2048_ptr_t));

	memcpy(t, a, sizeof(uint2048_ptr_t));

	return t;
}

void Dinit(uint2048_t a) {
	free(a);
}

void Print_Hex(uint2048_t a) {
	for (int i = 0; i < 32; i++) {
		printf("0x%.8X ", (*a)[i]);
	}
	printf("\n");
}
void	Add(uint2048_t a, uint2048_t b, uint2048_t out) {
	uint64_t q = 0;
	uint64_t w = 0;
	uint8_t overflow_bit = 0;
	
	for (int i = 31; i >= 0; i--) {
		q = (*a)[i];
		w = (*b)[i];
		uint64_t s = q + w + overflow_bit;
		overflow_bit = (s < q) || (s < w);
		(*out)[i] = s;
	}
}

/*
#define	MAX_DIG	((1UL << V_BIGBITS) - 1)
/* Computes x += y. 
void v_bignum_add(VBigDig *x, const VBigDig *y)
{
	unsigned int	i, xs = *x++, ys = *y++, s, carry, t;

	s = xs < ys ? xs : ys; // maximum digit count
	for (i = carry = 0; i < s; i++)
	{
		t = x[i] + y[i] + carry;
		x[i] = t;
		carry = t > MAX_DIG;
	}
	for (; carry && i < xs; i++)
	{
		t = x[i] + carry;
		x[i] = t;
		carry = t > MAX_DIG;
	}
}


void	Sub(uint2048_t a, uint2048_t b, uint2048_t out);
void	Mul(uint2048_t a, uint2048_t b, uint2048_t out) {
	uint2048_t counter = Init_from_val(b);
	uint2048_t zero = Init(0);

	while (!Equ(counter, zero)) {
		Add(a, out, out);
		Dec(counter);
	}
}
void	Div(uint2048_t a, uint2048_t b, uint2048_t out);
void	Pow(uint2048_t a, uint2048_t b, uint2048_t out);
void	Mod(uint2048_t a, uint2048_t b, uint2048_t out);

void Inc(uint2048_t a) {
	uint64_t q = 0;
	uint8_t overflow_bit = 1;

	uint8_t ind = 31;

	while (overflow_bit != 0 && ind >= 0) {
		q = (*a)[ind];
		uint64_t s = q + overflow_bit;
		overflow_bit = (s < q);
		(*a)[ind] = s;
		ind--;
	}
}
void Dec(uint2048_t a) {
	uint2048_t neg_one = (uint2048_ptr_t*)malloc(sizeof(uint2048_ptr_t));
	memset(neg_one, (uint32_t)(-1), sizeof(uint2048_ptr_t));

	Add(a, neg_one, a);
}

int Equ(uint2048_t a, uint2048_t b) {
	for (int i = 0; i < 32; i++) {
		if ((*a)[i] != (*b)[i]) { return 0; }
	}
	return 1;
}*/