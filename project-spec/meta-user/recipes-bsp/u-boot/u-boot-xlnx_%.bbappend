SRC_URI_append = " file://platform-top.h"
SRC_URI_append = " file://mw_xilinx_common.h"
SRC_URI += "file://bsp.cfg"


FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
