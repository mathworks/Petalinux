FILESEXTRAPATHS_prepend := "${THISDIR}/files/zynqmp/fs-overlay/etc/:"
#S = "${WORKDIR}/mw-fs-overlay"
#S = "${WORKDIR}"
#
SRCREV = "0b0c6af38738f2c132cfd41a240889acaa031c8f"

SRC_URI  += "git://git.denx.de/u-boot.git"
SRC_URI += " file://fw_env.config"

# remove patches introduced in older version
SRC_URI_remove = "file://0001-CVE-2019-13103.patch"
SRC_URI_remove = "file://0002-CVE-2019-13104.patch"
SRC_URI_remove = "file://0003-CVE-2019-13105.patch"
SRC_URI_remove = "file://0004-CVE-2019-13106.patch"
SRC_URI_remove = "file://0005-CVE-2019-14192-14193-14199.patch"
SRC_URI_remove = "file://0006-CVE-2019-14197-14200-14201-14202-14203-14204.patch"
SRC_URI_remove = "file://0007-CVE-2019-14194-14198.patch"
SRC_URI_remove = "file://0008-CVE-2019-14195.patch"
SRC_URI_remove = "file://0009-CVE-2019-14196.patch"


do_install_append() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/fw_env.config  ${D}${sysconfdir}/fw_env.config
}
PACKAGE_ARCH = "${MACHINE_ARCH}"
