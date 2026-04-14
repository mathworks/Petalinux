#!/bin/sh

source /etc/bootvars.conf

if [ "" != "$(find /sys/firmware/devicetree -name usp_rf_data_converter@* )" ];
then
        echo "+++ Starting rftool..."
	rftool-zcu208 1 &
	/mnt/rf-init
        touch /tmp/rf-init.done
fi

# Copy hosts and BUILDINFO files
if [ -f $_SD_ROOT/hosts ]; then
        cp -f $_SD_ROOT/hosts /etc/hosts
fi

if [ -f $_SD_ROOT/sshd_config ]; then
        cp $_SD_ROOT/sshd_config /etc/ssh/sshd_config
fi

if [ -f $_SD_ROOT/BUILDINFO ]; then
        cp -f $_SD_ROOT/BUILDINFO /etc/buildinfo
fi

# Set the correct size in /etc/fw_env.config
bootenvsize=$(ls -l $_SD_ROOT/uboot.env | awk '{printf "0x%x\n",$5}')
echo $bootenvsize
echo "+++ Updating uboot.env file-size..."
sed -i "s/0x8000/$bootenvsize/g" /etc/fw_env.config

if [ -f $_SD_ROOT/libboost-1-63-overlay.tar.gz ];
then
	tar xf $_SD_ROOT/libboost-1-63-overlay.tar.gz -C /usr/lib
fi

if [ -f $_SD_ROOT/stopsociod.service ]; then
    cp -f $_SD_ROOT/stopsociod.service /lib/systemd/system/stopsociod.service
	systemctl daemon-reload
	systemctl start stopsociod.service
fi

## Backup SSHKeys
systemctl start restoreSSHKeys
systemctl start backupSSHKeys
