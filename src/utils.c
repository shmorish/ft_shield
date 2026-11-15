#include "ft_shield.h"

ssize_t	send_str(int fd, const char *s)
{
	size_t	len;
	size_t	off;
	ssize_t	n;

	len = strlen(s);
	off = 0;
	while (off < len)
	{
		n = send(fd, s + off, len - off, MSG_NOSIGNAL);
		if (n <= 0)
			return (-1);
		off += (size_t)n;
	}
	return ((ssize_t)off);
}

ssize_t	read_line(int fd, char *buf, size_t max)
{
	size_t	i;
	char	ch;
	ssize_t	n;

	i = 0;
	while (i < max)
	{
		n = recv(fd, &ch, 1, 0);
		if (n < 0)
			return (-1);
		if (n == 0)
			break ;
		if (ch == '\n')
			break ;
		if (ch != '\r')
			buf[i++] = ch;
	}
	buf[i] = '\0';
	return ((ssize_t)i);
}
