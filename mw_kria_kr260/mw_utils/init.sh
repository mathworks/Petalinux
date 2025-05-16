#!/bin/sh

# This script is run after complete linux boot

### This command makes the board to use last booted image .BIN for initial bootup on every power cycle
### Once the user boots the board with the MathWorks BOOT.BIN, then running this command makes the board to bootup with this BOOT.BIN for every power cycle until user loads the other BOOT.BIN manually
sudo xmutil bootfw_update -v

### Below commands are required only for KR260 as the environment is stored in the QSPI memory.
### These commands will fetch the raw environment data present in the QSPPI memory in the file formats(uboot.env and uboot-redund.env) and place it under /mnt folder
sudo dd if=/dev/mtd12 of=/mnt/uboot.env bs=1k count=128
sudo dd if=/dev/mtd13 of=/mnt/uboot-redund.env bs=1k count=128
