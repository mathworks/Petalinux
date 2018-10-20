DESCRIPTION = "Add files that interact with linux u-boot utils"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

# Ensure the our fw_env.config file is written last
RDEPENDS_${PN} += "u-boot-fw-utils"

EXTRAPATHS_prepend += "${THISDIR}/files/common:"
EXTRAPATHS_prepend += "${THISDIR}/files/zynqmp:"

SRC_URI += "file://common/fs-overlay/etc/ \
            file://common/fs-overlay/usr/sbin/ \
	    file://zynqmp/fs-overlay/etc/ \ 
"

do_install() {
install -d ${D}${sysconfdir}
	# Place bootvars.conf in /etc (used by MW API scripts)
	install -m 0755 ${WORKDIR}/common/fs-overlay/etc/bootvars.conf ${D}${sysconfdir}/bootvars.conf
#	install -m 0755 ${WORKDIR}/common/fs-overlay/etc/inetd.conf ${D}${sysconfdir}/inetd.conf 
	install -m 0755 ${WORKDIR}/common/fs-overlay/etc/mdev.conf ${D}${sysconfdir}/mdev.conf
	install -m 0755 ${WORKDIR}/common/fs-overlay/etc/udhcpd.conf ${D}${sysconfdir}/udhcpd.conf

#install -d ${D}/etc/udev/rules.d

install -d ${D}/${sysconfdir}/bootvars.d/
	cp -r ${WORKDIR}/common/fs-overlay/etc/bootvars.d/* ${D}${sysconfdir}/bootvars.d/
	cp -r ${WORKDIR}/zynqmp/fs-overlay/etc/bootvars.d/* ${D}${sysconfdir}/bootvars.d/

#install -d ${D}/etc/init.d

#install -d ${D}/etc/network

#install -d ${D}/etc/ssh

# Replace default /etc/fw_env.config with one customized for MiniZed
# install -m 0755 ${WORKDIR}/fw_env.config ${D}${sysconfdir}/fw_env.config
       
# Place MathWorks API scripts in /usr/sbin
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
