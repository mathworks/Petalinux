SUMMARY = "Analog Devices Industrial IO library"
LICENSE = "LGPLv2.1"
LIC_FILES_CHKSUM = "file://COPYING.txt;md5=7c13b3376cea0ce68d2d2da0a1b3a72c"
DEPENDS = "avahi bison flex libxml2 ncurses"

SRCREV = "17b73d3ad23effb0dd536ee0caa88c62b8cd3f9b"

SRC_URI = "git://github.com/analogdevicesinc/libiio.git"

S = "${WORKDIR}/git"

inherit cmake gitpkgv

PV = "0.${SRCPV}"
PKGV = "0.${GITPKGV}"

