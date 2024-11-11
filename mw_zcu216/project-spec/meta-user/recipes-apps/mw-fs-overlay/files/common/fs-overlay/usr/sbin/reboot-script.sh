#!/bin/sh

has_systemd=$(ps -o comm 1 | grep systemd)

if [ "$has_systemd" == "systemd" ]; then
        systemctl reboot
else
        /sbin/reboot
fi

