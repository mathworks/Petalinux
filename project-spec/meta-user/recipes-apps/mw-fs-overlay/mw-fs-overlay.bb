DESCRIPTION = "Add files that interact with linux u-boot utils"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

RDEPENDS_${PN} += "u-boot-fw-utils"

EXTRAPATHS_prepend += "${THISDIR}/files/common:"
EXTRAPATHS_prepend += "${THISDIR}/files/zynqmp:"

SRC_URI += "file://common/fs-overlay/etc/ \
            file://common/fs-overlay/etc/init.d/ \
	    file://common/fs-overlay/etc/udev/rules.d/ \
	    file://common/fs-overlay/etc/ssh/ \
	    file://common/fs-overlay/usr/sbin/ \
	    file://zynqmp/fs-overlay/etc/ \ 
	    file://zynqmp/fs-overlay/etc/profile.d/ \
"

DEPENDS_append = " update-rc.d-native"

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
install -d ${D}${sysconfdir}/rcS.d
install -d ${D}${sysconfdir}/rc1.d
install -d ${D}${sysconfdir}/rc2.d
install -d ${D}${sysconfdir}/rc3.d
install -d ${D}${sysconfdir}/rc4.d
install -d ${D}${sysconfdir}/rc5.d

	cp -r ${WORKDIR}/common/fs-overlay/etc/init.d/* ${D}${sysconfdir}/init.d/
	
	#establish runlinks
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

#create symlinks for ssh
install -d ${D}${bindir}
       # ln -sf  ../bin/mw_setboot ${D}${sbindir}/mw_setboot

}

FILES_${PN} = " \
    ${sysconfdir}/bootvars.conf \
    ${sysconfdir}/mdev.conf \
    ${sysconfdir}/udhcpd.conf \
"

FILES_${PN} += "${sbindir}/"
FILES_${PN} += "${bindir}/"
FILES_${PN} += "${sysconfdir}/bootvars.d/"
FILES_${PN} += "${sysconfdir}/init.d/"
FILES_${PN} += "${sysconfdir}/udev/rules.d/"
FILES_${PN} += "${sysconfdir}/profile.d/"
FILES_${PN} += "${sysconfdir}/ssh/"
FILES_${PN} += "${sysconfdir}/rcS.d/"
FILES_${PN} += "${sysconfdir}/rc1.d/"
FILES_${PN} += "${sysconfdir}/rc2.d/"
FILES_${PN} += "${sysconfdir}/rc3.d/"
FILES_${PN} += "${sysconfdir}/rc4.d/"
FILES_${PN} += "${sysconfdir}/rc5.d/"

