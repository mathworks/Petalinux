#
# Adds iiod to MathWorks filesystem
#
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI += " file://init.d/iiod"

do_install_append() {
	install -d ${D}${sysconfdir}/init.d
	install -d ${D}${sysconfdir}/rcS.d
	install -d ${D}${sysconfdir}/rc1.d
	install -d ${D}${sysconfdir}/rc2.d
	install -d ${D}${sysconfdir}/rc3.d
	install -d ${D}${sysconfdir}/rc4.d
	install -d ${D}${sysconfdir}/rc5.d
	install -m 0755 ${WORKDIR}/init.d/iiod  ${D}${sysconfdir}/init.d/iiod
	update-rc.d -r ${D} iiod start 99 1 2 3 4 5 .
}