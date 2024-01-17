# Mount the FAT32 boot partition to /mnt
dirs755 += "/mnt"
PACKAGE_ARCH = "${MACHINE_ARCH}"

do_install:append() {
    sed -i '/mmcblk1p1/d' ${D}${sysconfdir}/fstab 
    cat >> ${D}${sysconfdir}/fstab <<EOF
/dev/mmcblk1p1       /mnt          auto      defaults,sync           0  0
EOF
}

