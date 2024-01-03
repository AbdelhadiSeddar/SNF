#!/usr/bin/bash
if [ -d "/run/systemd/system" ]
then 
    echo "Systemd Unsupported for now" 
else
    if [ -f "/var/lock/SRBNP" ]
    then
            /etc/init.d/SRBNP stop 
            sleep 1
    fi
    $(which cp) _service_systemVinit.sh /etc/init.d/SRBNP;
    $(which service) SRBNP start ;
fi