#!/bin/sh

# This script is run after complete linux boot

### This command makes the board to use last booted image .BIN for initial bootup on every power cycle
### Once the user boots the board with the MathWorks BOOT.BIN, then running this command makes the board to bootup with this BOOT.BIN for every power cycle until user loads the other BOOT.BIN manually

sudo xmutil bootfw_update -v
