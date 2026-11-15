#include "ft_shield.h"

static int	g_lock_fd = -1;

int	acquire_lock(void)
{
	g_lock_fd = open(LOCK_PATH, O_RDWR | O_CREAT, 0644);
	if (g_lock_fd < 0)
		return (-1);
	if (flock(g_lock_fd, LOCK_EX | LOCK_NB) != 0)
		return (-1);
	return (0);
}

void	write_pidfile(void)
{
	char	buf[32];
	int	fd;
	int	n;

	fd = open(PID_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return ;
	n = snprintf(buf, sizeof(buf), "%d\n", (int)getpid());
	if (n > 0)
		write(fd, buf, (size_t)n);
	close(fd);
}
