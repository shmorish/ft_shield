#include "ft_shield.h"

static void	dump(const char *name, const unsigned char *b, size_t n)
{
	size_t	i;

	printf("static const unsigned char %s[%zu] = {", name, n);
	i = 0;
	while (i < n)
	{
		if (i % 8 == 0)
			printf("\n\t");
		printf("0x%02x", b[i]);
		if (i + 1 < n)
			printf(", ");
		i++;
	}
	printf("\n};\n\n");
}

static void	fill_salt(unsigned char *salt)
{
	int	fd;
	int	i;
	time_t	now;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd >= 0)
	{
		if (read(fd, salt, SALT_LEN) == SALT_LEN)
		{
			close(fd);
			return ;
		}
		close(fd);
	}
	now = time(NULL);
	srand((unsigned int)now ^ (unsigned int)getpid());
	i = 0;
	while (i < SALT_LEN)
		salt[i++] = (unsigned char)(rand() & 0xff);
}

int	main(int ac, char **av)
{
	unsigned char	salt[SALT_LEN];
	unsigned char	out[HASH_LEN];
	t_sha256	ctx;

	if (ac != 2)
	{
		fprintf(stderr, "usage: gen_hash <password>\n");
		return (1);
	}
	fill_salt(salt);
	sha256_init(&ctx);
	sha256_update(&ctx, salt, SALT_LEN);
	sha256_update(&ctx, av[1], strlen(av[1]));
	sha256_final(&ctx, out);
	printf("#ifndef CREDENTIALS_H\n#define CREDENTIALS_H\n\n");
	dump("g_salt", salt, SALT_LEN);
	dump("g_hash", out, HASH_LEN);
	printf("#endif\n");
	return (0);
}

