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

int main(void)
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
	    printf("system time: %s\n", cur_time);
	    sleep(1);
    }
}

