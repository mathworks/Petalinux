#!/bin/sh

source /etc/bootvars.conf

# Create network override for eth0
cat > /etc/systemd/network/10-eth0.network << 'EOF'
[Match]
Name=eth0

[Network]
KeepConfiguration=static

EOF

systemctl restart systemd-networkd

## Reboot eth0 Interface
ifdown eth0
sleep 1
ifup eth0
sleep 1

if [ "" != "$(find /sys/firmware/devicetree -name usp_rf_data_converter@*)" ];
then
    echo "+++ Starting rftool..."
	rftool-zcu111 1 &
	mw-rf-init
	mw-rf-init
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
if [ ! -f $_SD_ROOT/uboot-redund.env ];
then
	# Remove /mnt/uboot-redund line if the image was not generated with redundant uboot-env setting
	sed -i 's/.*uboot-redund.*//' /etc/fw_env.config
fi

# Start a shell script to check for eth0 network address after 5s
systemd-run --on-active=5 --unit=checketh0-network ${_SD_ROOT}/ifupeth0.sh

if [ -f $_SD_ROOT/stopsociod.service ]; then
    cp -f $_SD_ROOT/stopsociod.service /lib/systemd/system/stopsociod.service
	systemctl daemon-reload
	systemctl start stopsociod.service
fi

## Backup SSHKeys
systemctl start restoreSSHKeys
systemctl start backupSSHKeys
