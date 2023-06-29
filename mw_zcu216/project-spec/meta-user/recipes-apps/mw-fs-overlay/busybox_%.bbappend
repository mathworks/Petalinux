FILESEXTRAPATHS_prepend := "${THISDIR}/files/common/fs-overlay/etc/:"
SRC_URI += " file://inetd.conf"

do_install_append() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/inetd.conf  ${D}${sysconfdir}/inetd.conf
}

