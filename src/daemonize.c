#include "ft_shield.h"

int	daemonize(void)
{
	int	fd;

	if (fork() > 0)
		_exit(0);
	if (setsid() < 0)
		return (-1);
	if (fork() > 0)
		_exit(0);
	umask(0);
	if (chdir("/") != 0)
		return (-1);
	fd = open("/dev/null", O_RDWR);
	if (fd < 0)
		return (-1);
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
	if (fd > STDERR_FILENO)
		close(fd);
	return (0);
}

