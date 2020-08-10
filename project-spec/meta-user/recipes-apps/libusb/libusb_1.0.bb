DISCRIPTION = "Userspace library for accessing USB devices"
SECTION = "libs"
LICENSE = "LGPL-2.1+"
HOMEPAGE = "http://libusb.info/"

#LIBUSB_VERSION_MAJOR = ${PV}
#LIBUSB_VERSION = $(LIBUSB_VERSION_MAJOR).21
#LIBUSB_SOURCE = libusb-$(LIBUSB_VERSION).tar.bz2


SRC_URI = "https://github.com/libusb/libusb/releases/download/v${PV}.21/libusb-${PV}.21.tar.bz2"
LIC_FILES_CHKSUM = "file://COPYING;md5=fbc093901857fcd118f065f900982c24"

S = "${WORKDIR}/libusb-${PV}.21"

inherit autotools pkgconfig

# Don't configure udev by default since it will cause a circular
# dependecy with udev package, which depends on libusb
EXTRA_OECONF = "--libdir=${base_libdir} --disable-udev"

do_install_append() {
	install -d ${D}${libdir}
	if [ ! ${D}${libdir} -ef ${D}${base_libdir} ]; then
		mv ${D}${base_libdir}/pkgconfig ${D}${libdir}
	fi
}

FILES_${PN} += "${base_libdir}/*.so.*"

FILES_${PN}-dev += "${base_libdir}/*.so ${base_libdir}/*.la"

SRC_URI[sha256sum] = "7dce9cce9a81194b7065ee912bcd55eeffebab694ea403ffb91b67db66b1824b"


