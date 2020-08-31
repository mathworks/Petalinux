DESCRIPTION = "Add files that interact with linux u-boot utils"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

RDEPENDS_${PN} += "u-boot-fw-utils"

EXTRAPATHS_prepend += "${THISDIR}/files/common:"
EXTRAPATHS_prepend += "${THISDIR}/files/zynqmp:"

SRC_URI += "file://common/fs-overlay/usr/sbin/ \
            file://common/fs-overlay/etc/ \
            file://zynqmp/fs-overlay/etc/ \ 
"


DEPENDS_append = " update-rc.d-native"

do_install() {

install -d ${D}/${sysconfdir}/
install -m 0755 ${WORKDIR}/common/fs-overlay/etc/bootvars.conf ${D}${sysconfdir}/bootvars.conf
#        cp -r ${WORKDIR}/common/fs-overlay/etc/* ${D}${sysconfdir}

install -d ${D}/${sbindir}/
	cp -r ${WORKDIR}/common/fs-overlay/usr/sbin/* ${D}${sbindir}

}

FILES_${PN} += "${sysconfdir}/*"
FILES_${PN} += "${sbindir}/"
FILES_${PN} += "${bindir}/"
FILES_${PN} += "${sysconfdir}/bootvars.d/"
FILES_${PN} += "${sysconfdir}/init.d/"
FILES_${PN} += "${sysconfdir}/rcS.d/"
FILES_${PN} += "${sysconfdir}/rc1.d/"
FILES_${PN} += "${sysconfdir}/rc2.d/"
FILES_${PN} += "${sysconfdir}/rc3.d/"
FILES_${PN} += "${sysconfdir}/rc4.d/"
FILES_${PN} += "${sysconfdir}/rc5.d/"


