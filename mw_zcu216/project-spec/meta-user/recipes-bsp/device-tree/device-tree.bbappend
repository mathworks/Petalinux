FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
FILESEXTRAPATHS_prepend := "${THISDIR}/files/mathworks:"

SRC_URI += "file://system-user.dtsi"

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

python () {
    if d.getVar("CONFIG_DISABLE"):
        d.setVarFlag("do_configure", "noexec", "1")
}

export PETALINUX
do_configure_append () {
	script="${PETALINUX}/etc/hsm/scripts/petalinux_hsm_bridge.tcl"
	data=${PETALINUX}/etc/hsm/data/
	eval xsct -sdx -nodisp ${script} -c ${WORKDIR}/config \
	-hdf ${DT_FILES_PATH}/hardware_description.${HDF_EXT} -repo ${S} \
	-data ${data} -sw ${DT_FILES_PATH} -o ${DT_FILES_PATH} -a "soc_mapping"
}
