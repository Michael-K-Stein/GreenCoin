
#include "BNMath.h"

//int arraysize(BN * a)		(sizeof(a) / sizeof(a[0]))
//int BN_Is_Even(BN * a)		!BN_Get_Bit_Value(a, 0)
//int BN_Is_Odd(BN * a)		BN_Get_Bit_Value(a, 0)

int_t MAX(int_t a, int_t b) {
	return (a > b) ? a : b;
}
int_t MIN(int_t a, int_t b) {
	return (a < b) ? a : b;
}
int_t ceil_div(float a, float q) {
	float b = ceil(a/q);
	int c = b / 1;
	return c;
}

void BN_Init(BN ** r) {
	*r = (BN*)malloc(sizeof(BN));
	BN * rt = *r;
	rt->sign = 1;
	rt->size = 0;
	rt->data = NULL_DATA;
}

void BN_Init_Stack(BN * r) {
	r->sign = 1;
	r->size = 0;
	r->data = NULL_DATA;
}

void BN_Free(BN * r) {
	if (r->data != NULL_DATA) {
		memset(r->data, 0, r->size * BN_INT_SIZE);
		free(r->data);
	}

	r->size = 0;
	r->data = NULL_DATA;

	free(r);
}

error_t BN_Resize_Decrease(BN * r, uint_t size) {
	uint_t * data;

	size = (uint_t)MAX(size, 1); // Override if size < 1

	data = calloc(size * BN_INT_SIZE);
	if (data == 0) { /* Could not allocate memory! */ return ERROR_FAILED; }


	if (r->size > 0) {
		memcpy(data, r->data, size * BN_INT_SIZE);
		/*printf("[0] 0x%.8X 0x%.8X\n", r->data[0], data[0]);
		printf("[1] 0x%.8X 0x%.8X\n", r->data[1], data[1]);
		printf("[2] 0x%.8X\n", r->data[2]);*/
		free(r->data);
	}

	r->size = size;
	r->data = data;

	return ERROR_NONE;
}
error_t BN_Resize(BN * r, uint_t size) {
	uint_t * data;

	size = (uint_t)MAX(size, 1); // Override if size < 1

	if (r->size == size) { return ERROR_NONE; }
	else if (r->size > size) {
		//return BN_Resize_Decrease(r, size);
		return ERROR_NONE;
	}

	data = calloc(size * BN_INT_SIZE, sizeof(char));
	if (data == 0) { /* Could not allocate memory! */ return ERROR_FAILED; }


	if (r->size > 0) {
		memcpy(data, r->data, r->size * BN_INT_SIZE);
		free(r->data);
	}

	r->size = size;
	r->data = data;

	return ERROR_NONE;
}

uint_t BN_Get_Length(const BN * r) {
	if (r->size == 0) { return 0; }
	
	uint_t i = 0;
	for (i = r->size - 1; i >= 0; i--) {
		if (r->data[i] != 0) {
			break;
		}
	}

	return i + 1;
}
uint_t BN_Get_Byte_Length(const BN * r) {
	uint_t n = 0;
	uint32_t m;

	if (r->size == 0) { return 0; }

	for (n = r->size - 1; n > 0; n--) {
		if (r->data[n] != 0) {
			break;
		}
	}

	m = r->data[n];
	n *= BN_INT_SIZE;

	for (; m != 0; m >>= 8) { n++; }

	return n;
}
uint_t BN_Get_Bit_Length(const BN * r) {
	uint_t n;
	uint32_t m;

	if (r->size == 0) { return 0; }

	for (n = r->size - 1; n > 0; n--) {
		if (r->data[n] != 0) { break; }
	}

	m = r->data[n];
	n *= BN_INT_SIZE * 8;

	// Final bit count
	for (; m != 0; m >>= 1) {
		n++;
	}

	return n;
}


error_t BN_Set_Bit_Value(BN * r, uint_t index, uint_t value) {
	uint_t quot;
	uint_t rema;

	quot = index / (BN_INT_SIZE * 8);
	rema = index % (BN_INT_SIZE * 8);

	error_t  error;
	error = BN_Resize(r, quot + 1);
	if (error) { return error; }

	if (value) { // 1
		r->data[quot] |= (1 << rema); // Or
	}
	else { // 0
		r->data[quot] &= ~(1 << rema); // And
	}

	return ERROR_NONE;
}
uint_t BN_Get_Bit_Value(const BN * r, uint_t index) {
	uint_t quot = index / (BN_INT_SIZE * 8);
	uint_t rema = index % (BN_INT_SIZE * 8);
	
	if (quot >= r->size) { // Index out of range
		return 0;
	}

	return (r->data[quot] >> rema) & 0x01;
}

int_t BN_Compare(const BN * a, const BN * b) {
	uint_t m = BN_Get_Length(a);
	uint_t n = BN_Get_Length(b);

	if (!m && !n) {
		return 0;
	}
	else if (m > n) {
		return a->sign;
	} else if (m < n) {
		return -b->sign;
	}

	if (a->sign > 0 && b->sign < 0) {
		return 1;
	} else if (a->sign < 0 && b->sign > 0) {
		return -1;
	}

	while (n--)
	{
		if (a->data[n] > b->data[n]) {
			return a->sign;
		} else if (a->data[n] < b->data[n]) {
			return -a->sign;
		}
	}

	return 0;
}
int_t BN_Compare_Int(const BN * a, int_t b) {
	uint_t value = (b >= 0) ? b : -b;
	
	// Demo BN from b
	BN t;
	t.sign = (b >= 0) ? 1 : -1;
	t.size = 1;
	t.data = &value;

	return BN_Compare(a, &t);
}
int_t BN_Compare_Abs(const BN * a, const BN * b) {
	uint_t m = BN_Get_Length(a);
	uint_t n = BN_Get_Length(b);

	if (!m && !n) {
		return 0; // Numbers are both of size 0 -> empty
	} else if (m > n) {
		return 1;
	} else if (m < n) {
		return -1;
	}

	while (n--)
	{
		if (a->data[n] > b->data[n]) {
			return 1;
		} else if (a->data[n] < b->data[n]) {
			return -1;
		}
	}

	return 0;
}


error_t BN_Copy(BN * r, const BN * a) {
	if (r == a) { // are r and a the exact same
		return ERROR_NONE;
	}

	uint_t n = BN_Get_Length(a);

	//Ajust the size of the destination operand
	error_t error = BN_Resize(r, n);
	//Any error to report?
	if (error) {
		return error;
	}

	memset(r->data, 0, r->size * BN_INT_SIZE);

	memcpy(r->data, a->data, n * BN_INT_SIZE);

	r->sign = a->sign;

	return ERROR_NONE;
}
error_t BN_Set_Value(BN * r, long long int a) {
	error_t error = BN_Resize(r, sizeof(a) / BN_INT_SIZE);
	if (error) {
		return error;
	}

	memset(r->data, 0, r->size * BN_INT_SIZE);

	r->data[0] = (a >= 0) ? a : -a;
	r->data[1] = (a >= 0) ? (a>>8*BN_INT_SIZE) : -(a >> 8 * BN_INT_SIZE);

	r->sign = (a >= 0) ? 1 : -1;

	return ERROR_NONE;
}


error_t BN_Randomize(BN * r, uint_t length) {
	uint_t n = (length + (BN_INT_SIZE * 8) - 1) / (BN_INT_SIZE * 8);
	uint_t m = length % (BN_INT_SIZE * 8);

	error_t error = BN_Resize(r, n);

	if (error) {
		return error;
	}

	memset(r->data, 0, r->size * BN_INT_SIZE);
	r->sign = 1;

	//Generate a random pattern
	for (int i = 0; i < length; i++) {
		error |= BN_Set_Bit_Value(r, i, rand()%2);
	}
	if (error) {
		return error;
	}

	if (n > 0 && m > 0)
	{
		r->data[n - 1] &= (1 << m) - 1;
	}

	return ERROR_NONE;
}


error_t BN_Add(BN * r, const BN * a, const BN * b) {
	error_t error = 0;

	int_t sign = a->sign;

	if (a->sign == b->sign) {
		error = BN_Add_Absolute(r, a, b);
		r->sign = sign;
	} else {
		if (BN_Compare_Abs(a, b) >= 0) {
			//Perform subtraction
			error = BN_Sub_Absolute(r, a, b);
			r->sign = sign;
		} else {
			error = BN_Sub_Absolute(r, b, a);
			r->sign = -sign;
		}
	}

	return error;
}
error_t BN_Add_Int(BN * r, const BN * a, int_t b) {
	uint_t value;
	
	// Create demo BN from b
	BN t;
	value = (b >= 0) ? b : -b;
	t.sign = (b >= 0) ? 1 : -1;
	t.size = 1;
	t.data = &value;

	return BN_Add(r, a, &t);
}
error_t BN_Add_Absolute(BN * r, const BN * a, const BN * b) {
	error_t error = 0;
	uint_t i;
	uint_t n;
	uint_t c;
	uint_t d;

	// If b and r are the same, swap a and b
	if (r == b)
	{
		const BN * t = a;
		a = b;
		b = t;
	} else if (r != a)
	{
		BN_Copy(r, a); // Copy a to r
	}

	n = BN_Get_Length(b);
	BN_Resize(r, n);

	r->sign = 1;

	c = 0; // Carry bit

	// Add operands
	for (i = 0; i < n; i++)
	{
		// Add carry bit
		d = r->data[i] + c;
		// Update carry bit
		if (d != 0) c = 0;
		// Perform addition
		d += b->data[i];
		// Update carry bit
		if (d < b->data[i]) c = 1;
		// Save result
		r->data[i] = d;
	}

	// Loop as long as the carry bit is set
	for (i = n; c && i < r->size; i++)
	{
		// Add carry bit
		r->data[i] += c;
		// Update carry bit
		if (r->data[i] != 0) c = 0;
	}

	// Check the final carry bit
	if (c && n >= r->size)
	{
		// Extend the size of the destination register
		BN_Resize(r, n + 1);
		// Add carry bit
		r->data[n] = 1;
	}

	return error;
}

error_t BN_Sub(BN * r, const BN * a, const BN * b) {
	error_t error = 0;
	int_t sign = a->sign;

	if (a->sign == b->sign)
	{
		if (BN_Compare_Abs(a, b) >= 0)
		{
			error = BN_Sub_Absolute(r, a, b);
			r->sign = sign;
		}
		else
		{
			error = BN_Sub_Absolute(r, b, a);
			r->sign = -sign;
		}
	} else {
		error = BN_Add_Absolute(r, a, b);
		r->sign = sign;
	}
	return error;
}
error_t BN_Sub_Int(BN * r, const BN * a, int_t b) {
	uint_t value;

	// Create BN from b
	BN t;
	value = (b >= 0) ? b : -b;
	t.sign = (b >= 0) ? 1 : -1;
	t.size = 1;
	t.data = &value;

	return BN_Sub(r, a, &t);
}
error_t BN_Sub_Absolute(BN * r, const BN * a, const BN * b) {
	error_t error = 0;
	uint_t c;
	uint_t d;
	uint_t i;
	uint_t m;
	uint_t n;

	if (BN_Compare_Abs(a, b) < 0)
	{
		const BN * t = b;
		a = b;
		b = t;
	}

	m = BN_Get_Length(a);
	n = BN_Get_Length(b);

	BN_Resize(r, m);

	r->sign = 1;

	c = 0; // Carry bit

	for (i = 0; i < n; i++)
	{
		d = a->data[i];

		//Check the carry bit
		if (c)
		{
			//Update carry bit
			if (d != 0) c = 0;
			//Propagate carry bit
			d -= 1;
		}

		//Update carry bit
		if (d < b->data[i]) c = 1;
		//Perform subtraction
		r->data[i] = d - b->data[i];
	}

	//Loop as long as the carry bit is set
	for (i = n; c && i < m; i++)
	{
		//Update carry bit
		if (a->data[i] != 0) c = 0;
		//Propagate carry bit
		r->data[i] = a->data[i] - 1;
	}

	//R and A are not the same instance?
	if (r != a)
	{
		//Copy the remaining words
		for (; i < m; i++)
		{
			r->data[i] = a->data[i];
		}

		//Zero the upper part of R
		for (; i < r->size; i++)
		{
			r->data[i] = 0;
		}
	}

	return error;
}


error_t BN_Shift_Left(BN * r, uint_t l) {
	error_t error = 0;
	uint_t i;

	//Number of 32-bit words to shift
	uint_t n1 = l / (BN_INT_SIZE * 8);
	//Number of bits to shift
	uint_t n2 = l % (BN_INT_SIZE * 8);

	if (!r->size || !l) {
		return ERROR_NONE;
	}

	error = BN_Resize(r, r->size + (l + 31) / 32);
	if (error) {
		return error;
	}

	//First, shift words
	if (n1 > 0)
	{
		//Process the most significant words
		for (i = r->size - 1; i >= n1; i--)
		{
			r->data[i] = r->data[i - n1];
		}

		//Fill the rest with zeroes
		for (i = 0; i < n1; i++)
		{
			r->data[i] = 0;
		}
	}

	//Then shift bits
	if (n2 > 0)
	{
		//Process the most significant words
		for (i = r->size - 1; i >= 1; i--)
		{
			r->data[i] = (r->data[i] << n2) | (r->data[i - 1] >> (32 - n2));
		}

		//The least significant word requires a special handling
		r->data[0] <<= n2;
	}

	return ERROR_NONE;
}
error_t BN_Shift_Right(BN * r, uint_t l) {
	uint_t i;
	uint_t m;

	//Number of 32-bit words to shift
	uint_t n1 = l / (BN_INT_SIZE * 8);
	//Number of bits to shift
	uint_t n2 = l % (BN_INT_SIZE * 8);

	//Check parameters
	if (n1 >= r->size)
	{// If we are moving more bits than there are, then reset all the data.
		memset(r->data, 0, r->size * BN_INT_SIZE);
		return ERROR_NONE;
	}

	//First, shift words
	if (n1 > 0)
	{
		//Process the least significant words
		for (m = r->size - n1, i = 0; i < m; i++)
		{
			r->data[i] = r->data[i + n1];
		}

		//Fill the rest with zeroes
		for (i = m; i < r->size; i++)
		{
			r->data[i] = 0;
		}
	}

	//Then shift bits
	if (n2 > 0)
	{
		//Process the least significant words
		for (m = r->size - n1 - 1, i = 0; i < m; i++)
		{
			r->data[i] = (r->data[i] >> n2) | (r->data[i + 1] << (32 - n2));
		}

		//The most significant word requires a special handling
		r->data[m] >>= n2;
	}

	// Check if we need to resize
	uint_t length_whole;
	uint_t length_bits;
	uint_t total_length;

	total_length = BN_Get_Bit_Length(r);
	length_whole = total_length / (BN_INT_SIZE * 8);
	length_bits = total_length % (BN_INT_SIZE * 8);

	uint_t actual_length;
	actual_length = ceil_div(((length_whole * BN_INT_SIZE * 8) + length_bits) , (BN_INT_SIZE * 8));

	if (actual_length != r->size && actual_length > 0) {
		// Resize
		BN_Resize(r, actual_length);
	}

	return ERROR_NONE;
}


error_t BN_Mul(BN * r, const BN * a, const BN * b) {
	error_t error = 0;
	int_t i;
	int_t m;
	int_t n;
	BN ta;
	BN tb;

	//Initialize multiple precision integers
	BN_Init_Stack(&ta);
	BN_Init_Stack(&tb);

	if (r == a)
	{
		//Copy A to TA
		BN_Copy(&ta, a);
		a = &ta;
	}

	if (r == b)
	{
		//Copy B to TB
		BN_Copy(&tb, b);
		b = &tb;
	}

	m = BN_Get_Length(a);
	n = BN_Get_Length(b);

	//Adjust the size of R
	BN_Resize(r, m + n);

	r->sign = (a->sign == b->sign) ? 1 : -1;

	//Clear the contents of the destination integer
	memset(r->data, 0, r->size * BN_INT_SIZE);

	//Perform multiplication
	if (m < n)
	{
		for (i = 0; i < m; i++)
		{
			BN_Mul_Core(&r->data[i], b->data, n, a->data[i]);
		}
	}
	else
	{
		for (i = 0; i < n; i++)
		{
			BN_Mul_Core(&r->data[i], a->data, m, b->data[i]);
		}
	}

	free(ta.data);
	free(tb.data);

	return error;
}
error_t BN_Mul_Int(BN * r, const BN * a, int_t b) {
	uint_t value;
	
	// Create BN from b
	BN t;
	value = (b >= 0) ? b : -b;
	t.sign = (b >= 0) ? 1 : -1;
	t.size = 1;
	t.data = &value;

	return BN_Mul(r, a, &t);
}


error_t BN_Div(BN * q, BN * r, const BN * a, const BN * b) {
	error_t error = 0;
	uint_t m;
	uint_t n;
	BN c;
	BN d;
	BN e;

	//Check whether the divisor is zero
	if (!BN_Compare_Int(b, 0)) {
		return ERROR_FAILED;
	}

	BN_Init_Stack(&c);
	BN_Init_Stack(&d);
	BN_Init_Stack(&e);

	BN_Copy(&c, a);
	BN_Copy(&d, b);
	BN_Set_Value(&e, 0);

	m = BN_Get_Bit_Length(&c);
	n = BN_Get_Bit_Length(&d);

	if (m > n) {
		BN_Shift_Left(&d, m - n);
	}

	while (n++ <= m)
	{
		BN_Shift_Left(&e, 1);

		int_t cmp = BN_Compare(&c, &d);

		if (cmp >= 0)
		{
			BN_Set_Bit_Value(&e, 0, 1);
			BN_Sub(&c, &c, &d);
		}

		BN_Shift_Right(&d, 1);
	}

	if (q != NULL) {
		BN_Copy(q, &e);
	}

	if (r != NULL) {
		BN_Copy(r, &c);
	}

	free(c.data);
	free(d.data);
	free(e.data);

	return error;
}
error_t BN_Div_Int(BN * q, BN * r, const BN * a, int_t b) {
	uint_t value;

	// Create BN from b
	BN t;
	value = (b >= 0) ? b : -b;
	t.sign = (b >= 0) ? 1 : -1;
	t.size = 1;
	t.data = &value;

	return BN_Div(q, r, a, &t);
}

error_t BN_Mod(BN * r, const BN * a, const BN * p) {
	error_t error = 0;
	int_t sign;
	uint_t m;
	uint_t n;
	BN c;

	if (BN_Compare_Int(p, 0) <= 0) {
		return ERROR_FAILED;
	}

	BN_Init_Stack(&c);

	sign = a->sign;
	m = BN_Get_Bit_Length(a);
	n = BN_Get_Bit_Length(p);

	BN_Copy(r, a);

	if (m >= n)
	{
		BN_Copy(&c, p);
		BN_Shift_Left(&c, m - n);

		while (BN_Compare_Abs(r, p) >= 0)
		{
			if (BN_Compare_Abs(r, &c) >= 0)
			{
				BN_Sub_Absolute(r, r, &c);
			}

			BN_Shift_Right(&c, 1);
		}
	}

	if (sign < 0)
	{
		BN_Sub_Absolute(r, p, r);
	}

	free(c.data);

	return error;
}

error_t BN_Add_Mod(BN * r, const BN * a, const BN * b, const BN * p) {
	error_t error = 0;

	BN_Add(r, a, b);
	BN_Mod(r, r, p);

	return error;
}

error_t BN_Sub_Mod(BN * r, const BN * a, const BN * b, const BN * p) {
	error_t error = 0;

	BN_Sub(r, a, b);
	BN_Mod(r, r, p);

	return error;
}

error_t BN_Mul_Mod(BN * r, const BN * a, const BN * b, const BN * p) {
	error_t error = 0;

	BN_Mul(r, a, b);
	BN_Mod(r, r, p);

	return error;
}

error_t BN_Exp_Mod(BN * r, const BN * a, const BN * e, const BN * p) {
	error_t error = 0;
	int_t i;
	int_t j;
	int_t n;
	uint_t d;
	uint_t k;
	uint_t u;
	BN b;
	BN c2;
	BN t;
	BN s[8];

	//Initialize
	BN_Init_Stack(&b);
	BN_Init_Stack(&c2);
	BN_Init_Stack(&t);

	//Initialize precomputed values
	for (i = 0; i < arraysize(s); i++)
	{
		BN_Init_Stack(&s[i]);
	}

	//Very small exponents are often selected with low Hamming weight.
	//The sliding window mechanism should be disabled in that case
	d = (BN_Get_Bit_Length(e) <= 32) ? 1 : 4;

	if (BN_Is_Even(p))
	{
		//Let B = A^2
		BN_Mul_Mod(&b, a, a, p);
		BN_Copy(&s[0], a);

		//Precompute S[i] = A^(2 * i + 1)
		for (i = 1; i < (1 << (d - 1)); i++)
		{
			BN_Mul_Mod(&s[i], &s[i - 1], &b, p);
		}

		//Let R = 1
		BN_Set_Value(r, 1);

		//The exponent is processed in a left-to-right fashion
		i = BN_Get_Bit_Length(e) - 1;

		//Perform sliding window exponentiation
		while (i >= 0)
		{
			//The sliding window exponentiation algorithm decomposes E
			//into zero and nonzero windows
			if (!BN_Get_Bit_Value(e, i))
			{
				//Compute R = R^2
				BN_Mul_Mod(r, r, r, p);
				//Next bit to be processed
				i--;
			}
			else
			{
				//Find the longest window
				n = MAX(i - d + 1, 0);

				//The least significant bit of the window must be equal to 1
				while (!BN_Get_Bit_Value(e, n)) n++;

				//The algorithm processes more than one bit per iteration
				for (u = 0, j = i; j >= n; j--)
				{
					//Compute R = R^2
					BN_Mul_Mod(r, r, r, p);
					//Compute the relevant index to be used in the precomputed table
					u = (u << 1) | BN_Get_Bit_Value(e, j);
				}

				//Perform a single multiplication per iteration
				BN_Mul_Mod(r, r, &s[u >> 1], p);
				//Next bit to be processed
				i = n - 1;
			}
		}
	}
	else
	{
		//Compute the smaller C = (2^32)^k such as C > P
		k = BN_Get_Length(p);

		//Compute C^2 mod P
		BN_Set_Value(&c2, 1);
		BN_Shift_Left(&c2, 2 * k * (BN_INT_SIZE * 8));
		BN_Mod(&c2, &c2, p);

		//Let B = A * C mod P
		if (BN_Compare(a, p) >= 0)
		{
			BN_Mod(&b, a, p);
			BN_Montgomery_Mul(&b, &b, &c2, k, p, &t);
		}
		else
		{
			BN_Montgomery_Mul(&b, a, &c2, k, p, &t);
		}

		//Let R = B^2 * C^-1 mod P
		BN_Montgomery_Mul(r, &b, &b, k, p, &t);
		//Let S[0] = B
		BN_Copy(&s[0], &b);

		//Precompute S[i] = B^(2 * i + 1) * C^-1 mod P
		for (i = 1; i < (1 << (d - 1)); i++)
		{
			BN_Montgomery_Mul(&s[i], &s[i - 1], r, k, p, &t);
		}

		//Let R = C mod P
		BN_Copy(r, &c2);
		BN_Montgomery_Red(r, r, k, p, &t);

		//The exponent is processed in a left-to-right fashion
		i = BN_Get_Bit_Length(e) - 1;

		//Perform sliding window exponentiation
		while (i >= 0)
		{
			//The sliding window exponentiation algorithm decomposes E
			//into zero and nonzero windows
			if (!BN_Get_Bit_Value(e, i))
			{
				//Compute R = R^2 * C^-1 mod P
				BN_Montgomery_Mul(r, r, r, k, p, &t);
				//Next bit to be processed
				i--;
			}
			else
			{
				//Find the longest window
				n = MAX(i - d + 1, 0);

				//The least significant bit of the window must be equal to 1
				while (!BN_Get_Bit_Value(e, n)) n++;

				//The algorithm processes more than one bit per iteration
				for (u = 0, j = i; j >= n; j--)
				{
					//Compute R = R^2 * C^-1 mod P
					BN_Montgomery_Mul(r, r, r, k, p, &t);
					//Compute the relevant index to be used in the precomputed table
					u = (u << 1) | BN_Get_Bit_Value(e, j);
				}

				//Compute R = R * T[u/2] * C^-1 mod P
				BN_Montgomery_Mul(r, r, &s[u >> 1], k, p, &t);
				//Next bit to be processed
				i = n - 1;
			}
		}

		//Compute R = R * C^-1 mod P
		BN_Montgomery_Red(r, r, k, p, &t);
	}

	free(b.data);
	free(c2.data);
	free(t.data);

	for (i = 0; i < arraysize(s); i++)
	{
		free(s[i].data);
	}

	return error;
}

error_t BN_Inv_Mod(BN * r, const BN * a, const BN * p) {
	error_t error = 0;
	BN b;
	BN c;
	BN q0;
	BN r0;
	BN t;
	BN u;
	BN v;

	//Initialize multiple precision integers
	BN_Init_Stack(&b);
	BN_Init_Stack(&c);
	BN_Init_Stack(&q0);
	BN_Init_Stack(&r0);
	BN_Init_Stack(&t);
	BN_Init_Stack(&u);
	BN_Init_Stack(&v);

	BN_Copy(&b, p);
	BN_Copy(&c, a);
	BN_Set_Value(&u, 0);
	BN_Set_Value(&v, 1);

	while (BN_Compare_Int(&c, 0) > 0)
	{
		BN_Div(&q0, &r0, &b, &c);

		BN_Copy(&b, &c);
		BN_Copy(&c, &r0);

		BN_Copy(&t, &v);
		BN_Mul(&q0, &q0, &v);
		BN_Sub(&v, &u, &q0);
		BN_Copy(&u, &t);
	}

	if (BN_Compare_Int(&b, 1))
	{
		//MPI_CHECK(ERROR_FAILURE);
	}

	if (BN_Compare_Int(&u, 0) > 0)
	{
		BN_Copy(r, &u);
	}
	else
	{
		BN_Add(r, &u, p);
	}

	free(b.data);
	free(c.data);
	free(q0.data);
	free(r0.data);
	free(t.data);
	free(u.data);
	free(v.data);

	return error;
}

error_t BN_Montgomery_Mul(BN * r, const BN * a, const BN * b, uint_t k, const BN * p, BN * t)
{
	error_t error = 0;
	uint_t i;
	uint_t m;
	uint_t n;
	uint_t q;

	//Use Newton's method to compute the inverse of P[0] mod 2^32
	for (m = 2 - p->data[0], i = 0; i < 4; i++)
	{
		m = m * (2 - m * p->data[0]);
	}

	//Precompute -1/P[0] mod 2^32;
	m = ~m + 1;

	//We assume that B is always less than 2^k
	n = MIN(b->size, k);

	//Make sure T is large enough
	BN_Resize(t, 2 * k + 1);
	//Let T = 0
	BN_Set_Value(t, 0);

	//Perform Montgomery multiplication
	for (i = 0; i < k; i++)
	{
		//Check current index
		if (i < a->size)
		{
			//Compute q = ((T[i] + A[i] * B[0]) * m) mod 2^32
			q = (t->data[i] + a->data[i] * b->data[0]) * m;
			//Compute T = T + A[i] * B
			BN_Mul_Core(t->data + i, b->data, n, a->data[i]);
		}
		else
		{
			//Compute q = (T[i] * m) mod 2^32
			q = t->data[i] * m;
		}

		//Compute T = T + q * P
		BN_Mul_Core(t->data + i, p->data, k, q);
	}

	//Compute R = T / 2^(32 * k)
	BN_Shift_Right(t, k * (BN_INT_SIZE * 8));
	BN_Copy(r, t);

	//A final subtraction is required
	if (BN_Compare(r, p) >= 0)
	{
		BN_Sub(r, r, p);
	}

	return error;
}
error_t BN_Montgomery_Red(BN * r, const BN * a, uint_t k, const BN * p, BN * t)
{
	uint_t value;

	//Let B = 1
	BN b;
	value = 1;
	b.sign = 1;
	b.size = 1;
	b.data = &value;

	//Compute R = A / 2^k mod P
	return BN_Montgomery_Mul(r, a, &b, k, p, t);
}


void BN_Mul_Core(uint_t *r, const uint_t *a, int_t m, const uint_t b)
{
	int_t i;
	uint32_t c;
	uint32_t u;
	uint32_t v;
	uint64_t p;

	//Clear variables
	c = 0;
	u = 0;
	v = 0;

	//Perform multiplication
	for (i = 0; i < m; i++)
	{
		p = (uint64_t)a[i] * b;
		u = (uint32_t)p;
		v = (uint32_t)(p >> 32);

		u += c;
		if (u < c) v++;

		u += r[i];
		if (u < r[i]) v++;

		r[i] = u;
		c = v;
	}

	//Propagate carry
	for (; c != 0; i++)
	{
		r[i] += c;
		c = (r[i] < c);
	}
}

void BN_Dump(FILE * stream, const char * prepend, const BN * a)
{
	uint_t i;

	//Process each word
	for (i = 0; i < a->size; i++)
	{
		//Beginning of a new line?
		if (i == 0 || ((a->size - i - 1) % 8) == 7)
			fprintf(stream, "%s", prepend);

		//Display current data
		fprintf(stream, "%08X ", a->data[a->size - 1 - i]);

		//End of current line?
		if (((a->size - i - 1) % 8) == 0 || i == (a->size - 1))
			fprintf(stream, "\r\n");
	}
}

error_t BN_Import(BN * r, const uint8_t * data, uint_t length, BN_ENDIAN_FORMAT format) {
	error_t error;
	uint_t i;

	//Check input format
	if (format == BN_LITTLE_ENDIAN)
	{
		//Skip trailing zeroes
		while (length > 0 && data[length - 1] == 0)
		{
			length--;
		}

		error = BN_Resize(r, (length + BN_INT_SIZE - 1) / BN_INT_SIZE);

		if (!error)
		{
			memset(r->data, 0, r->size * BN_INT_SIZE);
			r->sign = 1;

			//Import data
			for (i = 0; i < length; i++, data++)
			{
				r->data[i / BN_INT_SIZE] |= *data << ((i % BN_INT_SIZE) * 8);
			}
		}
	}
	else if (format == BN_BIG_ENDIAN)
	{
		//Skip leading zeroes
		while (length > 1 && *data == 0)
		{
			data++;
			length--;
		}

		error = BN_Resize(r, (length + BN_INT_SIZE - 1) / BN_INT_SIZE);

		if (!error)
		{
			memset(r->data, 0, r->size * BN_INT_SIZE);
			r->sign = 1;

			//Start from the least significant byte
			data += length - 1;

			//Import data
			for (i = 0; i < length; i++, data--)
			{
				r->data[i / BN_INT_SIZE] |= *data << ((i % BN_INT_SIZE) * 8);
			}
		}
	}
	else
	{
		error = ERROR_FAILED;
	}

	return error;
}

int BN_Is_Prime(BN * r) {

	uint_t order = BN_Get_Bit_Length(r);
	uint_t sqrt_order = ceil_div(order, 2);

	BN sqr;
	BN_Init_Stack(&sqr);

	for (int i = 0; i < sqrt_order; i++) {
		BN_Set_Bit_Value(&sqr, i, 1);
	}

	BN c;
	BN_Init_Stack(&c);
	BN_Set_Value(&c, 2);

	BN q;
	BN rem;
	BN_Init_Stack(&q);
	BN_Init_Stack(&rem);

	while (BN_Compare(&c, &sqr) <= 0) {
		BN_Div(&q, &rem, r, &c);

		if (BN_Compare_Int(&rem, 0) == 0) {
			free(sqr.data);
			free(c.data);
			free(q.data);
			free(rem.data);
			return 0;
		}

		BN_Add_Int(&c, &c, 1);
	}

	free(sqr.data);
	free(c.data);
	free(q.data);
	free(rem.data);

	return 1;
}