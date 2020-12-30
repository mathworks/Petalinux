FILESEXTRAPATHS_prepend := "${THISDIR}/files/common/fs-overlay/etc/network:"
SRC_URI += " file://interfaces"

do_install_append() {
    install -d ${D}${sysconfdir}/network/
    install -m 0644 ${WORKDIR}/interfaces  ${D}${sysconfdir}/network/interfaces

}
PACKAGE_ARCH = "${MACHINE_ARCH}"
