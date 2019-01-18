#!/bin/bash
#set -x #echo on
DTS_BASE="$(pwd)/../components/plnx_workspace/device-tree/device-tree/*.dts"
DTS_BASE_DIR="$(pwd)/../components/plnx_workspace/device-tree/device-tree/"
BUILD_BASE_DIR="$(pwd)/../build/tmp"
XIL_DTS_DIR=$(ls -d ${BUILD_BASE_DIR}/work/zcu111_zynqmp-xilinx-linux/device-tree/xilinx+gitAUTOINC+*)
OUTPUT_DIR="$(pwd)/dtb_build/"
echo "XIL DTS Dir = $XIL_DTS_DIR"

gcc  -E	-nostdinc -Ulinux -x assembler-with-cpp	-DMW_DATAWIDTH_SELECT=128 -I${DTS_BASE_DIR} -I${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -I${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/arch/arm64/boot/dts/xilinx -I${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -I${XIL_DTS_DIR} -o ${OUTPUT_DIR}`basename ${DTS_BASE}`128.pp ${DTS_BASE}

DTS_NAME=`basename -s .dts ${DTS_BASE}`

dtc -I dts -O dtb -R 8 -p 0x1000 -b 0 -i ${DTS_BASE_DIR} -i ${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -i ${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/arch/arm64/boot/dts/xilinx -i ${BUILD_BASE_DIR}/work-shared/zcu111-zynqmp/kernel-source/include -i ${XIL_DTS_DIR} -o ${OUTPUT_DIR}${DTS_NAME}128.dtb ${OUTPUT_DIR}`basename ${DTS_BASE}`128.pp

dtc -I dtb -O dts -o ${OUTPUT_DIR}/${DTS_NAME}128.dts ${OUTPUT_DIR}/${DTS_NAME}128.dtb

exit

