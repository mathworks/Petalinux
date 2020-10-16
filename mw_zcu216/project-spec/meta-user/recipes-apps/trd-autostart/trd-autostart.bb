SUMMARY = "TRD Init scripts"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "\
	file://trd-autostart.sh \
	file://remount \
	file://trd-automount.rules \
	"

S = "${WORKDIR}"

inherit update-rc.d

INITSCRIPT_NAME = "trd-autostart"
INITSCRIPT_PARAMS = "start 99 5 ."

do_install() {
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${S}/trd-autostart.sh ${D}${sysconfdir}/init.d/trd-autostart

	install -d ${D}${sysconfdir}/udev/rules.d/
	install -m 0755 ${S}/trd-automount.rules ${D}${sysconfdir}/udev/rules.d/trd-automount.rules

	install -d ${D}/${bindir}
	install -m 0755 ${S}/remount ${D}/${bindir}/
}

RDEPENDS_${PN}_append += "bash"
