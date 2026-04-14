#!/bin/bash -v

TIMESTAMP=`date +"%Y-%m-%d_%H-%M-%S"`
TIMESTAMP_SD=`date +"%Y-%m-%d"`

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
PLATFORM=${DIR##*/}
SD_DIR=${DIR}/images/MW_${PLATFORM}_${TIMESTAMP}
SD_ARCHIVE=${DIR}/images/${PLATFORM}_sdcard_zynqmp_${TIMESTAMP_SD}.zip

BUILD_BASE_DIR=`grep -e CONFIG_TMP_DIR_LOCATION ${DIR}/project-spec/configs/config | cut -c 25- | sed 's/"//g'`

# Create ramdisk image
$BUILD_BASE_DIR/sysroots-components/x86_64/u-boot-mkimage-native/usr/bin/mkimage -A arm64 -T ramdisk -C gzip -d images/linux/rootfs.cpio.gz images/uramdisk.image.gz

# Create BOOT.BIN
petalinux-package --boot --format BIN --bif ${DIR}/mw_build_utils/boot.bif -o ${DIR}/images/BOOT.BIN --force

# Create SD card directory
mkdir -p $SD_DIR
cp $DIR/images/BOOT.BIN $SD_DIR
cp $DIR/images/uramdisk.image.gz $SD_DIR
cp $DIR/images/linux/*.dtb $SD_DIR
mv $SD_DIR/system.dtb $SD_DIR/devicetree.dtb
cp $DIR/images/linux/Image $SD_DIR
cp $DIR/hardware/mw_base.bit $SD_DIR/system.bit
cp $DIR/mw_build_utils/sdcard/* $SD_DIR

# Zip SD card directory
zip $SD_ARCHIVE -j $SD_DIR/*

if [ $? -eq 0 ]
then
  echo "Successfully created ${SD_ARCHIVE}"
else
  echo "Could not create file" >&2
fi
