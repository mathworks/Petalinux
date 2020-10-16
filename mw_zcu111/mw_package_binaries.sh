#!/bin/bash -v

timestamp() {
    date +"%Y-%m-%d_%H-%M-%S"
}

timestamp_sdcard() {
    date +"%Y-%m-%d"
}

assemble_dtb() {
    DMAWIDTH=$1      # DT parameter: DMA data width
    DMASAMPLECNT=$2  # DT parameter: DMA sample count register location
    DTS_BASE_DIR=$3  # MathWorks DTSI files
    DTSI_FILES=$4    # MathWorks DTSI files
    MW_DTSI_FILES=$5 # MathWorks DTSI files
    BUILD_BASE_DIR=$6
    XIL_DTS_DIR=$7 # Xilinx device tree folder locations
    OUTPUT_DIR=$8  # images folder
    DTS_BASE=$9    # system-top.dtsi file located in components
    DST_DIR=${10}  # output dir location for all binaries
    OUTPUT_NAME=${11}  # prefix of user-specified DTB name, ie: devicetree or devicetree_axi4master
    USE_SHAREDMEM=${12:-0}
    PP_OUTPUT=`basename ${DTS_BASE}`_${OUTPUT_NAME}_${DMAWIDTH}.pp
    DTS_NAME=`basename -s .dts ${DTS_BASE}`_${OUTPUT_NAME}_${DMAWIDTH}

  
    gcc  -E -nostdinc -Ulinux -x assembler-with-cpp -DMW_SHAREDMEM_ENABLE=${USE_SHAREDMEM} -DMW_DATAWIDTH_SELECT=${DMAWIDTH} -DMW_SAMPLECNT_REG=${DMASAMPLECNT} -I${DTS_BASE_DIR} -I${DTSI_FILES} -I${MW_DTSI_FILES} -I${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -I${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/arch/arm64/boot/dts/xilinx -I${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -I${XIL_DTS_DIR} -o ${OUTPUT_DIR}${PP_OUTPUT} ${DTS_BASE}
    dtc -I dts -O dtb -R 8 -p 0x1000 -b 0 -i ${DTS_BASE_DIR} -i ${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -i ${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/arch/arm64/boot/dts/xilinx -i ${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -i ${XIL_DTS_DIR} -o ${OUTPUT_DIR}${DTS_NAME}.dtb ${OUTPUT_DIR}${PP_OUTPUT}
    # Convert to DTS for examining
    dtc -I dtb -O dts -o ${OUTPUT_DIR}/${DTS_NAME}.dts ${OUTPUT_DIR}/${DTS_NAME}.dtb
    # Copy 128-bit DTB into destination dir

    if [ ${DMAWIDTH} == 64 ]
    then
        echo "Using 64-bit dtb file: skipping suffix"
        cp ${OUTPUT_DIR}${DTS_NAME}.dtb ${DST_DIR}/${OUTPUT_NAME}.dtb
    else
        echo "Appending 128-bit to dtb file name"
        cp ${OUTPUT_DIR}${DTS_NAME}.dtb ${DST_DIR}/${OUTPUT_NAME}_${DMAWIDTH}.dtb
    fi

}

build/tmp/sysroots-components/x86_64/u-boot-mkimage-native/usr/bin/mkimage -A arm64 -T ramdisk -C gzip -d images/linux/rootfs.cpio.gz images/uramdisk.image.gz
petalinux-package --boot --format BIN --bif ./mw_build_utils/boot.bif -o ./images/BOOT.BIN --force

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
cp_cmd6="cp $src_dir/mw_build_utils/interfaces $dst_dir"

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

# Create .zip file
get_timestamp_sd=$(timestamp_sdcard)
zip zcu111_sdcard_zynqrf_$get_timestamp_sd.zip -j $dst_dir/*
