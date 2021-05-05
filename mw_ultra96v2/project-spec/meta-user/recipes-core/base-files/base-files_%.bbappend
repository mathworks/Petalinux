do_install_append () {
    echo "alias ls='ls --color=auto'" >> ${D}${sysconfdir}/profile
}
