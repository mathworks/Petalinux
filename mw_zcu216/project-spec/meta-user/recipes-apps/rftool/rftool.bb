#
# This file is the rftool recipe.
#

SUMMARY = "Simple rftool application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://board.h \
	   file://error_interface.h \
	   file://cmd_interface.h \
	   file://common.h \
	   file://gpio.h \
	   file://rfdc_interface.h \
	   file://tcp_interface.h \
	   file://rfdc_functions_w.h \
	   file://clock_interface.h \
	   file://power_interface.h \
	   file://data_interface.h \
	   file://local_mem.h \
	   file://tcp_interface.c \
	   file://common.c \
	   file://gpio.c \
	   file://gpio_interface.c \
	   file://gpio_interface.h \
	   file://version.h \
	   file://additional_cmds.h \
	   file://rfdc_interface.c \
	   file://cmd_interface.c \
	   file://error_interface.c \
	   file://local_mem.c \
	   file://power_interface.c \
	   file://data_interface.c \
	   file://rfdc_functions_w.c \
	   file://clock_interface.c \
	   file://axi_switch.c \
	   file://axi_switch.h \
	   file://rfsoc.c \
	   file://design.c \
	   file://design.h \
	   file://Makefile \
         "

S = "${WORKDIR}"

TARGET_CC_ARCH += "${LDFLAGS}"

DEPENDS += "libmetal rfdc rfclk"

do_install() {
	     install -d ${D}/${bindir}
	     install -m 0755 ${S}/rftool ${D}/${bindir}
}
