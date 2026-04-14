#!/bin/sh
source /etc/bootvars.conf
cp /mnt/sshd_config /etc/ssh/sshd_config
cp /mnt/BUILDINFO /etc/buildinfo
cp /mnt/fw_env.config /etc/fw_env.config