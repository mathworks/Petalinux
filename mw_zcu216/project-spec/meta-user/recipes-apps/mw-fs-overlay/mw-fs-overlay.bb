#
# MW filesystem additions
#
DESCRIPTION = "Add MW startup files and other utilities"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

RDEPENDS:${PN} += "libubootenv"

inherit systemd features_check

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
FILESEXTRAPATHS:prepend := "${THISDIR}/common:"
FILESEXTRAPATHS:prepend := "${THISDIR}/services:"

SRC_URI = " file://common file://zynqmp \
	file://services/hostname.service \
	file://services/inetd.service \
	file://services/network.service \
	file://services/network_scripts.service \
	file://services/nfs-common.service \
	file://services/sdcard_mount.service \
	file://services/sdinit.service \
	file://services/start_only.service \
	file://services/udhcpd.service \
	file://services/usb_network.service \
	file://services/user_app.service \
	file://services/user_init.service \
	file://services/backupSSHKeys.service \
	file://services/restoreSSHKeys.service \
	"

PACKAGE_ARCH = "${MACHINE_ARCH}"
DEPENDS:append = " update-rc.d-native"

SYSTEMD_SERVICE:${PN} = "sdcard_mount.service usb_network.service \
			sdinit.service network.service \
			network_scripts.service udhcpd.service \
			inetd.service user_app.service \
			nfs-common.service hostname.service \
			backupSSHKeys.service restoreSSHKeys.service"

do_install() {
	chmod -R 0755 ${WORKDIR}/common/fs-overlay/usr/sbin/
	chmod -R 0755 ${WORKDIR}/zynqmp/fs-overlay/etc/profile.d/
	install -d ${D}${sysconfdir}/
	install -m 0644 ${WORKDIR}/common/fs-overlay/etc/udhcpd.conf ${D}${sysconfdir}/udhcpd.conf

	install -d ${D}${sysconfdir}/bootvars.d/
	cp -r ${WORKDIR}/common/fs-overlay/etc/bootvars.conf ${D}${sysconfdir}/
	cp -r ${WORKDIR}/common/fs-overlay/etc/bootvars.d/* ${D}${sysconfdir}/bootvars.d/

	install -d ${D}${sbindir}/
	install -d ${D}${sysconfdir}/init.d

	if [ "${@bb.utils.contains('INIT_MANAGER','systemd','yes','no',d)}" = "yes" ]; then
		echo "Installing MW systemd services..."
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/backupSSHKeys  ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/restoreSSHKeys  ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/hostname  ${D}${sbindir}/update_hostname
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/network  ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/network_scripts  ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/sdcard_mount  ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/sdinit  ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/usb_network ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/user_app ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/user_init ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/user_init ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/start_only.sh ${D}${sbindir}/
		install -m 0755 ${WORKDIR}/common/fs-overlay/etc/init.d/start_only.sh ${D}${sysconfdir}/init.d/
		install -d ${D}${systemd_system_unitdir}/
		cp -r ${WORKDIR}/services/* ${D}${systemd_system_unitdir}/
	else
		echo "Installing MW sysvinit services..."
		cp -r ${WORKDIR}/common/fs-overlay/etc/init.d/* ${D}${sysconfdir}/init.d
		chmod -R 0755 ${D}${sysconfdir}/init.d
		install -d ${D}${sysconfdir}/rcS.d
		install -d ${D}${sysconfdir}/rc1.d
		install -d ${D}${sysconfdir}/rc2.d
		install -d ${D}${sysconfdir}/rc3.d
		install -d ${D}${sysconfdir}/rc4.d
		install -d ${D}${sysconfdir}/rc5.d
		update-rc.d -r ${D} sdcard_mount start 8 1 2 3 4 5 .
		update-rc.d -r ${D} restoreSSHKeys start 9 1 2 3 4 5 .
		update-rc.d -r ${D} network_scripts start 38 1 2 3 4 5 .
		update-rc.d -r ${D} hostname start 39 1 2 3 4 5 .
		update-rc.d -r ${D} usb_network start 39 1 2 3 4 5 .
		update-rc.d -r ${D} network start 40 1 2 3 4 5 .
		update-rc.d -r ${D} inetd start 41 1 2 3 4 5 .
		update-rc.d -r ${D} backupSSHKeys start 51 1 2 3 4 5 .
		update-rc.d -r ${D} user_init start 97 1 2 3 4 5 .
		update-rc.d -r ${D} user_app start 98 1 2 3 4 5 .
		update-rc.d -r ${D} sdinit start 99 1 2 3 4 5 .
	fi

	cp -r ${WORKDIR}/common/fs-overlay/usr/sbin/* ${D}/${sbindir}/
	chmod -R 0755 ${D}${sbindir}/

	install -d ${D}/${sysconfdir}/udev/rules.d/
	cp -r ${WORKDIR}/common/fs-overlay/etc/udev/rules.d/*  ${D}/${sysconfdir}/udev/rules.d/
}

FILES:${PN} += "${sysconfdir}/*"
FILES:${PN} += "${sbindir}/"
FILES:${PN} += "${systemd_system_unitdir}/"
SYSTEMD_AUTO_ENABLE = "enable"
INSANE_SKIP:${PN} += "installed-vs-shipped"
REQUIRED_DISTRO_FEATURES = "systemd"