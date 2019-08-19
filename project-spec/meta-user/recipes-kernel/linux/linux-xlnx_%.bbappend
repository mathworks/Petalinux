SRC_URI += "file://bsp.cfg \
            file://user_2018-08-10-12-34-00.cfg \
            file://user_2019-08-18-12-29-00.cfg \
            "

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append = " \
       	file://0001-power-supply-irps-Add-support-for-irps-supply.patch \
        file://0002-drivers-misc-add-support-for-DDR-memory-management.patch \
        file://0004-drivers-misc-add-support-for-selecting-mem-type.patch \
        file://0005-i2c-cadence-Implement-timeout.patch \
        file://0006-i2c-muxes-update-Mux-register-always.patch \
        file://0007-drivers-misc-change-ADC-packet-size-as-per-FIFO-size.patch \
"
