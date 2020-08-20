SRC_URI += "file://devtool-fragment.cfg"
KERNEL_FEATURES_append = " bsp.cfg"
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append = " \
		file://0001-power-supply-irps-Add-support-for-irps-supply.patch \
		file://0002-i2c-cadence-Implement-timeout.patch \
"
