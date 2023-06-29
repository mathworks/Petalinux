FILESEXTRAPATHS_prepend := "${THISDIR}/files/zynqmp/fs-overlay/etc/:"
SRC_URI += " file://fstab"

do_install_append() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/fstab  ${D}${sysconfdir}/fstab
}
PACKAGE_ARCH = "${MACHINE_ARCH}"
