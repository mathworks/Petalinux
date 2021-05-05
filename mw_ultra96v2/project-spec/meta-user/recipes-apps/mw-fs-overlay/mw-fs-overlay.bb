#
# MathWorks filesystem additions
#
DESCRIPTION = "Add files that interact with linux u-boot utils"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

RDEPENDS_${PN} += "u-boot-fw-utils"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += "file://common/ \
            file://zynqmp/ \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

DEPENDS_append = " update-rc.d-native"

do_install() {
	install -d ${D}/${sysconfdir}/
	install -m 0644 ${WORKDIR}/common/fs-overlay/etc/bootvars.conf ${D}${sysconfdir}/bootvars.conf
	install -m 0644 ${WORKDIR}/common/fs-overlay/etc/udhcpd.conf ${D}${sysconfdir}/udhcpd.conf

	install -d ${D}/${sysconfdir}/profile.d/
	cp -r ${WORKDIR}/zynqmp/fs-overlay/etc/profile.d/* ${D}${sysconfdir}/profile.d/

	install -d ${D}/${sysconfdir}/bootvars.d/
	cp -r ${WORKDIR}/common/fs-overlay/etc/bootvars.d/* ${D}${sysconfdir}/bootvars.d/
	cp -r ${WORKDIR}/zynqmp/fs-overlay/etc/bootvars.d/* ${D}${sysconfdir}/bootvars.d/

	install -d ${D}${sysconfdir}/init.d
	install -d ${D}${sysconfdir}/rcS.d
	install -d ${D}${sysconfdir}/rc1.d
	install -d ${D}${sysconfdir}/rc2.d
	install -d ${D}${sysconfdir}/rc3.d
	install -d ${D}${sysconfdir}/rc4.d
	install -d ${D}${sysconfdir}/rc5.d

	cp -r ${WORKDIR}/common/fs-overlay/etc/init.d/* ${D}${sysconfdir}/init.d
	chmod -R 775 ${D}${sysconfdir}/init.d

	update-rc.d -r ${D} symlink_init start 8 1 2 3 4 5 .
	update-rc.d -r ${D} sdcard_mount start 9 1 2 3 4 5 .
	update-rc.d -r ${D} module_coldplug start 11 1 2 3 4 5 .
	update-rc.d -r ${D} network_scripts start 38 1 2 3 4 5 .
	update-rc.d -r ${D} hostname start 39 1 2 3 4 5 .
	update-rc.d -r ${D} usb_network start 39 1 2 3 4 5 .
	update-rc.d -r ${D} network start 40 1 2 3 4 5 .
	update-rc.d -r ${D} inetd start 41 1 2 3 4 5 .
	update-rc.d -r ${D} udhcpd start 42 1 2 3 4 5 .
	update-rc.d -r ${D} restoreSSHkeys start 49 1 2 3 4 5 .
	update-rc.d -r ${D} alsa start 50 1 2 3 4 5 .
	update-rc.d -r ${D} backupSSHkeys start 51 1 2 3 4 5 .
	update-rc.d -r ${D} hdlrd_init start 95 1 2 3 4 5 .
	update-rc.d -r ${D} user_init start 97 1 2 3 4 5 .
	update-rc.d -r ${D} user_app start 98 1 2 3 4 5 .
	update-rc.d -r ${D} sdinit start 99 1 2 3 4 5 .

	install -d ${D}/${sysconfdir}/ssh/
	cp -r ${WORKDIR}/common/fs-overlay/etc/ssh/* ${D}${sysconfdir}/ssh/

	install -d ${D}/${sbindir}/
	cp -r ${WORKDIR}/common/fs-overlay/usr/sbin/* ${D}${sbindir}

	install -d ${D}/${sysconfdir}/udev/rules.d/
	cp -r ${WORKDIR}/common/fs-overlay/etc/udev/rules.d/*  ${D}${sysconfdir}/udev/rules.d/

}

FILES_${PN} += "${sysconfdir}/*"
FILES_${PN} += "${sbindir}/"