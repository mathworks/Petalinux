FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

#mki: this override only works if meta-user layer has higher priority than meta-xilinx-tools
SRC_URI += " file://0001-te-install-general-hooks-zynq.patch \
             file://0001-te-install-general-hooks-zynqmp.patch \
             file://git/lib/sw_apps/zynqmp_fsbl/src \
             file://git/lib/sw_apps/zynq_fsbl/src"
            