#
# MathWorks filesystem additions
#
DESCRIPTION = "Add files that interact with linux u-boot utils"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

RDEPENDS:${PN} += "libubootenv"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
FILESEXTRAPATHS:prepend := "${THISDIR}/common:"

SRC_URI += " file://common file://zynqmp "

PACKAGE_ARCH = "${MACHINE_ARCH}"
DEPENDS:append = " update-rc.d-native"

inherit systemd
SYSTEMD_SERVICE:${PN} += " hostname.service network.service network_scripts.service sdcard_mount.service sdinit.service usb_network.service user_app.service user_init.service "
SYSTEMD_AUTO_ENABLE:${PN} = "enable"   

do_install() {
	install -d ${D}/${sysconfdir}/
	install -m 0644 ${WORKDIR}/common/fs-overlay/etc/udhcpd.conf ${D}${sysconfdir}/udhcpd.conf

	install -d ${D}/${sysconfdir}/bootvars.d/
	cp -r ${WORKDIR}/common/fs-overlay/etc/bootvars.conf ${D}${sysconfdir}/
	cp -r ${WORKDIR}/common/fs-overlay/etc/bootvars.d/* ${D}${sysconfdir}/bootvars.d/
	cp -r ${WORKDIR}/zynqmp/fs-overlay/etc/bootvars.d/* ${D}${sysconfdir}/bootvars.d/

	install -d ${D}${sysconfdir}/init.d
	install -d ${D}${sysconfdir}/rcS.d
	install -d ${D}${sysconfdir}/rc1.d
	install -d ${D}${sysconfdir}/rc2.d
	install -d ${D}${sysconfdir}/rc3.d
	install -d ${D}${sysconfdir}/rc4.d
	install -d ${D}${sysconfdir}/rc5.d

	if ${@bb.utils.contains('DISTRO_FEATURES','sysvinit','true','false',d)}; then
		cp -r ${WORKDIR}/common/fs-overlay/etc/init.d/* ${D}${sysconfdir}/init.d
		chmod -R 0755 ${D}${sysconfdir}/init.d
		update-rc.d -r ${D} sdcard_mount start 8 1 2 3 4 5 .
		update-rc.d -r ${D} restoreSSHKeys start 9 1 2 3 4 5 .
		update-rc.d -r ${D} network_scripts start 38 1 2 3 4 5 .
		update-rc.d -r ${D} hostname start 39 1 2 3 4 5 .
		update-rc.d -r ${D} usb_network start 39 1 2 3 4 5 .
		update-rc.d -r ${D} network start 40 1 2 3 4 5 .
		update-rc.d -r ${D} backupSSHKeys start 51 1 2 3 4 5 .
		update-rc.d -r ${D} user_init start 97 1 2 3 4 5 .
		update-rc.d -r ${D} user_app start 98 1 2 3 4 5 .
		update-rc.d -r ${D} sdinit start 99 1 2 3 4 5 .
	fi
	install -d ${D}/${sbindir}/
	install -d ${D}${systemd_unitdir}/system
	cp -r ${WORKDIR}/common/fs-overlay/etc/init.d/* ${D}/${sbindir}/
	chmod -R 0755 ${D}/${sbindir}/

	cp -r ${WORKDIR}/common/fs-overlay/lib/systemd/system/* ${D}${systemd_unitdir}/system
	chmod -R 0644 ${D}/${systemd_unitdir}/system/*

	install -d ${D}/${sbindir}/
	cp -r ${WORKDIR}/common/fs-overlay/usr/sbin/* ${D}/${sbindir}/
	chmod -R 0755 ${D}/${sbindir}/*

	install -d ${D}/${sysconfdir}/udev/rules.d/
	cp -r ${WORKDIR}/common/fs-overlay/etc/udev/rules.d/*  ${D}/${sysconfdir}/udev/rules.d/
}

FILES:${PN} += "${sysconfdir}/*"
FILES:${PN} += "${sbindir}/"
FILES:${PN} += "${systemd_unitdir}/*"
INSANE_SKIP:${PN} += "installed-vs-shipped"