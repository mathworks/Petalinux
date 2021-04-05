FILESEXTRAPATHS_prepend := "${THISDIR}/files/zynqmp/fs-overlay/etc/:"

# Point to U-Boot v2019.01
SRCREV = "d3689267f92c5956e09cc7d1baa4700141662bff"

SRC_URI += "git://git.denx.de/u-boot.git"
SRC_URI += " file://fw_env.config"

# remove patches introduced in older version
SRC_URI_remove = "file://CVE-2018-1000205-1.patch"
SRC_URI_remove = "file://CVE-2018-1000205-2.patch"

do_install_append() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/fw_env.config  ${D}${sysconfdir}/fw_env.config
}
PACKAGE_ARCH = "${MACHINE_ARCH}"
