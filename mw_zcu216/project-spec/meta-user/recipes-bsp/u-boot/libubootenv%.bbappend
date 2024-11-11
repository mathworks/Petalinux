FILESEXTRAPATHS:prepend := "${THISDIR}/files/etc:"


SRC_URI:append = " file://fw_env.config"

do_install:append() {
	install -d ${D}${sysconfdir}
	UBOOT_CONFIG=${BASE_WORKDIR}/${MACHINE_ARCH}${HOST_VENDOR}-${TARGET_OS}/u-boot-xlnx/*/build/.config
	if [ -f $UBOOT_CONFIG ]; then
		env_size=$(sed -n 's/CONFIG_ENV_SIZE\=\(.*\)/\1/p' $UBOOT_CONFIG)
		echo "Set correct environment size, $env_size, from $UBOOT_CONFIG"
		sed -i "s/0x8000/${env_size}/g" ${WORKDIR}/fw_env.config
	fi
	install -m 0644 ${WORKDIR}/fw_env.config ${D}${sysconfdir}/fw_env.config
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
