FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
DESCRIPTION = "MW kernel"
LINUX_VERSION = "5.15"
MW_VERSION = "master"
PV = "${LINUX_VERSION}-${MW_VERSION}+git${SRCPV}"
KBRANCH = "mw-5.15.0"

SRCREV = "${@ "79f0591f4c8d7b8d202cf8d53758ae26f0ee662d" if bb.utils.to_boolean(d.getVar('BB_NO_NETWORK')) else d.getVar('AUTOREV')}"
KERNELURI = "git://github.com/mathworks/xilinx-linux.git;protocol=https"

# override kernel config file
KBUILD_DEFCONFIG:versal = "xilinx_versal_defconfig"
KBUILD_DEFCONFIG:zynmp = "mw_zynqmp_defconfig"
KBUILD_DEFCONFIG:zynq = "mw_zynq_defconfig"

SRC_URI:append = " file://bsp.cfg"
KERNEL_FEATURES:append = " bsp.cfg"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append += " file://bsp.cfg"
KERNEL_FEATURES:append += " bsp.cfg"
SRC_URI += "file://user_2022-05-23-16-27-00.cfg \
            file://user_2022-06-15-16-56-00.cfg \
            file://user_2022-07-05-15-54-00.cfg \
            file://user_2022-07-05-16-34-00.cfg \
            file://user_2022-08-25-16-58-00.cfg \
            "
