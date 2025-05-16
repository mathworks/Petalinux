#/bin/bash -v

timestamp() {
    date +"%Y-%m-%d_%H-%M-%S"
}

timestamp_sdcard() {
    date +"%Y-%m-%d"
}


petalinux-package --boot --u-boot --force


#src_dir="$(dirname "$(pwd)")"
src_dir="$(pwd)"
get_timestamp=$(timestamp)
dir_prefix="kria_kr260_linux_binaries_"
dst_dir=./$dir_prefix$get_timestamp
echo $dst_dir
mkdir_cmd="mkdir -p $dst_dir"

$mkdir_cmd

cp $src_dir/images/linux/system_top_wrapper.bit $dst_dir/system.bit
cp $src_dir/images/linux/BOOT.BIN $dst_dir
cp $src_dir/images/linux/Image $dst_dir
cp $src_dir/images/linux/boot.scr $dst_dir
cp $src_dir/images/linux/rootfs.cpio.gz.u-boot $dst_dir
cp $src_dir/images/linux/system.dtb $dst_dir
cp $src_dir/images/linux/system-zynqmp-sck-kr-g-revB.dtb $dst_dir
#By default the dtbo will be pl.dtbo, but the dtbo filename has to be changed as per the bit file name used in the Vivado design. All MathWorks applications use system_wrapper as bit file name
cp $src_dir/images/linux/pl.dtbo $dst_dir/system_wrapper.dtbo
cp $src_dir/mw_utils/init.sh $dst_dir
cp $src_dir/mw_utils/interfaces $dst_dir
cp $src_dir/mw_utils/shell.json $dst_dir

get_timestamp_sd=$(timestamp_sdcard)
zip kria_kr260_sdcard_$get_timestamp_sd.zip -j $dst_dir/*
rm -rf $dst_dir
