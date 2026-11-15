#include "ft_shield.h"

static const uint32_t	g_k[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static uint32_t	rotr(uint32_t x, int n)
{
	return ((x >> n) | (x << (32 - n)));
}

void	sha256_init(t_sha256 *ctx)
{
	ctx->len = 0;
	ctx->idx = 0;
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
}

static void	sha256_block(t_sha256 *ctx, const unsigned char *p)
{
	uint32_t	w[64];
	uint32_t	v[8];
	uint32_t	t[2];
	int		i;

	i = 0;
	while (i < 16)
	{
		w[i] = ((uint32_t)p[i * 4] << 24) | ((uint32_t)p[i * 4 + 1] << 16)
			| ((uint32_t)p[i * 4 + 2] << 8) | (uint32_t)p[i * 4 + 3];
		i++;
	}
	while (i < 64)
	{
		t[0] = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
		t[1] = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
		w[i] = w[i - 16] + t[0] + w[i - 7] + t[1];
		i++;
	}
	i = 0;
	while (i < 8)
	{
		v[i] = ctx->state[i];
		i++;
	}
	i = 0;
	while (i < 64)
	{
		t[0] = v[7] + (rotr(v[4], 6) ^ rotr(v[4], 11) ^ rotr(v[4], 25))
			+ ((v[4] & v[5]) ^ (~v[4] & v[6])) + g_k[i] + w[i];
		t[1] = (rotr(v[0], 2) ^ rotr(v[0], 13) ^ rotr(v[0], 22))
			+ ((v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]));
		v[7] = v[6];
		v[6] = v[5];
		v[5] = v[4];
		v[4] = v[3] + t[0];
		v[3] = v[2];
		v[2] = v[1];
		v[1] = v[0];
		v[0] = t[0] + t[1];
		i++;
	}
	i = 0;
	while (i < 8)
	{
		ctx->state[i] += v[i];
		i++;
	}
}

void	sha256_update(t_sha256 *ctx, const void *data, size_t len)
{
	const unsigned char	*p;
	size_t			take;

	p = (const unsigned char *)data;
	ctx->len += len;
	while (len > 0)
	{
		take = 64 - ctx->idx;
		if (take > len)
			take = len;
		memcpy(ctx->buf + ctx->idx, p, take);
		ctx->idx += take;
		p += take;
		len -= take;
		if (ctx->idx == 64)
		{
			sha256_block(ctx, ctx->buf);
			ctx->idx = 0;
		}
	}
}

void	sha256_final(t_sha256 *ctx, unsigned char out[HASH_LEN])
{
	unsigned char	pad;
	unsigned char	zero;
	unsigned char	lb[8];
	uint64_t		bits;
	int			i;

	bits = ctx->len * 8;
	pad = 0x80;
	sha256_update(ctx, &pad, 1);
	zero = 0;
	while (ctx->idx != 56)
		sha256_update(ctx, &zero, 1);
	i = 0;
	while (i < 8)
	{
		lb[i] = (unsigned char)(bits >> (56 - 8 * i));
		i++;
	}
	sha256_update(ctx, lb, 8);
	i = 0;
	while (i < 8)
	{
		out[i * 4] = (unsigned char)(ctx->state[i] >> 24);
		out[i * 4 + 1] = (unsigned char)(ctx->state[i] >> 16);
		out[i * 4 + 2] = (unsigned char)(ctx->state[i] >> 8);
		out[i * 4 + 3] = (unsigned char)(ctx->state[i]);
		i++;
	}
}
