FILESEXTRAPATHS_prepend := "${THISDIR}/files/:"
#S = "${WORKDIR}/mw-fs-overlay"
#S = "${WORKDIR}"
#SRC_URI += " file://fw_env.config"
SRC_URI += " file://init.d/iiod"

#inherit update-rc.d
DEPENDS_append = " update-rc.d-native"
#PACKAGE_WRITE_DEPS_append = " ${@bb.utils.contains('DISTRO_FEATURES','systemd','systemd-systemctl-native','',d)}"
#RDEPENDS_${PN} = "initd-functions \
#		 "

#RPROVIDES_${PN}-functions = "initd-functions"
#RCONFLICTS_${PN}-functions = "lsbinitscripts"
#FILES_${PN}-functions = "${sysconfdir}/init.d/functions*"
#HALTARGS ?= "-d -f"

S = "${WORKDIR}"


do_install_append() {
	install -d ${D}${sysconfdir}/init.d
	install -d ${D}${sysconfdir}/rcS.d
	install -d ${D}${sysconfdir}/rc1.d
	install -d ${D}${sysconfdir}/rc2.d
	install -d ${D}${sysconfdir}/rc3.d
	install -d ${D}${sysconfdir}/rc4.d
	install -d ${D}${sysconfdir}/rc5.d
#	install -d ${D}${sysconfdir}/rc6.d

	install -m 0755 ${WORKDIR}/init.d/iiod  ${D}${sysconfdir}/init.d/
# 	install -m 0755 ${WORKDIR}/rcS.d/S99iiod   ${D}${sysconfdir}/rcS.d/

#	update-rc.d -r ${D} S99iiod start 99 2 3 4 5 .
#	ln -sf ../init.d/S99iiod  ${D}${sysconfdir}/rcS.d/S99iiod
        update-rc.d -r ${D} iiod start 99 1 2 3 4 5 .

}
#PACKAGE_ARCH = "${MACHINE_ARCH}"
#FILES_${PN} += "${sysconfdir}/rcS.d/"
#FILES_${PN} += "${sysconfdir}/rc1.d/"
#FILES_${PN} += "${sysconfdir}/rc2.d/"
#FILES_${PN} += "${sysconfdir}/rc3.d/"
#FILES_${PN} += "${sysconfdir}/rc4.d/"
#FILES_${PN} += "${sysconfdir}/rc5.d/"
#FILES_${PN} += "${sysconfdir}/rc6.d/"

PACKAGE_ARCH = "${MACHINE_ARCH}"
