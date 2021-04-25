
#include "SHA256.h"

// Format a message into a standard 512bit block
size_t format_message(char * message, size_t message_len, unsigned char ** output_message_ptr) {

	size_t output_message_len = message_len + (512 - (message_len % 512));

	size_t pos;

	*output_message_ptr = (unsigned char *)malloc(output_message_len);

	unsigned char * output_message = *output_message_ptr;

	// Copy the message into the beginning of the formatted message
	memcpy(output_message, message, message_len);

	// Set bit after message to 1, and the rest to 0.
	output_message[message_len] = 0b10000000;

	pos = message_len + 1;

	while (pos < output_message_len) {
		output_message[pos++] = 0;
	}

	output_message[output_message_len - 9] = (uint64_t)message_len;

	return output_message_len;
}


uint32_t ft_bswap_uint32(uint32_t x)
{
	x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0xFF00FF);
	return (x << 16) | (x >> 16);
}

uint64_t ft_bswap_uint64(uint64_t x)
{
	x = ((x << 8) & 0xFF00FF00FF00FF00ULL)
		| ((x >> 8) & 0x00FF00FF00FF00FFULL);
	x = ((x << 16) & 0xFFFF0000FFFF0000ULL)
		| ((x >> 16) & 0x0000FFFF0000FFFFULL);
	return (x << 32) | (x >> 32);
}

char	*ft_strncpy(char *dst, const char *src, size_t n)
{
	size_t i;

	i = 0;
	while (src[i] != '\0' && i < n)
	{
		dst[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dst[i] = '\0';
		i++;
	}
	return (dst);
}

void	ft_uint32_arr_assign_add(uint32_t *dst, const uint32_t *src, size_t len)
{
	size_t i;

	i = 0;
	while (i < len)
	{
		dst[i] += src[i];
		i++;
	}
}

void	ft_uint32_arr_cpy(uint32_t *dst, const uint32_t *src, size_t len)
{
	size_t i;

	i = 0;
	while (i < len)
	{
		dst[i] = src[i];
		i++;
	}
}

void	ft_uint64_arr_assign_add(uint64_t *dst, const uint64_t *src, size_t len)
{
	size_t i;

	i = 0;
	while (i < len)
	{
		dst[i] += src[i];
		i++;
	}
}

void	ft_uint64_arr_cpy(uint64_t *dst, const uint64_t *src, size_t len)
{
	size_t i;

	i = 0;
	while (i < len)
	{
		dst[i] = src[i];
		i++;
	}
}



uint32_t	ft_brotate_left(uint32_t x, char rotations)
{
	return ((x << rotations) | (x >> (32 - rotations)));
}

uint32_t	ft_brotate_right(uint32_t x, char rotations)
{
	return ((x >> rotations) | (x << (32 - rotations)));
}

uint64_t	ft_brotate_right_64(uint64_t x, char rotations)
{
	return ((x >> rotations) | (x << (64 - rotations)));
}

uint32_t	sha256_op_ch(uint32_t x, uint32_t y, uint32_t z)
{
	return ((x & y) ^ (~x & z));
}

uint32_t sha256_op_maj(uint32_t x, uint32_t y, uint32_t z)
{
	return ((x & y) ^ (x & z) ^ (y & z));
}

uint32_t sha256_op_a(uint32_t x)
{
	return (ft_brotate_right(x, 2) ^ ft_brotate_right(x, 13) ^ ft_brotate_right(x, 22));
}

uint32_t sha256_op_b(uint32_t x)
{
	return (ft_brotate_right(x, 6) ^ ft_brotate_right(x, 11) ^ ft_brotate_right(x, 25));
}

uint32_t sha256_op_c(uint32_t x)
{
	return (ft_brotate_right(x, 7) ^ ft_brotate_right(x, 18) ^ (x >> 3));
}

uint32_t sha256_op_d(uint32_t x)
{
	return (ft_brotate_right(x, 17) ^ ft_brotate_right(x, 19) ^ (x >> 10));
}

char	*ft_uitoa_base(uintmax_t nb, intmax_t base, char letter)
{
	uintmax_t	temp;
	int			power;
	char		*str;

	temp = nb;
	power = 1;
	while (temp /= base)
		power++;
	if (!(str = (char *)calloc(power + 1, sizeof(char)))) {
		return (NULL);
	}
	while (power--)
	{
		if (nb % base >= 10)
			str[power] = nb % base - 10 + letter;
		else
			str[power] = nb % base + '0';
		nb /= base;
	}
	return (str);
}
char	*ft_uitoa_base_len(uintmax_t nb, intmax_t base, char letter, size_t len)
{
	int		i;
	int		diff;
	char	*str;
	char	*new_str;

	i = 0;
	str = ft_uitoa_base(nb, base, letter);
	diff = len - strlen(str);
	if (diff > 0)
	{
		if (!(new_str = (char*)calloc(len + 1, sizeof(char))))
			return (NULL);
		while (i < diff)
			new_str[i++] = '0';
		ft_strncpy(new_str + i, str, len - diff);
		free(str);
		return (new_str);
	}
	return (str);
}

void sha256_init_w_array(t_64_uint32 w_array, unsigned char *formatted_msg)
{
	int i;

	i = 0;
	while (i < 64)
	{
		if (i < 16) {
			w_array[i] = ft_bswap_uint32(((uint32_t *)formatted_msg)[i]);
		}
		else {
			w_array[i] = sha256_op_d(w_array[i - 2]) + w_array[i - 7] + sha256_op_c(w_array[i - 15]) + w_array[i - 16];
		}
		i++;
	}
}

void sha256_shuffle_buffers(t_8_uint32 buffers, t_64_uint32 w_array)
{
	int	i = 0;
	uint32_t temp_a;
	uint32_t temp_b;

	while (i < 64)
	{
		temp_a = buffers[7] + sha256_op_b(buffers[4]) + sha256_op_ch(buffers[4], buffers[5], buffers[6]) + g_sha256_k[i] + w_array[i];
		temp_b = sha256_op_a(buffers[0]) + sha256_op_maj(buffers[0], buffers[1], buffers[2]);
		buffers[7] = buffers[6];
		buffers[6] = buffers[5];
		buffers[5] = buffers[4];
		buffers[4] = buffers[3] + temp_a;
		buffers[3] = buffers[2];
		buffers[2] = buffers[1];
		buffers[1] = buffers[0];
		buffers[0] = temp_a + temp_b;
		i++;
	}
}

void	sha256_run_ops(t_8_uint32 buffers,
	unsigned char *formatted_msg, size_t msg_len)
{
	size_t			chunk_i;
	t_64_uint32		w_array;
	t_8_uint32		internal_buffers;

	chunk_i = 0;
	while (chunk_i < SHA256_CHUNK_COUNT(msg_len))
	{
		sha256_init_w_array(w_array, formatted_msg + chunk_i * SHA256_CHUNK_SIZE);
		ft_uint32_arr_cpy(internal_buffers, buffers, 8);
		sha256_shuffle_buffers(internal_buffers, w_array);
		ft_uint32_arr_assign_add(buffers, internal_buffers, 8);
		chunk_i++;
	}
}

char * build_hash(uint32_t *buffers, size_t buffer_count)
{
	char * hash;
	char * hash_tmp;
	size_t buffer_i;
	uint32_t buffer;

	buffer_i = 0;
	if (!(hash = (char*)calloc((buffer_count * 8) + 1, sizeof(char)))) {
		return NULL;
	}

	while (buffer_i < buffer_count)
	{
		buffer = 0 ? ft_bswap_uint32(buffers[buffer_i]) : buffers[buffer_i];
		if (!(hash_tmp = ft_uitoa_base_len(buffer, 16, 'a', 8))) {
			return (NULL);
		}
		ft_strncpy(hash + (buffer_i * 8), hash_tmp, 8);
		free(hash_tmp);
		buffer_i++;
	}
	return hash;
}

// The SHA-256 hash algorithm
char * Hash_SHA256(char * message, size_t message_len) {
	unsigned char	*formatted_msg;
	t_8_uint32		buffers;

	size_t formatted_size = format_message(message, message_len, &formatted_msg);
	if (formatted_size != SHA256_CHUNK_COUNT(message_len) * SHA256_CHUNK_SIZE) {
		//printf("Size missmatch!\n");
	}

	ft_uint32_arr_cpy(buffers, g_sha256_default_buffers, 8);
	sha256_run_ops(buffers, formatted_msg, message_len);
	free(formatted_msg);
	return build_hash(buffers, 8);
}
