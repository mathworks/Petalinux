UMMARY = "Library for interfacing with IIO devices"
HOMEPAGE = "https://wiki.analog.com/resources/tools-software/linux-software/libiio"
SECTION = "libs"
LICENSE = "LGPLv2.1+"
LIC_FILES_CHKSUM = "file://COPYING.txt;md5=7c13b3376cea0ce68d2d2da0a1b3a72c"


#SRCREV = "6ecff5d46e1b12c2859f0b63a73282940e3402bb" version 0.15
SRCREV = "4e22517c60f3c5e691320871956edede15459ae3"
#PV = "0.15+git${SRCPV}"


PV = "0.${SRCPV}"
PKGV = "0.${GITPKGV}"

SRC_URI = "git://github.com/analogdevicesinc/libiio.git;protocol=https"

S = "${WORKDIR}/git"

inherit cmake pythonnative systemd gitpkgv

DEPENDS = " \
    flex-native bison-native libaio \
    ${@bb.utils.contains('DISTRO_FEATURES', 'zeroconf', 'avahi', '', d)} \
"

EXTRA_OECMAKE = " \
    -DWITH_LOCAL_BACKEND=ON \
    -DWITH_XML_BACKEND=ON \
    -DWITH_NETWORK_BACKEND=ON \
    -DNEED_THREADS=1 \
    -DWITH_TESTS=ON \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DUDEV_RULES_INSTALL_DIR=${nonarch_base_libdir}/udev/rules.d \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '-DWITH_SYSTEMD=ON -DSYSTEMD_UNIT_INSTALL_DIR=${systemd_system_unitdir}', '', d)} \
"

EXTRA_OECMAKE += "-DCMAKE_SKIP_RPATH=TRUE"


PACKAGECONFIG ??= "USB_BACKEND NETWORK_BACKEND PYTHON_BINDINGS TESTS"

PACKAGECONFIG[USB_BACKEND] = "-DWITH_USB_BACKEND=ON,-DWITH_USB_BACKEND=OFF,libusb1,libxml2"
PACKAGECONFIG[NETWORK_BACKEND] = "-DWITH_NETWORK_BACKEND=ON,-DWITH_NETWORK_BACKEND=OFF,libxml2"
PACKAGECONFIG[PYTHON_BINDINGS] = ",,python"
PACKAGECONFIG[TESTS] = "-DWITH_TESTS=ON,-DWITH_TESTS=OFF,libxml2"

PACKAGES =+ "${PN}-iiod ${PN}-tests ${PN}-python"

RDEPENDS_${PN}-python = "${PN} python-ctypes python-stringold"

#FILES_${PN}-tests = "${bindir}"
FILES_${PN}-python = "${PYTHON_SITEPACKAGES_DIR}"

SYSTEMD_PACKAGES = "${PN}-iiod"
SYSTEMD_SERVICE_${PN}-iiod = "iiod.service"
