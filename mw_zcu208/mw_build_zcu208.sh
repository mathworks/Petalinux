#!/bin/sh

# Copyright 2025 The MathWorks, Inc.

# This script adds the meta-mathworks layer to PetaLinux configuration and runs the config for PetaLinux and builds the image

timestamp() {
    date +"%Y-%m-%d_%H-%M-%S"
}
timestamp_sdcard() {
    date +"%Y-%m-%d"
}

usage() {
    echo "Configure and build PetaLinux project with meta-mathworks (https://github.com/mathworks/meta-mathworks) Yocto layer"
    echo "Usage: $0 <petalinux-installation> <petalinux-project-root>"
    echo ""
    echo "Example:"
    echo "$0 /home/$USER/petalinux/2023.1 /home/$USER/xilinx-zcu208-2023.1"
}

unique_file_append_usage() {
    echo "Usage: Append the text iff it is not present in the file."
    echo "unique_file_append <file-name> <text>"
}

unique_file_append() {
    if [ $# -ne 2 ]; then
        unique_file_append_usage
        exit 1;
    fi
    if [ ! -f "$1" ]; then
        unique_file_append_usage
        echo "File $1 not found..."
        exit 1;
    fi

    TGTFILE=$1
    TXT=$2
    grep -q -E "$TXT" "$TGTFILE"
    ret=$?
    if [ $ret -eq 0 ]; then
        echo "+++ $TXT present.. skip"
    else
        echo "+++ Appending $TXT to $TGTFILE..."
        echo "$TXT" >> "$TGTFILE"
    fi
}

if [ $# -ne 2 ]; then
    usage
    exit 1
fi

PLNXHOME=$1
PLNXPRJ=$2

# Canonicalize paths to absolute paths
if command -v realpath >/dev/null 2>&1; then
    PLNXHOME=$(realpath "$PLNXHOME")
    PLNXPRJ=$(realpath "$PLNXPRJ")
else
    PLNXHOME=$(readlink -f "$PLNXHOME")
    PLNXPRJ=$(readlink -f "$PLNXPRJ")
fi

# PetaLinux folder validation
if [ ! -f "$PLNXHOME/settings.sh" ]; then
    echo "First argument must be a valid PetaLinux installation folder"
    exit 1
else
    echo "+++ Sourcing PetaLinux settings..."
    # shellcheck source=/dev/null
    source "$PLNXHOME/settings.sh"
fi

# PetaLinux project folder validation
if [ ! -d "$PLNXPRJ/project-spec/meta-user/conf" ]; then
    echo "Second argument must be a valid PetaLinux project folder"
    exit 1
fi

# Set meta-mathworks layer directory
METAMWDIR="$PLNXPRJ/project-spec/meta-mathworks"

# Clone meta-mathworks layer if not already present
if [ -d "$METAMWDIR" ]; then
    echo "+++ $METAMWDIR already exists, skipping clone."
else
    echo "+++ Cloning meta-mathworks layer..."
    git clone -b petalinux-v2023.1 https://github.com/mathworks/meta-mathworks.git "$METAMWDIR"
fi


PLNXCFG="$PLNXPRJ/project-spec/configs/config"
USRROOTFSCFG="$PLNXPRJ/project-spec/meta-user/conf/user-rootfsconfig"
ROOTFSCFG="$PLNXPRJ/project-spec/configs/rootfs_config"

# Add MW Layer
echo "+++ Updating $PLNXCFG"
cat <<EOF >> "$PLNXCFG"
CONFIG_USER_LAYER_0="$METAMWDIR"
EOF

# Add User rootfs
echo "+++ Updating $USRROOTFSCFG"
for cfg in CONFIG_libserialport CONFIG_libiio CONFIG_boost CONFIG_boost-dev CONFIG_lttng-ust CONFIG_lttng-ust-bin CONFIG_mw-fs-overlay CONFIG_rfdc CONFIG_rfdc-read-write CONFIG_rfdc-selftest CONFIG_libsdfecusrintf CONFIG_rfdc CONFIG_rfdc-read-write CONFIG_rfdc-selftest CONFIG_mw-rf-init CONFIG_rftool-zcu208
do
    unique_file_append "$USRROOTFSCFG" "$cfg"
done

# Add plnx rootfs
echo "+++ Updating $ROOTFSCFG"
cat <<EOF >> "$ROOTFSCFG"
CONFIG_libserialport=y
CONFIG_libiio=y
CONFIG_boost=y
CONFIG_lttng-ust=y
CONFIG_lttng-ust-bin=y
CONFIG_mw-fs-overlay=y
CONFIG_rfdc=y
CONFIG_rfdc-read-write=y
CONFIG_rfdc-selftest=y
CONFIG_mw-rf-init=y
CONFIG_rftool-zcu208=y

CONFIG_ADD_EXTRA_USERS="root:root;petalinux::passwd-expire;"
CONFIG_CREATE_NEW_GROUPS="aie;"
CONFIG_ADD_USERS_TO_GROUPS="petalinux:audio,video,aie;"
CONFIG_ADD_USERS_TO_SUDOERS="petalinux"
EOF

echo "Running silentconfig to apply changes..."
petalinux-config -p "$PLNXPRJ" --silentconfig
petalinux-config -c rootfs -p "$PLNXPRJ" --silentconfig

echo "+++ Done configuring PetaLinux."

echo "+++ Running petalinux-build..."
petalinux-build -p "$PLNXPRJ"

echo "+++ Running petalinux-package --boot -- overwriting BOOT.BIN"
petalinux-package --boot --uboot -p "$PLNXPRJ" --force 

echo "+++ Creating zip archive of final image..."
DIR="$PLNXPRJ"
PLATFORM=${DIR##*/}
SD_DIR="$DIR/images/MW_${PLATFORM}_$(timestamp)"
SD_ARCHIVE="$DIR/images/${PLATFORM}_sdcard_zynqMPSoC_$(timestamp_sdcard).zip"
mkdir -p "$SD_DIR"
echo "Packaging Image"
    cp $DIR/images/linux/BOOT.BIN $SD_DIR
    cp $DIR/images/linux/system.bit $SD_DIR
    cp $DIR/images/linux/Image $SD_DIR
    cp $DIR/images/linux/boot.scr $SD_DIR
    cp $DIR/images/linux/rootfs.cpio.gz.u-boot $SD_DIR
    cp $DIR/images/linux/system.dtb $SD_DIR/system.dtb
	cp $DIR/mw_utils/dhcp.script $SD_DIR/dhcp.script
	cp $DIR/mw_utils/init.sh $SD_DIR/init.sh
	cp $DIR/mw_utils/rf-init $SD_DIR/rf-init
	cp $DIR/mw_utils/libboost-1-63-overlay.tar.gz $SD_DIR/libboost-1-63-overlay.tar.gz
	cp $DIR/mw_utils/stopsociod.service $SD_DIR/stopsociod.service
	cp $DIR/mw_utils/interfaces $SD_DIR/interfaces

zip "$SD_ARCHIVE" -j "$SD_DIR"/*
if [ $? -eq 0 ]
then
    echo "Successfully Created ${SD_ARCHIVE}"
else
    echo "Could not create file" >&2
fi