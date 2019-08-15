SRC_URI += "file://bsp.cfg \
            file://user_2018-08-10-12-34-00.cfg \
            "

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append = " \
        file://0001-clk-lmx2594-Add-support-for-lmx2594-clock.patch  \
        file://0002-power-supply-irps-Add-support-for-irps-supply.patch \
        file://0003-drivers-misc-add-support-for-DDR-memory-management.patch \
"
