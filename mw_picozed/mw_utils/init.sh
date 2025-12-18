#!/bin/sh
source /etc/bootvars.conf
cp /mnt/fw_env.config /etc/fw_env.config
cp /mnt/sshd_config /etc/ssh/sshd_config
/etc/init.d/sshd restart
if [ -f $_SD_ROOT/libboost-1-63-overlay.tar.gz ];
then
		tar xf $_SD_ROOT/libboost-1-63-overlay.tar.gz -C /usr/lib
fi
# Add any custom code to be run at startup here
