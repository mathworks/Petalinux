DESCRIPTION = "Add files that interact with linux u-boot utils"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

RDEPENDS_${PN} += "u-boot-fw-utils"

EXTRAPATHS_prepend += "${THISDIR}/files/common:"
EXTRAPATHS_prepend += "${THISDIR}/files/zynqmp:"

SRC_URI += "file://common/fs-overlay/etc/ \
            file://common/fs-overlay/etc/init.d/ \
	    file://common/fs-overlay/etc/ssh/ \
	    file://common/fs-overlay/usr/sbin/ \
	    file://zynqmp/fs-overlay/etc/ \ 
	    file://zynqmp/fs-overlay/etc/profile.d/ \
"

do_install() {
install -d ${D}${sysconfdir}
	install -m 0755 ${WORKDIR}/common/fs-overlay/etc/bootvars.conf ${D}${sysconfdir}/bootvars.conf
	install -m 0755 ${WORKDIR}/common/fs-overlay/etc/mdev.conf ${D}${sysconfdir}/mdev.conf
	install -m 0755 ${WORKDIR}/common/fs-overlay/etc/udhcpd.conf ${D}${sysconfdir}/udhcpd.conf



install -d ${D}/${sysconfdir}/profile.d/
	cp -r ${WORKDIR}/zynqmp/fs-overlay/etc/profile.d/* ${D}${sysconfdir}/profile.d/

install -d ${D}/${sysconfdir}/bootvars.d/
	cp -r ${WORKDIR}/common/fs-overlay/etc/bootvars.d/* ${D}${sysconfdir}/bootvars.d/
	cp -r ${WORKDIR}/zynqmp/fs-overlay/etc/bootvars.d/* ${D}${sysconfdir}/bootvars.d/

install -d ${D}/${sysconfdir}/init.d/
	cp -r ${WORKDIR}/common/fs-overlay/etc/init.d/* ${D}${sysconfdir}/init.d/

install -d ${D}/${sysconfdir}/ssh/
	cp -r ${WORKDIR}/common/fs-overlay/etc/ssh/* ${D}${sysconfdir}/ssh/


install -d ${D}/${sbindir}/
	cp -r ${WORKDIR}/common/fs-overlay/usr/sbin/* ${D}${sbindir}
	
}

FILES_${PN} = " \
    ${sysconfdir}/bootvars.conf \
    ${sysconfdir}/mdev.conf \
    ${sysconfdir}/udhcpd.conf \
"

FILES_${PN} += "${sbindir}/"
FILES_${PN} += "${sysconfdir}/bootvars.d/"
FILES_${PN} += "${sysconfdir}/init.d/"
FILES_${PN} += "${sysconfdir}/profile.d/"
FILES_${PN} += "${sysconfdir}/ssh/"
FILES_${PN} += "${sysconfdir}/rcS.d/"
FILES_${PN} += "${sysconfdir}/rc1.d/"
FILES_${PN} += "${sysconfdir}/rc2.d/"
FILES_${PN} += "${sysconfdir}/rc3.d/"
FILES_${PN} += "${sysconfdir}/rc4.d/"
FILES_${PN} += "${sysconfdir}/rc5.d/"

