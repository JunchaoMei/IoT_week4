#!/bin/sh

DAEMON_NAME="timed"

start()
{
	printf "starting daemon process ${DAEMON_NAME}: "
	/usr/sbin/${DAEMON_NAME}
	touch /var/lock/${DAEMON_NAME}
	echo "successful"
}

stop()
{
	printf "stopping daemon process ${DAEMON_NAME}: "
	killall ${DAEMON_NAME}
	rm -f /var/lock/${DAEMON_NAME}
	echo "successful"
}

restart()
{
	stop
	start
}

case "$1" in
	start)
	start
	;;
	stop)
	stop
	;;
	restart|reload)
	;;
	*)
	echo "Usage: $0 {start|stop|restart}"
	exit 1
esac

exit $?