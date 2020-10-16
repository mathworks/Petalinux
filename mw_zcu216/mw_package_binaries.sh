#/bin/bash -v

timestamp() {
    date +"%Y-%m-%d_%H-%M-%S"
}

timestamp_sdcard() {
    date +"%Y-%m-%d"
}


petalinux-package --boot --format BIN --bif ./mw_build_utils/boot.bif -o ./images/BOOT.BIN --force


src_dir="$(pwd)"
get_timestamp=$(timestamp)
dir_prefix=${PWD##*/}_
dst_dir=./MW_$dir_prefix$get_timestamp
echo $dst_dir
mkdir_cmd="mkdir -p $dst_dir"

$mkdir_cmd

cp images/linux/system.bit $dst_dir
cp images/BOOT.BIN $dst_dir
cp images/linux/Image $dst_dir
cp images/linux/boot.scr $dst_dir
cp images/linux/rootfs.cpio.gz.u-boot $dst_dir
cp images/linux/system.dtb $dst_dir/devicetree.dtb

print_dst=$(echo $dst_dir | sed 's/\//\\/g')
echo "Copied location: $print_dst"



