SRC_URI_append = " file://platform-top.h"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append += "file://mw_xilinx_common.h"
SRC_URI_append += "file://bsp.cfg"

do_copy_configs () {
 cp ${WORKDIR}/mw_xilinx_common.h ${S}/include/configs/mw_xilinx_common.h
}

do_patch_append() {
    bb.build.exec_func('do_copy_configs', d)
}
