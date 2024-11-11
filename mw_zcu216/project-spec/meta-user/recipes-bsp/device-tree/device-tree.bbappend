FILESEXTRAPATHS:prepend := "${THISDIR}/files:${SYSCONFIG_PATH}:"
FILESEXTRAPATHS:prepend := "${THISDIR}/files/mathworks:${SYSCONFIG_PATH}:"

SRC_URI:append = " file://config file://system-user.dtsi"
SRC_URI += "file://axistream.dtsi"
SRC_URI += "file://base.dtsi"
SRC_URI += "file://zynqmp-mw-fpga-bridge-common.dtsi"
SRC_URI += "file://zynqmp-mw-common.dtsi"
SRC_URI += "file://mw-aximm-common.dtsi"
SRC_URI += "file://zynqmp-mw-axistream-iio-common.dtsi"
SRC_URI += "file://zynqmp-mw-cma.dtsi"
SRC_URI += "file://xilinx-mw-axistream-iio-common.dtsi"
SRC_URI += "file://xilinx-mw-axistream-dma.dtsi"
SRC_URI += "file://xilinx-mw-axistream-dma.h"
SRC_URI += "file://adi-mw-axistream-dma.h"
SRC_URI += "file://adi-mw-axistream-dma.dtsi"
SRC_URI += "file://adi-mw-axistream-iio-common.dtsi"
SRC_URI += "file://mw-axistream-iio-common.dtsi"
SRC_URI += "file://mw-axistream-iio-common.h"
SRC_URI += "file://mw-sharedmemory-iio.dtsi"

DEPENDS:append = "${@' lopper-native' if d.getVar('SYSTEM_DTFILE') != '' else ''}"

# We need the deployed output
PROC_TUNE:versal = "${@'cortexa72' if d.getVar('SYSTEM_DTFILE') != '' else ''}"
PROC_TUNE:zynqmp = "${@'cortexa53' if d.getVar('SYSTEM_DTFILE') != '' else ''}"

python () {
    if d.getVar("CONFIG_DISABLE"):
        d.setVarFlag("do_configure", "noexec", "1")
}

export PETALINUX
do_configure:append () {
    if [ ! -z "${SYSTEM_DTFILE}" ]; then
        user_dtsi="${TOPDIR}/../project-spec/decoupling-dtsi/system-user.dtsi"
        apu_dts="${TOPDIR}/../project-spec/decoupling-dtsi/${PROC_TUNE}-${SOC_FAMILY}-linux.dts"
	lopper -f -v --enhanced --permissive -i ${user_dtsi} ${apu_dts} system-default.dtb
    else
	script="${PETALINUX}/etc/hsm/scripts/petalinux_hsm_bridge.tcl"
	data=${PETALINUX}/etc/hsm/data/
	eval xsct -sdx -nodisp ${script} -c ${WORKDIR}/config \
	-hdf ${DT_FILES_PATH}/hardware_description.${HDF_EXT} -repo ${S} \
	-data ${data} -sw ${DT_FILES_PATH} -o ${DT_FILES_PATH} -a "soc_mapping"
    fi
}
