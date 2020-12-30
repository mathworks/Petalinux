SRC_URI += "file://bsp.cfg \
            file://devtool-fragment.cfg \
            "

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = " file://platform-top.h"
SRC_URI_append += "file://mw_xilinx_common.h"
SRC_URI_append += "file://bsp.cfg"
#SRC_URI_append += "file://devtool-fragment.cfg"

do_copy_configs () {
 cp ${WORKDIR}/mw_xilinx_common.h ${S}/include/configs/mw_xilinx_common.h
 }


do_configure_append () {
        if [ "${U_BOOT_AUTO_CONFIG}" = "1" ]; then
                install ${WORKDIR}/platform-auto.h ${S}/include/configs/
                install ${WORKDIR}/platform-top.h ${S}/include/configs/
        fi
}

do_configure_append_microblaze () {
        if [ "${U_BOOT_AUTO_CONFIG}" = "1" ]; then
                install -d ${B}/source/board/xilinx/microblaze-generic/
                install ${WORKDIR}/config.mk ${B}/source/board/xilinx/microblaze-generic/
        fi
}
