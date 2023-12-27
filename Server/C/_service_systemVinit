#!/bin/sh

#### BEGIN INIT INFO  
## Short-Description: Spectrom Requests-Based Network Protocol
#### END INIT INFO

_HOME=/var/tmp/SRBNP/
_BIN=SRBNP

start() {
        $_HOME$_BIN.run &
        ### Create the lock file ###
        /bin/touch /var/lock/$_BIN
}
stop() {
        pkill $_BIN &
        /bin/rm /var/lock/$_BIN
        echo "$BIN Terminated"
}
status() {
	if [ -f "/var/lock/$_BIN" ]; then
		if /bin/pgrep -x $_BIN.run >/dev/null
		then
		    echo -n $_BIN "Running\n"
		    if [ -f $_HOME$_BIN.run.log ]; then
			/bin/cat $_HOME$_BIN.run.log
		    fi;
		else
		    echo -n $_BIN "Crashed\n"
		fi;
	else
		echo -n $_BIN "Stopped\n"
	fi;
}
case "$1" in
  start)
        start
        ;;
  stop)
        stop
        ;;
  status)
        status
        ;;
  restart|reload|condrestart)
        stop
        sleep 1;
        start
        ;;
  *)
        echo $"Usage: $0 {start|stop|restart|reload|status}"
        exit 1
esac


exit 0
