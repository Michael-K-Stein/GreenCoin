
#include "Base64.h"

const char BASE64_CHAR_SEQUENCE[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};

static const signed char BASE64_REVERSE_SEQUENCE[256] = {
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

size_t B64_Encoded_Size(size_t len) {
	if (len % 3 != 0) {
		len += 3 - (len % 3);
	}
	len /= 3;
	len *= 4;

	return len;
}

size_t B64_Decoded_Size(char * base64_array) {

	size_t length = strlen(base64_array);

	if (length < 1) { return 0; }

	int padding_count = (int)(base64_array[length - 1] == '=') + (int)(base64_array[length - 2] == '=');

	length = ((length / 4) * 3) - padding_count;

	return length;
}

size_t B64_Encode(byte * input, size_t input_length, char ** output)
{
	char * out, * pos;
	
	byte * end, * in;

	size_t out_len;

	out_len = 4 * ((input_length + 2) / 3);

	if (out_len < input_length) {
		return -1;
	}

	*output = (char*)malloc(out_len + 1);
	out = *output;

	end = input + input_length;
	in = input;
	pos = out;

	while (end - in >= 3) {
		*pos++ = BASE64_CHAR_SEQUENCE[in[0] >> 2];
		*pos++ = BASE64_CHAR_SEQUENCE[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*pos++ = BASE64_CHAR_SEQUENCE[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*pos++ = BASE64_CHAR_SEQUENCE[in[2] & 0x3f];
		in += 3;
	}

	if (end - in) {
		*pos++ = BASE64_CHAR_SEQUENCE[in[0] >> 2];
		if (end - in == 1) {
			*pos++ = BASE64_CHAR_SEQUENCE[(in[0] & 0x03) << 4];
			*pos++ = '=';
		}
		else {
			*pos++ = BASE64_CHAR_SEQUENCE[((in[0] & 0x03) << 4) |
				(in[1] >> 4)];
			*pos++ = BASE64_CHAR_SEQUENCE[(in[1] & 0x0f) << 2];
		}
		*pos++ = '=';
	}

	*pos = 0x0;

	return out_len;
}

size_t B64_Decode(char * input, byte ** output) {
	size_t input_length = strlen(input);
	size_t length = B64_Decoded_Size(input);

	*output = (byte*)malloc(length+1);
	char * out = *output;

	char * pos = out;
	char * end = out + length;
	char * in = input;

	while (end - pos >= 3) {
		// 4 base64 -> 3 char
		// 4 * 6bits => 3 * 8bits = 24bits
		pos[0] = ((BASE64_REVERSE_SEQUENCE[in[0]] << 2) | (BASE64_REVERSE_SEQUENCE[in[1]] >> 4));
		pos[1] = (((BASE64_REVERSE_SEQUENCE[in[1]] & 0x0f) << 4) | (BASE64_REVERSE_SEQUENCE[in[2]] >> 2));
		pos[2] = (((BASE64_REVERSE_SEQUENCE[in[2]] & 0x03) << 6) | (BASE64_REVERSE_SEQUENCE[in[3]]));

		pos += 3;
		in += 4;
	}

	if (end - pos) {
		// =
		int padding_count = (int)(input[input_length - 1] == '=') + (int)(input[input_length - 2] == '=');
		if (padding_count == 1) {
			pos[0] = ((BASE64_REVERSE_SEQUENCE[in[0]] << 2) | (BASE64_REVERSE_SEQUENCE[in[1]] >> 4));
			pos[1] = (((BASE64_REVERSE_SEQUENCE[in[1]] & 0x0f) << 4) | (BASE64_REVERSE_SEQUENCE[in[2]] >> 2));
			pos += 2;
			in += 2;
		}
		else if (padding_count == 2) {
			pos[0] = ((BASE64_REVERSE_SEQUENCE[in[0]] << 2) | (BASE64_REVERSE_SEQUENCE[in[1]] >> 4));
			pos++;
			in += 2;
		}
		else {
			// Error?
		}
	}

	pos[0] = 0x0;

	return length;
}