SUMMARY = "Library for interfacing with IIO devices"
HOMEPAGE = "https://wiki.analog.com/resources/tools-software/linux-software/libiio"
BRANCH ?= "libiio-v0"
SECTION = "libs"
LICENSE = "LGPLv2.1+"
LIC_FILES_CHKSUM = "file://COPYING.txt;md5=7c13b3376cea0ce68d2d2da0a1b3a72c"
SRCREV = "${@ "c4498c27761d04d4ac631ec59c1613bfed079da5" if bb.utils.to_boolean(d.getVar('BB_NO_NETWORK')) else d.getVar('AUTOREV')}"
SRC_URI = "git://github.com/analogdevicesinc/libiio.git;protocol=https;branch=${BRANCH}"
PV = "0.25+git${SRCPV}"

S = "${WORKDIR}/git"

inherit cmake python3native systemd gitpkgv

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
    -DWITH_UTILS=ON \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DUDEV_RULES_INSTALL_DIR=${nonarch_base_libdir}/udev/rules.d \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '-DWITH_SYSTEMD=ON -DSYSTEMD_UNIT_INSTALL_DIR=${systemd_system_unitdir}', '', d)} \
"

EXTRA_OECMAKE += "-DCMAKE_SKIP_RPATH=TRUE"
EXTRA_OECMAKE += "${@bb.utils.contains('DISTRO_FEATURES', 'zeroconf', '', '-DHAVE_DNS_SD=off', d)}"

PACKAGECONFIG ??= "USB_BACKEND NETWORK_BACKEND PYTHON_BINDINGS TESTS"

PACKAGECONFIG[USB_BACKEND] = "-DWITH_USB_BACKEND=ON,-DWITH_USB_BACKEND=OFF,libusb1,libxml2"
PACKAGECONFIG[NETWORK_BACKEND] = "-DWITH_NETWORK_BACKEND=ON,-DWITH_NETWORK_BACKEND=OFF,libxml2"
PACKAGECONFIG[PYTHON_BINDINGS] = ",,python3"
PACKAGECONFIG[TESTS] = "-DWITH_TESTS=ON,-DWITH_TESTS=OFF,libxml2"

PACKAGES =+ "${PN}-iiod ${PN}-tests ${PN}-python"

RDEPENDS:${PN}-python = "${PN} python3-ctypes python3-stringold"

#FILES:${PN}-tests = "${bindir}"
FILES:${PN}-python = "${PYTHON_SITEPACKAGES_DIR}"

SYSTEMD_PACKAGES = "${PN}-iiod"
SYSTEMD_SERVICE:${PN}-iiod = "iiod.service"
