FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://platform-top.h file://bsp.cfg file://user_2025-03-17-06-40-00.cfg file://0001-xilinx-zynqmp-Remove-board-specific-env_get_location.patch"
