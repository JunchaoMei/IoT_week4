//gcc -o sampled sample_daemon.c
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
/*
#include <stdarg.h>
#include <sys/param.h>
#include <inttypes.h>
#include <pthread.h>
*/
#define OK           0
#define ERR_FORK     28
#define ERR_SETSID   58
#define ERR_CHDIR    37

#define DAEMON_NAME	"sampled123"

static void _signal_handler(const int signal)
{
	switch(signal)
	{
		case SIGHUP:
			break;
		case SIGTERM:
			syslog(LOG_INFO, "received SIGTERM, exiting.");
			closelog();
			exit(OK);
			break;
		default:
			syslog(LOG_INFO, "received unhandled signal");
	}
}

static void _do_work(void)
{
	for (int i = 0; true; ++i)
	{
		syslog(LOG_INFO, "iteration: %d", i);
		sleep(1);
	}
}

int main(void)
{
	openlog(DAEMON_NAME, LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_DAEMON);
	syslog(LOG_INFO, "starting sampled");

	pid_t pid = fork();

	if (pid < 0)
	{
		syslog(LOG_ERR, "Forking Error! '%s'\n", strerror(errno));
		exit(ERR_FORK);
	}

	if (pid > 0)
	{
		exit(OK);
	}

	if (setsid() < -1)
	{
		syslog(LOG_ERR, "Error: not the leader of the session! '%s'\n", strerror(errno));
		exit(ERR_SETSID);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	umask(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if (chdir("/") < 0)
	{
		syslog(LOG_ERR, "Error: cannot change to / (root) '%s'\n", strerror(errno));
		exit(ERR_CHDIR);
	}

	signal(SIGTERM, _signal_handler);
	signal(SIGHUP, _signal_handler);

	_do_work();

	exit(OK);
}