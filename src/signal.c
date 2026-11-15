#include "ft_shield.h"

static void	on_sigchld(int sig)
{
	(void)sig;
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
}

void	setup_signals(void)
{
	signal(SIGCHLD, on_sigchld);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
}
