FILESEXTRAPATHS_prepend := "${THISDIR}/files/common/fs-overlay/etc/:"
#S = "${WORKDIR}/mw-fs-overlay"
#S = "${WORKDIR}"
SRC_URI += " file://inetd.conf"

do_install_append() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/inetd.conf  ${D}${sysconfdir}/inetd.conf
}
#PACKAGE_ARCH = "${MACHINE_ARCH}"
#PACKAGE_ARCH = "aarch64"

