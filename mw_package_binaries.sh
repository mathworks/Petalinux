#/bin/bash
petalinux-package --boot --format BIN --bif ./mw_build_utils/boot.bif -o ./images/BOOT.BIN --force

cp images/linux/system.bit non_fit_boot/
cp images/linux/BOOT.BIN non_fit_boot/
cp images/linux/Image non_fit_boot/
cp images/linux/boot.scr non_fit_boot/
cp images/linux/rootfs.cpio.gz.u-boot non_fit_boot/
cp images/linux/system.dtb non_fit_boot/


cp -r non_fit_boot/ /mathworks/devel/sandbox/darenlee/deb9_64_darenlee_shared/ZCU216/petalinux_trd/
