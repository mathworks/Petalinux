FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://system-user.dtsi"
SRC_URI:append = " file://mw-aximm-common.dtsi"
SRC_URI:append = " file://zynqmp-mw-common.dtsi"
SRC_URI:append = " file://zynqmp-mw-fpga-bridge-common.dtsi"
SRC_URI:append = " file://base.dtsi"

require ${@'device-tree-sdt.inc' if d.getVar('SYSTEM_DTFILE') != '' else ''}
