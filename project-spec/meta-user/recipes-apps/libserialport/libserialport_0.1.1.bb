DISCRIPTION = "C++ library for serial port support."
SECTION = "libs"
LICENSE = "LGPLv3+"
HOMEPAGE = "http://sigrok.org/download/source/libserialport"

#SRC_URI = "${SOURCEFORGE_MIRROR}/libserial/libserial-${PV}.tar.gz"
SRC_URI = "https://sigrok.org/download/source/libserialport/libserialport-${PV}.tar.gz"
LIC_FILES_CHKSUM = "file://COPYING;md5=e6a600fd5e1d9cbde2d983680233ad02"

S = "${WORKDIR}/libserialport-${PV}"

SRC_URI_append = " file://0001-uclinux-detection.patch"
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

inherit autotools pkgconfig

SRC_URI[md5sum] = "b93f0325a6157198152b5bd7e8182b51"
SRC_URI[sha256sum] = "4a2af9d9c3ff488e92fb75b4ba38b35bcf9b8a66df04773eba2a7bbf1fa7529d"


