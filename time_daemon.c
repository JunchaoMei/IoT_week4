// Compilation Command: gcc -o timed time_daemon.c
// Check syslog: tail /var/log/syslog
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
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#define OK           0
#define ERR_FORK     28
#define ERR_SETSID   58
#define ERR_CHDIR    37
#define DAEMON_NAME	"timed"

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
		struct timeval tv;
		gettimeofday(&tv,NULL);
		uint64_t sec=tv.tv_sec;
		uint64_t min=tv.tv_sec/60;
		struct tm cur_tm;
		localtime_r((time_t*)&sec,&cur_tm);
		char cur_time[20];
		snprintf(cur_time,20,"%d-%02d-%02d %02d:%02d:%02d",cur_tm.tm_year+1900,cur_tm.tm_mon+1,cur_tm.tm_mday,cur_tm.tm_hour,cur_tm.tm_min,cur_tm.tm_sec);
		syslog(LOG_INFO, "system time: %s", cur_time);
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