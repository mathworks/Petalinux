FILESEXTRAPATHS_prepend := "${THISDIR}/files/common/fs-overlay/usr:"
#Append symbolic links
SRC_URI += " file://sbin/"

do_install_append() {
    install -d ${D}${bindir}
    cd ${D}${bindir}
	ln -s ../sbin/fw_getbitstream fw_getbitstream 
	ln -s ../sbin/fw_getdevicetree fw_getdevicetree 
	ln -s ../sbin/fw_getrdname fw_getrdname 
	ln -s ../sbin/fw_setbitstream fw_setbitstream 
	ln -s ../sbin/fw_setdevicetree fw_setdevicetree 
	ln -s ../sbin/fw_setrdname fw_setrdname 
	ln -s ../sbin/_mw_backup_network _mw_backup_network 
	ln -s ../sbin/_mw_getbootdev _mw_getbootdev 
	ln -s ../sbin/_mw_reboot_mode _mw_reboot_mode 
	ln -s ../sbin/_mw_restore_network _mw_restore_network 
	ln -s ../sbin/mw_setboot mw_setboot 
	ln -s ../sbin/_mw_setboot_default _mw_setboot_default 
	ln -s ../sbin/_mw_setbootfile _mw_setbootfile 
        ln -s ../../sbin/fw_printenv fw_printenv
        ln -s ../../sbin/fw_setenv fw_setenv
        ln -s ../../sbin/reboot reboot
}
 

