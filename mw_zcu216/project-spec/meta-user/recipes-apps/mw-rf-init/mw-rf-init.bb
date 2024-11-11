#
# mw-rf-init recipe.
#
DESCRIPTION = "RF init for interacting with RFTOOL at boot up"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://rf_init.c \
	   file://Makefile \
         "
S = "${WORKDIR}"
TARGET_CC_ARCH += "${LDFLAGS}"

do_install() {
	     install -d ${D}/${bindir}
	     install -m 0755 ${S}/mw-rf-init ${D}/${bindir}
}

