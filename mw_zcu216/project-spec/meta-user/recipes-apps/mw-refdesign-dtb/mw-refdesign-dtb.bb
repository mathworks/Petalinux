# mw-refdesign-dtb
DESCRIPTION = "Generate device-tree blobs for use with MathWorks example reference designs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

PACKAGE_ARCH = "${MACHINE_ARCH}"
# Depend on the virtual/kernel for kernel source and cmake-native for CMake
DEPENDS = "virtual/kernel cmake-native dtc-native"

# Specify the source files and other details as needed
SRC_URI = " file://base.dtsi \
	file://axilite.dts \
	file://axistream.dts \
	file://axistream_64.dts \
	file://dlhdl.dts \
	file://sharedmem_iio.dts \
	file://CMakeLists.txt \
	file://common \
	file://zynqmp \
	file://zynq \
"

S = "${WORKDIR}"

inherit deploy
# Inherit cmake to use the cmake class functions
inherit cmake

# Pass the architecture and cross-compile prefix to CMake
EXTRA_OECMAKE = " -DKERNEL_DIR=${STAGING_KERNEL_DIR} -DARCH=${KERNEL_ARCH} -DCROSS_COMPILE=${TARGET_PREFIX}"

do_install() {
	install -d ${D}/boot/mwdtbs
	install -Dm 0644 ${B}/*.dtb ${D}/boot/mwdtbs/
}

do_deploy() {
	install -d ${D}/boot/mwdtbs
	install -Dm 0644 ${B}/*.dtb ${D}/boot/mwdtbs/
}

FILES:${PN} += "/boot/mwdtbs/*.dtb"
INSANE_SKIP:${PN} += "installed-vs-skipped"

