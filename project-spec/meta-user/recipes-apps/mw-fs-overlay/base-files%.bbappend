FILESEXTRAPATHS_prepend := "${THISDIR}/files/zynqmp/fs-overlay/etc/:"
#S = "${WORKDIR}/mw-fs-overlay"
#S = "${WORKDIR}"
#SRC_URI += " file://fw_env.config"
SRC_URI += " file://fstab"

do_install_append() {
    install -d ${D}${sysconfdir}
  #  install -m 0644 ${WORKDIR}/fw_env.config  ${D}${sysconfdir}/fw_env.config
    install -m 0644 ${WORKDIR}/fstab  ${D}${sysconfdir}/fstab

}
PACKAGE_ARCH = "${MACHINE_ARCH}"
