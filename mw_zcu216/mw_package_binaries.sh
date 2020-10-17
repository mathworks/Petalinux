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
dst_dir=./$dir_prefix$get_timestamp
echo $dst_dir
mkdir_cmd="mkdir -p $dst_dir"

$mkdir_cmd

cp $src_dir/images/linux/system.bit $dst_dir
cp $src_dir/images/BOOT.BIN $dst_dir
cp $src_dir/images/linux/Image $dst_dir
cp $src_dir/images/linux/boot.scr $dst_dir
cp $src_dir/images/linux/rootfs.cpio.gz.u-boot $dst_dir
cp $src_dir/images/linux/system.dtb $dst_dir/devicetree.dtb
cp $src_dir/mw_build_utils/interfaces $dst_dir

print_dst=$(echo $dst_dir | sed 's/\//\\/g')
echo "Copied location: $print_dst"

get_timestamp_sd=$(timestamp_sdcard)
zip zcu216_sdcard_zynqrf_$get_timestamp_sd.zip -j $dst_dir/*
