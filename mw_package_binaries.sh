#!/bin/bash -v
timestamp() {
      date +"%Y-%m-%d_%H-%M-%S"
}

timestamp_sdcard() {
    date +"%Y-%m-%d"
}

build/tmp/sysroots-components/x86_64/u-boot-mkimage-native/usr/bin/mkimage -A arm64 -T ramdisk -C gzip -d images/linux/rootfs.cpio.gz images/uramdisk.image.gz
petalinux-package --boot --format BIN --bif ./images/boot.bif -o ./images/BOOT.BIN --force

src_dir="$(pwd)"
get_timestamp=$(timestamp)
dir_prefix=${PWD##*/}_
dst_dir=./MW_$dir_prefix$get_timestamp
echo $dst_dir
mkdir_cmd="mkdir -p $dst_dir"
cp_cmd1="cp $src_dir/images/BOOT.BIN $dst_dir"
cp_cmd2="cp $src_dir/images/uramdisk.image.gz $dst_dir"
cp_cmd3="cp $src_dir/images/linux/system.dtb $dst_dir/devicetree.dtb"
cp_cmd4="cp $src_dir/images/linux/Image $dst_dir"
cp_cmd5="cp $src_dir/images/linux/system.bit $dst_dir"
cp_cmd6="cp $src_dir/images/interfaces $dst_dir"

$mkdir_cmd; $cp_cmd1; $cp_cmd2; $cp_cmd3; $cp_cmd4; $cp_cmd5; $cp_cmd6

print_dst=$(echo $dst_dir | sed 's/\//\\/g')
echo "Copied location: \\$print_dst"

# Create 128-bit device-tree
DTS_BASE="$(pwd)/components/plnx_workspace/device-tree/device-tree/*.dts"
DTS_BASE_DIR="$(pwd)/components/plnx_workspace/device-tree/device-tree/"
BUILD_BASE_DIR="$(pwd)/build/tmp"
XIL_DTS_DIR=$(ls -d ${BUILD_BASE_DIR}/work/zcu111_zynqmp-xilinx-linux/device-tree/xilinx+gitAUTOINC+*)
OUTPUT_DIR="$(pwd)/images/"
DTSI_FILES="$(pwd)/project-spec/meta-user/recipes-bsp/device-tree/files/"
MW_DTSI_FILES="$(pwd)/project-spec/meta-user/recipes-bsp/device-tree/files/mathworks"

echo "XIL DTS Dir = $XIL_DTS_DIR"

gcc  -E -nostdinc -Ulinux -x assembler-with-cpp -DMW_DATAWIDTH_SELECT=128 -I${DTS_BASE_DIR} -I${DTSI_FILES} -I${MW_DTSI_FILES} -I${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -I${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/arch/arm64/boot/dts/xilinx -I${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -I${XIL_DTS_DIR} -o ${OUTPUT_DIR}`basename ${DTS_BASE}`128.pp ${DTS_BASE}

DTS_NAME=`basename -s .dts ${DTS_BASE}`

dtc -I dts -O dtb -R 8 -p 0x1000 -b 0 -i ${DTS_BASE_DIR} -i ${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -i ${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/arch/arm64/boot/dts/xilinx -i ${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -i ${XIL_DTS_DIR} -o ${OUTPUT_DIR}${DTS_NAME}128.dtb ${OUTPUT_DIR}`basename ${DTS_BASE}`128.pp
# Convert to DTS for examining
dtc -I dtb -O dts -o ${OUTPUT_DIR}/${DTS_NAME}128.dts ${OUTPUT_DIR}/${DTS_NAME}128.dtb

# Copy 128-bit DTB into destination dir
cp ${OUTPUT_DIR}${DTS_NAME}128.dtb ${dst_dir}/devicetree_128.dtb

# Create .zip file
get_timestamp_sd=$(timestamp_sdcard)
zip zcu111_sdcard_zynqrf_$get_timestamp_sd.zip -j $dst_dir/*
