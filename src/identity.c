#include "ft_shield.h"

void	print_login(void)
{
	const char	*env;
	char		*tty;
	struct stat	st;
	struct passwd	*pw;

	env = getenv("SUDO_USER");
	if (env && *env)
	{
		printf("%s\n", env);
		fflush(stdout);
		return ;
	}
	tty = ttyname(STDIN_FILENO);
	if (tty && stat(tty, &st) == 0)
	{
		pw = getpwuid(st.st_uid);
		if (pw && pw->pw_name)
		{
			printf("%s\n", pw->pw_name);
			fflush(stdout);
			return ;
		}
	}
	pw = getpwuid(getuid());
	if (pw && pw->pw_name)
	{
		printf("%s\n", pw->pw_name);
		fflush(stdout);
	}
}

