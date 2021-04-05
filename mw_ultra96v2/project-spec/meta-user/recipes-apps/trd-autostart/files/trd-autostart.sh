#!/bin/sh

# Execute userhook if present
USERHOOK_SD=/run/media/mmcblk0p1/autostart.sh
if [ -f $USERHOOK_SD ]; then
	sh $USERHOOK_SD &
fi
